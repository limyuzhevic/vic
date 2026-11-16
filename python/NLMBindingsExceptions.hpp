// Custom Exception Hierarchy for Python Bindings
// Provides better error handling and specific exception types

#ifndef NLM_BINDINGS_EXCEPTIONS_HPP
#define NLM_BINDINGS_EXCEPTIONS_HPP

#include <pybind11/pybind11.h>
#include <stdexcept>
#include <string>

namespace py = pybind11;

// Base NLM exception
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& msg) : std::runtime_error(msg) {}
};

// Specific NLM exceptions
class ConfigError : public NLMException {
public:
    explicit ConfigError(const std::string& msg) : NLMException("ConfigError: " + msg) {}
};

class BrainError : public NLMException {
public:
    explicit BrainError(const std::string& msg) : NLMException("BrainError: " + msg) {}
};

class MemoryError : public NLMException {
public:
    explicit MemoryError(const std::string& msg) : NLMException("MemoryError: " + msg) {}
};

class PlasticityError : public NLMException {
public:
    explicit PlasticityError(const std::string& msg) : NLMException("PlasticityError: " + msg) {}
};

class DevelopmentError : public NLMException {
public:
    explicit DevelopmentError(const std::string& msg) : NLMException("DevelopmentError: " + msg) {}
};

class CognitionError : public NLMException {
public:
    explicit CognitionError(const std::string& msg) : NLMException("CognitionError: " + msg) {}
};

class NeuromodulationError : public NLMException {
public:
    explicit NeuromodulationError(const std::string& msg) : NLMException("NeuromodulationError: " + msg) {}
};

// Register exception handlers for Python
inline void registerNLMBindingsExceptions(py::module_& m) {
    py::register_exception<NLMException, std::runtime_error>(m, "NLMException");
    py::register_exception<ConfigError, NLMException>(m, "ConfigError");
    py::register_exception<BrainError, NLMException>(m, "BrainError");
    py::register_exception<MemoryError, NLMException>(m, "MemoryError");
    py::register_exception<PlasticityError, NLMException>(m, "PlasticityError");
    py::register_exception<DevelopmentError, NLMException>(m, "DevelopmentError");
    py::register_exception<CognitionError, NLMException>(m, "CognitionError");
    py::register_exception<NeuromodulationError, NLMException>(m, "NeuromodulationError");
}

#endif // NLM_BINDINGS_EXCEPTIONS_HPP
