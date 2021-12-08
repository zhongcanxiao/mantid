// Mantid Repository : https://github.com/mantidproject/mantid
//
// Copyright &copy; 2021 ISIS Rutherford Appleton Laboratory UKRI,
//   NScD Oak Ridge National Laboratory, European Spallation Source,
//   Institut Laue - Langevin
// SPDX - License - Identifier: GPL - 3.0 +
//----------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------
#include "MantidCurveFitting/Functions/DecoupAsymPowderMagRot.h"
#include "MantidAPI/FunctionFactory.h"
#include "MantidCurveFitting/MuonHelpers.h"

#include <cmath>

namespace Mantid::CurveFitting::Functions {

using namespace CurveFitting;

using namespace Kernel;

using namespace API;

using namespace CurveFitting::MuonHelper;

DECLARE_FUNCTION(DecoupAsymPowderMagRot)

void DecoupAsymPowderMagRot::init() {
  declareParameter("Asymmetry", 1.0, "a scaling parameter for the overall asymmetry");
  declareParameter("CharField", 1.0, "the characteristic field");
}

void DecoupAsymPowderMagRot::function1D(double *out, const double *xValues, const size_t nData) const {
  const double asym = getParameter("Asymmetry");
  const double charField = getParameter("CharField");

  for (size_t i = 0; i < nData; i++) {
    auto A_z = getAz(xValues[i], charField);
    out[i] = asym * (1 - A_z);
  }
}

void DecoupAsymPowderMagRot::functionDeriv1D(Jacobian *out, const double *xValues, const size_t nData) {
  const double charField = getParameter("CharField");

  for (size_t i = 0; i < nData; i++) {
    double diffasym = 1 - getAz(xValues[i], charField);
    double diffcharfield = -xValues[i] / pow(charField, 2);
    double b = xValues[i] / charField;
    double diffb = ((pow(b, 2) - 1) * ((pow(b, 2) + 3) * log((b + 1.0) / (b - 1.0)) - (2 * b))) / (8 * pow(b, 4));
    double diffAz = diffcharfield * diffb;
    out->set(i, 0, diffasym);
    out->set(i, 1, diffAz);
  }
}
} // namespace Mantid::CurveFitting::Functions
