
// Boost Includes ==============================================================
#include <boost/python.hpp>
#include <boost/cstdint.hpp>

// Includes ====================================================================
#include "MantidAPI/Algorithm.h"
#include "MantidAPI/IAlgorithm.h"
#include "MantidAPI/FrameworkManager.h"
#include "MantidAPI/Workspace.h"
#include "MantidKernel/LibraryManager.h"
#include "MantidAPI/FrameworkManager.h"

// Using =======================================================================
using namespace boost::python;

// Declarations ================================================================
namespace  {
	
struct Mantid_API_IAlgorithm_Wrapper: Mantid::API::IAlgorithm
{
    Mantid_API_IAlgorithm_Wrapper(PyObject* py_self_, const Mantid::API::IAlgorithm& p0):
       Mantid::API::IAlgorithm(p0), py_self(py_self_) {}

    Mantid_API_IAlgorithm_Wrapper(PyObject* py_self_):
        Mantid::API::IAlgorithm(), py_self(py_self_) {}

    Mantid::Kernel::StatusCode initialize() {
        return call_method< Mantid::Kernel::StatusCode >(py_self, "initialize");
    }

    Mantid::Kernel::StatusCode execute() {
        return call_method< Mantid::Kernel::StatusCode >(py_self, "execute");
    }

    Mantid::Kernel::StatusCode finalize() {
        return call_method< Mantid::Kernel::StatusCode >(py_self, "finalize");
    }

    bool isInitialized() const {
        return call_method< bool >(py_self, "isInitialized");
    }

    bool isFinalized() const {
        return call_method< bool >(py_self, "isFinalized");
    }

    bool isExecuted() const {
        return call_method< bool >(py_self, "isExecuted");
    }

    PyObject* py_self;
};

struct Mantid_Kernel_Workspace_Wrapper: Mantid::API::Workspace
{
    const std::string id() const {
        return call_method< const std::string >(py_self, "id");
    }

    long int getMemorySize() const {
        return call_method< long int >(py_self, "getMemorySize");
    }

    long int default_getMemorySize() const {
        return Mantid::API::Workspace::getMemorySize();
    }

    PyObject* py_self;
};

struct Mantid_API_Algorithm_Wrapper: Mantid::API::Algorithm
{
    Mantid_API_Algorithm_Wrapper(PyObject* py_self_):
        Mantid::API::Algorithm(), py_self(py_self_) {}

    const std::string& name() const {
        return call_method< const std::string& >(py_self, "name");
    }

    const std::string& default_name() const {
        return Mantid::API::Algorithm::name();
    }

    const std::string& version() const {
        return call_method< const std::string& >(py_self, "version");
    }

    const std::string& default_version() const {
        return Mantid::API::Algorithm::version();
    }

    bool isInitialized() const {
        return call_method< bool >(py_self, "isInitialized");
    }

    bool default_isInitialized() const {
        return Mantid::API::Algorithm::isInitialized();
    }

    bool isExecuted() const {
        return call_method< bool >(py_self, "isExecuted");
    }

    bool default_isExecuted() const {
        return Mantid::API::Algorithm::isExecuted();
    }

    bool isFinalized() const {
        return call_method< bool >(py_self, "isFinalized");
    }

    bool default_isFinalized() const {
        return Mantid::API::Algorithm::isFinalized();
    }

    void setProperty(const std::string& p0, const std::string& p1) {
        call_method< void >(py_self, "setProperty", p0, p1);
    }

    void default_setProperty(const std::string& p0, const std::string& p1) {
        Mantid::API::Algorithm::setProperty(p0, p1);
    }

    bool existsProperty(const std::string& p0) const {
        return call_method< bool >(py_self, "existsProperty", p0);
    }

    bool default_existsProperty(const std::string& p0) const {
        return Mantid::API::Algorithm::existsProperty(p0);
    }

    std::string getPropertyValue(const std::string& p0) const {
        return call_method< std::string >(py_self, "getPropertyValue", p0);
    }

