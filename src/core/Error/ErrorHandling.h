// Error handling interface for NLM components
// This provides standardized error handling across all modules

#ifndef NLM_ERROR_HANDLING_H
#define NLM_ERROR_HANDLING_H

#include <string>
#include <vector>
#include <memory>
#include <stdexcept>

namespace nlm {

// Error severity levels
enum class ErrorSeverity {
    INFO,
    WARNING,
    ERROR,
    CRITICAL
};

// Error categories for better organization
enum class ErrorCategory {
    CONFIG,
    MEMORY,
    PLASTICITY,
    NEUROMODULATION,
    SIMULATION,
    INTEGRATION,
    IO,
    VALIDATION,
    SYSTEM
};

// Standard error codes
enum class ErrorCode {
    // Config errors
    CONFIG_FILE_NOT_FOUND = 1000,
    CONFIG_SYNTAX_ERROR,
    CONFIG_KEY_NOT_FOUND,
    CONFIG_VALUE_INVALID,
    
    // Memory errors
    MEMORY_ALLOCATION_FAILED =  Raja error
gmentation errors 2000,
    MEMORY_CORRUPTION = 2001,
    MEMORY_ACCESS_VIOLATION = 2002,
    
    // Plasticity errors
    PLASTICITY_RULE_FAILED = 3000,
    STDP_COMPUTATION_FAILED =3000,
    
    // Neuromodulation errors
    NEUROMODULATION_SIGNAL_FAILED = 4000,
    REWARD_PROCESSING_FAILED = 4001,
    PREDICTION_ERROR_COMPUTATION_FAILED = 4002,
    
    // Simulation errors
    SIMULATION_STEP_FAILED = 5000,
    BRAIN_INITIALIZATION_FAILED = 5001,
    BRAIN_RESET_FAILED = 5002,
    
    // Integration errors
    INTEGRATION_MISSING_COMPONENT = 6000,
    INTEGRATION_CONNECTION_FAILED = 6001,
    INTEGRATION_VALIDATION_FAILED = 6002,
    
    // IO errors
    IO_FILE_OPEN_FAILED = 7000,
    IO_FILE_READ_FAILED = 7001,
    IO_FILE_WRITE_FAILED = 7002,
    IO_FILE_PARSE_FAILED = 7003,
    
    // Validation errors
    VALIDATION_NEGATIVE_VALUE = 8000,
    VALIDATION_OUT_OF_RANGE = 8001,
    VALIDATION_DUPLICATE_KEY = 8002,
    
    // System errors
    SYSTEM_NULL_POINTER = 9000,
    SYSTEM_INVALID_STATE = 9001,
    SYSTEM_THREADING_ERROR = 9002,
    
    // Success
    SUCCESS = 0
};

// Standard error result with context
struct ErrorResult {
    ErrorCode code;
    ErrorCategory category;
    ErrorSeverity severity;
    std::string message;
    std::string context;
    bool isSuccess() const { return code == ErrorCode::SUCCESS; }
};

// Standard exception class for NLM errors
class NlmException : public std::runtime_error {
public:
    NlmException(ErrorCode code, const std::string& message)
        : std::runtime_error("NLM Error: " + message),
          code(code), message(message) {}
    
    NlmException(ErrorCode code, ErrorCategory category, const std::string& message)
        : std::runtime_error("NLM Error: " + message),
          code(code), message(message), category(category) {}
    
    ErrorCode getCode() const { return code; }
    ErrorCategory getCategory() const { return category; }
    const std::string& getMessage() const { return message; }
    
private:
    ErrorCode code;
    ErrorCategory category;
    std::string message;
};

// Global error handler interface
class ErrorHandler {
public:
    virtual ~ErrorHandler() = default;
    
    // Report an error
    virtual void reportError(const ErrorResult& error) = 0;
    
    // Report an exception
    virtual void reportException(const std::exception& ex) = 0;
    
    // Get error statistics
    virtual size_t getErrorCount() const = 0;
    virtual size_t getErrorCount(ErrorCategory category) const = 0;
    
    // Clear error history
    virtual void clearErrors() = 0;
    
    // Check if any errors occurred
    virtual bool hasErrors() const = 0;
};

// Global error handler instance (singleton pattern)
class GlobalErrorHandler : public ErrorHandler {
public:
    static GlobalErrorHandler& instance() {
        static GlobalErrorHandler handler;
        return handler;
    }
    
