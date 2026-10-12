// Error handling infrastructure
// Provides comprehensive error reporting and management
#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <stdexcept>

namespace nlm {

// Error severity levels
enum class ErrorSeverity : uint8_t {
    Debug = 0,           // Debugging information
    Info = 1,            // General information
    Warning = 2,         // Warning conditions
    Error = 3,           // Error conditions
    Critical = 4,        // Critical errors
    Fatal = 5            // Fatal errors
};

// Error codes for standardized error reporting
enum class ErrorCode : uint16_t {
    // System errors
    Success = 0,
    InvalidParameter,
    MemoryAllocationFailed,
    NotInitialized,
    AlreadyInitialized,
    
    // Neuron errors
    NeuronNotFound,
    InvalidNeuronId,
    NeuronAlreadyExists,
    NeuronAlreadyInitialized,
    
    // Synapse errors
    SynapseNotFound,
    InvalidSynapseId,
    SynapseCreationFailed,
    SynapseConnectionFailed,
    
    // Simulation errors
    InvalidStep,
    SimulationNotRunning,
    TimeStepError,
    StepExecutionFailed,
    
    // Resource errors
    ResourceNotAvailable,
    ResourceLocked,
    ResourceOutOfRange,
    ResourceExhausted,
    
    // Configuration errors
    ConfigFileNotFound,
    ConfigFileInvalid,
    ConfigKeyNotFound,
    ConfigValueInvalid,
    
    // Memory errors
    MemoryCorruption,
    InvalidMemoryAccess,
    
    // Development errors
    DevelopmentModeNotEnabled,
    DevelopmentFeatureNotAvailable,
    
    // General errors
    UnknownError = 65535
};

// Error context for detailed error reporting
struct ErrorContext {
    std::string file;
    uint32_t line;
    std::string function;
    std::string message;
    
    ErrorContext(const std::string& file, uint32_t line, const std::string& function,
                  const std::string& message = "")
        : file(file), line(line), function(function), message(message) {}
};

// Basic error class
class Error {
public:
    ErrorCode code;
    std::string message;
    ErrorSeverity severity;
    ErrorContext context;
    
    Error(ErrorCode code, const std::string& message,
          ErrorSeverity severity = ErrorSeverity::Error,
          const ErrorContext& context = ErrorContext("", 0, ""))
        : code(code), message(message), severity(severity), context(context) {}
    
    virtual ~Error() = default;
    
    virtual std::string getFullMessage() const {
        std::string full = "[" + errorCodeToString(code) + "] " + message;
        if (!context.file.empty()) {
            full += " (" + context.file + ":" + std::to_string(context.line) + "; " + context.function + ")";
        }
        return full;
    }
    
    virtual bool isRecoverable() const {
        return severity <= ErrorSeverity::Warning;
    }
};

// Runtime error class for fatal errors
class RuntimeError : public Error {
public:
    RuntimeError(ErrorCode code, const std::string& message,
                 ErrorSeverity severity = ErrorSeverity::Fatal)
        : Error(code, message, severity) {}
    
    std::string getFullMessage() const override {
        std::string full = "[RUNTIME ERROR] " + Error::getFullMessage();
        return full;
    }
    
    bool isRecoverable() const override {
        return false;
    }
};

// Validation error class for recoverable errors
class ValidationError : public Error {
public:
    ValidationError(ErrorCode code, const std::string& message,
                    ErrorSeverity severity = ErrorSeverity::Error)
        : Error(code, message, severity) {}
    
    std::string getFullMessage() const override {
        std::string full = "[VALIDATION ERROR] " + Error::getFullMessage();
        return full;
    }
    
    bool isRecoverable() const override {
        return true;
    }
};

// Error handler function type
using ErrorHandler = std::function<void(const Error&)>;

// Error registry for error code to message mapping
class ErrorRegistry {
private:
    std::unordered_map<ErrorCode, std::string> errorMessages;
    static ErrorRegistry* instance;
    
    ErrorRegistry() {
        // Initialize with standard error messages
        errorMessages[ErrorCode::Success] = "Operation completed successfully";
        errorMessages[ErrorCode::InvalidParameter] = "Invalid parameter provided";
        errorMessages[ErrorCode::MemoryAllocationFailed] = "Memory allocation failed";
        errorMessages[ErrorCode::NeuronNotFound] = "Neuron not found";
        errorMessages[ErrorCode::SynapseNotFound] = "Synapse not found";
        errorMessages[ErrorCode::SimulationNotRunning] = "Simulation is not running";
        errorMessages[ErrorCode::ConfigFileNotFound] = "Configuration file not found";
        errorMessages[ErrorCode::UnknownError] = "Unknown error occurred";
    }
    
public:
    static ErrorRegistry& getInstance() {
        if (!instance) {
            instance = new ErrorRegistry();
        }
        return *instance;
    }
    
    void registerError(ErrorCode code, const std::string& message) {
        errorMessages[code] = message;
    }
    
    std::string getErrorMessage(ErrorCode code) const {
        auto it = errorMessages.find(code);
        if (it != errorMessages.end()) {
            return it->second;
        }
        return errorMessages.at(ErrorCode::UnknownError);
    }
    
    static const char* errorCodeToString(ErrorCode code) {
        switch (code) {
            case ErrorCode::Success: return "SUCCESS";
            case ErrorCode::InvalidParameter: return "INVALID_PARAMETER";
            case ErrorCode::MemoryAllocationFailed: return "MEMORY_ALLOCATION_FAILED";
            case ErrorCode::NotInitialized: return "NOT_INITIALIZED";
            case ErrorCode::NeuronNotFound: return "NEURON_NOT_FOUND";
            case ErrorCode::InvalidNeuronId: return "INVALID_NEURON_ID";
            case ErrorCode::SynapseNotFound: return "SYNAPSE_NOT_FOUND";
            case ErrorCode::SimulationNotRunning: return "SIMULATION_NOT_RUNNING";
            case ErrorCode::ConfigFileNotFound: return "CONFIG_FILE_NOT_FOUND";
            default: return "UNKNOWN_ERROR";
        }
    }
};

// Error utility functions
namespace error {
    
    // Create error with context
    inline Error createError(ErrorCode code, const std::string& message,
                            ErrorSeverity severity = ErrorSeverity::Error) {
        return Error(code, message, severity);
    }
    
    // Create validation error
    inline ValidationError createValidationError(ErrorCode code, const std::string& message,
                                                ErrorSeverity severity = ErrorSeverity::Error) {
        return ValidationError(code, message, severity);
    }
    
    // Create runtime error
    inline RuntimeError createRuntimeError(ErrorCode code, const std::string& message,
                                          ErrorSeverity severity = ErrorSeverity::Fatal) {
        return RuntimeError(code, message, severity);
    }
    
    // Convert error code to string
    inline const char* errorCodeToString(ErrorCode code) {
        return ErrorRegistry::errorCodeToString(code);
    }
    
    // Get error message
    inline std::string getErrorMessage(ErrorCode code) {
        return ErrorRegistry::getInstance().getErrorMessage(code);
    }
    
    // Register custom error message
    inline void registerError(ErrorCode code, const std::string& message) {
        ErrorRegistry::getInstance().registerError(code, message);
    }
    
} // namespace error

} // namespace nlm
