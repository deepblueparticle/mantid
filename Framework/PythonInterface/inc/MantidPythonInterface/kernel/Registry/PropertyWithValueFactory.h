#ifndef MANTID_PYTHONINTERFACE_PROPERTYWITHVALUEFACTORY_H_
#define MANTID_PYTHONINTERFACE_PROPERTYWITHVALUEFACTORY_H_
/**
    Copyright &copy; 2011 ISIS Rutherford Appleton Laboratory, NScD Oak Ridge
   National Laboratory & European Spallation Source

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

    File change history is stored at: <https://github.com/mantidproject/mantid>.
    Code Documentation is available at: <http://doxygen.mantidproject.org>
 */
//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include "MantidPythonInterface/kernel/Registry/PropertyValueHandler.h"
#include <boost/python/list.hpp>
#include <memory>

namespace Mantid {
//---------------------------------------------------------------------------
// Forward declarations
//---------------------------------------------------------------------------
namespace Kernel {
class Property;
}

namespace PythonInterface {
namespace Registry {
/**
 * Defines a static factory class that creates PropertyWithValue
 * instances from python objects.
 */
class DLLExport PropertyWithValueFactory {
public:
  static std::unique_ptr<Kernel::Property>
  create(const std::string &name, const boost::python::object &defaultValue,
         const boost::python::object &validator, const unsigned int direction);

  static std::unique_ptr<Kernel::Property>
  create(const std::string &name, const boost::python::object &defaultValue,
         const unsigned int direction);

  static std::unique_ptr<Kernel::Property>
  createTimeSeries(const std::string &name,
                   const boost::python::list &defaultValue);

private:
  /// Return a handler that maps the python type to a C++ type
  static const PropertyValueHandler &lookup(PyObject *const object);
  /// Return a string based on the python array type
  static const std::string isArray(PyObject *const object);
};
} // namespace Registry
} // namespace PythonInterface
} // namespace Mantid

#endif // MANTID_PYTHONINTERFACE_PROPERTYWITHVALUEFACTORY_H_
