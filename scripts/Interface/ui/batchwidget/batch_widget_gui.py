from __future__ import (absolute_import, division, print_function)
try:
    from mantidplot import *
    canMantidPlot = True
except ImportError:
    canMantidPlot = False

from PyQt4 import QtGui
from mantidqtpython import MantidQt
from ui.batchwidget.ui_batch_widget_window import Ui_BatchWidgetWindow


def row(path):
    return MantidQt.MantidWidgets.Batch.RowLocation(path)


class DataProcessorGui(QtGui.QMainWindow, Ui_BatchWidgetWindow):
    def __init__(self):
        super(QtGui.QMainWindow, self).__init__()
        self.setupUi(self)
        self.clipboard = None

    def on_remove_runs_request(self, runs_to_remove):
        self.table.removeRows(runs_to_remove)

    def on_cell_updated(self, row, col, cell_content):
        print("Updated row {} col {} with text {}".format(row.path(), col, cell_content))
        pass

    def on_row_inserted(self, rowLoc):
        print("Row inserted at {}".format(rowLoc.path()))
        #if rowLoc.depth() > 2 or rowLoc.rowRelativeToParent() >= 5:
        #    self.table.removeRowAt(rowLoc)

    def on_copy_runs_request(self):
        self.clipboard = self.table.selectedSubtrees()
        self.table.clearSelection()
        if self.clipboard is not None:
            print(self.clipboard)
        else:
            print ("Bad selection for copy.")

    def on_paste_rows_request(self):
        replacement_roots = self.table.selectedSubtreeRoots()
        if replacement_roots is not None and self.clipboard is not None:
            if replacement_roots:
                self.table.replaceRows(replacement_roots, self.clipboard)
            else:
                self.table.appendSubtreesAt(row([]), self.clipboard)
        else:
            print("Bad selection for paste")

    def setup_layout(self):
        self.table = MantidQt.MantidWidgets.Batch.JobTreeView(["Run(s)",
                                                               "Angle",
                                                               "Transmission Run(s)",
                                                               "Q min",
                                                               "Q max",
                                                               "dQ/Q",
                                                               "Scale",
                                                               "Options"], self)
        self.table_signals = MantidQt.MantidWidgets.Batch.JobTreeViewSignalAdapter(self.table)

        self.table_signals.removeRowsRequested.connect(self.on_remove_runs_request)
        self.table_signals.copyRowsRequested.connect(self.on_copy_runs_request)
        self.table_signals.pasteRowsRequested.connect(self.on_paste_rows_request)
        self.table_signals.cellChanged.connect(self.on_cell_updated)
        self.table_signals.rowInserted.connect(self.on_row_inserted)

        self.table.appendChildRowOf(row([]))
        self.table.appendChildRowOf(row([]))

        # Add the widget to this interface
        self.layoutBase.addWidget(self.table)
        return True
