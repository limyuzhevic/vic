#pragma once

#include <string>
#include <stdexcept>
#include <system_error>

namespace nlm {

// Base exception class for all NLM errors
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& message) 
        : std::runtime_error(message) {}
    
    NLMException(const std::string& message, const std::string& details)
        : std::runtime_error(formatMessage(message, details)) {}
    
protected:
    static std::string formatMessage(const std::string& message, const std::string& details) {
        return "NLM Error: " + message + ". Details: " + details;
    }
};

// Configuration-related exceptions
class ConfigException : public NLMException {
public:
    explicit ConfigException(const std::string& message) : NLMException(message) {}
    ConfigException(const std::string& message, const std::string& details) 
        : NLMException(message, details) {}
};

class ConfigValidationException : public ConfigException {
public:
    explicit ConfigValidationException(const std::string& message) : ConfigException(message) {}
    ConfigValidationException(const std::string& message, const std::string& details) 
        : ConfigException(message, details) {}
};

class ConfigLoadException : public ConfigException {
public:
    explicit ConfigLoadException(const std::string& message) : ConfigException(message) {}
    ConfigLoadException(const std::string& message, const std::string& details) 
        : ConfigException(message, details) {}
};

// Brain/Neural system exceptions
class BrainException : public NLMException {
public:
    explicit BrainException(const std::string& message) : NLMException(message) {}
    BrainException(const std::string& message, const std::string& details) 
        : NLMException(message, details) {}
};

class InitializationException : public BrainException {
public:
    explicit InitializationException(const std::string& message) : BrainException(message) {}
    InitializationException(const std::string& message, const std::string& details) 
        : BrainException(message, details) {}
};

class InvalidNeuronException : public BrainException {
public:
    explicit InvalidNeuronException(const std::string& message) : BrainException(message) {}
    InvalidNeuronException(const std::string& message, const std::string& details) 
        : BrainException(message, details) {}
};

// Memory system exceptions
class MemoryException : public NLMException {
public:
    explicit MemoryException(const std::string& message) : NLMException(message) {}
    MemoryException(const std::string& message, const std::string& details) 
        : NLMException(message, details) {}
};

// Neuromodulation exceptions
class NeuromodulationException : public NLMException {
public:
    explicit NeuromodulationException(const std::string& message) : NLMException(message) {}
    NeuromodulationException(const std::string& message, const std::string& details) 
        : NLMException(message, details) {}
};

// Development system exceptions
class DevelopmentException : public NLMException {
public:
    explicit DevelopmentException(const std::string& message) : NLMException(message) {}
    DevelopmentException(const std::string& message, const std::string& details) 
        : NLMException(message, details) {}
};

// Experimental setup exceptions
class ExperimentException : public NLMException {
public:
    explicit ExperimentException(const std::string& message) : NLMException(message) {}
    ExperimentException(const std::string& message, const std::string& details) 
        : NLMException(message, details) {}
};

// Exception helper functions
namespace ExceptionHelpers {
    // Validate configuration value ranges
    void validatePositive(const std::string& key, double value, double max = 1e6);
    void validateNonNegative(const std::string& key, double value, double max = 1e6);
    void validateRange(const std::string& key, double value, double min, double max);
    void validateEnum(const std::string& key, int value, int max);
    
    // Convert error codes to human-readable messages
    std::string errorCodeToMessage(int errorCode);
    
    // Get system error information
    std::string getSystemErrorMessage();
}

} // namespace nlm