    std::string default_getPropertyValue(const std::string& p0) const {
        return Mantid::API::Algorithm::getPropertyValue(p0);
    }

    Mantid::API::Property* getProperty(const std::string& p0) const {
        return call_method< Mantid::API::Property* >(py_self, "getProperty", p0);
    }

    Mantid::API::Property* default_getProperty(const std::string& p0) const {
        return Mantid::API::Algorithm::getProperty(p0);
    }

    const std::vector<Mantid::API::Property*,std::allocator<Mantid::API::Property*> >& getProperties() const {
        return call_method< const std::vector<Mantid::API::Property*,std::allocator<Mantid::API::Property*> >& >(py_self, "getProperties");
    }

    const std::vector<Mantid::API::Property*,std::allocator<Mantid::API::Property*> >& default_getProperties() const {
        return Mantid::API::Algorithm::getProperties();
    }

    Mantid::Kernel::StatusCode init() {
        return call_method< Mantid::Kernel::StatusCode >(py_self, "init");
    }

    Mantid::Kernel::StatusCode exec() {
        return call_method< Mantid::Kernel::StatusCode >(py_self, "exec");
    }

    Mantid::Kernel::StatusCode final() {
        return call_method< Mantid::Kernel::StatusCode >(py_self, "final");
    }

    PyObject* py_self;
};


}// namespace 


