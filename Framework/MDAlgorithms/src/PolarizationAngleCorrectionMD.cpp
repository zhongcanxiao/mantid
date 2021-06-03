// Mantid Repository : https://github.com/mantidproject/mantid
//
// Copyright &copy; 2018 ISIS Rutherford Appleton Laboratory UKRI,
//   NScD Oak Ridge National Laboratory, European Spallation Source,
//   Institut Laue - Langevin & CSNS, Institute of High Energy Physics, CAS
// SPDX - License - Identifier: GPL - 3.0 +
#include "MantidMDAlgorithms/PolarizationAngleCorrectionMD.h"
#include "MantidAPI/AnalysisDataService.h"
#include "MantidAPI/MDGeometry.h"
#include "MantidAPI/MultipleExperimentInfos.h"
#include "MantidAPI/Run.h"
#include "MantidDataObjects/MDBoxIterator.h"
#include "MantidDataObjects/MDEventFactory.h"
#include "MantidGeometry/MDGeometry/IMDDimension.h"
#include "MantidKernel/BoundedValidator.h"
#include "MantidKernel/PhysicalConstants.h"
#include "MantidKernel/Property.h"
#include "MantidKernel/PropertyWithValue.h"
#include "MantidKernel/SpecialCoordinateSystem.h"
#include "MantidKernel/TimeSeriesProperty.h"
#include "MantidMDAlgorithms/MDWSDescription.h"
#include <math.h>

using namespace Mantid::Kernel;
using namespace Mantid::API;
using namespace Mantid::Geometry;
using namespace Mantid::DataObjects;

