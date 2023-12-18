# Mantid Repository : https://github.com/mantidproject/mantid
#
# Copyright &copy; 2023 ISIS Rutherford Appleton Laboratory UKRI,
#   NScD Oak Ridge National Laboratory, European Spallation Source,
#   Institut Laue - Langevin & CSNS, Institute of High Energy Physics, CAS
# SPDX - License - Identifier: GPL - 3.0 +
from mantid.utils.reflectometry.orso_helper import MantidORSODataColumns, MantidORSODataset, MantidORSOSaver

from mantid.kernel import Direction
from mantid.api import AlgorithmFactory, MatrixWorkspaceProperty, FileProperty, FileAction, PythonAlgorithm, PropertyMode, FileFinder

from pathlib import Path
from typing import Optional, Tuple, Union, List
import re


class Prop:
    INPUT_WS = "InputWorkspace"
    WRITE_RESOLUTION = "WriteResolution"
    FILENAME = "Filename"


class SaveISISReflectometryORSO(PythonAlgorithm):
    """
    See https://www.reflectometry.org/ for more information about the ORSO .ort format
    """

    _FACILITY = "ISIS"
    _ISIS_DOI_PREFIX = "10.5286/ISIS.E.RB"
    _RB_NUM_LOGS = ("rb_proposal", "experiment_identifier")
    _INVALID_HEADER_COMMENT = "Mantid@ISIS output may not be fully ORSO compliant"
    _REDUCTION_ALG = "ReflectometryReductionOneAuto"
    _REDUCTION_WORKFLOW_ALG = "ReflectometryISISLoadAndProcess"
    _STITCH_ALG = "Stitch1DMany"
    _REBIN_ALG = "Rebin"
    _Q_UNIT = "MomentumTransfer"

    def category(self):
        return "Reflectometry\\ISIS"

    def name(self):
        """Return the name of the algorithm."""
        return "SaveISISReflectometryORSO"

    def summary(self):
        """Return a summary of the algorithm."""
        return "Saves ISIS processed reflectometry workspaces into the ASCII implementation of the ORSO data standard."

    def PyInit(self):
        self.declareProperty(
            MatrixWorkspaceProperty(name=Prop.INPUT_WS, defaultValue="", direction=Direction.Input, optional=PropertyMode.Mandatory),
            doc="The workspace containing the reduced reflectivity data to save.",
        )

        self.declareProperty(
            name=Prop.WRITE_RESOLUTION,
            defaultValue=True,
            direction=Direction.Input,
            doc="Whether to compute resolution values and write them as the fourth data column.",
        )

        self.declareProperty(
            FileProperty(Prop.FILENAME, "", FileAction.Save, MantidORSOSaver.FILE_EXT), doc="File path to save the .ort file to"
        )

    def validateInputs(self):
        """Return a dictionary containing issues found in properties."""
        issues = dict()

        ws = self.getProperty(Prop.INPUT_WS).value
        if ws and not self._is_momentum_transfer_ws(ws):
            issues[Prop.INPUT_WS] = f"{Prop.INPUT_WS} must have units of {self._Q_UNIT}"
        return issues

    def _is_momentum_transfer_ws(self, ws) -> bool:
        return ws.getAxis(0).getUnit().unitID() == self._Q_UNIT

    def PyExec(self):
        ws = self.getProperty(Prop.INPUT_WS).value

        # We cannot include all the mandatory information required by the standard, so we include a comment to highlight
        # this at the top of the file. In future this comment will not be needed, or we may need to add validation to
        # determine if it should be included (although ideally validation would be implemented in the orsopy library).
        orso_saver = MantidORSOSaver(self.getProperty(Prop.FILENAME).value, self._INVALID_HEADER_COMMENT)

        # Create the file contents
        orso_saver.add_dataset(self._create_dataset(ws, ws.name()))

        # Write the file to disk in the ORSO ASCII format
        if Path(orso_saver.filename).is_file():
            self.log().warning("File already exists and will be overwritten")

        try:
            orso_saver.save_orso_ascii()
        except OSError as e:
            raise RuntimeError(
                f"Error writing ORSO file. Check that the filepath is valid and does not contain any invalid characters.\n{e}"
            )

    def _create_dataset(self, ws, dataset_name: str = None) -> MantidORSODataset:
        reduction_history = self._get_reduction_alg_history(ws)
        data_columns = self._create_data_columns(ws, reduction_history)
        dataset = self._create_dataset_with_mandatory_header(ws, dataset_name, reduction_history, data_columns)
        self._add_optional_header_info(dataset, ws)
        return dataset

    def _create_data_columns(self, ws, reduction_history) -> MantidORSODataColumns:
        """
        Set up the column headers and data values
        """
        resolution = self._get_resolution(ws, reduction_history)

        alg = self.createChildAlgorithm("ConvertToPointData", InputWorkspace=ws, OutputWorkspace="pointData")
        alg.execute()
        point_data = alg.getProperty("OutputWorkspace").value

        q_data = point_data.extractX()[0]
        reflectivity = point_data.extractY()[0]
        reflectivity_error = point_data.extractE()[0]
        q_resolution = resolution if resolution is None else q_data * resolution

        data_columns = MantidORSODataColumns(q_data, reflectivity, reflectivity_error, q_resolution)

        return data_columns

    def _create_dataset_with_mandatory_header(
        self, ws, dataset_name: str, reduction_history, data_columns: MantidORSODataColumns
    ) -> MantidORSODataset:
        """
        Create a dataset with the data columns and the mandatory information populated in the header
        """
        return MantidORSODataset(
            dataset_name,
            data_columns,
            ws,
            reduction_timestamp=self._get_reduction_timestamp(reduction_history),
            creator_name=self.name(),
            creator_affiliation=MantidORSODataset.SOFTWARE_NAME,
        )

    def _add_optional_header_info(self, dataset: MantidORSODataset, ws) -> None:
        """
        Populate the non_mandatory information in the header
        """
        run = ws.getRun()
        rb_number, doi = self._get_rb_number_and_doi(run)
        dataset.set_facility(self._FACILITY)
        dataset.set_proposal_id(rb_number)
        dataset.set_doi(doi)

        reduction_workflow_histories = self._get_reduction_workflow_alg_histories(ws)
        if not reduction_workflow_histories:
            self.log().warning(f"Unable to find history for {self._REDUCTION_WORKFLOW_ALG} - some metadata will be excluded from the file.")
            return

        for file in self._get_individual_angle_files(ws, reduction_workflow_histories):
            dataset.add_measurement_file(True, file)

    def _get_rb_number_and_doi(self, run) -> Union[Tuple[str, str], Tuple[None, None]]:
        """
        Check if the experiment RB number can be found in the workspace logs.
        This can be stored under different log names depending on whether time slicing was performed.
        If found, the RB number is used to provide the ISIS experiment DOI.
        """
        for log_name in self._RB_NUM_LOGS:
            if run.hasProperty(log_name):
                rb_num = str(run.getProperty(log_name).value)
                return rb_num, f"{self._ISIS_DOI_PREFIX}{rb_num}"
        return None, None

    def _get_resolution(self, ws, reduction_history) -> Optional[float]:
        if not self.getProperty(Prop.WRITE_RESOLUTION).value:
            return None

        # Attempt to get the resolution from the workspace history if it hasn't been passed in
        history = ws.getHistory()
        if not history.empty():
            for history in reversed(history.getAlgorithmHistories()):
                if history.name() == self._STITCH_ALG:
                    # The absolute value of the stitch parameter is the resolution
                    return abs(float(history.getPropertyValue("Params")))

            if reduction_history:
                rebin_alg = reduction_history.getChildHistories()[-1]
                if rebin_alg.name() == self._REBIN_ALG:
                    rebin_params = rebin_alg.getPropertyValue("Params").split(",")
                    if len(rebin_params) == 3:
                        # The absolute value of the middle rebin parameter is the resolution
                        return abs(float(rebin_params[1]))

        self.log().warning("Unable to find resolution from workspace history.")
        return None

    def _get_reduction_timestamp(self, reduction_history):
        """
        Get the reduction algorithm execution date, which is in UTC, and convert it to a
        datetime object expressed in local time
        """
        if not reduction_history:
            return None

        try:
            return MantidORSODataset.create_local_datetime_from_utc_string(reduction_history.executionDate().toISO8601String())
        except ValueError:
            self.log().warning(
                "Could not parse reduction timestamp into required format - this information will be excluded from the file."
            )
            return None

    def _get_individual_angle_files(self, ws, reduction_workflow_histories) -> List[str]:
        """
        Find the names of the individual angle files that were used in the reduction
        """
        angle_files = []
        instrument = ws.getInstrument().getName()

        for history in reduction_workflow_histories:
            input_runs = history.getPropertyValue("InputRunList")
            try:
                angle_files.extend(self._get_file_names_from_run_numbers(input_runs, instrument))
            except RuntimeError:
                self.log().warning(
                    f"Could not find all angle filenames for run(s) {input_runs}. Angle file information will be excluded from the file."
                )
                return []
        return angle_files

    @staticmethod
    def _get_file_names_from_run_numbers(run_numbers: str, instrument_name: str) -> List[str]:
        """
        Get the fully qualified file names from a string of comma-separated run numbers.
        """
        # If the run number part is all digits then append the instrument name to ensure we look up the correct run
        # files. If we don't do this we will look up files for the user's default instrument, which isn't guaranteed
        # to match the reduced workspace instrument.
        run_list = [f"{instrument_name}{run}" if re.search(r"\D+", run.split(".")[0]) is None else run for run in run_numbers.split(",")]

        filepaths = FileFinder.findRuns(",".join(run_list))
        return [Path(filepath).name for filepath in filepaths]

    def _get_reduction_alg_history(self, ws):
        """
        Find the first occurrence of the reduction algorithm in the workspace history, otherwise return None
        """
        ws_history = ws.getHistory()
        if ws_history.empty():
            return None

        for history in reversed(ws_history.getAlgorithmHistories()):
            if history.name() == self._REDUCTION_ALG:
                return history

            if history.name() == self._REDUCTION_WORKFLOW_ALG:
                for child_history in reversed(history.getChildHistories()):
                    if child_history.name() == self._REDUCTION_ALG:
                        return child_history

        return None

    def _get_reduction_workflow_alg_histories(self, ws):
        """
        Return a list containing all the occurrences of the reduction workflow algorithm in the workspace history
        """
        ws_history = ws.getHistory()
        if ws_history.empty():
            return []

        return [history for history in ws_history.getAlgorithmHistories() if history.name() == self._REDUCTION_WORKFLOW_ALG]


# Register algorithm with Mantid
AlgorithmFactory.subscribe(SaveISISReflectometryORSO)