// Module ======================================================================
#if _WIN32
BOOST_PYTHON_MODULE(MantidAPI)
{
#else
BOOST_PYTHON_MODULE(libMantidAPI)
{
#endif	
	
	class_< Mantid::API::IAlgorithm, boost::noncopyable, Mantid_API_IAlgorithm_Wrapper >("IAlgorithm", no_init)
        .def("initialize", pure_virtual(&Mantid::API::IAlgorithm::initialize))
        .def("execute", pure_virtual(&Mantid::API::IAlgorithm::execute))
        .def("finalize", pure_virtual(&Mantid::API::IAlgorithm::finalize))
        .def("isInitialized", pure_virtual(&Mantid::API::IAlgorithm::isInitialized))
        .def("isFinalized", pure_virtual(&Mantid::API::IAlgorithm::isFinalized))
        .def("isExecuted", pure_virtual(&Mantid::API::IAlgorithm::isExecuted))
    ;
		
    class_< Mantid::API::FrameworkManager >("FrameworkManager", init<  >())
        .def(init< const Mantid::API::FrameworkManager& >())
        .def("initialize", &Mantid::API::FrameworkManager::initialize)
        .def("clear", &Mantid::API::FrameworkManager::clear)
        .def("createAlgorithm", (Mantid::API::IAlgorithm* (Mantid::API::FrameworkManager::*)(const std::string&) )&Mantid::API::FrameworkManager::createAlgorithm, return_value_policy< manage_new_object >())
        .def("createAlgorithm", (Mantid::API::IAlgorithm* (Mantid::API::FrameworkManager::*)(const std::string&, const std::string&) )&Mantid::API::FrameworkManager::createAlgorithm, return_value_policy< manage_new_object >())
        .def("exec", &Mantid::API::FrameworkManager::exec, return_value_policy< manage_new_object >())
        .def("getWorkspace", &Mantid::API::FrameworkManager::getWorkspace, return_value_policy< manage_new_object >())
    ;

    class_< Mantid::API::Workspace, boost::noncopyable, Mantid_Kernel_Workspace_Wrapper >("Workspace", no_init)
        .def("id", pure_virtual(&Mantid::API::Workspace::id))
        .def("getMemorySize", &Mantid::API::Workspace::getMemorySize, &Mantid_Kernel_Workspace_Wrapper::default_getMemorySize)
        .def("setTitle", &Mantid::API::Workspace::setTitle)
        .def("setComment", &Mantid::API::Workspace::setComment)
        .def("getComment", &Mantid::API::Workspace::getComment, return_value_policy< copy_const_reference >())
        .def("getTitle", &Mantid::API::Workspace::getTitle, return_value_policy< copy_const_reference >())
    ;
    
    class_< Mantid::Kernel::LibraryManager >("LibraryManager", init<  >())
        .def(init< const Mantid::Kernel::LibraryManager& >())
	.def("OpenLibrary", (bool (Mantid::Kernel::LibraryManager::*)(const std::string&) )&Mantid::Kernel::LibraryManager::OpenLibrary)
        .def("OpenLibrary", (bool (Mantid::Kernel::LibraryManager::*)(const std::string&, const std::string&) )&Mantid::Kernel::LibraryManager::OpenLibrary)
   ;
   
  class_< Mantid::API::Algorithm, boost::noncopyable, Mantid_API_Algorithm_Wrapper >("Algorithm", init<  >())
        .def("name", &Mantid::API::Algorithm::name, &Mantid_API_Algorithm_Wrapper::default_name, return_value_policy< copy_const_reference >())
        .def("version", &Mantid::API::Algorithm::version, &Mantid_API_Algorithm_Wrapper::default_version, return_value_policy< copy_const_reference >())
        .def("isInitialized", &Mantid::API::Algorithm::isInitialized, &Mantid_API_Algorithm_Wrapper::default_isInitialized)
        .def("isExecuted", &Mantid::API::Algorithm::isExecuted, &Mantid_API_Algorithm_Wrapper::default_isExecuted)
        .def("isFinalized", &Mantid::API::Algorithm::isFinalized, &Mantid_API_Algorithm_Wrapper::default_isFinalized)
        .def("setProperty", &Mantid::API::Algorithm::setProperty, &Mantid_API_Algorithm_Wrapper::default_setProperty)
        .def("existsProperty", &Mantid::API::Algorithm::existsProperty, &Mantid_API_Algorithm_Wrapper::default_existsProperty)
        .def("getPropertyValue", &Mantid::API::Algorithm::getPropertyValue, &Mantid_API_Algorithm_Wrapper::default_getPropertyValue)
        .def("getProperty", &Mantid::API::Algorithm::getProperty, &Mantid_API_Algorithm_Wrapper::default_getProperty, return_value_policy< manage_new_object >())
        .def("getProperties", &Mantid::API::Algorithm::getProperties, &Mantid_API_Algorithm_Wrapper::default_getProperties, return_value_policy< copy_const_reference >())
        .def("initialize", &Mantid::API::Algorithm::initialize)
        .def("execute", &Mantid::API::Algorithm::execute)
        .def("finalize", &Mantid::API::Algorithm::finalize)
        //.def("createSubAlgorithm", &Mantid::API::Algorithm::createSubAlgorithm)
        //.def("subAlgorithms", (const std::vector<Mantid::API::Algorithm*,std::allocator<Mantid::API::Algorithm*> >& (Mantid::API::Algorithm::*)() const)&Mantid::API::Algorithm::subAlgorithms, return_value_policy< copy_const_reference >())
        //.def("subAlgorithms", (std::vector<Mantid::API::Algorithm*,std::allocator<Mantid::API::Algorithm*> >& (Mantid::API::Algorithm::*)() )&Mantid::API::Algorithm::subAlgorithms, return_value_policy< copy_const_reference >())
    ;
   
   scope* Mantid_Kernel_StatusCode_scope = new scope(
    class_< Mantid::Kernel::StatusCode >("StatusCode", no_init)
        .def(init< const Mantid::Kernel::StatusCode& >())
    );

    scope().attr("FAILURE") = (int)Mantid::Kernel::StatusCode::FAILURE;
    scope().attr("SUCCESS") = (int)Mantid::Kernel::StatusCode::SUCCESS;
    scope().attr("RECOVERABLE") = (int)Mantid::Kernel::StatusCode::RECOVERABLE;

    delete Mantid_Kernel_StatusCode_scope;

}