    void reportError(const ErrorResult& error) override {
        errors.push_back(error);
        logError(error);
    }
    
    void reportException(const std::exception& ex) override {
        errors.push_back(createErrorFromException(ex));
        logException(ex);
    }
    
    size_t getErrorCount() const override { return errors.size(); }
    
    size_t getErrorCount(ErrorCategory category) const override {
        return std::count_if(errors.begin(), errors.end(),
                           [category](const ErrorResult& e) { return e.category == category; });
    }
    
    void clearErrors() override { errors.clear(); }
    
    bool hasErrors() const override { return !errors.empty(); }
    
    // Get all errors
    const std::vector<ErrorResult>& getErrors() const { return errors; }
    
protected:
    GlobalErrorHandler() = default;
    
    virtual void logError(const ErrorResult& error) {
        // Use existing NLM logging system
        std::string logLevel;
        switch (error.severity) {
            case ErrorSeverity::INFO: logLevel = "INFO"; break;
            case ErrorSeverity::WARNING: logLevel = "WARNING"; break;
            case ErrorSeverity::ERROR: logLevel = "ERROR"; break;
            case ErrorSeverity::CRITICAL: logLevel = "CRITICAL"; break;
        }
        
        if (error.context.empty()) {
            NLM_LOG_##logLevel(error.message);
        } else {
            NLM_LOG_##logLevel("[".append(error.context).append("] ").append(error.message));
        }
    }
    
    virtual void logException(const std::exception& ex) {
        NLM_LOG_ERROR(ex.what());
    }
    
    ErrorResult createErrorFromException(const std::exception& ex) {
        ErrorResult error;
        error.severity = ErrorSeverity::ERROR;
        error.message = ex.what();
        error.context = "Exception";
        return error;
    }
    
private:
    std::vector<ErrorResult> errors;
};

// Convenience functions for common error scenarios
namespace ErrorHelpers {
    
    // Create error result from components
    ErrorResult makeError(ErrorCode code, ErrorCategory category,
                         ErrorSeverity severity, const std::string& message,
                         const std::string& context = "") {
        ErrorResult error;
        error.code = code;
        error.category = category;
        error.severity = severity;
        error.message = message;
        error.context = context;
        return error;
    }
    
    // Create success result
    ErrorResult makeSuccess(const std::string& message = "Success") {
        ErrorResult error;
        error.code = ErrorCode::SUCCESS;
        error.category = ErrorCategory::SYSTEM;
        error.severity = ErrorSeverity::INFO;
        error.message = message;
        error.context = "Success";
        return error;
    }
    
    // Safe execution wrapper
    template<typename T>
    ErrorResult safeExecute(T func, const std::string& context = "") {
        try {
            func();
            return makeSuccess("Operation completed successfully");
        } catch (const std::exception& e) {
            return makeError(ErrorCode::SYSTEM_INVALID_STATE,
                           ErrorCategory::SYSTEM,
                           ErrorSeverity::ERROR,
                           std::string("Exception in ").append(context).append(": ").append(e.what()),
                           context);
        }
    }
}

// Macro helpers for common error patterns
#define NLM_RETURN_IF_ERROR(condition, error_code, message) \
    if (condition) { \
        GlobalErrorHandler::instance().reportError( \
            ErrorHelpers::makeError(error_code, ErrorCategory::SYSTEM, \
                                   ErrorSeverity::ERROR, message, #condition)); \
        return false; \
    }

#define NLM_THROW_IF_ERROR(condition, error_code, message) \
    if (condition) { \
        throw NlmException(error_code, #condition.append(": ").append(message)); \
    }

#define NLM_CHECK_RETURN(condition, success_value) \
    if (!(condition)) { \
        return success_value; \
    }

// Error check macros with early returns for boolean functions
#define NLM_RETURN_ERROR_ON_FAILURE(result, error_code, message) \
    if (!(result).isSuccess()) { \
        GlobalErrorHandler::instance().reportError(result); \
        return ErrorResult(error_code, ErrorCategory::SYSTEM, ErrorSeverity::ERROR, message, #result); \
    }

} // namespace nlm

#endif // NLM_ERROR_HANDLING_H