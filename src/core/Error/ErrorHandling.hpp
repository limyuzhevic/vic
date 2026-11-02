#pragma once

#include <string>
#include <stdexcept>
#include <vector>
#include <memory>
#include <sstream>

namespace nlm {

// Error codes for standardized error identification
enum class ErrorCode {
    // Success/No error
    Success = 0,
    
    // Configuration errors
    ConfigFileNotFound = 1,
    ConfigFileParseError = 2,
    ConfigInvalidKey = 3,
    ConfigValueTypeError = 4,
    
    // Random number generator errors
    InvalidRandomRange = 5,
    InvalidRandomSeed = 6,
    CannotPickFromEmpty = 7,
    
    // Neural network errors
    NeuronNotFound = 8,
    SynapseNotFound = 9,
    InvalidNeuronState = 10,
    PlasticityNotEnabled = 11,
    
    // Memory system errors
    MemoryFull = 12,
    MemoryNotAvailable = 13,
    InvalidMemoryOperation = 14,
    
    // Checkpoint errors
    CheckpointFileError = 15,
    CheckpointCorruption = 16,
    CheckpointVersionMismatch = 17,
    
    // World/environment errors
    InvalidWorldState = 18,
    WorldBoundaryExceeded = 19,
    
    // System errors
    OutOfMemory = 20,
    InvalidArgument = 21,
    NotImplemented = 22,
    
    // Integration errors (Phase 6)
    SystemNotIntegrated = 23,
    IntegrationOrderError = 24,
    
    // Generic errors
    Unknown = 999,
    OperationCancelled = 1000
};

/**
 * @brief Standardized exception for NLM errors.
 * 
 * Provides consistent error reporting across the entire NLM system with
 * detailed error messages, error codes, and optional context information.
 */
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& message)
        : std::runtime_error(message), errorCode_(ErrorCode::Unknown), 
          file_("unknown"), line_(0), function_("unknown") {}
    
    NLMError(ErrorCode code, const std::string& message)
        : std::runtime_error(formatError(code, message)), 
          errorCode_(code), file_("unknown"), line_(0), function_("unknown") {}
    
    NLMError(ErrorCode code, const std::string& message,
             const std::string& file, int line, const std::string& function)
        : std::runtime_error(formatError(code, message, file, line, function)), 
          errorCode_(code), file_(file), line_(line), function_(function) {}
    
    // Accessors
    ErrorCode getErrorCode() const noexcept { return errorCode_; }
    const std::string& getFile() const noexcept { return file_; }
    int getLine() const noexcept { return line_; }
    const std::string& getFunction() const noexcept { return function_; }
    
    // Helper methods for common error types
    static NLMError configFileError(const std::string& filepath, const std::string& reason);
    static NLMError invalidRange(const std::string& operation, const std::string& param1, const std::string& param2);
    static NLMError notImplemented(const std::string& feature);
    static NLMError systemNotIntegrated(const std::string& system);
    
private:
    static std::string formatError(ErrorCode code, const std::string& message);
    static std::string formatError(ErrorCode code, const std::string& message,
                                   const std::string& file, int line, const std::string& function);
    
    ErrorCode errorCode_;
    std::string file_;
    int line_;
    std::string function_;
};

/**
 * @brief Result type for functions that can fail.
 * 
 * Provides a unified way to return success/failure status along with
 * optional result values and error information. Similar to std::expected
 * but with NLM-specific error handling.
 */
template<typename T>
class Result {
public:
    // Constructors
    Result(const T& value) : value_(value), error_(ErrorCode::Success), errorMessage_("") {}
    Result(T&& value) : value_(std::move(value)), error_(ErrorCode::Success), errorMessage_("") {}
    Result(ErrorCode error, const std::string& errorMessage)
        : value_(T()), error_(error), errorMessage_(errorMessage) {}
    Result(ErrorCode error, const std::string& errorMessage,
           const std::string& file, int line, const std::string& function)
        : value_(T()), error_(error), errorMessage_(errorMessage),
          file_(file), line_(line), function_(function) {}
    
    // Accessors
    bool success() const noexcept { return error_ == ErrorCode::Success; }
    bool failure() const noexcept { return !success(); }
    
    const T& value() const & { return value_; }
    T& value() & { return value_; }
    const T&& value() const && { return std::move(value_); }
    T&& value() && { return std::move(value_); }
    
    const std::string& errorMessage() const noexcept { return errorMessage_; }
    ErrorCode errorCode() const noexcept { return error_; }
    
    const std::string& getFile() const noexcept { return file_; }
    int getLine() const noexcept { return line_; }
    const std::string& getFunction() const noexcept { return function_; }
    
    // Exception conversion
    void raise() const {
        if (failure()) {
            if (file_.empty()) {
                throw NLMError(error_, errorMessage_);
            } else {
                throw NLMError(error_, errorMessage_, file_, line_, function_);
            }
        }
    }
    
