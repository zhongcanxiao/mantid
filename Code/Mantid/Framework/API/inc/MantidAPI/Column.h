#ifndef MANTID_API_ICOLUMN_H_
#define MANTID_API_ICOLUMN_H_

//----------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------

#include "MantidKernel/Logger.h"

#include <boost/shared_ptr.hpp>
#include <string>
#include <typeinfo>
#include <limits>

namespace Mantid
{

//----------------------------------------------------------------------
// Forward declarations
//----------------------------------------------------------------------
namespace Kernel
{
  class Logger;
}

namespace API
{
/** \class Column

    Column is the base class for columns of TableWorkspace.


    \author Roman Tolchenov
    \date 31/10/2008

    Copyright &copy; 2007-8 ISIS Rutherford Appleton Laboratory & NScD Oak Ridge National Laboratory

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

    File change history is stored at: <https://svn.mantidproject.org/mantid/trunk/Code/Mantid>.
    Code Documentation is available at: <http://doxygen.mantidproject.org>
*/
#ifdef _WIN32
#ifdef IN_MANTID_API
  #define Column_DllExport __declspec( dllexport )
#else
  #define Column_DllExport __declspec( dllimport )
#endif
#else
  #define Column_DllExport
  #define Column_DllImport
#endif

class Column_DllExport Column
{
public:
    Column():m_type("int"){};
    /// Virtual destructor
    virtual ~Column() {}

    /// Name (caption) of the column.
    const std::string& name()const{return m_name;}

    /// Type of the column data.
    const std::string& type()const{return m_type;}
    /// return value casted to double (should be pure virtual)
    virtual double operator[](size_t i)const{UNUSED_ARG(i); return std::numeric_limits<double>::quiet_NaN();}
    /// Renames the column.
    void setName(const std::string& str){m_name = str;}

    /// Number of individual elements in the column.
    virtual int size()const = 0;

    /// Returns typeid for the data in the column
    virtual const std::type_info& get_type_info()const = 0;

    /// Returns typeid for the pointer type to the data element in the column
    virtual const std::type_info& get_pointer_type_info()const = 0;
    /// Is the column to be read-only? @retur true by default.
    virtual bool getReadOnly() const
    { return true; }

    /// Prints out the value to a stream
    virtual void print(std::ostream& s, int index) const = 0;

    /// Read in a string and set the value at the given index
    virtual void read(const std::string & text, int index)
    { UNUSED_ARG(text);UNUSED_ARG(index); }

    /// Specialized type check
    virtual bool isBool()const = 0;

    /// Must return overall memory size taken by the column.
    virtual long int sizeOfData()const = 0;

    /// Virtual destructor. Fully clone any column.
    virtual Column* clone() const = 0;

    /// Templated method for returning a value. No type checks are done.
    template<class T>
    T& cell(int index)
    {
        return *static_cast<T*>(void_pointer(index));
    }
 

    /// Templated method for returning a value (const version). No type checks are done.
    template<class T>
    const T& cell(int index)const
    {
        return *static_cast<const T*>(void_pointer(index));
    }

    /// Type check.
    template<class T>
    bool isType()const
    {
        return get_type_info() == typeid(T);
    }

protected:
    /// Sets the new column size.
    virtual void resize(int count) = 0;
    /// Inserts an item.
    virtual void insert(int index) = 0;
    /// Removes an item.
    virtual void remove(int index) = 0;
    /// Pointer to a data element
    virtual void* void_pointer(int index) = 0;
    /// Pointer to a data element
    virtual const void* void_pointer(int index) const = 0;

    std::string m_name;///< name
    std::string m_type;///< type
    friend class ColumnFactoryImpl;
    friend class ITableWorkspace;
    template<class T> friend class ColumnVector;
    /// Logger
    static Kernel::Logger& g_log;
};

/**  @class Boolean
    As TableColumn stores its data in a std::vector bool type cannot be used 
    in the same way as the other types. Class Boolean is used instead.
*/
struct Column_DllExport Boolean
{
    /// Default constructor
    Boolean():value(false){}
    /// Conversion from bool
    Boolean(bool b):value(b){}
    /// Returns bool
    operator bool(){return value;}
    /// equal to operator
    bool operator==(const Boolean& b)const
    {return(this->value==b.value);		
    }
    //
    operator double(void)const{return double(this->value);}
    bool value;///< boolean value
};

/// Printing Boolean to an output stream
Column_DllExport std::ostream& operator<<(std::ostream& ,const API::Boolean& );

typedef boost::shared_ptr<Column> Column_sptr;
typedef boost::shared_ptr<const Column> Column_const_sptr;

} // namespace API
} // Namespace Mantid
#endif /*MANTID_API_ICOLUMN_H_*/
