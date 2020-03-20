// Mantid Repository : https://github.com/mantidproject/mantid
//
// Copyright &copy; 2018 ISIS Rutherford Appleton Laboratory UKRI,
//   NScD Oak Ridge National Laboratory, European Spallation Source,
//   Institut Laue - Langevin & CSNS, Institute of High Energy Physics, CAS
// SPDX - License - Identifier: GPL - 3.0 +
#pragma once

#include "MantidKernel/System.h"

#include <memory>
#include <string>
#include <vector>

namespace MantidQt {
namespace MantidWidgets {
namespace DataProcessor {
class RowData;
using RowData_sptr = std::shared_ptr<RowData>;
} // namespace DataProcessor
} // namespace MantidWidgets
} // namespace MantidQt

namespace DataProcessorTestHelper {

// Add a property value from a list to the given row data with an optional
// prefix
DLLExport void
addPropertyValue(MantidQt::MantidWidgets::DataProcessor::RowData_sptr rowData,
                 const std::vector<std::string> &list, const size_t index,
                 const std::string &property, const std::string &prefix = "");

// Add a property value to the given row data
DLLExport void
addPropertyValue(MantidQt::MantidWidgets::DataProcessor::RowData_sptr rowData,
                 const std::string &property, const std::string &value);

// Utility to create a row data class from a string list of simple inputs
DLLExport MantidQt::MantidWidgets::DataProcessor::RowData_sptr
makeRowData(const std::vector<std::string> &list,
            const std::vector<std::string> &prefixes = {"TOF_", "", "TRANS_"},
            const size_t numSlices = 0);
} // namespace DataProcessorTestHelper
