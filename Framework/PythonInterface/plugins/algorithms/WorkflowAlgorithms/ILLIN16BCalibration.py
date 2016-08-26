#pylint: disable=no-init
from mantid.kernel import *
from mantid.api import (WorkspaceProperty, FileProperty, FileAction,
                        DataProcessorAlgorithm, AlgorithmFactory)
from mantid.simpleapi import *


class ILLIN16BCalibration(DataProcessorAlgorithm):

    _input_file = None
    _out_ws = None
    _map_file = None
    _peak_range = None
    _intensity_scale = None



    def category(self):
        return 'Workflow\\Inelastic;Inelastic\\Calibration'


    def summary(self):
        return 'Creates a calibration workspace in energy trnasfer for IN16B.'


    def PyInit(self):
        self.declareProperty(FileProperty(name='Run',defaultValue='',
                                          action=FileAction.Load,
                                          extensions=['nxs']),
                             doc='List of input file (s)')

        self.declareProperty(FileProperty(name='MapFile', defaultValue='',
                                          action=FileAction.OptionalLoad,
                                          extensions=['xml']),
                             doc='Detector grouping map file')

        self.declareProperty(FloatArrayProperty(name='PeakRange', values=[-0.01, 0.01],
                                                validator=FloatArrayMandatoryValidator()),
                             doc='Peak range in energy transfer in meV')

        self.declareProperty(name='ScaleFactor', defaultValue=1.0,
                             doc='Intensity scaling factor')

        self.declareProperty(WorkspaceProperty('OutputWorkspace', 'calib',
                                               direction=Direction.Output),
                             doc='Output workspace for calibration data')


    def PyExec(self):
        self._setup()

        # Looking for mirror_sense: another possibility was to add an input option mirror_sense
        __ws_in = Load(self._input_file)
        gRun = __ws_in.getRun()
        DeleteWorkspace(__ws_in)

        if gRun.hasProperty('Doppler.mirror_sense'):
            # mirror_sense 14 : two wings
            # mirror_sense 16 : one wing
            if gRun.getLogData('Doppler.mirror_sense').value == 14:
                unmirror_option = 3
                self.log().information('Input run has two wings, using UnmirrorOption 3 (sum of left and right)')
            else:
                unmirror_option = 0
                self.log().information('Input run has one wing, using UnmirrorOption 0')

            # Do an energy transfer reduction
            __temp = IndirectILLReduction(Run=self._input_file,
                                          MapFile=self._map_file,
                                          SumRuns=True, DebugMode=False,
                                          UnmirrorOption=unmirror_option)
        else:
            self.log().warning('Input run (IN16B) has no property Doppler.mirror_sense. Check your input file.')
            self.log().information('Input run has one wing, using UnmirrorOption 0')
            __temp = IndirectILLReduction(Run=self._input_file,
                                        MapFile=self._map_file,
                                        SumRuns=True, DebugMode=False,
                                        UnmirrorOption=0)


        # Integrate within peak range
        __ws_name = __temp.getItem(0).getName()
        __ws_name = Integration(InputWorkspace=__ws_name,
                    RangeLower=float(self._peak_range[0]),
                    RangeUpper=float(self._peak_range[1]))

        # Process automatic scaling
        if self._intensity_scale == 1:
            number_histograms = __temp.getItem(0).getNumberHistograms()

            __ws_mask, num_zero_spectra = FindDetectorsOutsideLimits(InputWorkspace=__ws_name)
            DeleteWorkspace(__ws_mask)

            __temp_sum = SumSpectra(InputWorkspace=__ws_name)

            total = __temp_sum.readY(0)[0]
            DeleteWorkspace(__temp_sum)

            self._intensity_scale = 1 / (total / (number_histograms - num_zero_spectra))

        DeleteWorkspace(__temp)

        # Apply scaling factor
        Scale(InputWorkspace=__ws_name,
              OutputWorkspace=self._out_ws,
              Factor=self._intensity_scale,
              Operation='Multiply')

        DeleteWorkspace(__ws_name)

        self.setProperty('OutputWorkspace', self._out_ws)


    def _setup(self):
        """
        Gets properties.
        """
        self._input_file = self.getProperty('Run').value
        self._out_ws = self.getPropertyValue('OutputWorkspace')

        self._map_file = self.getPropertyValue('MapFile')
        self._peak_range = self.getProperty('PeakRange').value

        self._intensity_scale = self.getProperty('ScaleFactor').value

    def validateInputs(self):
        """
        Validates input ranges.
        """
        issues = dict()

        issues['PeakRange'] = self._validate_range('PeakRange')

        return issues


    def _validate_range(self, property_name):
        """
        Validates a range property.

        @param property_name Name of the property to validate
        @returns String detailing error, None if no error
        """

        prop_range = self.getProperty(property_name).value
        if len(prop_range) == 2:
            if prop_range[0] > prop_range[1]:
                return 'Invalid range'
        else:
            return 'Incorrect number of values (should be 2)'


        return None


# Register algorithm with Mantid
AlgorithmFactory.subscribe(ILLIN16BCalibration)
