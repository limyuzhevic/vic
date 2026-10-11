// nlm/core/Exception/NLMException.hpp
#pragma once

#include <string>
#include <stdexcept>

namespace nlm {

/**
 * @class NLMException
 * @brief Base exception class for NLM errors
 * 
 * This is the base class for all NLM exceptions. It provides a common
 * interface for error reporting and handling.
 */
class NLMException : public std::runtime_error {
public:
    /**
     * @brief Construct a new NLMException object
     * 
     * @param message Error message describing the problem
     */
    explicit NLMException(const std::string& message) 
        : std::runtime_error(message) {}
    
    /**
     * @brief Get the error category for this exception
     * 
     * @return std::string Error category
     */
    virtual std::string getCategory() const { return "NLMException"; }
};

/**
 * @class ConfigException
 * @brief Base class for configuration-related errors
 * 
 * This exception is thrown when there are errors in configuration loading,
 * parsing, or validation.
 */
class ConfigException : public NLMException {
public:
    explicit ConfigException(const std::string& message) 
        : NLMException("Configuration Error: " + message) {}
    
    std::string getCategory() const override { return "ConfigException"; }
};

/**
 * @class ConfigFileNotFoundException
 * @brief Thrown when a configuration file is not found
 * 
 * This exception is thrown when an attempt is made to load a configuration
 * file that does not exist or cannot be accessed.
 */
class ConfigFileNotFoundException : public ConfigException {
public:
    explicit ConfigFileNotFoundException(const std::string& filepath) 
        : ConfigException("Configuration file not found: " + filepath) {}
    
    std::string getCategory() const override { return "ConfigFileNotFoundException"; }
};

/**
 * @class ConfigParseException
 * @brief Thrown when a configuration file cannot be parsed
 * 
 * This exception is thrown when there is an error parsing a configuration
 * file, such as invalid syntax or format.
 */
class ConfigParseException : public ConfigException {
public:
    explicit ConfigParseException(const std::string& message) 
        : ConfigException("Configuration parse error: " + message) {}
    
    std::string getCategory() const override { return "ConfigParseException"; }
};

/**
 * @class ConfigFileWriteException
 * @brief Thrown when a configuration file cannot be written
 * 
 * This exception is thrown when an attempt is made to save a configuration
 * file but the operation fails due to I/O errors or permissions.
 */
class ConfigFileWriteException : public ConfigException {
public:
    explicit ConfigFileWriteException(const std::string& filepath) 
        : ConfigException("Cannot write configuration file: " + filepath) {}
    
    std::string getCategory() const override { return "ConfigFileWriteException"; }
};

/**
 * @class ConfigValidationException
 * @brief Thrown when configuration values are invalid
 * 
 * This exception is thrown when configuration validation fails, such as
 * when a value is outside the valid range or has an incorrect type.
 */
class ConfigValidationException : public ConfigException {
public:
    explicit ConfigValidationException(const std::string& message) 
        : ConfigException("Configuration validation error: " + message) {}
    
    std::string getCategory() const override { return "ConfigValidationException"; }
};

/**
 * @class BrainInitializationException
 * @brief Thrown when brain initialization fails
 * 
 * This exception is thrown when the brain fails to initialize properly,
 * such as when there are insufficient resources or invalid configuration.
 */
class BrainInitializationException : public NLMException {
public:
    explicit BrainInitializationException(const std::string& message) 
        : NLMException("Brain Initialization Error: " + message) {}
    
    std::string getCategory() const override { return "BrainInitializationException"; }
};

/**
 * @class MemoryAllocationException
 * @brief Thrown when memory allocation fails
 * 
 * This exception is thrown when the system cannot allocate the required
 * memory for neural structures or data.
 */
class MemoryAllocationException : public NLMException {
public:
    explicit MemoryAllocationException(const std::string& message) 
        : NLMException("Memory Allocation Error: " + message) {}
    
    std::string getCategory() const override { return "MemoryAllocationException"; }
};

/**
 * @class SimulationStepException
 * @brief Thrown when simulation step fails
 * 
 * This exception is thrown when an error occurs during simulation steps,
 * such as numerical instability or invalid state transitions.
 */
class SimulationStepException : public NLMException {
public:
    explicit SimulationStepException(const std::string& message) 
        : NLMException("Simulation Step Error: " + message) {}
    
    std::string getCategory() const override { return "SimulationStepException"; }
};

/**
 * @brief Handle NLM exceptions with detailed logging
 * 
 * This function provides a centralized way to handle NLM exceptions,
 * ensuring proper error logging and reporting.
 * 
 * @param e The exception to handle
 */
inline void handleNLMException(const NLMException& e) {
    // Log the exception with category information
    NLM_LOG_ERROR("NLM Exception [" + std::string(e.getCategory()) + "]: " + std::string(e.what()));
    
    // Could add additional handling here, such as:
    // - Sending error reports to monitoring systems
    // - Creating diagnostic dumps
    // - Triggering cleanup procedures
}

/**
 * @brief Convert NLM exception to string for logging
 * 
 * @param e The exception to convert
 * @return std::string String representation of the exception
 */
inline std::string formatNLMException(const NLMException& e) {
    std::stringstream ss;
    ss << "[" << e.getCategory() << "] " << e.what();
    return ss.str();
}

} // namespace nlm