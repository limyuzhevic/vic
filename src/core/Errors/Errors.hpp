#pragma once

#include <string>
#include <stdexcept>
#include <sstream>

namespace nlm {

// Base error class for all NLM exceptions
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& message)
        : std::runtime_error(message) {}
    
    NLMError(const std::string& component, const std::string& operation, const std::string& message)
        : std::runtime_error(formatMessage(component, operation, message)) {}
    
protected:
    static std::string formatMessage(const std::string& component, const std::string& operation, const std::string& message) {
        std::ostringstream oss;
        oss << "[" << component << "] " << operation << ": " << message;
        return oss.str();
    }
};

// Configuration-specific error
class ConfigError : public NLMError {
public:
    explicit ConfigError(const std::string& message)
        : NLMError("Config", "error", message) {}
    
    ConfigError(const std::string& operation, const std::string& message)
        : NLMError("Config", operation, message) {}
};

// Validation error for configuration parameters
class ConfigValidationError : public ConfigError {
public:
    explicit ConfigValidationError(const std::string& parameter, const std::string& message)
        : ConfigError("validation", "Parameter \"" + parameter + "\": " + message) {}
};

// Memory allocation error
class MemoryError : public NLMError {
public:
    explicit MemoryError(const std::string& message)
        : NLMError("Memory", "error", message) {}
    
    MemoryError(const std::string& operation, const std::string& message)
        : NLMError("Memory", operation, message) {}
};

// File operation error
class FileError : public NLMError {
public:
    explicit FileError(const std::string& message)
        : NLMError("File", "error", message) {}
    
    FileError(const std::string& operation, const std::string& filename, const std::string& message)
        : NLMError("File", operation + " \"" + filename + "\": " + message) {}
};

// Configuration loading error
class ConfigLoadError : public ConfigError {
public:
    ConfigLoadError(const std::string& filename, const std::string& message)
        : ConfigError("load", "Failed to load configuration from \"" + filename + "\": " + message) {}
};

// Parameter setting error
class ParameterSetError : public ConfigError {
public:
    ParameterSetError(const std::string& parameter, const std::string& message)
        : ConfigError("set", "Failed to set parameter \"" + parameter + "\": " + message) {}
};

} // namespace nlm
