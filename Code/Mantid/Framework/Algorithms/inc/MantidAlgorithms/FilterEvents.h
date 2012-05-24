#ifndef MANTID_ALGORITHMS_FILTEREVENTS_H_
#define MANTID_ALGORITHMS_FILTEREVENTS_H_

#include "MantidKernel/System.h"
#include "MantidAPI/Algorithm.h"
#include "MantidDataObjects/EventWorkspace.h"
#include "MantidDataObjects/SplittersWorkspace.h"
#include "MantidAPI/ISplittersWorkspace.h"
#include "MantidKernel/TimeSplitter.h"
#include "MantidAPI/ITableWorkspace.h"

namespace Mantid
{
namespace Algorithms
{

  /** FilterEvents : Filter Events in EventWorkspace to multiple EventsWorkspace by Splitters
    
    @date 2012-04-04

    Copyright &copy; 2012 ISIS Rutherford Appleton Laboratory & NScD Oak Ridge National Laboratory

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
  class DLLExport FilterEvents : public API::Algorithm
  {
  public:
    FilterEvents();
    virtual ~FilterEvents();
    
    /// Algorithm's name for identification overriding a virtual method
    virtual const std::string name() const { return "FilterEvents";};
    /// Algorithm's version for identification overriding a virtual method
    virtual int version() const { return 1;};
    /// Algorithm's category for identification overriding a virtual method
    virtual const std::string category() const { return "Events\\EventFiltering";}

  private:
    /// Sets documentation strings for this algorithm
    virtual void initDocs();
    // Implement abstract Algorithm methods
    void init();
    // Implement abstract Algorithm methods
    void exec();

    void processSplittersWorkspace();

    void createOutputWorkspaces(std::string outputwsnamebase);

    void importDetectorTOFCalibration(std::string detcalfilename);

    void filterEventsBySplitters();

    DataObjects::EventWorkspace_sptr mEventWorkspace;
    DataObjects::SplittersWorkspace_sptr mSplittersWorkspace;

    std::set<int> mWorkspaceGroups;
    Kernel::TimeSplitterType mSplitters;
    std::map<int, DataObjects::EventWorkspace_sptr> mOutputWorkspaces;

    std::vector<detid_t> mCalibDetectorIDs;
    std::vector<double> mCalibOffsets;

    bool mFilterByPulseTime;

    API::ITableWorkspace_sptr mInformationWS;
    bool mWithInfo;


  };


} // namespace Algorithms
} // namespace Mantid

#endif  /* MANTID_ALGORITHMS_FILTEREVENTS_H_ */
