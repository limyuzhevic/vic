// =============================================================================
// ERROR HANDLING SYSTEM
// =============================================================================

#ifndef NLM_ERROR_HANDLING_H
#define NLM_ERROR_HANDLING_H

#include <string>
#include <map>
#include <vector>
#include <memory>
#include <stdexcept>
#include <iostream>

namespace nlm {

// Error codes for standardized error reporting
enum class ErrorCode {
    // Success
    SUCCESS = 0,
    
    // Configuration errors
    CONFIG_FILE_NOT_FOUND = 100,
    CONFIG_FILE_CORRUPTED = 101,
    CONFIG_INVALID_VALUE = 102,
    CONFIG_KEY_NOT_FOUND = 103,
    
    // Brain errors
    BRAIN_INITIALIZATION_FAILED = 200,
    BRAIN_LOAD_FAILED = 201,
    BRAIN_SAVE_FAILED = 202,
    BRAIN_INVALID_STATE = 203,
    
    // Memory errors
    MEMORY_ALLOCATION_FAILED = 300,
    MEMORY_INVALID_ACCESS = 301,
    MEMORY_FULL = 302,
    
    // Plasticity errors
    PLASTICITY_INVALID_PARAMETERS = 400,
    PLASTICITY_OPERATION_FAILED = 401,
    
    // Development errors
    DEVELOPMENT_INVALID_STAGE = 500,
    DEVELOPMENT_OPERATION_FAILED = 501,
    
    // Neuromodulation errors
    NEUROMODULATION_INVALID_LEVEL = 600,
    NEUROMODULATION_OPERATION_FAILED = 601,
    
    // Cognition errors
    COGNITION_INVALID_STATE = 700,
    COGNITION_OPERATION_FAILED = 701,
    
    // Prediction errors
    PREDICTION_INVALID_STATE = 800,
    PREDICTION_OPERATION_FAILED = 801,
    
    // General errors
    INVALID_ARGUMENT = 1000,
    OUT_OF_MEMORY = 1001,
    OPERATION_TIMEOUT = 1002,
    UNKNOWN_ERROR = 9999
};

/**
 * @brief Standard exception class for NLM errors.
 * 
 * Provides detailed error information including error code, message, and context.
 */
class NLMException : public std::runtime_error {
public:
    NLMException(ErrorCode code, const std::string& message)
        : std::runtime_error(formatMessage(code, message)),
          code_(code), message_(message) {}
    
    NLMException(ErrorCode code, const std::string& message, const std::string& context)
        : std::runtime_error(formatMessage(code, message, context)),
          code_(code), message_(message), context_(context) {}
    
    ErrorCode getCode() const { return code_; }
    const std::string& getMessage() const { return message_; }
    const std::string& getContext() const { return context_; }
    
    static std::string formatMessage(ErrorCode code, const std::string& message) {
        return "[NLM Error " + std::to_string(static_cast<int>(code)) + "] " + message;
    }
    
    static std::string formatMessage(ErrorCode code, const std::string& message, const std::string& context) {
        return "[NLM Error " + std::to_string(static_cast<int>(code)) + "] " + message +
               (context.empty() ? "" : " (Context: " + context + ")");
    }
    
private:
    ErrorCode code_;
    std::string message_;
    std::string context_;
};

/**
 * @brief Result template for functions that may succeed or fail.
 * 
 * Provides a standardized way to return values along with error information.
 * 
 * @tparam T Type of value returned on success
 */
template<typename T>
class Result {
public:
    Result(T value) : value_(std::move(value)), errorCode_(ErrorCode::SUCCESS) {}
    Result(ErrorCode code, const std::string& message) : errorCode_(code), errorMessage_(message) {}
    Result(ErrorCode code, const std::string& message, const std::string& context) 
        : errorCode_(code), errorMessage_(message), errorContext_(context) {}
    
    bool isSuccess() const { return errorCode_ == ErrorCode::SUCCESS; }
    bool isFailure() const { return errorCode_ != ErrorCode::SUCCESS; }
    
