// Mantid Repository : https://github.com/mantidproject/mantid
//
// Copyright &copy; 2019 ISIS Rutherford Appleton Laboratory UKRI,
//     NScD Oak Ridge National Laboratory, European Spallation Source
//     & Institut Laue - Langevin
// SPDX - License - Identifier: GPL - 3.0 +

#include "MantidDataHandling/LoadNGEM.h"
#include "MantidAPI/Axis.h"
#include "MantidAPI/MultipleFileProperty.h"
#include "MantidAPI/RegisterFileLoader.h"
#include "MantidDataObjects/Workspace2D.h"
#include "MantidDataObjects/WorkspaceCreation.h"
#include "MantidKernel/BinaryStreamReader.h"
#include "MantidKernel/BoundedValidator.h"
#include "MantidKernel/OptionalBool.h"
#include "MantidKernel/Unit.h"
#include "MantidKernel/UnitFactory.h"

#include <boost/algorithm/string.hpp>
#include <fstream>
#include <sys/stat.h>

namespace Mantid {
namespace DataHandling {

DECLARE_FILELOADER_ALGORITHM(LoadNGEM)

namespace {
constexpr int NUM_OF_SPECTRA = 16384;

/**
 * @brief Byte swap a 64 bit word as the nGEM detector outputs in big endian
 * format. So must be swapped to have correct values on x86 and x64
 * architectures.
 *
 * @param word The 64 bit word to swap.
 * @return uint64_t The swapped word.
 */
uint64_t swapUint64(uint64_t word) {
  word = ((word << 8) & 0xFF00FF00FF00FF00ULL) |
         ((word >> 8) & 0x00FF00FF00FF00FFULL);
  word = ((word << 16) & 0xFFFF0000FFFF0000ULL) |
         ((word >> 16) & 0x0000FFFF0000FFFFULL);
  return (word << 32) | (word >> 32);
}
} // namespace

/**
 * @brief The confidence that a file can be loaded.
 *
 * @param descriptor A description of the file.
 * @return int The level of certainty that the file can be loaded with this
 * loader.
 */
int LoadNGEM::confidence(Kernel::FileDescriptor &descriptor) const {
  if (descriptor.extension() == ".edb") {
    return 100;
  } else {
    return 0;
  }
}

/**
 * @brief Initialisation of the algorithm, setting up the properties.
 */
void LoadNGEM::init() {
  // Filename property.
  const std::vector<std::string> extentions{".edb"};
  declareProperty(
      std::make_unique<Mantid::API::MultipleFileProperty>("Filename",
                                                          extentions),
      "The name of the nGEM file to load. Selecting multiple files will "
      "combine them into one workspace.");
  // Output workspace
  declareProperty(
      std::make_unique<Mantid::API::WorkspaceProperty<Mantid::API::Workspace>>(
          "OutputWorkspace", "", Kernel::Direction::Output),
      "The output workspace");

  auto mustBePositive = boost::make_shared<Kernel::BoundedValidator<int>>();
  mustBePositive->setLower(0);

  auto mustBePositiveDbl =
      boost::make_shared<Kernel::BoundedValidator<double>>();
  mustBePositiveDbl->setLower(0.0);

  // Bin Width
  declareProperty("BinWidth", 10.0, mustBePositiveDbl,
                  "The width of the time bins in the output.");

  declareProperty("MinEventsPerFrame", 0, mustBePositive,
                  "The minimum number of events required in a frame before a "
                  "it is considered 'good'.");
  declareProperty("MaxEventsPerFrame", Mantid::EMPTY_INT(), mustBePositive,
                  "The maximum number of events allowed in a frame to be "
                  "considered 'good'.");
  declareProperty(
      std::make_unique<Mantid::Kernel::PropertyWithValue<bool>>(
          "GenerateEventsPerFrame", false, Mantid::Kernel::Direction::Input),
      "Generate a workspace to show the number of events captured by each "
      "frame. (optional, default False).");
}

/**
 * @brief Execute the algorithm.
 */
void LoadNGEM::exec() {
  progress(0);

  std::vector<std::vector<std::string>> filePaths = getProperty("Filename");

  const int minEventsReq(getProperty("MinEventsPerFrame"));
  const int maxEventsReq(getProperty("MaxEventsPerFrame"));

  int maxToF = -1;
  int minToF = 2147483647;
  const double binWidth(getProperty("BinWidth"));

  int rawFrames = 0;
  int goodFrames = 0;
  std::vector<double> frameEventCounts;
  int eventCountInFrame = 0;

  std::vector<DataObjects::EventList> histograms, histogramsInFrame;
  histograms.resize(NUM_OF_SPECTRA);
  histogramsInFrame.resize(NUM_OF_SPECTRA);
  progress(0.04);

  size_t totalFilePaths(filePaths.size());
  int counter(1);
  for (const auto &filePath : filePaths) {
    loadSingleFile(filePath, eventCountInFrame, maxToF, minToF, rawFrames,
                   goodFrames, minEventsReq, maxEventsReq, frameEventCounts,
                   histograms, histogramsInFrame, totalFilePaths, counter);
  }
  // Add the final frame of events (as they are not followed by a T0 event)
  addFrameToOutputWorkspace(rawFrames, goodFrames, eventCountInFrame,
                            minEventsReq, maxEventsReq, frameEventCounts,
                            histograms, histogramsInFrame);
  progress(0.90);

  DataObjects::EventWorkspace_sptr dataWorkspace;
  createEventWorkspace(maxToF, binWidth, histograms, dataWorkspace);

  addToSampleLog("raw_frames", rawFrames, dataWorkspace);
  addToSampleLog("good_frames", goodFrames, dataWorkspace);
  addToSampleLog("max_ToF", maxToF, dataWorkspace);
  addToSampleLog("min_ToF", minToF, dataWorkspace);

  loadInstrument(dataWorkspace);

  setProperty("OutputWorkspace", dataWorkspace);
  if (this->getProperty("GenerateEventsPerFrame")) {
    createCountWorkspace(frameEventCounts);
  }
  progress(1.00);
}

/**
 * @brief Load a single file into the histograms.
 *
 * @param filePath The path to the file.
 * @param eventCountInFrame The number of events in the current frame.
 * @param maxToF The highest detected ToF
 * @param minToF The lowest detected ToF
 * @param rawFrames The number of T0 Events detected so far.
 * @param goodFrames The number of good frames detected so far.
 * @param minEventsReq The number of events required to be a good frame.
 * @param maxEventsReq The max events allowed to be a good frame.
 * @param frameEventCounts A vector of the number of events in each good frame.
 * @param histograms The main set of histograms for the data so far.
 * @param histogramsInFrame The set of histograms for the current frame.
 * @param totalFilePaths The total number of file paths.
 * @param fileCount The number of file paths processed.
 */
void LoadNGEM::loadSingleFile(
    const std::vector<std::string> &filePath, int &eventCountInFrame,
    int &maxToF, int &minToF, int &rawFrames, int &goodFrames,
    const int &minEventsReq, const int &maxEventsReq,
    MantidVec &frameEventCounts,
    std::vector<DataObjects::EventList> &histograms,
    std::vector<DataObjects::EventList> &histogramsInFrame,
    const size_t &totalFilePaths, int &fileCount) {
  // Create file reader
  if (filePath.size() > 1) {
    throw std::runtime_error("Invalid filename parameter.");
  }
  std::ifstream file(filePath[0].c_str(), std::ifstream::binary);
  if (!file.is_open()) {
    throw std::runtime_error("File could not be found.");
  }
  std::array<char, 16> buffer;

  const size_t totalNumEvents = verifyFileSize(file) / 16;
  size_t numProcessedEvents = 0;

  while (!file.eof()) {
    // Load an event into the variable.
    file.read(buffer.data(), 16);
    auto eventBigEndian = reinterpret_cast<EventUnion *>(buffer.data());

    // Correct for the big endian format.
    EventUnion event;
    correctForBigEndian(eventBigEndian, event);

    if (event.coincidence.check()) { // Check for coincidence event.
      ++eventCountInFrame;
      uint64_t pixel = event.coincidence.getPixel();
      int tof =
          event.coincidence.timeOfFlight / 1000; // Convert to microseconds (us)

      if (tof > maxToF) {
        maxToF = tof;
      } else if (tof < minToF) {
        minToF = tof;
      }
      histogramsInFrame[pixel].addEventQuickly(
          Mantid::Types::Event::TofEvent(tof));

    } else if (event.tZero.check()) { // Check for T0 event.
      addFrameToOutputWorkspace(rawFrames, goodFrames, eventCountInFrame,
                                minEventsReq, maxEventsReq, frameEventCounts,
                                histograms, histogramsInFrame);

      if (reportProgressAndCheckCancel(numProcessedEvents, eventCountInFrame,
                                       totalNumEvents, totalFilePaths,
                                       fileCount)) {
        return;
      }
    } else { // Catch all other events and notify.
      g_log.warning() << "Unexpected event type loaded.\n";
    }
  }
  g_log.information() << "Finished loading a file.\n";
  ++fileCount;
}

/**
 * @brief Correct a big endian event to be compatible with x64 and x86
 * architecture.
 *
 * @param bigEndian The big endian formatted event.
 * @param smallEndian The resulting small endian formatted event.
 */
void LoadNGEM::correctForBigEndian(EventUnion *&bigEndian,
                                   EventUnion &smallEndian) {
  smallEndian.splitWord.words[0] = swapUint64(bigEndian->splitWord.words[1]);
  smallEndian.splitWord.words[1] = swapUint64(bigEndian->splitWord.words[0]);
}

/**
 * @brief Add a string value to the sample logs.
 *
 * @param logName The name of the log.
 * @param logText The content of the log.
 * @param ws The workspace to add the log to.
 */
void LoadNGEM::addToSampleLog(const std::string &logName,
                              const std::string &logText,
                              DataObjects::EventWorkspace_sptr &ws) {
  Mantid::API::Algorithm_sptr sampLogAlg = createChildAlgorithm("AddSampleLog");
  sampLogAlg->setProperty("Workspace", ws);
  sampLogAlg->setProperty("LogType", "String");
  sampLogAlg->setProperty("LogName", logName);
  sampLogAlg->setProperty("LogText", logText);
  sampLogAlg->executeAsChildAlg();
}

/**
 * @brief Add a number value to the sample logs.
 *
 * @param logName Name of the log.
 * @param logNumber The value of the log.
 * @param ws The workspace to add the log to.
 */
void LoadNGEM::addToSampleLog(const std::string &logName, const int &logNumber,
                              DataObjects::EventWorkspace_sptr &ws) {
  Mantid::API::Algorithm_sptr sampLogAlg = createChildAlgorithm("AddSampleLog");
  sampLogAlg->setProperty("Workspace", ws);
  sampLogAlg->setProperty("LogType", "Number");
  sampLogAlg->setProperty("LogName", logName);
  sampLogAlg->setProperty("LogText", std::to_string(logNumber));
  sampLogAlg->executeAsChildAlg();
}

/**
 * @brief Ensure that the file fits into 16, as the detector spits out 128 bit
 * words (16 bytes)
 *
 * @param file The file to check.
 * @return size_t The size of the file.
 */
size_t LoadNGEM::verifyFileSize(std::ifstream &file) {
  // Check that the file fits into 16 byte sections.
  file.seekg(0, file.end);
  size_t size = file.tellg();
  if (size % 16 != 0) {
    g_log.warning()
        << "Invalid file size. File is size is " << size
        << " bytes which is not a multiple of 16. There may be some bytes "
           "missing from the data. \n";
  }
  file.seekg(0);
  return size;
}

/**
 * @brief Add a frame to the main set of histograms.
 *
 * @param rawFrames The number of T0 Events detected so far.
 * @param goodFrames The number of good frames detected so far.
 * @param eventCountInFrame The number of events in the current frame.
 * @param minEventsReq The number of events required to be a good frame.
 * @param maxEventsReq The max events allowed to be a good frame.
 * @param frameEventCounts A vector of the number of events in each good frame.
 * @param histograms The main set of histograms for the data so far.
 * @param histogramsInFrame The set of histograms for the current frame.
 */
void LoadNGEM::addFrameToOutputWorkspace(
    int &rawFrames, int &goodFrames, const int &eventCountInFrame,
    const int &minEventsReq, const int &maxEventsReq,
    MantidVec &frameEventCounts,
    std::vector<DataObjects::EventList> &histograms,
    std::vector<DataObjects::EventList> &histogramsInFrame) {
  ++rawFrames;
  if (eventCountInFrame >= minEventsReq && eventCountInFrame <= maxEventsReq) {
    // Add number of event counts to workspace.
    frameEventCounts.emplace_back(eventCountInFrame);
    ++goodFrames;

#pragma omp parallel for
    // Add histograms that match parameters to workspace
    for (auto i = 0; i < NUM_OF_SPECTRA; ++i) {
      if (histogramsInFrame[i].getNumberEvents() > 0) {
        histograms[i] += histogramsInFrame[i];
        histogramsInFrame[i].clear();
      }
    }
  }
}

/**
 * @brief Report the progress of the loader through the current file.
 *
 * @param numProcessedEvents The number of events processed so far.
 * @param eventCountInFrame The number of events in the current frame.
 * @param totalNumEvents The total number of events in the file.
 * @param totalFilePaths The total number of file paths to process.
 * @param fileCount The number of files processed.
 * @return true If user has cancelled the load.
 * @return false If the user has not cancelled.
 */
bool LoadNGEM::reportProgressAndCheckCancel(size_t &numProcessedEvents,
                                            int &eventCountInFrame,
                                            const size_t &totalNumEvents,
                                            const size_t &totalFilePaths,
                                            const int &fileCount) {
  numProcessedEvents += eventCountInFrame;
  std::string message(std::to_string(fileCount) + "/" +
                      std::to_string(totalFilePaths));
  progress(double(numProcessedEvents) / double(totalNumEvents) / 1.11111,
           message);
  eventCountInFrame = 0;
  // Check for cancel flag.
  return this->getCancel();
}

/**
 * @brief Creates an event workspace and fills it with the data.
 *
 * @param maxToF The largest ToF seen so far.
 * @param binWidth The width of each bin.
 * @param histograms The main histogram event data.
 * @param dataWorkspace The workspace to add the data to.
 */
void LoadNGEM::createEventWorkspace(
    const int &maxToF, const double &binWidth,
    std::vector<DataObjects::EventList> &histograms,
    DataObjects::EventWorkspace_sptr &dataWorkspace) {
  std::vector<double> xAxis;
  for (auto i = 0; i < (maxToF / binWidth); i++) {
    xAxis.push_back(i * binWidth);
  }

  dataWorkspace = DataObjects::create<DataObjects::EventWorkspace>(
      NUM_OF_SPECTRA,
      Mantid::HistogramData::Histogram(Mantid::HistogramData::BinEdges(xAxis)));
#pragma omp parallel for
  for (auto spectrumNo = 0u; spectrumNo < histograms.size(); ++spectrumNo) {
    dataWorkspace->getSpectrum(spectrumNo) = histograms[spectrumNo];
    dataWorkspace->getSpectrum(spectrumNo).setSpectrumNo(spectrumNo + 1);
    dataWorkspace->getSpectrum(spectrumNo).setDetectorID(spectrumNo + 1);
  }
  dataWorkspace->setAllX(HistogramData::BinEdges{xAxis});
  dataWorkspace->getAxis(0)->unit() =
      Kernel::UnitFactory::Instance().create("TOF");
  dataWorkspace->setYUnit("Counts");
}

/**
 * @brief Create a count workspace to allow for the selection of "good"
 * frames.
 *
 * @param frameEventCounts A Vector of the number of events per frame.
 */
void LoadNGEM::createCountWorkspace(
    const std::vector<double> &frameEventCounts) {
  std::vector<double> xAxisCounts;
  for (auto i = 0u; i <= frameEventCounts.size(); i++) {
    xAxisCounts.push_back(i);
  }

  DataObjects::Workspace2D_sptr countsWorkspace =
      DataObjects::create<DataObjects::Workspace2D>(
          1, Mantid::HistogramData::Histogram(
                 Mantid::HistogramData::BinEdges(xAxisCounts)));

  countsWorkspace->mutableY(0) = frameEventCounts;
  std::string countsWorkspaceName(this->getProperty("OutputWorkspace"));
  countsWorkspaceName.append("_event_counts");
  countsWorkspace->setYUnit("Counts");
  boost::shared_ptr<Kernel::Units::Label> XLabel =
      boost::dynamic_pointer_cast<Kernel::Units::Label>(
          Kernel::UnitFactory::Instance().create("Label"));
  XLabel->setLabel("Frame");
  countsWorkspace->getAxis(0)->unit() = XLabel;

  this->declareProperty(
      std::make_unique<Mantid::API::WorkspaceProperty<Mantid::API::Workspace>>(
          "CountsWorkspace", countsWorkspaceName, Kernel::Direction::Output),
      "Counts of events per frame.");
  progress(1.00);
  this->setProperty("CountsWorkspace", countsWorkspace);
}

/**
 * @brief Load the nGEM instrument into a workspace.
 *
 * @param dataWorkspace The workspace to load into.
 */
void LoadNGEM::loadInstrument(DataObjects::EventWorkspace_sptr &dataWorkspace) {
  auto loadInstrument = this->createChildAlgorithm("LoadInstrument");
  loadInstrument->setPropertyValue("InstrumentName", "NGEM");
  loadInstrument->setProperty<Mantid::API::MatrixWorkspace_sptr>("Workspace",
                                                                 dataWorkspace);
  loadInstrument->setProperty("RewriteSpectraMap",
                              Mantid::Kernel::OptionalBool(false));
  loadInstrument->execute();
}

/**
 * @brief Validate inputs into the loader GUI.
 *
 * @return std::map<std::string, std::string> A map that is empty if there are
 * no issues, and contains a key of the input field and a value of the error
 * message otherwise.
 */
std::map<std::string, std::string> LoadNGEM::validateInputs() {
  std::map<std::string, std::string> results;

  int MinEventsPerFrame = getProperty("MinEventsPerFrame");
  int MaxEventsPerFrame = getProperty("MaxEventsPerFrame");

  if (MaxEventsPerFrame < MinEventsPerFrame) {
    results["MaxEventsPerFrame"] =
        "MaxEventsPerFrame is less than MinEvents per frame";
  }
  return results;
}

} // namespace DataHandling
} // namespace Mantid