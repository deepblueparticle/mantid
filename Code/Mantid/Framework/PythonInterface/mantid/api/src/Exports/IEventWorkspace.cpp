#include "MantidAPI/IEventWorkspace.h"
#include "MantidAPI/IEventList.h"
#include "MantidAPI/WorkspaceProperty.h"
#include "MantidPythonInterface/kernel/Registry/RegisterSingleValueHandler.h"
#include "MantidPythonInterface/kernel/PropertyWithValue.h"

#include <boost/python/class.hpp>
#include <boost/python/register_ptr_to_python.hpp>

using Mantid::API::IEventWorkspace;
using Mantid::API::IEventWorkspace_sptr;
using Mantid::API::IEventList;
using Mantid::API::WorkspaceProperty;
using Mantid::API::IWorkspaceProperty;
using Mantid::Kernel::PropertyWithValue;
using namespace boost::python;

void export_IEventWorkspace()
{
  class_<IEventWorkspace, bases<Mantid::API::MatrixWorkspace>, boost::noncopyable>("IEventWorkspace", no_init)
    .def("getNumberEvents", &IEventWorkspace::getNumberEvents, 
         "Returns the number of events in the workspace")
    .def("getTofMin", &IEventWorkspace::getTofMin, 
         "Returns the minimum TOF value (in microseconds) held by the workspace")
    .def("getTofMax", &IEventWorkspace::getTofMax, 
         "Returns the maximum TOF value (in microseconds) held by the workspace")
    .def("getEventList", (IEventList*(IEventWorkspace::*)(const int) ) &IEventWorkspace::getEventListPtr,
        return_internal_reference<>(), "Return the event list managing the events at the given workspace index")
    .def("clearMRU", &IEventWorkspace::clearMRU, "Clear the most-recently-used lists")
    ;

  REGISTER_SINGLEVALUE_HANDLER(IEventWorkspace_sptr);
}