    const T& value() const {
        if (!isSuccess()) {
            throw NLMException(errorCode_, "Attempted to access value of failed result", errorContext_);
        }
        return value_;
    }
    
    T& value() {
        if (!isSuccess()) {
            throw NLMException(errorCode_, "Attempted to access value of failed result", errorContext_);
        }
        return value_;
    }
    
    ErrorCode getErrorCode() const { return errorCode_; }
    const std::string& getErrorMessage() const { return errorMessage_; }
    const std::string& getErrorContext() const { return errorContext_; }
    
    void throwIfFailure() const {
        if (isFailure()) {
            throw NLMException(errorCode_, errorMessage_, errorContext_);
        }
    }
    
    std::string toString() const {
        if (isSuccess()) {
            return "Success: " + std::to_string(value_);
        } else {
            return "Failure: [Error " + std::to_string(static_cast<int>(errorCode_)) + "] " +
                   errorMessage_ + (errorContext_.empty() ? "" : " (Context: " + errorContext_ + ")");
        }
    }
    
private:
    T value_;
    ErrorCode errorCode_;
    std::string errorMessage_;
    std::string errorContext_;
};

/**
 * @brief Error reporting utilities.
 */
class ErrorReport {
public:
    static std::string getErrorCodeName(ErrorCode code) {
        static const std::map<ErrorCode, std::string> codeNames = {
            {ErrorCode::SUCCESS, "SUCCESS"},
            {ErrorCode::CONFIG_FILE_NOT_FOUND, "CONFIG_FILE_NOT_FOUND"},
            {ErrorCode::CONFIG_FILE_CORRUPTED, "CONFIG_FILE_CORRUPTED"},
            {ErrorCode::CONFIG_INVALID_VALUE, "CONFIG_INVALID_VALUE"},
            {ErrorCode::CONFIG_KEY_NOT_FOUND, "CONFIG_KEY_NOT_FOUND"},
            {ErrorCode::BRAIN_INITIALIZATION_FAILED, "BRAIN_INITIALIZATION_FAILED"},
            {ErrorCode::BRAIN_LOAD_FAILED, "BRAIN_LOAD_FAILED"},
            {ErrorCode::BRAIN_SAVE_FAILED, "BRAIN_SAVE_FAILED"},
            {ErrorCode::BRAIN_INVALID_STATE, "BRAIN_INVALID_STATE"},
            {ErrorCode::MEMORY_ALLOCATION_FAILED, "MEMORY_ALLOCATION_FAILED"},
            {ErrorCode::MEMORY_INVALID_ACCESS, "MEMORY_INVALID_ACCESS"},
            {ErrorCode::MEMORY_FULL, "MEMORY_FULL"},
            {ErrorCode::PLASTICITY_INVALID_PARAMETERS, "PLASTICITY_INVALID_PARAMETERS"},
            {ErrorCode::PLASTICITY_OPERATION_FAILED, "PLASTICITY_OPERATION_FAILED"},
            {ErrorCode::DEVELOPMENT_INVALID_STAGE, "DEVELOPMENT_INVALID_STAGE"},
            {ErrorCode::DEVELOPMENT_OPERATION_FAILED, "DEVELOPMENT_OPERATION_FAILED"},
            {ErrorCode::NEUROMODULATION_INVALID_LEVEL, "NEUROMODULATION_INVALID_LEVEL"},
            {ErrorCode::NEUROMODULATION_OPERATION_FAILED, "NEUROMODULATION_OPERATION_FAILED"},
            {ErrorCode::COGNITION_INVALID_STATE, "COGNITION_INVALID_STATE"},
            {ErrorCode::COGNITION_OPERATION_FAILED, "COGNITION_OPERATION_FAILED"},
            {ErrorCode::PREDICTION_INVALID_STATE, "PREDICTION_INVALID_STATE"},
            {ErrorCode::PREDICTION_OPERATION_FAILED, "PREDICTION_OPERATION_FAILED"},
            {ErrorCode::INVALID_ARGUMENT, "INVALID_ARGUMENT"},
            {ErrorCode::OUT_OF_MEMORY, "OUT_OF_MEMORY"},
            {ErrorCode::OPERATION_TIMEOUT, "OPERATION_TIMEOUT"},
            {ErrorCode::UNKNOWN_ERROR, "UNKNOWN_ERROR"}
        };
        
        auto it = codeNames.find(code);
        return (it != codeNames.end()) ? it->second : "UNKNOWN_ERROR";
    }
    