    // operator* and operator-> for convenience (throws on failure)
    T& operator*() { 
        raise(); 
        return value_; 
    }
    
    const T& operator*() const { 
        raise(); 
        return value_; 
    }
    
    T* operator->() { 
        raise(); 
        return &value_; 
    }
    
    const T* operator->() const { 
        raise(); 
        return &value_; 
    }
    
private:
    T value_;
    ErrorCode error_;
    std::string errorMessage_;
    std::string file_;
    int line_;
    std::string function_;
};

// Helper function for creating error results
template<typename T>
Result<T> makeError(ErrorCode error, const std::string& errorMessage) {
    return Result<T>(error, errorMessage);
}

// Helper function for creating error results with location info
template<typename T>
Result<T> makeError(ErrorCode error, const std::string& errorMessage,
                    const std::string& file, int line, const std::string& function) {
    return Result<T>(error, errorMessage, file, line, function);
}

// Helper function for creating success results
template<typename T>
Result<T> makeSuccess(const T& value) {
    return Result<T>(value);
}

// Helper function for creating success results with rvalue
template<typename T>
Result<T> makeSuccess(T&& value) {
    return Result<T>(std::move(value));
}

/**
 * @brief Context information for error reporting.
 * 
 * Provides structured context information that can be attached to errors
 * to help with debugging and error analysis.
 */
struct ErrorContext {
    std::string component;
    std::string operation;
    std::string details;
    std::string suggestion;
    
    ErrorContext() = default;
    ErrorContext(const std::string& comp, const std::string& op)
        : component(comp), operation(op) {}
    
    ErrorContext& withDetails(const std::string& detail) {
        details = detail;
        return *this;
    }
    
    ErrorContext& withSuggestion(const std::string& sugg) {
        suggestion = sugg;
        return *this;
    }
};

/**
 * @brief Global error handler interface.
 * 
 * Allows customization of error handling behavior across the entire NLM system.
 * Can be used to implement custom error reporting, recovery, or logging strategies.
 */
class ErrorHandler {
public:
    virtual ~ErrorHandler() = default;
    
    /**
     * @brief Handle an error that occurred during an operation.
     * 
     * @param error The error that occurred.
     * @param context Context information about the operation.
     * @return Action to take (continue, abort, retry, etc.).
     */
    virtual bool handleError(const NLMError& error, const ErrorContext& context) = 0;
    
    /**
     * @brief Get the global error handler instance.
     * 
     * @return Reference to the global error handler.
     */
    static ErrorHandler& getGlobal();
    
    /**
     * @brief Set the global error handler.
     * 
     * @param handler The new error handler to use.
     */
    static void setGlobal(std::unique_ptr<ErrorHandler> handler);
};

/**
 * @brief Default error handler that logs and throws.
 * 
 * The default error handler logs errors using the NLM logging system
 * and throws the error for immediate handling by the caller.
 */
class DefaultErrorHandler : public ErrorHandler {
public:
    bool handleError(const NLMError& error, const ErrorContext& context) override;
};

/**
 * @brief Safe error handling utilities.
 * 
 * Provides helper functions for safe error handling in existing code.
 */
class SafeErrorHandler {
public:
    /**
     * @brief Execute a function and handle any NLM errors.
     * 
     * @param func Function to execute.
     * @param context Context information for error reporting.
     * @return Result of the function execution.
     */
    template<typename F, typename... Args>
    static auto executeSafely(const ErrorContext& context, F&& func, Args&&... args) {
        try {
            auto result = func(std::forward<Args>(args)...);
            if constexpr (std::is_same_v<decltype(result), Result<typename std::remove_reference<decltype(result)>::type::value_type>>) {
                if (result.failure()) {
                    NLM_LOG_ERROR("Operation failed: " + result.errorMessage() + 
                                " (code: " + std::to_string(static_cast<int>(result.errorCode())) + ")");
                    ErrorHandler::getGlobal().handleError(
                        NLMError(result.errorCode(), result.errorMessage(), 
                                result.getFile(), result.getLine(), result.getFunction()),
                        context
                    );
                }
                return result;
            } else {
                return result;
            }
        } catch (const NLMError& e) {
            NLM_LOG_ERROR("Exception in " + context.component + "::" + context.operation + ": " + e.what());
            ErrorHandler::getGlobal().handleError(e, context);
            throw;
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Unexpected exception in " + context.component + "::" + context.operation + ": " + e.what());
            ErrorHandler::getGlobal().handleError(
                NLMError(ErrorCode::Unknown, std::string("Unexpected exception: ") + e.what()),
                context
            );
            throw;
        }
    }
};

// Global error handler instance
inline std::unique_ptr<ErrorHandler>& globalErrorHandler() {
    static std::unique_ptr<ErrorHandler> handler = std::make_unique<DefaultErrorHandler>();
    return handler;
}

} // namespace nlm