namespace Mantid {
namespace MDAlgorithms {

// Register the algorithm into the AlgorithmFactory
DECLARE_ALGORITHM(PolarizationAngleCorrectionMD)

/// Algorithm's name for identification. @see Algorithm::name
const std::string PolarizationAngleCorrectionMD::name() const { return "PolarizationAngleCorrectionMD"; }

/// Algorithm's version for identification. @see Algorithm::version
int PolarizationAngleCorrectionMD::version() const { return 1; }

/// Summary
const std::string PolarizationAngleCorrectionMD::summary() const {
  return "Apply detailed balance to MDEventWorkspace";
}

/// category
const std::string PolarizationAngleCorrectionMD::category() const { return "MDAlgorithms"; }

//---------------------------------------------------------------------------------------------------------
/**
 * @brief Define input and output properties
 */
void PolarizationAngleCorrectionMD::init() {

  declareProperty(
      std::make_unique<WorkspaceProperty<API::IMDEventWorkspace>>("InputWorkspace", "", Kernel::Direction::Input),
      "An input MDEventWorkspace.  Must be in Q_sample/Q_lab frame.  Must have an axis as DeltaE");

  auto anglerange = std::make_shared<BoundedValidator<double>>();
  anglerange->setLower(-180.);
  anglerange->setUpper(180.);
  declareProperty("PolarizationAngle", 0., anglerange,
                  "An in-plane polarization angle PAP_APA​, between -180 and 180 degrees");

  auto precisionrange = std::make_shared<BoundedValidator<double>>();
  precisionrange->setLower(0.);
  precisionrange->setUpper(1.);
  declareProperty("Precision", 1., precisionrange,
                  "Precision (between 0 and 1). Any event whose cosine of 2 of its schaf angle less than this "
                  "precision will be ignored.");

  declareProperty(
      std::make_unique<WorkspaceProperty<API::IMDEventWorkspace>>("OutputWorkspace", "", Kernel::Direction::Output),
      "The output MDEventWorkspace with detailed balance applied");
}

//---------------------------------------------------------------------------------------------------------
/**
 * @brief Main execution body
 */
void PolarizationAngleCorrectionMD::exec() {
  // Get input workspace
  API::IMDEventWorkspace_sptr input_ws = getProperty("InputWorkspace");

  // Process input workspace and create output workspace
  std::string output_ws_name = getPropertyValue("OutputWorkspace");

  API::IMDEventWorkspace_sptr output_ws(0);
  if (input_ws->getName() == output_ws_name) {
    // Calcualte in-place
    output_ws = input_ws;
  } else {
    // Clone input workace to output workspace
    output_ws = input_ws->clone();
  }

  // Apply detailed balance to MDEvents
  CALL_MDEVENT_FUNCTION(applyPolarizationAngleCorrection, output_ws);

  // refresh cache for MDBoxes: set correct Box signal
  output_ws->refreshCache();

  // Clear masking (box flags) from the output workspace
  output_ws->clearMDMasking();

  // Set output
  setProperty("OutputWorkspace", output_ws);
}

//---------------------------------------------------------------------------------------------------------
/**
 * @brief Validate inputs
 * Input MDEventWorkspace dimensions:
 * - in Q_sample or Q_lab frame, the 4th dimension is DeltaE, or
 * - the first dimension is |Q| and second is DeltaE
 * validate that run objects have Ei defined (number greater than 0)
 * first input is an MD event workspace as validated above
second input is Temperature - must be either a float>0, or a string pointing to a log name.
if temperature points to a log name, it must be present in each experiment info, and the average must be greater than 0
 */
std::map<std::string, std::string> PolarizationAngleCorrectionMD::validateInputs() {
  std::map<std::string, std::string> output;

  // Get input workspace
  API::IMDEventWorkspace_sptr input_ws = getProperty("InputWorkspace");

  // check input dimension
  std::string dim_error = checkInputMDDimension();
  if (dim_error != "") {
    output["InputWorkspace"] = dim_error;
  }

  return output;
}

//---------------------------------------------------------------------------------------------------------
/**
 * @brief Check input MDEventWorkspace dimension
 * validate dimensions: input workspace is in Q_sample or Q_lab frame, and the 4th dimension is DeltaE, or the first
 * dimension is |Q| and second is DeltaE
 * @return
 */
std::string PolarizationAngleCorrectionMD::checkInputMDDimension() {
  std::string errormsg("");

  API::IMDEventWorkspace_sptr inputws = getProperty("InputWorkspace");
  size_t numdims = inputws->getNumDims();
  if (numdims < 4) {
    errormsg = "Input workspace must have at least 4 dimensions";
  } else {
    // Get and check the dimensions: Q3D or Q1D
    const Mantid::Kernel::SpecialCoordinateSystem coordsys = inputws->getSpecialCoordinateSystem();
    if (coordsys == Mantid::Kernel::SpecialCoordinateSystem::QLab) {
      mIsQSample = false;
    } else if (coordsys == Mantid::Kernel::SpecialCoordinateSystem::QSample) {
      // q3d
      mIsQSample = true;
    } else {
      // not supported
      errormsg = "InputWorkspace is not in Q-Sample or Q-lab frame";
    }

    // Check DeltaE
    if (errormsg.size() > 0 && inputws->getDimension(3)->getName() != "DeltaE") {
      errormsg = "4-th dimension is " + inputws->getDimension(3)->getName() + ".  Must be DeltaE";
      return errormsg;
    }
  }

  return errormsg;
}

//---------------------------------------------------------------------------------------------
/**
 * @brief Apply detailed balance to each MDEvent in MDEventWorkspace
 */
template <typename MDE, size_t nd>
void PolarizationAngleCorrectionMD::applyPolarizationAngleCorrection(
    typename Mantid::DataObjects::MDEventWorkspace<MDE, nd>::sptr ws) {
  // Get Box from MDEventWorkspace
  MDBoxBase<MDE, nd> *box1 = ws->getBox();
  std::vector<API::IMDNode *> boxes;
  box1->getBoxes(boxes, 1000, true);
  auto numBoxes = int(boxes.size());

  // Add the boxes in parallel. They should be spread out enough on each
  // core to avoid stepping on each other.
  // cppcheck-suppress syntaxError
  PRAGMA_OMP( parallel for if (!ws->isFileBacked()))
  for (int i = 0; i < numBoxes; ++i) {
    PARALLEL_START_INTERUPT_REGION
    auto *box = dynamic_cast<MDBox<MDE, nd> *>(boxes[i]);
    if (box && !box->getIsMasked()) {
      // get the MEEvents from box
      std::vector<MDE> &events = box->getEvents();
      // Add events, with bounds checking
      for (auto it = events.begin(); it != events.end(); ++it) {
        // Modify the event
        // FIXME TODO : SPEC TASK #109

        float factor = 1.0;

        // calcalate and set intesity
        auto intensity = it->getSignal() * factor;
        it->setSignal(intensity);

        // calculate and set error
        auto error2 = it->getErrorSquared() * factor * factor;
        // error2 *= factor * factor;
        it->setErrorSquared(error2);
      }
    }
    box->releaseEvents();
    PARALLEL_END_INTERUPT_REGION
  }
  PARALLEL_CHECK_INTERUPT_REGION

  return;
}

//---------------------------------------------------------------------------------------------------------
/**
 * @brief Retrieve sample log Ei
 * Temperature value can be specified by either property Temperature, or
 * it can be calcualted from sample temperture log in the MDWorkspace
 */
std::string PolarizationAngleCorrectionMD::getEi(API::IMDEventWorkspace_sptr mdws) {
  // Get temperture sample log name
  std::string Estring("Ei");
  std::stringstream eiss;

  // the input property could be a valid float; if not must search the experiment info
  mEiMap.clear();
  uint16_t numexpinfo = mdws->getNumExperimentInfo();

  for (uint16_t i = 0; i < numexpinfo; ++i) {

    // if user specified is not a valid float
    ExperimentInfo_const_sptr expinfo = mdws->getExperimentInfo(i);
    if (expinfo->run().hasProperty(Estring)) {
      std::string eistr = expinfo->run().getProperty(Estring)->value();
      try {
        double ei = boost::lexical_cast<double>(eistr);
        if (ei > 0) {
          // Ei is allowed
          mEiMap[i] = ei;
        } else {
          // Ei is not greater than 0 and is not allowed
          eiss << "Experiment Info Ei " << ei << " cannot be zero or less than zero.";
        }
      } catch (...) {
        // unable cast to double
        eiss << "Experiment Info Ei " << eistr << " cannot be cast to a double number";
      }
    } else {
      // does not have Ei
      eiss << "Experiment Info " << i << " does not have " << Estring;
    }
  }

  std::string ei_error = eiss.str();

  return ei_error;
}

} // namespace MDAlgorithms
} // namespace Mantid