    static std::string getErrorDescription(ErrorCode code) {
        static const std::map<ErrorCode, std::string> descriptions = {
            {ErrorCode::CONFIG_FILE_NOT_FOUND, "Configuration file not found"},
            {ErrorCode::CONFIG_FILE_CORRUPTED, "Configuration file is corrupted"},
            {ErrorCode::CONFIG_INVALID_VALUE, "Invalid configuration value"},
            {ErrorCode::CONFIG_KEY_NOT_FOUND, "Configuration key not found"},
            {ErrorCode::BRAIN_INITIALIZATION_FAILED, "Failed to initialize brain"},
            {ErrorCode::BRAIN_LOAD_FAILED, "Failed to load brain state"},
            {ErrorCode::BRAIN_SAVE_FAILED, "Failed to save brain state"},
            {ErrorCode::BRAIN_INVALID_STATE, "Invalid brain state"},
            {ErrorCode::MEMORY_ALLOCATION_FAILED, "Memory allocation failed"},
            {ErrorCode::MEMORY_INVALID_ACCESS, "Invalid memory access"},
            {ErrorCode::MEMORY_FULL, "Memory is full"},
            {ErrorCode::PLASTICITY_INVALID_PARAMETERS, "Invalid plasticity parameters"},
            {ErrorCode::PLASTICITY_OPERATION_FAILED, "Plasticity operation failed"},
            {ErrorCode::DEVELOPMENT_INVALID_STAGE, "Invalid developmental stage"},
            {ErrorCode::DEVELOPMENT_OPERATION_FAILED, "Development operation failed"},
            {ErrorCode::NEUROMODULATION_INVALID_LEVEL, "Invalid neuromodulation level"},
            {ErrorCode::NEUROMODULATION_OPERATION_FAILED, "Neuromodulation operation failed"},
            {ErrorCode::COGNITION_INVALID_STATE, "Invalid cognition state"},
            {ErrorCode::COGNITION_OPERATION_FAILED, "Cognition operation failed"},
            {ErrorCode::PREDICTION_INVALID_STATE, "Invalid prediction state"},
            {ErrorCode::PREDICTION_OPERATION_FAILED, "Prediction operation failed"},
            {ErrorCode::INVALID_ARGUMENT, "Invalid argument provided"},
            {ErrorCode::OUT_OF_MEMORY, "Out of memory"},
            {ErrorCode::OPERATION_TIMEOUT, "Operation timed out"},
            {ErrorCode::UNKNOWN_ERROR, "Unknown error"}
        };
        
        auto it = descriptions.find(code);
        return (it != descriptions.end()) ? it->second : "Unknown error";
    }
    
    static std::string formatErrorReport(const std::string& operation, ErrorCode code, 
                                        const std::string& details = "", 
                                        const std::string& context = "") {
        std::ostringstream oss;
        oss << "=== ERROR REPORT ===\n";
        oss << "Operation: " << operation << "\n";
        oss << "Error Code: " << static_cast<int>(code) << " (" << getErrorCodeName(code) << ")\n";
        oss << "Error Description: " << getErrorDescription(code) << "\n";
        if (!details.empty()) {
            oss << "Details: " << details << "\n";
        }
        if (!context.empty()) {
            oss << "Context: " << context << "\n";
        }
        oss << "===================";
        return oss.str();
    }
};

// Template function for safe result handling
template<typename T>
Result<T> makeResult(T value) {
    return Result<T>(std::move(value));
}

template<typename T>
Result<T> makeErrorResult(ErrorCode code, const std::string& message) {
    return Result<T>(code, message);
}

template<typename T>
Result<T> makeErrorResult(ErrorCode code, const std::string& message, const std::string& context) {
    return Result<T>(code, message, context);
}

} // namespace nlm

#endif // NLM_ERROR_HANDLING_H
