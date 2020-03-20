// Mantid Repository : https://github.com/mantidproject/mantid
//
// Copyright &copy; 2018 ISIS Rutherford Appleton Laboratory UKRI,
//   NScD Oak Ridge National Laboratory, European Spallation Source,
//   Institut Laue - Langevin & CSNS, Institute of High Energy Physics, CAS
// SPDX - License - Identifier: GPL - 3.0 +
#pragma once

#include "MantidAPI/IMDEventWorkspace_fwd.h"

#include "MantidDataObjects/MDBin.h"
#include "MantidDataObjects/MDEvent.h"
#include "MantidDataObjects/MDEventFactory.h"
#include "MantidDataObjects/MDEventWorkspace.h"
#include "MantidDataObjects/MDLeanEvent.h"

#include <boost/shared_ptr.hpp>

namespace Mantid {
namespace DataObjects {

/** MDEventFactory : collection of methods
 * to create MDLeanEvent* instances, by specifying the number
 * of dimensions as a parameter.
 *
 * @author Janik Zikovsky
 * @date 2011-02-24 15:08:43.105134
 */
// preliminary definition to satisfy gcc
template <size_t nd> class LOOP;

class DLLExport MDEventFactory {
  /** definition which states how many dimensions to generate. This defines the
   *number of dimensions in MD wokspace supported by Mantid
   *IF THIS NUMBER CHANGES, ONE HAS TO RUN generate_mdevent_declarations.py
   *located with MDEventFactory.cpp file TO REINSTANTIATE AUTOGENERATED CODE
   */
  enum { MAX_MD_DIMENSIONS_NUM = 9 };

public:
  /** enum defines fifferent box types generated by createBox factory
   * We will use typecast from integer to these types so it is important to
   * define consisten numbers to these types    */
  enum BoxType {
    MDBoxWithLean = 0,     //< MDBox generated for MDLeanEvent
    MDGridBoxWithLean = 1, //< MDGridBox generated for MDLeanEvent
    MDBoxWithFat = 2,      //< MDBox generated for MDEvent
    MDGridBoxWithFat = 3,  //< MDGridBox generated for MDEvent
    NumBoxTypes =
        4 //< Number of different types of the events, used as metaloop splitter
  };
  // create MD workspace factory call
  static API::IMDEventWorkspace_sptr CreateMDWorkspace(
      size_t nd, const std::string &eventType = "MDLeanEvent",
      const Mantid::API::MDNormalization &preferredNormalization =
          Mantid::API::MDNormalization::VolumeNormalization,
      const Mantid::API::MDNormalization &preferredNormalizationHisto =
          Mantid::API::MDNormalization::VolumeNormalization);

  // create MDBox factory call
  static API::IMDNode *createBox(
      size_t nDimensions, BoxType Type, API::BoxController_sptr &splitter,
      const std::vector<Mantid::Geometry::MDDimensionExtents<coord_t>>
          &extentsVector =
              std::vector<Mantid::Geometry::MDDimensionExtents<coord_t>>(),
      const uint32_t depth = 0, const size_t nBoxEvents = UNDEF_SIZET,
      const size_t boxID = UNDEF_SIZET);

  /** Returns max number of MD dimensions allowed by current Mantid version*/
  static size_t getMaxNumDim() { return size_t(MAX_MD_DIMENSIONS_NUM); }

private:
  using fpCreateBox = API::IMDNode
      *(*)(API::BoxController *,
           const std::vector<Mantid::Geometry::MDDimensionExtents<coord_t>> &,
           const uint32_t, const size_t, const size_t);
  // vector of function pointers to the functions which create MDBox or
  // MDGridBox;
  static std::vector<fpCreateBox> boxCreatorFP;

  // typedef for the class function pointer to the function, which creates MD
  // Workspaces
  using fpCreateMDWS =
      API::IMDEventWorkspace *(*)(const std::string &,
                                  const Mantid::API::MDNormalization &,
                                  const Mantid::API::MDNormalization &);
  // vector of function pointers to the funcions
  static std::vector<fpCreateMDWS> wsCreatorFP;

  // local wrapper around the MDWorkspace constructor as the constructor's
  // address can not be taken . This can also help in a future when writing
  // custom memory allocator
  template <size_t nd>
  static API::IMDEventWorkspace *createMDWorkspaceND(
      const std::string &eventType,
      const Mantid::API::MDNormalization &preferredNormalization,
      const Mantid::API::MDNormalization &preferredNormalizationHisto);

  // local wrappers around the MDBox/MDGridBox constructors as the constructor's
  // address can not be taken. This can also help in a future when writing
  // custom memory allocator
  template <size_t nd>
  static API::IMDNode *createMDBoxLean(
      API::BoxController *splitter,
      const std::vector<Mantid::Geometry::MDDimensionExtents<coord_t>>
          &extentsVector,
      const uint32_t depth, const size_t nBoxEvents, const size_t boxID);
  template <size_t nd>
  static API::IMDNode *createMDBoxFat(
      API::BoxController *splitter,
      const std::vector<Mantid::Geometry::MDDimensionExtents<coord_t>>
          &extentsVector,
      const uint32_t depth, const size_t nBoxEvents, const size_t boxID);
  template <size_t nd>
  static API::IMDNode *createMDGridBoxLean(
      API::BoxController *splitter,
      const std::vector<Mantid::Geometry::MDDimensionExtents<coord_t>>
          &extentsVector,
      const uint32_t depth, const size_t nBoxEvents = 0,
      const size_t boxID = 0);
  template <size_t nd>
  static API::IMDNode *createMDGridBoxFat(
      API::BoxController *splitter,
      const std::vector<Mantid::Geometry::MDDimensionExtents<coord_t>>
          &extentsVector,
      const uint32_t depth, const size_t nBoxEvents = 0,
      const size_t boxID = 0);
  // 0-dimensions terminator
  static API::IMDNode *createMDBoxWrong(
      API::BoxController *,
      const std::vector<Mantid::Geometry::MDDimensionExtents<coord_t>> &,
      const uint32_t, const size_t, const size_t);

