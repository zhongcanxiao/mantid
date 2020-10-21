// Mantid Repository : https://github.com/mantidproject/mantid
//
// Copyright &copy; 2014 ISIS Rutherford Appleton Laboratory UKRI,
//   NScD Oak Ridge National Laboratory, European Spallation Source,
//   Institut Laue - Langevin & CSNS, Institute of High Energy Physics, CAS
// SPDX - License - Identifier: GPL - 3.0 +
#pragma once

#include "MantidKernel/System.h"

#include "DllConfig.h"
#include "IALCDataLoadingView.h"

#include "ui_ALCDataLoadingView.h"

namespace MantidQt {
namespace MantidWidgets {
class LogValueSelector;
} // namespace MantidWidgets
} // namespace MantidQt

namespace MantidQt {
namespace CustomInterfaces {

/** ALCDataLoadingView : ALC Data Loading view interface implementation using Qt
  widgets
*/

class MANTIDQT_MUONINTERFACE_DLL ALCDataLoadingView
    : public IALCDataLoadingView {
public:
  ALCDataLoadingView(QWidget *widget);
  ~ALCDataLoadingView();

  // -- IALCDataLoadingView interface
  // ------------------------------------------------------------

  void initialize() override;

  void initInstruments() override;
  std::string getInstrument() const override;
  std::string getPath() const override;
  std::string getRunsExpression() const override;
  std::string log() const override;
  std::string function() const override;
  std::string deadTimeType() const override;
  std::string deadTimeFile() const override;
  std::string detectorGroupingType() const override;
  std::string getForwardGrouping() const override;
  std::string getBackwardGrouping() const override;
  std::string redPeriod() const override;
  std::string greenPeriod() const override;
  bool subtractIsChecked() const override;
  std::string calculationType() const override;
  boost::optional<std::pair<double, double>> timeRange() const override;

  void setDataCurve(Mantid::API::MatrixWorkspace_sptr workspace,
                    std::size_t const &workspaceIndex = 0) override;
  void displayError(const std::string &error) override;
  bool displayWarning(const std::string &warning) override;
  void setAvailableLogs(const std::vector<std::string> &logs) override;
  void setAvailablePeriods(const std::vector<std::string> &periods) override;
  void setTimeLimits(double tMin, double tMax) override;
  void setTimeRange(double tMin, double tMax) override;
  void help() override;
  void disableAll() override;
  void enableAll() override;
  void instrumentChanged(QString instrument) override;
  void pathChanged(QString path) override;
  void handleRunsEditingFinsihed() override;
  void enableLoad(bool enable) override;

  // -- End of IALCDataLoadingView interface
  // -----------------------------------------------------

private:
  /// Common function to set available items in a combo box
  void setAvailableItems(QComboBox *comboBox,
                         const std::vector<std::string> &items);

  /// UI form
  Ui::ALCDataLoadingView m_ui;

  /// The widget used
  QWidget *const m_widget;
};

} // namespace CustomInterfaces
} // namespace MantidQt
