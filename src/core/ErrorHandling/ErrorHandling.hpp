#pragma once

#include <string>
#include <system_error>
#include <stdexcept>
#include <format>

namespace nlm {

// Error categories for NLM
enum class NLMErrorCategory {
    ConfigError = 1,
    MemoryError = 2,
    NeuralError = 3,
    PlasticityError = 4,
    IntegrationError = 5,
    SystemError = 6
};

// Custom exception classes
class NLMException : public std::runtime_error {
public:
    NLMException(const std::string& message, NLMErrorCategory category)
        : std::runtime_error(message), category_(category) {}
    
    NLMErrorCategory getCategory() const { return category_; }
    
private:
    NLMErrorCategory category_;
};

class ConfigException : public NLMException {
public:
    ConfigException(const std::string& message, ConfigErrorCode code)
        : NLMException("Config Error: " + message, NLMErrorCategory::ConfigError), code_(code) {}
    
    ConfigErrorCode getErrorCode() const { return code_; }
    
private:
    ConfigErrorCode code_;
};

class MemoryException : public NLMException {
public:
    MemoryException(const std::string& message, MemoryErrorCode code)
        : NLMException("Memory Error: " + message, NLMErrorCategory::MemoryError), code_(code) {}
    
    MemoryErrorCode getErrorCode() const { return code_; }
    
private:
    MemoryErrorCode code_;
};

class NeuralException : public NLMException {
public:
    NeuralException(const std::string& message, NeuralErrorCode code)
        : NLMException("Neural Error: " + message, NLMErrorCategory::NeuralError), code_(code) {}
    
    NeuralErrorCode getErrorCode() const { return code_; }
    
private:
    NeuralErrorCode code_;
};

// Error handling utilities
class ErrorHandler {
public:
    // Convert NLM errors to standard exceptions
    static std::exception_ptr convertToStandardException(const NLMException& nlmException);
    
    // Log error with appropriate context
    static void logError(const std::string& errorMessage, NLMErrorCategory category = NLMErrorCategory::SystemError);
    
    // Validate parameters with descriptive error messages
    template<typename T>
    static T validateParameter(T value, T min, T max, const std::string& paramName) {
        if (value < min || value > max) {
            throw NLMException(std::format("{0} must be between {1} and {2}, got {3}", 
                                         paramName, min, max, value), 
                              NLMErrorCategory::ConfigError);
        }
        return value;
    }
    
    // Check if a pointer is null with descriptive error
    static void checkNotNull(const void* ptr, const std::string& paramName) {
        if (!ptr) {
            throw NLMException(std::format("{0} cannot be null", paramName), 
                              NLMErrorCategory::SystemError);
        }
    }
    
    // Validate file operations
    static void validateFileOperation(const std::string& filepath, bool operationSuccessful, 
                                     const std::string& operationName);
};

// Error code enums
enum class ConfigErrorCode {
    FileNotFound = 1,
    ParseError = 2,
    InvalidKey = 3,
    TypeMismatch = 4,
    MissingRequiredKey = 5
};

enum class MemoryErrorCode {
    AllocationFailed = 1,
    CapacityExceeded = 2,
    InvalidAccess = 3,
    CorruptionDetected = 4
};

enum class NeuralErrorCode {
    NeuronNotFound = 1,
    SynapseNotFound = 2,
    InvalidConnection = 3,
    SimulationStepError = 4
};

} // namespace nlm