  // helper class to generate methaloop on MD workspaces dimensions:
  template <size_t nd> friend class LOOP;

  static LOOP<MAX_MD_DIMENSIONS_NUM> CODE_GENERATOR;
};

//### BEGIN AUTO-GENERATED CODE
/* Code below Auto-generated by 'generate_mdevent_declarations.py'
 *     on 2016-06-03 10:28:44.608989
 *
 * DO NOT EDIT!
 */

/** Macro that makes it possible to call a templated method for
 * a MDEventWorkspace using a IMDEventWorkspace_sptr as the input.
 *
 * @param funcname :: name of the function that will be called.
 * @param workspace :: IMDEventWorkspace_sptr input workspace.
 */

#define CALL_MDEVENT_FUNCTION(funcname, workspace)                             \
  {                                                                            \
    MDEventWorkspace<MDLeanEvent<1>, 1>::sptr MDEW_MDLEANEVENT_1 =             \
        boost::dynamic_pointer_cast<MDEventWorkspace<MDLeanEvent<1>, 1>>(      \
            workspace);                                                        \
    if (MDEW_MDLEANEVENT_1)                                                    \
      funcname<MDLeanEvent<1>, 1>(MDEW_MDLEANEVENT_1);                         \
    MDEventWorkspace<MDLeanEvent<2>, 2>::sptr MDEW_MDLEANEVENT_2 =             \
        boost::dynamic_pointer_cast<MDEventWorkspace<MDLeanEvent<2>, 2>>(      \
            workspace);                                                        \
    if (MDEW_MDLEANEVENT_2)                                                    \
      funcname<MDLeanEvent<2>, 2>(MDEW_MDLEANEVENT_2);                         \
    MDEventWorkspace<MDLeanEvent<3>, 3>::sptr MDEW_MDLEANEVENT_3 =             \
        boost::dynamic_pointer_cast<MDEventWorkspace<MDLeanEvent<3>, 3>>(      \
            workspace);                                                        \
    if (MDEW_MDLEANEVENT_3)                                                    \
      funcname<MDLeanEvent<3>, 3>(MDEW_MDLEANEVENT_3);                         \
    MDEventWorkspace<MDLeanEvent<4>, 4>::sptr MDEW_MDLEANEVENT_4 =             \
        boost::dynamic_pointer_cast<MDEventWorkspace<MDLeanEvent<4>, 4>>(      \
            workspace);                                                        \
    if (MDEW_MDLEANEVENT_4)                                                    \
      funcname<MDLeanEvent<4>, 4>(MDEW_MDLEANEVENT_4);                         \
    MDEventWorkspace<MDLeanEvent<5>, 5>::sptr MDEW_MDLEANEVENT_5 =             \
        boost::dynamic_pointer_cast<MDEventWorkspace<MDLeanEvent<5>, 5>>(      \
            workspace);                                                        \
    if (MDEW_MDLEANEVENT_5)                                                    \
      funcname<MDLeanEvent<5>, 5>(MDEW_MDLEANEVENT_5);                         \
    MDEventWorkspace<MDLeanEvent<6>, 6>::sptr MDEW_MDLEANEVENT_6 =             \
        boost::dynamic_pointer_cast<MDEventWorkspace<MDLeanEvent<6>, 6>>(      \
            workspace);                                                        \
    if (MDEW_MDLEANEVENT_6)                                                    \
      funcname<MDLeanEvent<6>, 6>(MDEW_MDLEANEVENT_6);                         \
    MDEventWorkspace<MDLeanEvent<7>, 7>::sptr MDEW_MDLEANEVENT_7 =             \
        boost::dynamic_pointer_cast<MDEventWorkspace<MDLeanEvent<7>, 7>>(      \
            workspace);                                                        \
    if (MDEW_MDLEANEVENT_7)                                                    \
      funcname<MDLeanEvent<7>, 7>(MDEW_MDLEANEVENT_7);                         \
    MDEventWorkspace<MDLeanEvent<8>, 8>::sptr MDEW_MDLEANEVENT_8 =             \
        boost::dynamic_pointer_cast<MDEventWorkspace<MDLeanEvent<8>, 8>>(      \
            workspace);                                                        \
    if (MDEW_MDLEANEVENT_8)                                                    \
      funcname<MDLeanEvent<8>, 8>(MDEW_MDLEANEVENT_8);                         \
    MDEventWorkspace<MDLeanEvent<9>, 9>::sptr MDEW_MDLEANEVENT_9 =             \
        boost::dynamic_pointer_cast<MDEventWorkspace<MDLeanEvent<9>, 9>>(      \
            workspace);                                                        \
    if (MDEW_MDLEANEVENT_9)                                                    \
      funcname<MDLeanEvent<9>, 9>(MDEW_MDLEANEVENT_9);                         \
    MDEventWorkspace<MDEvent<1>, 1>::sptr MDEW_MDEVENT_1 =                     \
        boost::dynamic_pointer_cast<MDEventWorkspace<MDEvent<1>, 1>>(          \
            workspace);                                                        \
    if (MDEW_MDEVENT_1)                                                        \
      funcname<MDEvent<1>, 1>(MDEW_MDEVENT_1);                                 \
    MDEventWorkspace<MDEvent<2>, 2>::sptr MDEW_MDEVENT_2 =                     \
        boost::dynamic_pointer_cast<MDEventWorkspace<MDEvent<2>, 2>>(          \
            workspace);                                                        \
    if (MDEW_MDEVENT_2)                                                        \
      funcname<MDEvent<2>, 2>(MDEW_MDEVENT_2);                                 \
    MDEventWorkspace<MDEvent<3>, 3>::sptr MDEW_MDEVENT_3 =                     \
        boost::dynamic_pointer_cast<MDEventWorkspace<MDEvent<3>, 3>>(          \
            workspace);                                                        \
    if (MDEW_MDEVENT_3)                                                        \
      funcname<MDEvent<3>, 3>(MDEW_MDEVENT_3);                                 \
    MDEventWorkspace<MDEvent<4>, 4>::sptr MDEW_MDEVENT_4 =                     \
        boost::dynamic_pointer_cast<MDEventWorkspace<MDEvent<4>, 4>>(          \
            workspace);                                                        \
    if (MDEW_MDEVENT_4)                                                        \
      funcname<MDEvent<4>, 4>(MDEW_MDEVENT_4);                                 \
    MDEventWorkspace<MDEvent<5>, 5>::sptr MDEW_MDEVENT_5 =                     \
        boost::dynamic_pointer_cast<MDEventWorkspace<MDEvent<5>, 5>>(          \
            workspace);                                                        \
    if (MDEW_MDEVENT_5)                                                        \
      funcname<MDEvent<5>, 5>(MDEW_MDEVENT_5);                                 \
    MDEventWorkspace<MDEvent<6>, 6>::sptr MDEW_MDEVENT_6 =                     \
        boost::dynamic_pointer_cast<MDEventWorkspace<MDEvent<6>, 6>>(          \
            workspace);                                                        \
    if (MDEW_MDEVENT_6)                                                        \
      funcname<MDEvent<6>, 6>(MDEW_MDEVENT_6);                                 \
    MDEventWorkspace<MDEvent<7>, 7>::sptr MDEW_MDEVENT_7 =                     \
        boost::dynamic_pointer_cast<MDEventWorkspace<MDEvent<7>, 7>>(          \
            workspace);                                                        \
    if (MDEW_MDEVENT_7)                                                        \
      funcname<MDEvent<7>, 7>(MDEW_MDEVENT_7);                                 \
    MDEventWorkspace<MDEvent<8>, 8>::sptr MDEW_MDEVENT_8 =                     \
        boost::dynamic_pointer_cast<MDEventWorkspace<MDEvent<8>, 8>>(          \
            workspace);                                                        \
    if (MDEW_MDEVENT_8)                                                        \
      funcname<MDEvent<8>, 8>(MDEW_MDEVENT_8);                                 \
    MDEventWorkspace<MDEvent<9>, 9>::sptr MDEW_MDEVENT_9 =                     \
        boost::dynamic_pointer_cast<MDEventWorkspace<MDEvent<9>, 9>>(          \
            workspace);                                                        \
    if (MDEW_MDEVENT_9)                                                        \
      funcname<MDEvent<9>, 9>(MDEW_MDEVENT_9);                                 \
  }

/** Macro that makes it possible to call a templated method for
 * a MDEventWorkspace using a IMDEventWorkspace_sptr as the input.
 *
 * @param funcname :: name of the function that will be called.
 * @param workspace :: IMDEventWorkspace_sptr input workspace.
 */

#define CALL_MDEVENT_FUNCTION3(funcname, workspace)                            \
  {                                                                            \
    MDEventWorkspace<MDLeanEvent<3>, 3>::sptr MDEW_MDLEANEVENT_3 =             \
        boost::dynamic_pointer_cast<MDEventWorkspace<MDLeanEvent<3>, 3>>(      \
            workspace);                                                        \
    if (MDEW_MDLEANEVENT_3)                                                    \
      funcname<MDLeanEvent<3>, 3>(MDEW_MDLEANEVENT_3);                         \
    MDEventWorkspace<MDLeanEvent<4>, 4>::sptr MDEW_MDLEANEVENT_4 =             \
        boost::dynamic_pointer_cast<MDEventWorkspace<MDLeanEvent<4>, 4>>(      \
            workspace);                                                        \
    if (MDEW_MDLEANEVENT_4)                                                    \
      funcname<MDLeanEvent<4>, 4>(MDEW_MDLEANEVENT_4);                         \
    MDEventWorkspace<MDLeanEvent<5>, 5>::sptr MDEW_MDLEANEVENT_5 =             \
        boost::dynamic_pointer_cast<MDEventWorkspace<MDLeanEvent<5>, 5>>(      \
            workspace);                                                        \
    if (MDEW_MDLEANEVENT_5)                                                    \
      funcname<MDLeanEvent<5>, 5>(MDEW_MDLEANEVENT_5);                         \
    MDEventWorkspace<MDLeanEvent<6>, 6>::sptr MDEW_MDLEANEVENT_6 =             \
        boost::dynamic_pointer_cast<MDEventWorkspace<MDLeanEvent<6>, 6>>(      \
            workspace);                                                        \
    if (MDEW_MDLEANEVENT_6)                                                    \
      funcname<MDLeanEvent<6>, 6>(MDEW_MDLEANEVENT_6);                         \
    MDEventWorkspace<MDLeanEvent<7>, 7>::sptr MDEW_MDLEANEVENT_7 =             \
        boost::dynamic_pointer_cast<MDEventWorkspace<MDLeanEvent<7>, 7>>(      \
            workspace);                                                        \
    if (MDEW_MDLEANEVENT_7)                                                    \
      funcname<MDLeanEvent<7>, 7>(MDEW_MDLEANEVENT_7);                         \
    MDEventWorkspace<MDLeanEvent<8>, 8>::sptr MDEW_MDLEANEVENT_8 =             \
        boost::dynamic_pointer_cast<MDEventWorkspace<MDLeanEvent<8>, 8>>(      \
            workspace);                                                        \
    if (MDEW_MDLEANEVENT_8)                                                    \
      funcname<MDLeanEvent<8>, 8>(MDEW_MDLEANEVENT_8);                         \
    MDEventWorkspace<MDLeanEvent<9>, 9>::sptr MDEW_MDLEANEVENT_9 =             \
        boost::dynamic_pointer_cast<MDEventWorkspace<MDLeanEvent<9>, 9>>(      \
            workspace);                                                        \
    if (MDEW_MDLEANEVENT_9)                                                    \
      funcname<MDLeanEvent<9>, 9>(MDEW_MDLEANEVENT_9);                         \
    MDEventWorkspace<MDEvent<3>, 3>::sptr MDEW_MDEVENT_3 =                     \
        boost::dynamic_pointer_cast<MDEventWorkspace<MDEvent<3>, 3>>(          \
            workspace);                                                        \
    if (MDEW_MDEVENT_3)                                                        \
      funcname<MDEvent<3>, 3>(MDEW_MDEVENT_3);                                 \
    MDEventWorkspace<MDEvent<4>, 4>::sptr MDEW_MDEVENT_4 =                     \
        boost::dynamic_pointer_cast<MDEventWorkspace<MDEvent<4>, 4>>(          \
            workspace);                                                        \
    if (MDEW_MDEVENT_4)                                                        \
      funcname<MDEvent<4>, 4>(MDEW_MDEVENT_4);                                 \
    MDEventWorkspace<MDEvent<5>, 5>::sptr MDEW_MDEVENT_5 =                     \
        boost::dynamic_pointer_cast<MDEventWorkspace<MDEvent<5>, 5>>(          \
            workspace);                                                        \
    if (MDEW_MDEVENT_5)                                                        \
      funcname<MDEvent<5>, 5>(MDEW_MDEVENT_5);                                 \
    MDEventWorkspace<MDEvent<6>, 6>::sptr MDEW_MDEVENT_6 =                     \
        boost::dynamic_pointer_cast<MDEventWorkspace<MDEvent<6>, 6>>(          \
            workspace);                                                        \
    if (MDEW_MDEVENT_6)                                                        \
      funcname<MDEvent<6>, 6>(MDEW_MDEVENT_6);                                 \
    MDEventWorkspace<MDEvent<7>, 7>::sptr MDEW_MDEVENT_7 =                     \
        boost::dynamic_pointer_cast<MDEventWorkspace<MDEvent<7>, 7>>(          \
            workspace);                                                        \
    if (MDEW_MDEVENT_7)                                                        \
      funcname<MDEvent<7>, 7>(MDEW_MDEVENT_7);                                 \
    MDEventWorkspace<MDEvent<8>, 8>::sptr MDEW_MDEVENT_8 =                     \
        boost::dynamic_pointer_cast<MDEventWorkspace<MDEvent<8>, 8>>(          \
            workspace);                                                        \
    if (MDEW_MDEVENT_8)                                                        \
      funcname<MDEvent<8>, 8>(MDEW_MDEVENT_8);                                 \
    MDEventWorkspace<MDEvent<9>, 9>::sptr MDEW_MDEVENT_9 =                     \
        boost::dynamic_pointer_cast<MDEventWorkspace<MDEvent<9>, 9>>(          \
            workspace);                                                        \
    if (MDEW_MDEVENT_9)                                                        \
      funcname<MDEvent<9>, 9>(MDEW_MDEVENT_9);                                 \
  }

/** Macro that makes it possible to call a templated method for
 * a MDEventWorkspace using a IMDEventWorkspace_sptr as the input.
 *
 * @param funcname :: name of the function that will be called.
 * @param workspace :: IMDEventWorkspace_sptr input workspace.
 */

#define CONST_CALL_MDEVENT_FUNCTION(funcname, workspace)                       \
  {                                                                            \
    const MDEventWorkspace<MDLeanEvent<1>, 1>::sptr CONST_MDEW_MDLEANEVENT_1 = \
        boost::dynamic_pointer_cast<                                           \
            const MDEventWorkspace<MDLeanEvent<1>, 1>>(workspace);             \
    if (CONST_MDEW_MDLEANEVENT_1)                                              \
      funcname<MDLeanEvent<1>, 1>(CONST_MDEW_MDLEANEVENT_1);                   \
    const MDEventWorkspace<MDLeanEvent<2>, 2>::sptr CONST_MDEW_MDLEANEVENT_2 = \
        boost::dynamic_pointer_cast<                                           \
            const MDEventWorkspace<MDLeanEvent<2>, 2>>(workspace);             \
    if (CONST_MDEW_MDLEANEVENT_2)                                              \
      funcname<MDLeanEvent<2>, 2>(CONST_MDEW_MDLEANEVENT_2);                   \
    const MDEventWorkspace<MDLeanEvent<3>, 3>::sptr CONST_MDEW_MDLEANEVENT_3 = \
        boost::dynamic_pointer_cast<                                           \
            const MDEventWorkspace<MDLeanEvent<3>, 3>>(workspace);             \
    if (CONST_MDEW_MDLEANEVENT_3)                                              \
      funcname<MDLeanEvent<3>, 3>(CONST_MDEW_MDLEANEVENT_3);                   \
    const MDEventWorkspace<MDLeanEvent<4>, 4>::sptr CONST_MDEW_MDLEANEVENT_4 = \
        boost::dynamic_pointer_cast<                                           \
            const MDEventWorkspace<MDLeanEvent<4>, 4>>(workspace);             \
    if (CONST_MDEW_MDLEANEVENT_4)                                              \
      funcname<MDLeanEvent<4>, 4>(CONST_MDEW_MDLEANEVENT_4);                   \
    const MDEventWorkspace<MDLeanEvent<5>, 5>::sptr CONST_MDEW_MDLEANEVENT_5 = \
        boost::dynamic_pointer_cast<                                           \
            const MDEventWorkspace<MDLeanEvent<5>, 5>>(workspace);             \
    if (CONST_MDEW_MDLEANEVENT_5)                                              \
      funcname<MDLeanEvent<5>, 5>(CONST_MDEW_MDLEANEVENT_5);                   \
    const MDEventWorkspace<MDLeanEvent<6>, 6>::sptr CONST_MDEW_MDLEANEVENT_6 = \
        boost::dynamic_pointer_cast<                                           \
            const MDEventWorkspace<MDLeanEvent<6>, 6>>(workspace);             \
    if (CONST_MDEW_MDLEANEVENT_6)                                              \
      funcname<MDLeanEvent<6>, 6>(CONST_MDEW_MDLEANEVENT_6);                   \
    const MDEventWorkspace<MDLeanEvent<7>, 7>::sptr CONST_MDEW_MDLEANEVENT_7 = \
        boost::dynamic_pointer_cast<                                           \
            const MDEventWorkspace<MDLeanEvent<7>, 7>>(workspace);             \
    if (CONST_MDEW_MDLEANEVENT_7)                                              \
      funcname<MDLeanEvent<7>, 7>(CONST_MDEW_MDLEANEVENT_7);                   \
    const MDEventWorkspace<MDLeanEvent<8>, 8>::sptr CONST_MDEW_MDLEANEVENT_8 = \
        boost::dynamic_pointer_cast<                                           \
            const MDEventWorkspace<MDLeanEvent<8>, 8>>(workspace);             \
    if (CONST_MDEW_MDLEANEVENT_8)                                              \
      funcname<MDLeanEvent<8>, 8>(CONST_MDEW_MDLEANEVENT_8);                   \
    const MDEventWorkspace<MDLeanEvent<9>, 9>::sptr CONST_MDEW_MDLEANEVENT_9 = \
        boost::dynamic_pointer_cast<                                           \
            const MDEventWorkspace<MDLeanEvent<9>, 9>>(workspace);             \
    if (CONST_MDEW_MDLEANEVENT_9)                                              \
      funcname<MDLeanEvent<9>, 9>(CONST_MDEW_MDLEANEVENT_9);                   \
    const MDEventWorkspace<MDEvent<1>, 1>::sptr CONST_MDEW_MDEVENT_1 =         \
        boost::dynamic_pointer_cast<const MDEventWorkspace<MDEvent<1>, 1>>(    \
            workspace);                                                        \
    if (CONST_MDEW_MDEVENT_1)                                                  \
      funcname<MDEvent<1>, 1>(CONST_MDEW_MDEVENT_1);                           \
    const MDEventWorkspace<MDEvent<2>, 2>::sptr CONST_MDEW_MDEVENT_2 =         \
        boost::dynamic_pointer_cast<const MDEventWorkspace<MDEvent<2>, 2>>(    \
            workspace);                                                        \
    if (CONST_MDEW_MDEVENT_2)                                                  \
      funcname<MDEvent<2>, 2>(CONST_MDEW_MDEVENT_2);                           \
    const MDEventWorkspace<MDEvent<3>, 3>::sptr CONST_MDEW_MDEVENT_3 =         \
        boost::dynamic_pointer_cast<const MDEventWorkspace<MDEvent<3>, 3>>(    \
            workspace);                                                        \
    if (CONST_MDEW_MDEVENT_3)                                                  \
      funcname<MDEvent<3>, 3>(CONST_MDEW_MDEVENT_3);                           \
    const MDEventWorkspace<MDEvent<4>, 4>::sptr CONST_MDEW_MDEVENT_4 =         \
        boost::dynamic_pointer_cast<const MDEventWorkspace<MDEvent<4>, 4>>(    \
            workspace);                                                        \
    if (CONST_MDEW_MDEVENT_4)                                                  \
      funcname<MDEvent<4>, 4>(CONST_MDEW_MDEVENT_4);                           \
    const MDEventWorkspace<MDEvent<5>, 5>::sptr CONST_MDEW_MDEVENT_5 =         \
        boost::dynamic_pointer_cast<const MDEventWorkspace<MDEvent<5>, 5>>(    \
            workspace);                                                        \
    if (CONST_MDEW_MDEVENT_5)                                                  \
      funcname<MDEvent<5>, 5>(CONST_MDEW_MDEVENT_5);                           \
    const MDEventWorkspace<MDEvent<6>, 6>::sptr CONST_MDEW_MDEVENT_6 =         \
        boost::dynamic_pointer_cast<const MDEventWorkspace<MDEvent<6>, 6>>(    \
            workspace);                                                        \
    if (CONST_MDEW_MDEVENT_6)                                                  \
      funcname<MDEvent<6>, 6>(CONST_MDEW_MDEVENT_6);                           \
    const MDEventWorkspace<MDEvent<7>, 7>::sptr CONST_MDEW_MDEVENT_7 =         \
        boost::dynamic_pointer_cast<const MDEventWorkspace<MDEvent<7>, 7>>(    \
            workspace);                                                        \
    if (CONST_MDEW_MDEVENT_7)                                                  \
      funcname<MDEvent<7>, 7>(CONST_MDEW_MDEVENT_7);                           \
    const MDEventWorkspace<MDEvent<8>, 8>::sptr CONST_MDEW_MDEVENT_8 =         \
        boost::dynamic_pointer_cast<const MDEventWorkspace<MDEvent<8>, 8>>(    \
            workspace);                                                        \
    if (CONST_MDEW_MDEVENT_8)                                                  \
      funcname<MDEvent<8>, 8>(CONST_MDEW_MDEVENT_8);                           \
    const MDEventWorkspace<MDEvent<9>, 9>::sptr CONST_MDEW_MDEVENT_9 =         \
        boost::dynamic_pointer_cast<const MDEventWorkspace<MDEvent<9>, 9>>(    \
            workspace);                                                        \
    if (CONST_MDEW_MDEVENT_9)                                                  \
      funcname<MDEvent<9>, 9>(CONST_MDEW_MDEVENT_9);                           \
  }

// ------------- Typedefs for MDBox ------------------

/// Typedef for a MDBox with 1 dimension
using MDBox1Lean = MDBox<MDLeanEvent<1>, 1>;
/// Typedef for a MDBox with 2 dimensions
using MDBox2Lean = MDBox<MDLeanEvent<2>, 2>;
/// Typedef for a MDBox with 3 dimensions
using MDBox3Lean = MDBox<MDLeanEvent<3>, 3>;
/// Typedef for a MDBox with 4 dimensions
using MDBox4Lean = MDBox<MDLeanEvent<4>, 4>;
/// Typedef for a MDBox with 5 dimensions
using MDBox5Lean = MDBox<MDLeanEvent<5>, 5>;
/// Typedef for a MDBox with 6 dimensions
using MDBox6Lean = MDBox<MDLeanEvent<6>, 6>;
/// Typedef for a MDBox with 7 dimensions
using MDBox7Lean = MDBox<MDLeanEvent<7>, 7>;
/// Typedef for a MDBox with 8 dimensions
using MDBox8Lean = MDBox<MDLeanEvent<8>, 8>;
/// Typedef for a MDBox with 9 dimensions
using MDBox9Lean = MDBox<MDLeanEvent<9>, 9>;
/// Typedef for a MDBox with 1 dimension
using MDBox1 = MDBox<MDEvent<1>, 1>;
/// Typedef for a MDBox with 2 dimensions
using MDBox2 = MDBox<MDEvent<2>, 2>;
/// Typedef for a MDBox with 3 dimensions
using MDBox3 = MDBox<MDEvent<3>, 3>;
/// Typedef for a MDBox with 4 dimensions
using MDBox4 = MDBox<MDEvent<4>, 4>;
/// Typedef for a MDBox with 5 dimensions
using MDBox5 = MDBox<MDEvent<5>, 5>;
/// Typedef for a MDBox with 6 dimensions
using MDBox6 = MDBox<MDEvent<6>, 6>;
/// Typedef for a MDBox with 7 dimensions
using MDBox7 = MDBox<MDEvent<7>, 7>;
/// Typedef for a MDBox with 8 dimensions
using MDBox8 = MDBox<MDEvent<8>, 8>;
/// Typedef for a MDBox with 9 dimensions
using MDBox9 = MDBox<MDEvent<9>, 9>;

// ------------- Typedefs for MDBoxBase ------------------

/// Typedef for a MDBoxBase with 1 dimension
using MDBoxBase1Lean = MDBoxBase<MDLeanEvent<1>, 1>;
/// Typedef for a MDBoxBase with 2 dimensions
using MDBoxBase2Lean = MDBoxBase<MDLeanEvent<2>, 2>;
/// Typedef for a MDBoxBase with 3 dimensions
using MDBoxBase3Lean = MDBoxBase<MDLeanEvent<3>, 3>;
/// Typedef for a MDBoxBase with 4 dimensions
using MDBoxBase4Lean = MDBoxBase<MDLeanEvent<4>, 4>;
/// Typedef for a MDBoxBase with 5 dimensions
using MDBoxBase5Lean = MDBoxBase<MDLeanEvent<5>, 5>;
/// Typedef for a MDBoxBase with 6 dimensions
using MDBoxBase6Lean = MDBoxBase<MDLeanEvent<6>, 6>;
/// Typedef for a MDBoxBase with 7 dimensions
using MDBoxBase7Lean = MDBoxBase<MDLeanEvent<7>, 7>;
/// Typedef for a MDBoxBase with 8 dimensions
using MDBoxBase8Lean = MDBoxBase<MDLeanEvent<8>, 8>;
/// Typedef for a MDBoxBase with 9 dimensions
using MDBoxBase9Lean = MDBoxBase<MDLeanEvent<9>, 9>;
/// Typedef for a MDBoxBase with 1 dimension
using MDBoxBase1 = MDBoxBase<MDEvent<1>, 1>;
/// Typedef for a MDBoxBase with 2 dimensions
using MDBoxBase2 = MDBoxBase<MDEvent<2>, 2>;
/// Typedef for a MDBoxBase with 3 dimensions
using MDBoxBase3 = MDBoxBase<MDEvent<3>, 3>;
/// Typedef for a MDBoxBase with 4 dimensions
using MDBoxBase4 = MDBoxBase<MDEvent<4>, 4>;
/// Typedef for a MDBoxBase with 5 dimensions
using MDBoxBase5 = MDBoxBase<MDEvent<5>, 5>;
/// Typedef for a MDBoxBase with 6 dimensions
using MDBoxBase6 = MDBoxBase<MDEvent<6>, 6>;
/// Typedef for a MDBoxBase with 7 dimensions
using MDBoxBase7 = MDBoxBase<MDEvent<7>, 7>;
/// Typedef for a MDBoxBase with 8 dimensions
using MDBoxBase8 = MDBoxBase<MDEvent<8>, 8>;
/// Typedef for a MDBoxBase with 9 dimensions
using MDBoxBase9 = MDBoxBase<MDEvent<9>, 9>;

// ------------- Typedefs for MDGridBox ------------------

/// Typedef for a MDGridBox with 1 dimension
using MDGridBox1Lean = MDGridBox<MDLeanEvent<1>, 1>;
/// Typedef for a MDGridBox with 2 dimensions
using MDGridBox2Lean = MDGridBox<MDLeanEvent<2>, 2>;
/// Typedef for a MDGridBox with 3 dimensions
using MDGridBox3Lean = MDGridBox<MDLeanEvent<3>, 3>;
/// Typedef for a MDGridBox with 4 dimensions
using MDGridBox4Lean = MDGridBox<MDLeanEvent<4>, 4>;
/// Typedef for a MDGridBox with 5 dimensions
using MDGridBox5Lean = MDGridBox<MDLeanEvent<5>, 5>;
/// Typedef for a MDGridBox with 6 dimensions
using MDGridBox6Lean = MDGridBox<MDLeanEvent<6>, 6>;
/// Typedef for a MDGridBox with 7 dimensions
using MDGridBox7Lean = MDGridBox<MDLeanEvent<7>, 7>;
/// Typedef for a MDGridBox with 8 dimensions
using MDGridBox8Lean = MDGridBox<MDLeanEvent<8>, 8>;
/// Typedef for a MDGridBox with 9 dimensions
using MDGridBox9Lean = MDGridBox<MDLeanEvent<9>, 9>;
/// Typedef for a MDGridBox with 1 dimension
using MDGridBox1 = MDGridBox<MDEvent<1>, 1>;
/// Typedef for a MDGridBox with 2 dimensions
using MDGridBox2 = MDGridBox<MDEvent<2>, 2>;
/// Typedef for a MDGridBox with 3 dimensions
using MDGridBox3 = MDGridBox<MDEvent<3>, 3>;
/// Typedef for a MDGridBox with 4 dimensions
using MDGridBox4 = MDGridBox<MDEvent<4>, 4>;
/// Typedef for a MDGridBox with 5 dimensions
using MDGridBox5 = MDGridBox<MDEvent<5>, 5>;
/// Typedef for a MDGridBox with 6 dimensions
using MDGridBox6 = MDGridBox<MDEvent<6>, 6>;
/// Typedef for a MDGridBox with 7 dimensions
using MDGridBox7 = MDGridBox<MDEvent<7>, 7>;
/// Typedef for a MDGridBox with 8 dimensions
using MDGridBox8 = MDGridBox<MDEvent<8>, 8>;
/// Typedef for a MDGridBox with 9 dimensions
using MDGridBox9 = MDGridBox<MDEvent<9>, 9>;

// ------------- Typedefs for MDEventWorkspace ------------------

/// Typedef for a MDEventWorkspace with 1 dimension
using MDEventWorkspace1Lean = MDEventWorkspace<MDLeanEvent<1>, 1>;
/// Typedef for a MDEventWorkspace with 2 dimensions
using MDEventWorkspace2Lean = MDEventWorkspace<MDLeanEvent<2>, 2>;
/// Typedef for a MDEventWorkspace with 3 dimensions
using MDEventWorkspace3Lean = MDEventWorkspace<MDLeanEvent<3>, 3>;
/// Typedef for a MDEventWorkspace with 4 dimensions
using MDEventWorkspace4Lean = MDEventWorkspace<MDLeanEvent<4>, 4>;
/// Typedef for a MDEventWorkspace with 5 dimensions
using MDEventWorkspace5Lean = MDEventWorkspace<MDLeanEvent<5>, 5>;
/// Typedef for a MDEventWorkspace with 6 dimensions
using MDEventWorkspace6Lean = MDEventWorkspace<MDLeanEvent<6>, 6>;
/// Typedef for a MDEventWorkspace with 7 dimensions
using MDEventWorkspace7Lean = MDEventWorkspace<MDLeanEvent<7>, 7>;
/// Typedef for a MDEventWorkspace with 8 dimensions
using MDEventWorkspace8Lean = MDEventWorkspace<MDLeanEvent<8>, 8>;
/// Typedef for a MDEventWorkspace with 9 dimensions
using MDEventWorkspace9Lean = MDEventWorkspace<MDLeanEvent<9>, 9>;
/// Typedef for a MDEventWorkspace with 1 dimension
using MDEventWorkspace1 = MDEventWorkspace<MDEvent<1>, 1>;
/// Typedef for a MDEventWorkspace with 2 dimensions
using MDEventWorkspace2 = MDEventWorkspace<MDEvent<2>, 2>;
/// Typedef for a MDEventWorkspace with 3 dimensions
using MDEventWorkspace3 = MDEventWorkspace<MDEvent<3>, 3>;
/// Typedef for a MDEventWorkspace with 4 dimensions
using MDEventWorkspace4 = MDEventWorkspace<MDEvent<4>, 4>;
/// Typedef for a MDEventWorkspace with 5 dimensions
using MDEventWorkspace5 = MDEventWorkspace<MDEvent<5>, 5>;
/// Typedef for a MDEventWorkspace with 6 dimensions
using MDEventWorkspace6 = MDEventWorkspace<MDEvent<6>, 6>;
/// Typedef for a MDEventWorkspace with 7 dimensions
using MDEventWorkspace7 = MDEventWorkspace<MDEvent<7>, 7>;
/// Typedef for a MDEventWorkspace with 8 dimensions
using MDEventWorkspace8 = MDEventWorkspace<MDEvent<8>, 8>;
/// Typedef for a MDEventWorkspace with 9 dimensions
using MDEventWorkspace9 = MDEventWorkspace<MDEvent<9>, 9>;

// ------------- Typedefs for MDBin ------------------

/// Typedef for a MDBin with 1 dimension
using MDBin1Lean = MDBin<MDLeanEvent<1>, 1>;
/// Typedef for a MDBin with 2 dimensions
using MDBin2Lean = MDBin<MDLeanEvent<2>, 2>;
/// Typedef for a MDBin with 3 dimensions
using MDBin3Lean = MDBin<MDLeanEvent<3>, 3>;
/// Typedef for a MDBin with 4 dimensions
using MDBin4Lean = MDBin<MDLeanEvent<4>, 4>;
/// Typedef for a MDBin with 5 dimensions
using MDBin5Lean = MDBin<MDLeanEvent<5>, 5>;
/// Typedef for a MDBin with 6 dimensions
using MDBin6Lean = MDBin<MDLeanEvent<6>, 6>;
/// Typedef for a MDBin with 7 dimensions
using MDBin7Lean = MDBin<MDLeanEvent<7>, 7>;
/// Typedef for a MDBin with 8 dimensions
using MDBin8Lean = MDBin<MDLeanEvent<8>, 8>;
/// Typedef for a MDBin with 9 dimensions
using MDBin9Lean = MDBin<MDLeanEvent<9>, 9>;
/// Typedef for a MDBin with 1 dimension
using MDBin1 = MDBin<MDEvent<1>, 1>;
/// Typedef for a MDBin with 2 dimensions
using MDBin2 = MDBin<MDEvent<2>, 2>;
/// Typedef for a MDBin with 3 dimensions
using MDBin3 = MDBin<MDEvent<3>, 3>;
/// Typedef for a MDBin with 4 dimensions
using MDBin4 = MDBin<MDEvent<4>, 4>;
/// Typedef for a MDBin with 5 dimensions
using MDBin5 = MDBin<MDEvent<5>, 5>;
/// Typedef for a MDBin with 6 dimensions
using MDBin6 = MDBin<MDEvent<6>, 6>;
/// Typedef for a MDBin with 7 dimensions
using MDBin7 = MDBin<MDEvent<7>, 7>;
/// Typedef for a MDBin with 8 dimensions
using MDBin8 = MDBin<MDEvent<8>, 8>;
/// Typedef for a MDBin with 9 dimensions
using MDBin9 = MDBin<MDEvent<9>, 9>;

/* CODE ABOWE WAS AUTO-GENERATED BY generate_mdevent_declarations.py - DO NOT
 * EDIT! */

//### END AUTO-GENERATED CODE
//##################################################################

} // namespace DataObjects
} // namespace Mantid
