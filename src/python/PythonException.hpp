// Python Exception Mapping for NLM
// Comprehensive mapping from C++ errors to Python exceptions for seamless integration

#ifndef NLM_PYTHON_EXCEPTION_HPP
#define NLM_PYTHON_EXCEPTION_HPP

#include <string>
#include <map>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "Errors.hpp"

// Python exception class wrapper
class PythonNLMException : public pybind11::python_type {
public:
    PythonNLMException(const std::string& message, const std::string& category = "CORE")
        : message_(message), category_(category) {
        // Convert C++ exception message to Python exception
        pybind11::str py_message(message.c_str());
        pybind11::str py_category(category.c_str());
        
        // Initialize base Python exception with message
        this->args = pybind11::make_tuple(py_message);
        this->kwargs["category"] = py_category;
        this->kwargs["message"] = py_message;
    }
    
    const std::string& getMessage() const { return message_; }
    const std::string& getCategory() const { return category_; }
    
    static std::string getCategoryString(ErrorCategory category) {
        static const std::map<ErrorCategory, std::string> categoryStrings = {
            {ErrorCategory::CORE, "CORE"},
            {ErrorCategory::CONFIGURATION, "VALUE"},
            {ErrorCategory::MEMORY, "MEMORY"},
            {ErrorCategory::VALIDATION, "TYPE"},
            {ErrorCategory::FILESYSTEM, "OS"},
            {ErrorCategory::NEURAL, "RUNTIME"},
            {ErrorCategory::WORLD, "RUNTIME"},
            {ErrorCategory::PYTHON, "RUNTIME"},
            {ErrorCategory::PERFORMANCE, "RUNTIME"}
        };
        auto it = categoryStrings.find(category);
        return it != categoryStrings.end() ? it->second : "RuntimeError";
    }
    
    // Convert C++ NLMException to Python exception
    static pybind11::error_already_set convertToPythonException(const NLMException& cppException) {
        pybind11::str py_message(cppException.getMessage().c_str());
        pybind11::str py_category(getCategoryString(cppException.getCategory()).c_str());
        
        // Set the Python exception
        PyErr_SetString(pybind11::reinterpret_borrow<PyObject>(PyExc_RuntimeError), 
                       cppException.getMessage().c_str());
        
        // Add additional attributes if possible
        // This would require custom Python exception class setup
        
        // Return error already set
        return pybind11::error_already_set();
    }
    
    // Python exception mappings for each NLM error type
    static pybind11::error_already_set mapException(const NLMException& cppException) {
        switch (cppException.getCategory()) {
            case ErrorCategory::CONFIGURATION:
                PyErr_SetString(pybind11::reinterpret_borrow<PyObject>(PyExc_ValueError), 
                               cppException.getMessage().c_str());
                break;
            case ErrorCategory::MEMORY:
                PyErr_SetString(pybind11::reinterpret_borrow<PyObject>(PyExc_MemoryError), 
                               cppException.getMessage().c_str());
                break;
            case ErrorCategory::VALIDATION:
                PyErr_SetString(pybind11::reinterpret_borrow<PyObject>(PyExc_TypeError), 
                               cppException.getMessage().c_str());
                break;
            case ErrorCategory::FILESYSTEM:
                PyErr_SetString(pybind11::reinterpret_borrow<PyObject>(PyExc_OSError), 
                               cppException.getMessage().c_str());
                break;
            case ErrorCategory::NEURAL:
            case ErrorCategory::WORLD:
            case ErrorCategory::PYTHON:
            case ErrorCategory::PERFORMANCE:
            case ErrorCategory::CORE:
            default:
                PyErr_SetString(pybind11::reinterpret_borrow<PyObject>(PyExc_RuntimeError), 
                               cppException.getMessage().c_str());
                break;
        }
        
        return pybind11::error_already_set();
    }
    
    // Get Python exception class for a given error category
    static pybind11::handle getPythonExceptionClass(ErrorCategory category) {
        static const std::map<ErrorCategory, PyObject*> exceptionClasses = {
            {ErrorCategory::CONFIGURATION, PyExc_ValueError},
            {ErrorCategory::MEMORY, PyExc_MemoryError},
            {ErrorCategory::VALIDATION, PyExc_TypeError},
            {ErrorCategory::FILESYSTEM, PyExc_OSError},
            {ErrorCategory::NEURAL, PyExc_RuntimeError},
            {ErrorCategory::WORLD, PyExc_RuntimeError},
            {ErrorCategory::PYTHON, PyExc_RuntimeError},
            {ErrorCategory::PERFORMANCE, PyExc_RuntimeError},
            {ErrorCategory::CORE, PyExc_RuntimeError}
        };
        
        auto it = exceptionClasses.find(category);
        if (it != exceptionClasses.end()) {
            return pybind11::reinterpret_borrow<pybind11::handle>(it->second);
        }
        return pybind11::reinterpret_borrow<pybind11::handle>(PyExc_RuntimeError);
    }
    
private:
    std::string message_;
    std::string category_;
};

