#include "MantidGeometry/MDGeometry/MDFrameFactory.h"
#include "MantidKernel/MDUnitFactory.h"
#include "MantidKernel/MDUnit.h"
#include "MantidKernel/UnitLabelTypes.h"
#include "MantidGeometry/MDGeometry/MDFrame.h"

namespace Mantid {
namespace Geometry {

GeneralFrame *
GeneralFrameFactory::createRaw(const MDFrameArgument &argument) const {
  return new GeneralFrame(argument.frameString, argument.unitString);
}

/// Indicate an ability to intepret the string
bool GeneralFrameFactory::canInterpret(const MDFrameArgument &) const {
  return true; // This can interpret everything
}

QLab *QLabFrameFactory::createRaw(const MDFrameArgument &) const {
  return new QLab;
}

bool QLabFrameFactory::canInterpret(const MDFrameArgument &argument) const {
  // We only need to check the frame QLab only makes sense in inverse Angstroms
  return argument.frameString == QLab::QLabName;
}

QSample *QSampleFrameFactory::createRaw(const MDFrameArgument &) const {
  return new QSample;
}

bool QSampleFrameFactory::canInterpret(const MDFrameArgument &argument) const {
  // We only need to check the frame QSample only makes sense in inverse
  // Angstroms
  return argument.frameString == QSample::QSampleName;
}


HKL *HKLFrameFactory::createRaw(const MDFrameArgument &argument) const
{
  using namespace Mantid::Kernel;
  auto unitFactoryChain = Kernel::makeMDUnitFactoryChain();
  auto productMDUnit = unitFactoryChain->create(argument.unitString);
  return new HKL(productMDUnit);
}

bool HKLFrameFactory::canInterpret(const MDFrameArgument &argument) const
{
    using namespace Mantid::Kernel;
    auto unitFactoryChain = Kernel::makeMDUnitFactoryChain();
    auto mdUnit = unitFactoryChain->create(argument.unitString);
    // We expect units to be RLU or A^-1
    const bool compatibleUnit = (mdUnit->getUnitLabel() == Units::Symbol::InverseAngstrom || mdUnit->getUnitLabel() == Units::Symbol::RLU);
    // Check both the frame name and the unit name
    return argument.frameString == HKL::HKLName && compatibleUnit;
}

MDFrameFactory_uptr makeMDFrameFactoryChain() {
  typedef MDFrameFactory_uptr FactoryType;
  auto first = FactoryType(new QLabFrameFactory);
  first->setSuccessor(FactoryType(new QSampleFrameFactory))
  .setSuccessor(FactoryType(new HKLFrameFactory))
  .setSuccessor(FactoryType(new GeneralFrameFactory));
  return first;
}

} // namespace Geometry
} // namespace Mantid
