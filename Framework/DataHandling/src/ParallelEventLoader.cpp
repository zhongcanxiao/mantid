#include "MantidDataHandling/ParallelEventLoader.h"
#include "MantidDataObjects/EventWorkspace.h"
#include "MantidGeometry/Instrument.h"
#include "MantidGeometry/Instrument/ComponentInfo.h"
#include "MantidGeometry/Instrument/DetectorInfo.h"
#include "MantidParallel/IO/EventLoader.h"
#include "MantidTypes/Event/TofEvent.h"

namespace Mantid {
namespace DataHandling {

std::vector<int32_t> bankOffsets(const API::ExperimentInfo &ws,
                                 const std::vector<std::string> &bankNames) {
  const auto instrument = ws.getInstrument();
  const auto &compInfo = ws.componentInfo();
  const auto &detInfo = ws.detectorInfo();
  const auto &detIDs = detInfo.detectorIDs();

  // Monitors are not loaded by LoadEventNexus, so we have to exclude them when
  // computing an offset based on detector IDs. Currently this is computed in a
  // naive way and works only if all monitors have IDs smaller than any
  // detector.
  int32_t monitorOffset{0};
  bool sawDetector{false};
  for (size_t i = 0; i < detInfo.size(); ++i) {
    if (detInfo.isMonitor(i)) {
      if (sawDetector)
        throw std::runtime_error("Monitors are not corresponding to the first "
                                 "detector IDs in the instrument. This is "
                                 "currently not supported by "
                                 "ParallelEventLoader");
      ++monitorOffset;
    } else {
      sawDetector = true;
    }
  }

  std::vector<int32_t> bankOffsets;
  for (const auto &bankName : bankNames) {
    // Removing "_events" from bankName
    auto bank =
        instrument->getComponentByName(bankName.substr(0, bankName.size() - 7));
    if (bank) {
      const auto &detectors =
          compInfo.detectorsInSubtree(compInfo.indexOf(bank->getComponentID()));
      const size_t detIndex = detectors.front();
      bankOffsets.push_back(detIDs[detIndex] - static_cast<int32_t>(detIndex) +
                            monitorOffset);
      if ((detIDs[detectors.back()] - detIDs[detectors.front()]) !=
          static_cast<int32_t>(detectors.size()) - 1)
        throw std::runtime_error("Detector ID range in bank is not contiguous. "
                                 "Cannot use ParallelEventLoader.");
    } else {
      throw std::runtime_error(
          "ParallelEventLoader: Bank " + bankName +
          " not found. Cannot determine detector ID offset.");
    }
  }
  return bankOffsets;
}

void ParallelEventLoader::load(DataObjects::EventWorkspace &ws,
                               const std::string &filename,
                               const std::string &groupName,
                               const std::vector<std::string> &bankNames) {
  const size_t size = ws.getNumberHistograms();
  std::vector<std::vector<Types::Event::TofEvent> *> eventLists(size, nullptr);
  for (size_t i = 0; i < size; ++i)
    DataObjects::getEventsFrom(ws.getSpectrum(i), eventLists[i]);

  Parallel::IO::EventLoader::load(filename, groupName, bankNames,
                                  bankOffsets(ws, bankNames),
                                  std::move(eventLists));
}

} // namespace DataHandling
} // namespace Mantid
