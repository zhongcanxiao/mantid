// Mantid Repository : https://github.com/mantidproject/mantid
//
// Copyright &copy; 2021 ISIS Rutherford Appleton Laboratory UKRI,
//   NScD Oak Ridge National Laboratory, European Spallation Source,
//   Institut Laue - Langevin & CSNS, Institute of High Energy Physics, CAS
// SPDX - License - Identifier: GPL - 3.0 +

#include "PreviewModel.h"
#include "GUI/Common/IJobManager.h"
#include "MantidAPI/AnalysisDataService.h"
#include "MantidAPI/MatrixWorkspace.h"
#include "MantidAPI/Run.h"
#include "MantidGeometry/IDTypes.h"
#include "MantidGeometry/Instrument/DetectorInfo.h"
#include "MantidKernel/Logger.h"
#include "MantidKernel/Strings.h"
#include "MantidKernel/TimeSeriesProperty.h"
#include "MantidKernel/Tolerance.h"

#include <boost/utility/in_place_factory.hpp>

#include <optional>
#include <string>

using namespace Mantid::API;
using namespace Mantid::Kernel;

namespace {
Mantid::Kernel::Logger g_log("Reflectometry Preview Model");
}

namespace MantidQt::CustomInterfaces::ISISReflectometry {

PreviewModel::PreviewModel() {
  // This simplifies testing greatly, as it ensures that m_runDetails is never null
  createRunDetails("");
}

/** Set the loaded workspace from the ADS if it exists
 *
 * @param workspaceName : the workspace name to look for
 * @returns : true if the loaded workspace was set, false if it was not found in the ADS
 * @throws : if the workspace exists in the ADS but is an unexpected type
 */
bool PreviewModel::loadWorkspaceFromAds(std::string const &workspaceName) {
  auto &adsInstance = AnalysisDataService::Instance();
  if (!adsInstance.doesExist(workspaceName)) {
    return false;
  }
  auto ws = adsInstance.retrieveWS<MatrixWorkspace>(workspaceName);
  if (!ws) {
    throw std::runtime_error("Unsupported workspace type; expected MatrixWorkspace");
  }

  createRunDetails(workspaceName);
  m_runDetails->setLoadedWs(ws);
  return true;
}

/** Load a workspace and perform standard ISIS reflectometry preprocessing on it.
 * This is done asynchronously. The caller should subscribe to the job manager to
 * get a callback when loading is finished.
 *
 * @param workspaceName : the workspace name to load
 * @param jobManager : the job manager that will perform the loading
 */
void PreviewModel::loadAndPreprocessWorkspaceAsync(std::string const &workspaceName, IJobManager &jobManager) {
  createRunDetails(workspaceName);
  // Note that the run details are automatically updated with the resulting workspace (as we pass by-ref) on completion
  jobManager.startPreprocessing(*m_runDetails);
}

/** Sum spectra across banks
 *
 * @param wsIndices : the workspace indices of the spectra to sum
 * @param jobManager : the job manager that will execute the algorithm
 */
void PreviewModel::sumBanksAsync(IJobManager &jobManager) { jobManager.startSumBanks(*m_runDetails); }

void PreviewModel::reduceAsync(IJobManager &jobManager) { jobManager.startReduction(*m_runDetails); }

MatrixWorkspace_sptr PreviewModel::getLoadedWs() const { return m_runDetails->getLoadedWs(); }
MatrixWorkspace_sptr PreviewModel::getSummedWs() const { return m_runDetails->getSummedWs(); }
MatrixWorkspace_sptr PreviewModel::getReducedWs() const { return m_runDetails->getReducedWs(); }

std::optional<double> PreviewModel::getDefaultTheta() const {
  auto theta = getThetaFromLogs("Theta");
  if (theta && *theta > Tolerance) {
    return theta;
  }
  return std::nullopt;
}

std::vector<Mantid::detid_t> PreviewModel::getSelectedBanks() const { return m_runDetails->getSelectedBanks(); }

void PreviewModel::setLoadedWs(Mantid::API::MatrixWorkspace_sptr workspace) { m_runDetails->setLoadedWs(workspace); }

void PreviewModel::setTheta(double theta) { m_runDetails->setTheta(theta); }
void PreviewModel::setSelectedBanks(std::vector<Mantid::detid_t> selectedBanks) {
  m_runDetails->setSelectedBanks(std::move(selectedBanks));
}

ProcessingInstructions PreviewModel::getProcessingInstructions() const {
  return m_runDetails->getProcessingInstructions();
}

void PreviewModel::setSelectedRegion(Selection const &selection) {
  // TODO We will need to allow for more complex selections, but for now the selection just consists two y indices
  if (selection.size() != 2) {
    throw std::runtime_error("Program error: unexpected selection size; expected 2, got " +
                             std::to_string(selection.size()));
  }
  // For now we just support a y axis of spectrum number so round to the nearest integer
  auto const start = static_cast<int>(std::round(selection[0]));
  auto const end = static_cast<int>(std::round(selection[1]));
  auto processingInstructions = ProcessingInstructions(std::to_string(start) + "-" + std::to_string(end));
  m_runDetails->setProcessingInstructions(std::move(processingInstructions));
}

void PreviewModel::createRunDetails(const std::string &workspaceName) {
  m_runDetails = std::make_unique<PreviewRow>(std::vector<std::string>{workspaceName});
}

void PreviewModel::exportSummedWsToAds() const {
  if (auto summedWs = m_runDetails->getSummedWs()) {
    AnalysisDataService::Instance().addOrReplace("preview_summed_ws", summedWs);
  } else {
    g_log.error("Could not export summed WS. No rectangular selection has been made on the instrument viewer.");
  }
}

void PreviewModel::exportReducedWsToAds() const {
  if (auto reducedWs = m_runDetails->getReducedWs()) {
    AnalysisDataService::Instance().addOrReplace("preview_reduced_ws", reducedWs);
  } else {
    g_log.error(
        "Could not export reduced WS. No selection has been made on the instrument viewer and/or region selector.");
  }
}

std::optional<double> PreviewModel::getThetaFromLogs(const std::string &logName) const {
  const Mantid::API::Run &run = getLoadedWs()->run();
  if (!run.hasProperty(logName)) {
    return std::nullopt;
  }
  Property *logData = run.getLogData(logName);
  auto logPWV = dynamic_cast<const PropertyWithValue<double> *>(logData);
  auto logTSP = dynamic_cast<const TimeSeriesProperty<double> *>(logData);

  if (logPWV) {
    return *logPWV;
  } else if (logTSP && logTSP->realSize() > 0) {
    return logTSP->lastValue();
  }
  return std::nullopt;
}

} // namespace MantidQt::CustomInterfaces::ISISReflectometry
