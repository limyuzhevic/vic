#pragma once

#include <stdexcept>
#include <string>
#include <memory>
#include <pybind11/pybind11.h>

namespace py = pybind11;
namespace nlm {

// Base exception for all NLM errors
class NLMException : public std::exception {
public:
    explicit NLMException(const std::string& message) : message_(message) {}
    const char* what() const noexcept override { return message_.c_str(); }
    virtual const char* type() const noexcept { return "NLMException"; }
protected:
    std::string message_;
};

// Configuration-related errors
class InvalidArgumentError : public NLMException {
public:
    explicit InvalidArgumentError(const std::string& message) 
        : NLMException("InvalidArgumentError: " + message) {}
    const char* type() const noexcept override { return "InvalidArgumentError"; }
};

class ConfigError : public NLMException {
public:
    explicit ConfigError(const std::string& message) 
        : NLMException("ConfigError: " + message) {}
    const char* type() const noexcept { return "ConfigError"; }
};

// Simulation-related errors
class SimulationError : public NLMException {
public:
    explicit SimulationError(const std::string& message) 
        : NLMException("SimulationError: " + message) {}
    const char* type() const noexcept override { return "SimulationError"; }
};

class InitializationError : public SimulationError {
public:
    explicit InitializationError(const std::string& message) 
        : SimulationError("InitializationError: " + message) {}
    const char* type() const noexcept override { return "InitializationError"; }
};

// Memory-related errors
class MemoryError : public NLMException {
public:
    explicit MemoryError(const std::string& message) 
        : NLMException("MemoryError: " + message) {}
    const char* type() const noexcept override { return "MemoryError"; }
};

// Resource management errors
class ResourceError : public NLMException {
public:
    explicit ResourceError(const std::string& message) 
        : NLMException("ResourceError: " + message) {}
    const char* type() const noexcept override { return "ResourceError"; }
};

// Forward declarations for error registration
class Config;
class Brain;
class SimpleWorld;
class AgentBrain;

// Utility function to convert C++ exceptions to Python exceptions
inline void register_nlm_exceptions(py::module_& m) {
    // Register custom exceptions
    py::register_exception<NLMException>(m, "NLMException", PyExc_RuntimeError);
    py::register_exception<InvalidArgumentError>(m, "InvalidArgumentError", PyExc_ValueError);
    py::register_exception<ConfigError>(m, "ConfigError", PyExc_RuntimeError);
    py::register_exception<SimulationError>(m, "SimulationError", PyExc_RuntimeError);
    py::register_exception<InitializationError>(m, "InitializationError", PyExc_RuntimeError);
    py::register_exception<MemoryError>(m, "MemoryError", PyExc_RuntimeError);
    py::register_exception<ResourceError>(m, "ResourceError", PyExc_RuntimeError);
    
    // Register std::runtime_error as fallback
    py::register_exception<std::runtime_error>(m, "RuntimeError", PyExc_RuntimeError);
}

// Helper function for automatic exception conversion
try_and_catch
inline py::object convert_to_py_exception(const std::exception& e) {
    const char* what = e.what();
    
    // Try to match exception types
    try {
        throw; // Re-throw to use our exception handling
    } catch (const NLMException& ex) {
        return py::cast(ex);
    } catch (const InvalidArgumentError& ex) {
        return py::cast(ex);
    } catch (const ConfigError& ex) {
        return py::cast(ex);
    } catch (const SimulationError& ex) {
        return py::cast(ex);
    } catch (const InitializationError& ex) {
        return py::cast(ex);
    } catch (const MemoryError& ex) {
        return py::cast(ex);
    } catch (const ResourceError& ex) {
        return py::cast(ex);
    } catch (const std::runtime_error& ex) {
        return py::cast(ex);
    } catch (const std::exception& ex) {
        return py::cast(NLMException(std::string("Unknown exception: ") + what));
    }
    
    return py::cast(NLMException("Unknown exception"));
}

} // namespace nlm
