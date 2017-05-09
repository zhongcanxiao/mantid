from __future__ import (absolute_import, division, print_function)

import mantid.simpleapi as mantid
import os


def split_into_tof_d_spacing_groups(run_details, processed_spectra):
    """
    Splits a processed list containing all focused banks into TOF and
    d-spacing groups. It also sets the names of the output workspaces
    to the run number(s) - Result<Unit>-<Bank Number> e.g.
    123-130-ResultTOF-3
    :param run_details: The run details associated with this run
    :param processed_spectra: A list containing workspaces, one entry per focused bank.
    :return: A workspace group for dSpacing and TOF in that order
    """
    d_spacing_output = []
    tof_output = []
    run_number = str(run_details.output_run_string)
    ext = run_details.file_extension if run_details.file_extension else ""
    for name_index, ws in enumerate(processed_spectra):
        d_spacing_out_name = run_number + ext + "-ResultD-" + str(name_index + 1)
        tof_out_name = run_number + ext + "-ResultTOF-" + str(name_index + 1)

        d_spacing_output.append(mantid.ConvertUnits(InputWorkspace=ws, OutputWorkspace=d_spacing_out_name,
                                                    Target="dSpacing"))
        tof_output.append(mantid.ConvertUnits(InputWorkspace=ws, OutputWorkspace=tof_out_name, Target="TOF"))

    # Group the outputs
    d_spacing_group_name = run_number + ext + "-Results-D-Grp"
    d_spacing_group = mantid.GroupWorkspaces(InputWorkspaces=d_spacing_output, OutputWorkspace=d_spacing_group_name)
    tof_group_name = run_number + ext + "-Results-TOF-Grp"
    tof_group = mantid.GroupWorkspaces(InputWorkspaces=tof_output, OutputWorkspace=tof_group_name)

    return d_spacing_group, tof_group


def save_focused_data(d_spacing_group, tof_group, output_paths, run_number_string, inst_prefix):
    """
    Saves out focused data into nxs, GSAS and .dat formats. Requires the grouped workspace
    in TOF and dSpacing, the a dictionary of output paths generated by abstract_inst,
    the user inputted run number string and the prefix of the instrument.
    :param d_spacing_group: The focused workspace group in dSpacing
    :param tof_group: The focused workspace group in TOF
    :param output_paths: A dictionary containing the full paths to save to
    :param run_number_string: The user input run number(s) as a string to generate output name
    :param inst_prefix: The instrument prefix to generate output name
    :return: None
    """
    mantid.SaveGSS(InputWorkspace=tof_group, Filename=output_paths["gss_filename"], SplitFiles=False, Append=False)
    mantid.SaveNexusProcessed(InputWorkspace=tof_group, Filename=output_paths["nxs_filename"], Append=False)

    dat_folder_name = "dat_files"
    dat_file_destination = os.path.join(output_paths["output_folder"], dat_folder_name)
    if not os.path.exists(dat_file_destination):
        os.makedirs(dat_file_destination)

    _save_xye(ws_group=d_spacing_group, ws_units="d", run_number=run_number_string,
              output_folder=dat_file_destination, inst_prefix=inst_prefix)
    _save_xye(ws_group=tof_group, ws_units="TOF", run_number=run_number_string,
              output_folder=dat_file_destination, inst_prefix=inst_prefix)


def _save_xye(ws_group, ws_units, run_number, output_folder, inst_prefix):
    """
    Saves XYE data into .dat files. This expects the .dat folder to be created and passed.
    It saves the specified group with the specified units into a file whose name contains that
    information.
    :param ws_group: The workspace group to save out to .dat files
    :param ws_units: The units of all workspaces in that group
    :param run_number: The run number string to use when generating the filename
    :param output_folder: The output folder to save these files to
    :param inst_prefix: The prefix of the instrument to generate the filename
    :return:
    """
    prefix_filename = str(inst_prefix) + str(run_number)
    for bank_index, ws in enumerate(ws_group):
        bank_index += 1  # Ensure we start a 1 when saving out
        outfile_name = prefix_filename + "-b_" + str(bank_index) + "-" + ws_units + ".dat"
        full_file_path = os.path.join(output_folder, outfile_name)

        mantid.SaveFocusedXYE(InputWorkspace=ws, Filename=full_file_path, SplitFiles=False, IncludeHeader=False)
