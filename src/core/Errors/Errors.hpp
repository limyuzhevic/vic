#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <pybind11/pybind11.h>

namespace nlm {

// Base exception class for all NLM errors
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& message) 
        : std::runtime_error("NLM: " + message) {}
    
    virtual ~NLMError() = default;
};

// Configuration-related errors
class ConfigurationError : public NLMError {
public:
    explicit ConfigurationError(const std::string& message)
        : NLMError("Configuration: " + message) {}
};

class ValidationError : public NLMError {
public:
    explicit ValidationError(const std::string& message)
        : NLMError("Validation: " + message) {}
};

// Brain-related errors
class BrainError : public NLMError {
public:
    explicit BrainError(const std::string& message)
        : NLMError("Brain: " + message) {}
};

class InitializationError : public BrainError {
public:
    explicit InitializationError(const std::string& message)
        : BrainError("Initialization failed: " + message) {}
};

class SimulationError : public BrainError {
public:
    explicit SimulationError(const std::string& message)
        : BrainError("Simulation error: " + message) {}
};

// World-related errors
class WorldError : public NLMError {
public:
    explicit WorldError(const std::string& message)
        : NLMError("World: " + message) {}
};

// I/O related errors
class IOError : public NLMError {
public:
    explicit IOError(const std::string& message)
        : NLMError("IO: " + message) {}
};

// Plugin and extensibility errors
class PluginError : public NLMError {
public:
    explicit PluginError(const std::string& message)
        : NLMError("Plugin: " + message) {}
};

// Helper function to create formatted error messages
inline std::string formatError(const char* file, int line, const char* function, 
                               const std::string& message) {
    std::ostringstream oss;
    oss << "[" << file << ":" << line << " " << function << "] " << message;
    return oss.str();
}

// Macro for easy error throwing in Python bindings
#define THROW_NLM_ERROR(msg) throw nlm::NLMError(formatError(__FILE__, __LINE__, __func__, msg))

#define THROW_VALIDATION_ERROR(msg) throw nlm::ValidationError(formatError(__FILE__, __LINE__, __func__, msg))

#define THROW_CONFIGURATION_ERROR(msg) throw nlm::ConfigurationError(formatError(__FILE__, __LINE__, __func__, msg))

#define THROW_BRAIN_ERROR(msg) throw nlm::BrainError(formatError(__FILE__, __LINE__, __func__, msg))

#define THROW_WORLD_ERROR(msg) throw nlm::WorldError(formatError(__FILE__, __LINE__, __func__, msg))

#define THROW_IO_ERROR(msg) throw nlm::IOError(formatError(__FILE__, __LINE__, __func__, msg))

#define THROW_PLUGIN_ERROR(msg) throw nlm::PluginError(formatError(__FILE__, __LINE__, __func__, msg))

} // namespace nlm
