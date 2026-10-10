// NLM Python Bindings Exceptions
// Custom Python exceptions for better error handling

#pragma once

#include <pybind11/pybind11.h>
#include <string>

namespace py = pybind11;
namespace nlm {

// Base exception for all NLM Python errors
class NLMError : public py::exception_base {
public:
    using py::exception_base::exception_base;
    
    static void register_exception(py::module_& m) {
        py::register_exception_translator([](std::exception_ptr p) {
            try {
                if (p) std::rethrow_exception(p);
            } catch (const NLMError& e) {
                PyErr_SetString(e.ptr(), e.what());
            } catch (const std::runtime_error& e) {
                PyErr_SetString(PyExc_RuntimeError, e.what());
            } catch (const std::invalid_argument& e) {
                PyErr_SetString(PyExc_ValueError, e.what());
            } catch (const std::out_of_range& e) {
                PyErr_SetString(PyExc_IndexError, e.what());
            } catch (const std::exception& e) {
                PyErr_SetString(PyExc_RuntimeError, e.what());
            }
        });
    }
};

// Configuration error
class ConfigError : public NLMError {
public:
    ConfigError(const std::string& msg) : NLMError(msg) {}
    static void register(py::module_& m) {
        py::register_exception<ConfigError>(m, "ConfigError", PyExc_RuntimeError);
    }
};

// Brain error
class BrainError : public NLMError {
public:
    BrainError(const std::string& msg) : NLMError(msg) {}
    static void register(py::module_& m) {
        py::register_exception<BrainError>(m, "BrainError", PyExc_RuntimeError);
    }
};

// World error
class WorldError : public NLMError {
public:
    WorldError(const std::string& msg) : NLMError(msg) {}
    static void register(py::module_& m) {
        py::register_exception<WorldError>(m, "WorldError", PyExc_RuntimeError);
    }
};

// Agent error
class AgentError : public NLMError {
public:
    AgentError(const std::string& msg) : NLMError(msg) {}
    static void register(py::module_& m) {
        py::register_exception<AgentError>(m, "AgentError", PyExc_RuntimeError);
    }
};

// Validation error
class ValidationError : public NLMError {
public:
    ValidationError(const std::string& msg) : NLMError(msg) {}
    static void register(py::module_& m) {
        py::register_exception<ValidationError>(m, "ValidationError", PyExc_ValueError);
    }
};

// Memory error
class MemoryError : public NLMError {
public:
    MemoryError(const std::string& msg) : NLMError(msg) {}
    static void register(py::module_& m) {
        py::register_exception<MemoryError>(m, "MemoryError", PyExc_MemoryError);
    }
};

} // namespace nlm
