// Mantid Repository : https://github.com/mantidproject/mantid
//
// Copyright &copy; 2021 ISIS Rutherford Appleton Laboratory UKRI,
//   NScD Oak Ridge National Laboratory, European Spallation Source,
//   Institut Laue - Langevin & CSNS, Institute of High Energy Physics, CAS
// SPDX - License - Identifier: GPL - 3.0 +
#pragma once

#include "Common/DllConfig.h"
#include "GUI/Batch/IBatchView.h"
#include "GUI/Common/IJobManager.h"
#include "IInstViewModel.h"
#include "IPreviewModel.h"
#include "IPreviewPresenter.h"
#include "IPreviewView.h"

#include <memory>

namespace MantidQt::Widgets {
class IRegionSelector;
}

namespace MantidQt::CustomInterfaces::ISISReflectometry {

class MANTIDQT_ISISREFLECTOMETRY_DLL PreviewPresenter : public IPreviewPresenter,
                                                        public PreviewViewSubscriber,
                                                        public JobManagerSubscriber {
public:
  struct Dependencies {
    IPreviewView *view{nullptr};
    std::unique_ptr<IPreviewModel> model;
    std::unique_ptr<IJobManager> jobManager;
    std::unique_ptr<IInstViewModel> instViewModel;
    std::unique_ptr<MantidQt::Widgets::IRegionSelector> regionSelector{nullptr};
  };

  PreviewPresenter(Dependencies dependencies);
  virtual ~PreviewPresenter() = default;

  // PreviewViewSubscriber overrides
  void notifyLoadWorkspaceRequested() override;

  void notifyInstViewZoomRequested() override;
  void notifyInstViewEditRequested() override;
  void notifyInstViewSelectRectRequested() override;
  void notifyInstViewShapeChanged() override;

  void notifyRegionSelectorExportAdsRequested() override;

  void notifyRectangularROIModeRequested() override;

  // JobManagerSubscriber overrides
  void notifyLoadWorkspaceCompleted() override;
  void notifySumBanksCompleted() override;

private:
  IPreviewView *m_view{nullptr};
  std::unique_ptr<IPreviewModel> m_model;
  std::unique_ptr<IJobManager> m_jobManager;
  std::unique_ptr<IInstViewModel> m_instViewModel;
  std::unique_ptr<MantidQt::Widgets::IRegionSelector> m_regionSelector;
};
} // namespace MantidQt::CustomInterfaces::ISISReflectometry
