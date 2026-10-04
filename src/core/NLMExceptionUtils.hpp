// NLM Exception Classes for Python Bindings

#include "NLMErrors.hpp"
#include <pybind11/pybind11.h>

namespace py = pybind11;
namespace nlm {

/**
 * Register NLM exceptions with Python bindings
 * Provides Python-friendly exception classes for error handling
 */
inline void registerNLMErrors(py::module_& m) {
    // Register base NLM error class
    py::register_exception<NLMError>(m, "NLMError", PyExc_RuntimeError);
    
    // Register specific error classes
    py::register_exception<ConfigError>(m, "ConfigError", PyExc_ValueError);
    py::register_exception<BrainError>(m, "BrainError", PyExc_RuntimeError);
    py::register_exception<NeuronError>(m, "NeuronError", PyExc_RuntimeError);
    py::register_exception<MemoryError>(m, "MemoryError", PyExc_RuntimeError);
    py::register_exception<WorldError>(m, "WorldError", PyExc_RuntimeError);
    py::register_exception<AgentError>(m, "AgentError", PyExc_RuntimeError);
}

/**
 * Macro for converting C++ exceptions to Python exceptions
 * Provides consistent error handling throughout the codebase
 */
#define CATCH_NLM_EXCEPTIONS() \
    catch (const NLMError& e) { \
        throw py::value_error(e.what()); \
    } \
    catch (const std::exception& e) { \
        throw py::value_error(std::string("Unexpected error: ") + e.what()); \
    } \
    catch (...) { \
        throw py::value_error("Unknown error occurred"); \
    }

/**
 * Macro for validating parameters in Python bindings
 * Provides consistent parameter validation across the codebase
 */
#define VALIDATE_PARAM(param_name, condition, error_message) \
    do { \
        if (!(condition)) { \
            throw py::value_error(error_message); \
        } \
    } while(0)

/**
 * Macro for validating pointer arguments
 * Prevents null pointer dereferences in Python bindings
 */
#define VALIDATE_PTR(ptr, error_message) \
    do { \
        if (!(ptr)) { \
            throw py::value_error(error_message); \
        } \
    } while(0)

} // namespace nlm
