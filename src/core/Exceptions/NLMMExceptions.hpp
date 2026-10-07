#pragma once

#include <string>
#include <stdexcept>

namespace nlm {

// Base exception for all NLM errors
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& message) : std::runtime_error(message) {}
};

// Configuration-specific exceptions
class ConfigException : public NLMException {
public:
    explicit ConfigException(const std::string& message) : NLMException("Configuration Error: " + message) {}
};

class ConfigParseException : public ConfigException {
public:
    explicit ConfigParseException(const std::string& message) : ConfigException("Parse Error: " + message) {}
};

class ConfigValidationException : public ConfigException {
public:
    explicit ConfigValidationException(const std::string& message) : ConfigException("Validation Error: " + message) {}
};

class ConfigFileException : public ConfigException {
public:
    explicit ConfigFileException(const std::string& message) : ConfigException("File Error: " + message) {}
};

// Logger-specific exceptions
class LogException : public NLMException {
public:
    explicit LogException(const std::string& message) : NLMException("Logger Error: " + message) {}
};

class LogFileException : public LogException {
public:
    explicit LogFileException(const std::string& message) : LogException("File Error: " + message) {}
};

// Random number generator exceptions
class RandomException : public NLMException {
public:
    explicit RandomException(const std::string& message) : NLMException("Random Error: " + message) {}
};

// Brain/Neural exceptions
class BrainException : public NLMException {
public:
    explicit BrainException(const std::string& message) : NLMException("Brain Error: " + message) {}
};

class NeuronException : public BrainException {
public:
    explicit NeuronException(const std::string& message) : BrainException("Neuron Error: " + message) {}
};

// Environment exceptions
class EnvironmentException : public NLMException {
public:
    explicit EnvironmentException(const std::string& message) : NLMException("Environment Error: " + message) {}
};

// Memory exceptions
class MemoryException : public NLMException {
public:
    explicit MemoryException(const std::string& message) : NLMException("Memory Error: " + message) {}
};

// Plasticity exceptions
class PlasticityException : public NLMException {
public:
    explicit PlasticityException(const std::string& message) : NLMException("Plasticity Error: " + message) {}
};

// Experiment exceptions
class ExperimentException : public NLMException {
public:
    explicit ExperimentException(const std::string& message) : NLMException("Experiment Error: " + message) {}
};

// Utility exception classes for better error categorization
class ValidationError : public NLMException {
public:
    explicit ValidationError(const std::string& message) : NLMException("Validation Error: " + message) {}
};

class InitializationError : public NLMException {
public:
    explicit InitializationError(const std::string& message) : NLMException("Initialization Error: " + message) {}
};

class ResourceError : public NLMException {
public:
    explicit ResourceError(const std::string& message) : NLMException("Resource Error: " + message) {}
};

class OperationError : public NLMException {
public:
    explicit OperationError(const std::string& message) : NLMException("Operation Error: " + message) {}
};

// Exception with error code support
class ErrorCodeException : public NLMException {
public:
    ErrorCodeException(const std::string& message, int errorCode) 
        : NLMException(message), errorCode_(errorCode) {}
    
    int errorCode() const { return errorCode_; }
    
private:
    int errorCode_;
};

} // namespace nlm