// Convenience functions for exception mapping
inline pybind11::error_already_set convertException(const NLMException& cppException) {
    return PythonNLMException::mapException(cppException);
}

inline pybind11::error_already_set convertException(const std::exception& stdException) {
    // Convert standard C++ exceptions to Python exceptions
    try {
        throw; // Re-throw to get the exception type
    } catch (const std::runtime_error& e) {
        PyErr_SetString(PyExc_RuntimeError, e.what());
    } catch (const std::invalid_argument& e) {
        PyErr_SetString(PyExc_ValueError, e.what());
    } catch (const std::out_of_range& e) {
        PyErr_SetString(PyExc_IndexError, e.what());
    } catch (const std::length_error& e) {
        PyErr_SetString(PyExc_MemoryError, e.what());
    } catch (...) {
        PyErr_SetString(PyExc_RuntimeError, "Unknown exception occurred");
    }
    return pybind11::error_already_set();
}

// Macro for C++ to Python exception conversion in bindings
#define NLM_CPP_TO_PYTHON_EXCEPTION(cpp_exception) \
    try { \
        cpp_exception; \
    } catch (const NLMException& e) { \
        return PythonNLMException::mapException(e); \
    } catch (...) { \
        return PythonNLMException::mapException( \
            NLMException("Unknown exception occurred") \
        ); \
    }

// Macro for Python exception throwing in C++ code (from Python bindings)
#define NLM_PYTHON_THROW(message, category) \
    do { \
        std::string errorMsg = message; \
        ErrorCategory cat = ErrorCategory::CORE; \
        if (category == "CONFIGURATION") cat = ErrorCategory::CONFIGURATION; \
        else if (category == "MEMORY") cat = ErrorCategory::MEMORY; \
        else if (category == "VALIDATION") cat = ErrorCategory::VALIDATION; \
        else if (category == "FILESYSTEM") cat = ErrorCategory::FILESYSTEM; \
        else if (category == "NEURAL") cat = ErrorCategory::NEURAL; \
        else if (category == "WORLD") cat = ErrorCategory::WORLD; \
        else if (category == "PYTHON") cat = ErrorCategory::PYTHON; \
        else if (category == "PERFORMANCE") cat = ErrorCategory::PERFORMANCE; \
        throw NLMException(errorMsg, cat, LogLevel::ERROR, NLM_CURRENT_LOCATION); \
    } while(0)

// Helper to extract error information for Python
inline pybind11::dict getErrorInfo(const NLMException& error) {
    pybind11::dict info;
    info["message"] = error.getMessage();
    info["category"] = PythonNLMException::getCategoryString(error.getCategory());
    info["severity"] = error.getSeverityString(error.getSeverity());
    info["file"] = error.getContext().location.file;
    info["line"] = error.getContext().location.line;
    info["function"] = error.getContext().location.function;
    info["operation"] = error.getContext().operation;
    
    // Add context values
    pybind11::dict context_dict;
    for (const auto& pair : error.getContext().values) {
        context_dict[pair.first] = pair.second;
    }
    info["context"] = context_dict;
    
    return info;
}

// Exception handling in Python bindings
class PythonExceptionHandler {
public:
    // Wrap a C++ function call and convert exceptions to Python
    template<typename Func, typename... Args>
    static auto wrapFunction(Func&& func, Args&&... args) -> decltype(func(std::forward<Args>(args)...)) {
        try {
            return func(std::forward<Args>(args)...);
        } catch (const NLMException& e) {
            return PythonNLMException::mapException(e);
        } catch (const std::exception& e) {
            return PythonNLMException::mapException(
                NLMException(std::string("Standard C++ exception: ") + e.what())
            );
        }
    }
    
    // Batch exception handling for multiple operations
    template<typename Func, typename... Args>
    static std::vector<decltype(func(std::forward<Args>(args)...))> 
    wrapBatchFunction(Func&& func, const std::vector<Args...>& argsList) {
        std::vector<decltype(func(std::forward<Args>(args)...))> results;
        results.reserve(argsList.size());
        
        for (const auto& args : argsList) {
            try {
                auto result = func(args...);
                results.push_back(result);
            } catch (const NLMException& e) {
                results.push_back(PythonNLMException::mapException(e));
            } catch (const std::exception& e) {
                results.push_back(PythonNLMException::mapException(
                    NLMException(std::string("Standard C++ exception: ") + e.what())
                ));
            }
        }
        
        return results;
    }
};

#endif // NLM_PYTHON_EXCEPTION_HPP