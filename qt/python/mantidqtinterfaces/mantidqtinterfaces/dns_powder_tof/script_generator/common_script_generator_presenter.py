# Mantid Repository : https://github.com/mantidproject/mantid
#
# Copyright &copy; 2021 ISIS Rutherford Appleton Laboratory UKRI,
#     NScD Oak Ridge National Laboratory, European Spallation Source
#     & Institut Laue - Langevin
# SPDX - License - Identifier: GPL - 3.0 +
"""
Common Presenter for DNS Script generators
"""

from mantidqtinterfaces.dns_powder_tof.data_structures.dns_observer import DNSObserver


class DNSScriptGeneratorPresenter(DNSObserver):
    """
    Common Presenter for DNS Script generators
    """

    def __init__(self, name=None, parent=None, view=None, model=None):
        super().__init__(parent=parent, name=name, view=view, model=model)
        # connect statements
        self._data = None
        self._scriptpath = ''
        self._script = None
        self._script_number = 0
        self._scripttext = ''

        # connect signals
        self.view.sig_progress_canceled.connect(self._progress_canceled)
        self.view.sig_generate_script.connect(self._generate_script)

    def _generate_script(self):
        """
        Setting and Running of a Mantid Script generated by script_maker
        """
        if not self._get_sampledata():
            return
        self.request_from_abo()  # pylint: disable=E1102
        # is registered in parameter abo
        # to ask for automatic data reduction
        opt_name = self.name[0:-16] + 'options'
        options = self.param_dict[opt_name]
        paths = self.param_dict['paths']
        fselector = self.param_dict['file_selector']
        script, error = self.model.script_maker(options, paths, fselector)
        self.raise_error(error, critical=True, doraise=error)
        if script != [''] and not error:
            self._scripttext = "\n".join(script)
            self.view.set_script_output(self._scripttext)
            self.view.process_events()
            self._save_script(self._scripttext)
            self.view.open_progress_dialog(len(script) - 1)
            self.view.process_events()
            error = self.model.run_script(script)
            if error:
                self.view.show_statusmessage(error, 30, clear=True)
            else:
                self._script_number += 1
            self._finish_script_run()

    def _finish_script_run(self):
        pass

    def _get_sampledata(self):
        sampledata = self.param_dict['file_selector']['full_data']
        if not sampledata:
            self.raise_error('no data selected', critical=True)
            return False
        return sampledata

    def get_option_dict(self):
        if self.view is not None:
            self.own_dict.update(self.view.get_state())
        self.own_dict['script_path'] = self._scriptpath
        self.own_dict['script_number'] = self._script_number
        self.own_dict['script_text'] = self._scripttext
        return self.own_dict

    def update_progress(self, i, _endi=None):
        self.view.set_progress(i)

    def _progress_canceled(self):
        self.model.cancel_progress()

    def _set_script_filename(self, filename='script.py'):
        own_dict = self.get_option_dict()
        if own_dict['automatic_filename']:
            self.view.set_filename(filename)

    def _save_script(self, script):
        sampledata = self.param_dict['file_selector']['full_data']
        scriptdir = self.param_dict['paths']['script_dir']
        own_options = self.get_option_dict()
        filename = self.model.get_filename(own_options['script_filename'],
                                           sampledata,
                                           own_options['automatic_filename'])
        if scriptdir:
            filename, scriptpath = self.model.save_script(
                script, filename, scriptdir)
            self.view.show_statusmessage(f'script saved to: {scriptpath}',
                                         30,
                                         clear=True)
            self._set_script_filename(filename)
        else:
            self.raise_error('No script filepath set, script will not be '
                             'saved.')
