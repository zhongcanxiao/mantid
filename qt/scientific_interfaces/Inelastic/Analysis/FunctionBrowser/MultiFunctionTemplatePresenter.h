// Mantid Repository : https://github.com/mantidproject/mantid
//
// Copyright &copy; 2024 ISIS Rutherford Appleton Laboratory UKRI,
//   NScD Oak Ridge National Laboratory, European Spallation Source,
//   Institut Laue - Langevin & CSNS, Institute of High Energy Physics, CAS
// SPDX - License - Identifier: GPL - 3.0 +
#pragma once

#include "DllConfig.h"
#include "FunctionTemplatePresenter.h"

namespace MantidQt {
namespace CustomInterfaces {
namespace IDA {

class ConvFunctionTemplateModel;
class MultiFunctionTemplateView;

class MANTIDQT_INELASTIC_DLL MultiFunctionTemplatePresenter : public FunctionTemplatePresenter {
public:
  explicit MultiFunctionTemplatePresenter(MultiFunctionTemplateView *view,
                                          std::unique_ptr<ConvFunctionTemplateModel> model);

  MultiFunctionTemplateView *view() const;
  ConvFunctionTemplateModel *model() const;

  void setSubType(size_t subTypeIndex, int typeIndex) override;

  void setFunction(std::string const &funStr) override;

  EstimationDataSelector getEstimationDataSelector() const override;
  void updateParameterEstimationData(DataForParameterEstimationCollection &&data) override;
  void estimateFunctionParameters() override;

protected:
  void updateView() override;

private:
  void updateViewParameters();
  void updateViewParameterNames();
};

} // namespace IDA
} // namespace CustomInterfaces
} // namespace MantidQt
