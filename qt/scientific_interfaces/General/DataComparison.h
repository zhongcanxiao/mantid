// Mantid Repository : https://github.com/mantidproject/mantid
//
// Copyright &copy; 2018 ISIS Rutherford Appleton Laboratory UKRI,
//   NScD Oak Ridge National Laboratory, European Spallation Source,
//   Institut Laue - Langevin & CSNS, Institute of High Energy Physics, CAS
// SPDX - License - Identifier: GPL - 3.0 +
#pragma once

//----------------------
// Includes
//----------------------
#include "MantidAPI/MatrixWorkspace_fwd.h"
#include "MantidQtWidgets/Common/UserSubWindow.h"
#include "MantidQtWidgets/Common/WorkspaceObserver.h"
#include "ui_DataComparison.h"

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_magnifier.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_zoomer.h>

namespace MantidQt {
namespace CustomInterfaces {
class DataComparison : public MantidQt::API::UserSubWindow,
                       public MantidQt::API::WorkspaceObserver {
  Q_OBJECT

public:
  /// The name of the interface as registered into the factory
  static std::string name() { return "Data Comparison"; }
  // This interface's categories.
  static QString categoryInfo() { return "General"; }

public:
  /// Default Constructor
  explicit DataComparison(QWidget *parent = nullptr);

  /// Tests if a workspace is shown in the UI
  bool containsWorkspace(Mantid::API::MatrixWorkspace_const_sptr ws);

private slots:
  /// Add selected data to plot
  void addData();
  /// Remove selected data from plot
  void removeSelectedData();
  /// Remove all data from plot
  void removeAllData();
  /// Create a diff of the two selected workspaces
  void diffSelected();
  /// Remove the diff from the plot
  void clearDiff();
  /// Handles replotting workspace spectra
  void plotWorkspaces();
  /// Handles updating the plot
  void updatePlot();
  /// Handles updating th eplot after a spectum index is changed
  void workspaceIndexChanged();
  /// Handles creating and plotting a diff worksapce
  void plotDiffWorkspace();
  /// Toggle the pan plot tool
  void togglePan(bool enabled);
  /// Toggle the zoom plot tool
  void toggleZoom(bool enabled);
  /// Resets the zoom level to show all curves
  void resetView();

private:
  /// Enumeration for column index
  enum Column { COLOUR, WORKSPACE_NAME, SPEC_OFFSET, CURRENT_SPEC };

  /// Initialize the layout
  void initLayout() override;
  /// Adds a workspace to the data table
  void addDataItem(Mantid::API::Workspace_const_sptr ws);
  /// Normalises spectra offsets in table
  void normaliseSpectraOffsets();
  /// Gets an initial curve colour for a new workspace
  int getInitialColourIndex();

private:
  // Handlers for ADS events
  void
  preDeleteHandle(const std::string &wsName,
                  const boost::shared_ptr<Mantid::API::Workspace> ws) override;
  void renameHandle(const std::string &oldName,
                    const std::string &newName) override;
  void afterReplaceHandle(
      const std::string &wsName,
      const boost::shared_ptr<Mantid::API::Workspace> ws) override;

  // The form generated by Qt Designer
  Ui::DataComparison m_uiForm;

  // The plot object
  QwtPlot *m_plot;
  // Curves shown on plot, indexed by workspace name
  QMap<QString, boost::shared_ptr<QwtPlotCurve>> m_curves;

  // Plot zoom tool
  QwtPlotZoomer *m_zoomTool;
  // Plot pan tool
  QwtPlotPanner *m_panTool;
  // Plot magnify tool
  QwtPlotMagnifier *m_magnifyTool;

  boost::shared_ptr<QwtPlotCurve> m_diffCurve;
  // The two workspaces that are currently being diffed
  QPair<QString, QString> m_diffWorkspaceNames;
};
} // namespace CustomInterfaces
} // namespace MantidQt
