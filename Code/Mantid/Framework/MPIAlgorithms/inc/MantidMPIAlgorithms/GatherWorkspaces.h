#ifndef MANTID_MPIALGORITHMS_GATHERWORKSPACES_H_
#define MANTID_MPIALGORITHMS_GATHERWORKSPACES_H_

//----------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------
#include "MantidAPI/Algorithm.h"
#include "MantidDataObjects/EventWorkspace.h"

namespace Mantid
{
namespace MPIAlgorithms
{
/** GatherWorkspaces be viewed as ConjoinWorkspaces for MPI.
    It stitches together the input workspaces provided by each of the processes into
    a single workspace in the root process.

    The spectra in the output workspace will be ordered by the rank of the input processes.
    It is up to the caller to ensure this results in the required ordering.
    Furthermore, there are all sorts of things that ought to be consistent for this
    algorithm to make sense (e.g. the instrument). The general philosophy, though, is to
    leave the responsibility for this to the user and only check the vital things (i.e. that
    the number of bins is consistent).

    Required Properties:
    <UL>
    <LI> InputWorkspace - The name of the separate workspaces (must be the same for all processes).</LI>
    <LI> OutputWorkspace - The name of the output workspace. Will only be created by the root process.</LI>
    </UL>

    Copyright &copy; 2011 ISIS Rutherford Appleton Laboratory & NScD Oak Ridge National Laboratory

    This file is part of Mantid.

    Mantid is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    Mantid is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    File change history is stored at: <https://svn.mantidproject.org/mantid/trunk/Code/Mantid>
    Code Documentation is available at: <http://doxygen.mantidproject.org>
*/
class GatherWorkspaces : public API::Algorithm
{
public:
  /// (Empty) Constructor
  GatherWorkspaces() : API::Algorithm() {}
  /// Virtual destructor
  virtual ~GatherWorkspaces() {}
  /// Algorithm's name
  virtual const std::string name() const { return "GatherWorkspaces"; }
  /// Algorithm's version
  virtual int version() const { return (1); }
  /// Algorithm's category for identification
  virtual const std::string category() const { return "MPI"; }
  /// Sum for boostmpi MantidVec
  struct vplus
        : public std::binary_function<MantidVec, MantidVec, MantidVec>
  {       // functor for operator+
        MantidVec operator()(const MantidVec& _Left, const MantidVec& _Right) const
        {       // apply operator+ to operands
                MantidVec v(_Left.size());
                std::transform(_Left.begin(), _Left.end(), _Right.begin(), v.begin(), std::plus<double>());
                return (v);
        }

  };
   /// Sum for error for boostmpi MantidVec
  struct eplus : public std::binary_function<MantidVec, MantidVec, MantidVec>
  {       // functor for operator+
        MantidVec operator()(const MantidVec& _Left, const MantidVec& _Right) const
        {       // apply operator+ to operands
                MantidVec v(_Left.size());
                std::transform(_Left.begin(), _Left.end(), _Right.begin(), v.begin(), SumGaussError<double>());
                return (v);
        }
  };


private:
  void init();
  void exec();
  //! Functor used for computing the sum of the square values of a vector, using the accumulate algorithm
  template <class T> struct SumGaussError: public std::binary_function<T,T,T>
  {
    SumGaussError(){}
    /// Sums the arguments in quadrature
    inline T operator()(const T& l, const T& r) const
    {
      return std::sqrt(l*l+r*r);
    }
  };

  void execEvent();
  DataObjects::EventWorkspace_const_sptr eventW;
  std::size_t totalSpec;
  int hist;
  std::size_t numBins;

};

} // namespace MPIAlgorithms
} // namespace Mantid

#endif /*MANTID_MPIALGORITHMS_GATHERWORKSPACES_H_*/
