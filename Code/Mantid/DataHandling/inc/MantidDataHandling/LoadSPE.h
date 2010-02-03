#ifndef MANTID_DATAHANDLING_LOADSPE_H_
#define MANTID_DATAHANDLING_LOADSPE_H_

//---------------------------------------------------
// Includes
//---------------------------------------------------
#include "MantidAPI/Algorithm.h"

namespace Mantid
{
namespace DataHandling
{
/**
  Loads an SPE format file into a Mantid workspace.

  Required properties:
  <UL>
  <LI> Filename - The SPE format file to be read </LI>
  <LI> Workspace - The name to give to the output workspace </LI>
  </UL>

  @author Russell Taylor, Tessella plc
  @date 02/02/2010

  Copyright &copy; 2010 STFC Rutherford Appleton Laboratory

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
class DLLExport LoadSPE : public API::Algorithm
{
public:
  /// Constructor
  LoadSPE() : API::Algorithm() {}
  /// Virtual destructor
  virtual ~LoadSPE() {}
  /// Algorithm's name
  virtual const std::string name() const { return "LoadSPE"; }
  /// Algorithm's version
  virtual const int version() const { return (1); }
  /// Algorithm's category for identification
  virtual const std::string category() const { return "DataHandling"; }

private:
  // Initialisation code
  void init();
  // Execution code
  void exec();

  void readHistogram(FILE* speFile, API::MatrixWorkspace_sptr workspace, int index);
  void reportFormatError(const std::string& what);

  std::string m_filename; ///< The file to load
};

} // namespace DataHandling
} // namespace Mantid

#endif /*MANTID_DATAHANDLING_LoadSPE_H_*/
