#pragma once

#include <string>
#include <stdexcept>

namespace nlm {

/**
 * Base exception class for all NLM errors
 */
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& message)
        : std::runtime_error(message) {}
    
    virtual ~NLMError() = default;
};

/**
 * Configuration-related errors
 */
class ConfigError : public NLMError {
public:
    explicit ConfigError(const std::string& message)
        : NLMError("Configuration Error: " + message) {}
    
    ConfigError(const std::string& message, const std::string& key)
        : NLMError("Configuration Error at key \"" + key + "\": " + message) {}
    
    virtual ~ConfigError() = default;
};

/**
 * Brain-related errors
 */
class BrainError : public NLMError {
public:
    explicit BrainError(const std::string& message)
        : NLMError("Brain Error: " + message) {}
    
    BrainError(const std::string& message, const std::string& operation)
        : NLMError("Brain Error during \"" + operation + "\": " + message) {}
    
    virtual ~BrainError() = default;
};

/**
 * Memory system errors
 */
class MemoryError : public NLMError {
public:
    explicit MemoryError(const std::string& message)
        : NLMError("Memory Error: " + message) {}
    
    MemoryError(const std::string& message, int systemId)
        : NLMError("Memory Error in system " + std::to_string(systemId) + ": " + message) {}
    
    virtual ~MemoryError() = default;
};

/**
 * Validation and parameter errors
 */
class ValidationError : public NLMError {
public:
    explicit ValidationError(const std::string& message)
        : NLMError("Validation Error: " + message) {}
    
    ValidationError(const std::string& message, const std::string& parameter)
        : NLMError("Validation Error for parameter \"" + parameter + "\": " + message) {}
    
    virtual ~ValidationError() = default;
};

/**
 * File I/O errors
 */
class FileError : public NLMError {
public:
    explicit FileError(const std::string& message)
        : NLMError("File Error: " + message) {}
    
    FileError(const std::string& message, const std::string& filepath)
        : NLMError("File Error in \"" + filepath + "\": " + message) {}
    
    virtual ~FileError() = default;
};

/**
 * Integration and connectivity errors
 */
class IntegrationError : public NLMError {
public:
    explicit IntegrationError(const std::string& message)
        : NLMError("Integration Error: " + message) {}
    
    IntegrationError(const std::string& message, const std::string& system)
        : NLMError("Integration Error between \"" + system + "\": " + message) {}
    
    virtual ~IntegrationError() = default;
};

/**
 * Simulation and runtime errors
 */
class SimulationError : public NLMError {
public:
    explicit SimulationError(const std::string& message)
        : NLMError("Simulation Error: " + message) {}
    
    SimulationError(const std::string& message, int step)
        : NLMError("Simulation Error at step " + std::to_string(step) + ": " + message) {}
    
    virtual ~SimulationError() = default;
};

} // namespace nlm
