#pragma once

#include <string>
#include <memory>
#include <stdexcept>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <sstream>
#include <functional>

namespace nlm {

// Error severity levels
enum class ErrorSeverity {
    Debug,
    Info,
    Warning,
    Error,
    Critical,
    Fatal
};

// Error categories
enum class ErrorCategory {
    None = 0,
    Configuration = 1 << 0,    // Configuration loading/parsing errors
    Initialization = 1 << 1,    // Brain/system initialization errors
    Memory = 1 << 2,            // Memory allocation errors
    FileSystem = 1 << 3,        // File I/O errors
    Network = 1 << 4,           // Network/communication errors
    Math = 1 << 5,              // Mathematical calculation errors
    Algorithm = 1 << 6,         // Algorithm-specific errors
    Resource = 1 << 7,          // Resource allocation errors
    Validation = 1 << 8,        // Data validation errors
    Integration = 1 << 9,        // Integration/coordination errors
    System = 1 << 10             // System-level errors
};

// Error codes for specific error conditions
enum class ErrorCode {
    // Configuration errors
    ConfigFileNotFound = 1000,
    ConfigInvalidFormat = 1001,
    ConfigKeyNotFound =1002,
    ConfigValueInvalid = 1003,
    
    // Initialization errors
    BrainInitFailed = 2000,
    MemorySystemNotInitialized = 2001,
    PredictionSystemNotInitialized = 2002,
    NeuromodulationSystemNotInitialized = 2003,
    DevelopmentSystemNotInitialized = 2004,
    ConceptFormationNotInitialized = 2005,
    AttentionSystemNotInitialized = 2006,
    WorkingMemoryNotInitialized = 2007,
    EpisodicMemoryNotInitialized = 2008,
    
    // Memory errors
    MemoryAllocationFailed = 3000,
    MemoryPoolExhausted = 3001,
    MemoryCorruptionDetected = 3002,
    
    // File system errors
   FileNotFound = 4000,
    FilePermissionDenied = 4001,
    FileCorrupted = 4002,
    DirectoryNotFound = 4003,
    
    // Algorithm errors
    ConvergenceFailed = 5000,
    NumericalInstability = 5001,
    InvalidInputData = 5002,
    OutOfRangeValue = 5003,
    
    // Resource errors
    ResourceExhausted = 6000,
    Timeout = 6001,
    CapacityExceeded = 6002,
    
    // Integration errors
    ComponentNotAvailable = 7000,
    InterfaceNotImplemented = 7001,
    ConfigurationMismatch = 7002,
    
    // System errors
    SystemNotReady = 8000,
    InvalidState = 8001,
    OperationNotSupported = 8002,
    
    // Generic errors
    UnknownError = 9000,
    NotImplemented = 9001,
    InvalidParameter = 9002,
    OperationAborted = 9003
};

// Error context information
struct ErrorContext {
    std::string component;
    std::string operation;
    std::string message;
    std::string file;
    int line = 0;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    
    // Additional context data
    std::unordered_map<std::string, std::string> metadata;
    std::unordered_map<std::string, double> metrics;
    
    // Stack trace information (platform-specific)
    std::vector<std::string> stackTrace;
    
    // Related errors
    std::vector<std::shared_ptr<ErrorContext>> relatedErrors;
};

// Enhanced error base class
class Error : public std::exception {
public:
    Error(const ErrorContext& context, ErrorSeverity severity = ErrorSeverity::Error)
        : context_(context), severity_(severity), timestamp_(std::chrono::system_clock::now()) {
        buildMessage();
    }
    
    virtual ~Error() = default;
    
    const char* what() const noexcept override {
        return message_.c_str();
    }
    
    // Error properties
    ErrorSeverity severity() const { return severity_; }
    const std::string& component() const { return context_.component; }
    const std::string& operation() const { return context_.operation; }
    const std::string& message() const { return message_; }
    const std::string& file() const { return context_.file; }
    int line() const { return context_.line; }
    const std::string& function() const { return context_.function; }
    const std::chrono::system_clock::time_point& timestamp() const { return timestamp_; }
    
    // Context access
    const std::unordered_map<std::string, std::string>& metadata() const { return context_.metadata; }
    const std::unordered_map<std::string, double>& metrics() const { return context_.metrics; }
    
    // Error classification
    bool isRecoverable() const {
        return severity_ <= ErrorSeverity::Warning;
    }
    
    bool isCritical() const {
        return severity_ >= ErrorSeverity::Critical;
    }
    
    // Error formatting
    virtual std::string format() const {
        std::ostringstream oss;
        oss << "[" << static_cast<int>(severity_) << "] ";
        oss << "[" << component() << "] ";
        oss << "[" << operation() << "] ";
        oss << message();
        if (!context_.file.empty()) {
            oss << " (" << context_.file;
            if (context_.line > 0) {
                oss << ":" << context_.line;
            }
            oss << ")";
        }
        return oss.str();
    }
    
    // Error chaining
    Error& addRelatedError(std::shared_ptr<ErrorContext> error) {
        context_.relatedErrors.push_back(std::move(error));
        return *this;
    }
    
    const std::vector<std::shared_ptr<ErrorContext>>& relatedErrors() const {
        return context_.relatedErrors;
    }
    
protected:
    virtual void buildMessage() {
        std::ostringstream oss;
        oss << "[" << static_cast<int>(severity_) << "] ";
        oss << "[" << context_.component << "] ";
        oss << "[" << context_.operation << "] ";
        oss << context_.message;
        message_ = oss.str();
    }
    
    std::string message_;
    ErrorContext context_;
    ErrorSeverity severity_;
    std::chrono::system_clock::time_point timestamp_;
};

// Specific error types for common scenarios
class ConfigurationError : public Error {
public:
    ConfigurationError(const ErrorContext& context, ErrorCode code, ErrorSeverity severity = ErrorSeverity::Error)
        : Error(context, severity), code_(code) {}
    
    ErrorCode code() const { return code_; }
    
    std::string format() const override {
        std::ostringstream oss;
        oss << "[CONFIG ERROR " << static_cast<int>(code_) << "] ";
        oss << Error::format();
        return oss.str();
    }
    
private:
    ErrorCode code_;
};

class InitializationError : public Error {
public:
    InitializationError(const ErrorContext& context, ErrorCode code, ErrorSeverity severity = ErrorSeverity::Error)
        : Error(context, severity), code_(code) {}
    
    ErrorCode code() const { return code_; }
    
    std::string format() const override {
        std::ostringstream oss;
        oss << "[INIT ERROR " << static_cast<int>(code_) << "] ";
        oss << Error::format();
        return oss.str();
    }
    
private:
    ErrorCode code_;
};

class MemoryError : public Error {
public:
    MemoryError(const ErrorContext& context, ErrorCode code, ErrorSeverity severity = ErrorSeverity::Error)
        : Error(context, severity), code_(code) {}
    
    ErrorCode code() const { return code_; }
    
    std::string format() const override {
        std::ostringstream oss;
        oss << "[MEMORY ERROR " << static_cast<int>(code_) << "] ";
        oss << Error::format();
        return oss.str();
    }
    
private:
    ErrorCode code_;
};

class SystemError : public Error {
public:
    SystemError(const ErrorContext& context, ErrorCode code, ErrorSeverity severity = ErrorSeverity::Error)
        : Error(context, severity), code_(code) {}
    
    ErrorCode code() const { return code_; }
    
    std::string format() const override {
        std::ostringstream oss;
        oss << "[SYSTEM ERROR " << static_cast<int>(code_) << "] ";
        oss << Error::format();
        return oss.str();
    }
    
private:
    ErrorCode code_;
};

// Error reporting interface
class IErrorReporter {
public:
    virtual ~IErrorReporter() = default;
    virtual void reportError(const Error& error) = 0;
    virtual void setErrorHandler(std::function<void(const Error&)> handler) = 0;
    virtual void clearErrorHandler() = 0;
    virtual bool hasErrorHandler() const = 0;
    virtual void setSeverityFilter(ErrorSeverity minSeverity) = 0;
    virtual ErrorSeverity getSeverityFilter() const = 0;
    virtual void setCategoryFilter(ErrorCategory categories) = 0;
    virtual ErrorCategory getCategoryFilter() const = 0;
    virtual void setMaxErrors(size_t maxErrors) = 0;
    virtual size_t getMaxErrors() const = 0;
    virtual size_t getCurrentErrorCount() const = 0;
    virtual void clearErrors() = 0;
    virtual std::vector<std::shared_ptr<Error>> getRecentErrors(size_t limit = 10) const = 0;
    virtual std::string generateErrorReport() const = 0;
    virtual void exportErrors(const std::string& filepath) const = 0;
};

// Error reporter implementation
class ErrorReporter : public IErrorReporter {
public:
    ErrorReporter();
    ~ErrorReporter() override;
    
    void reportError(const Error& error) override;
    void setErrorHandler(std::function<void(const Error&)> handler) override;
    void clearErrorHandler() override;
    bool hasErrorHandler() const override;
    
    void setSeverityFilter(ErrorSeverity minSeverity) override;
    ErrorSeverity getSeverityFilter() const override;
    
    void setCategoryFilter(ErrorCategory categories) override;
    ErrorCategory getCategoryFilter() const override;
    
    void setMaxErrors(size_t maxErrors) override;
    size_t getMaxErrors() const override;
    size_t getCurrentErrorCount() const override;
    
    void clearErrors() override;
    std::vector<std::shared_ptr<Error>> getRecentErrors(size_t limit = 10) const override;
    std::string generateErrorReport() const override;
    void exportErrors(const std::string& filepath) const override;
    
    // Error suppression
    void suppressError(const std::string& pattern);
    void enableErrorSuppression(bool enable);
    bool isErrorSuppressionEnabled() const;
    
    // Error recovery
    bool attemptRecovery(const Error& error);
    void registerRecoveryHandler(ErrorCode code, std::function<bool(const Error&)> handler);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Error context builder for convenient error creation
class ErrorBuilder {
public:
    ErrorBuilder(const std::string& component, const std::string& operation);
    
    ErrorBuilder& setMessage(const std::string& message);
    ErrorBuilder& setErrorCode(ErrorCode code);
    ErrorBuilder& setSeverity(ErrorSeverity severity);
    ErrorBuilder& setFile(const std::string& file, int line = 0, const std::string& function = "");
    ErrorBuilder& addMetadata(const std::string& key, const std::string& value);
    ErrorBuilder& addMetadata(const std::string& key, double value);
    ErrorBuilder& addMetric(const std::string& key, double value);
    ErrorBuilder& addRelatedError(std::shared_ptr<ErrorContext> error);
    
    std::shared_ptr<Error> build() const;
    std::shared_ptr<ConfigurationError> buildConfigurationError() const;
    std::shared_ptr<InitializationError> buildInitializationError() const;
    std::shared_ptr<MemoryError> buildMemoryError() const;
    std::shared_ptr<SystemError> buildSystemError() const;
    
private:
    ErrorContext context_;
};

// Global error reporting instance
ErrorReporter& getGlobalErrorReporter();

// Convenience macros for error reporting
#define NLM_THROW_ERROR(component, operation, message, severity) \
    do { \
        nlm::ErrorContext ctx; \
        ctx.component = component; \
        ctx.operation = operation; \
        ctx.message = message; \
        ctx.file = __FILE__; \
        ctx.line = __LINE__; \
        ctx.function = __func__; \
        ctx.timestamp = std::chrono::system_clock::now(); \
        throw nlm::Error(ctx, severity); \
    } while(0)

#define NLM_THROW_CONFIG_ERROR(component, operation, message, code, severity) \
    do { \
        nlm::ErrorContext ctx; \
        ctx.component = component; \
        ctx.operation = operation; \
        ctx.message = message; \
        ctx.file = __FILE__; \
        ctx.line = __LINE__; \
        ctx.function = __func__; \
        ctx.timestamp = std::chrono::system_clock::now(); \
        throw nlm::ConfigurationError(ctx, code, severity); \
    } while(0)

#define NLM_THROW_INIT_ERROR(component, operation, message, code, severity) \
    do { \
        nlm::ErrorContext ctx; \
        ctx.component = component; \
        ctx.operation = operation; \
        ctx.message = message; \
        ctx.file = __FILE__; \
        ctx.line = __LINE__; \
        ctx.function = __func__; \
        ctx.timestamp = std::chrono::system_clock::now(); \
        throw nlm::InitializationError(ctx, code, severity); \
    } while(0)

#define NLM_REPORT_ERROR(component, operation, message, severity) \
    nlm::getGlobalErrorReporter().reportError( \
        nlm::ErrorBuilder(component, operation) \
            .setMessage(message) \
            .setSeverity(severity) \
            .build() \
    )

#define NLM_REPORT_CONFIG_ERROR(component, operation, message, code, severity) \
    nlm::getGlobalErrorReporter().reportError( \
        nlm::ErrorBuilder(component, operation) \
            .setMessage(message) \
            .setErrorCode(code) \
            .setSeverity(severity) \
            .buildConfigurationError() \
    )

// Error handling utilities
template<typename T>
T safe_operation(const std::string& operation, const std::string& component,
                std::function<T()> func, ErrorCode errorCode = ErrorCode::UnknownError,
                ErrorSeverity severity = ErrorSeverity::Error) {
    try {
        return func();
    } catch (const std::exception& e) {
        ErrorContext ctx;
        ctx.component = component;
        ctx.operation = operation;
        ctx.message = std::string("Exception during operation: ") + e.what();
        ctx.file = "unknown";
        ctx.line = 0;
        ctx.function = "unknown";
        ctx.timestamp = std::chrono::system_clock::now();
        
        Error error(ctx, severity);
        nlm::getGlobalErrorReporter().reportError(error);
        
        throw error; // Re-throw as NLM error
    }
}

// Error recovery utility
template<typename T>
T recoverable_operation(const std::string& operation, const std::string& component,
                        std::function<T()> func, std::function<bool(const Error&)> recoveryAction,
                        ErrorCode errorCode = ErrorCode::UnknownError) {
    try {
        return func();
    } catch (const Error& e) {
        if (recoveryAction(e)) {
            // Recovery was attempted and succeeded
            return func(); // Retry the operation
        }
        
        // Recovery failed, report the error
        nlm::getGlobalErrorReporter().reportError(e);
        
        // Re-throw the original error
        throw;
    } catch (const std::exception& e) {
        ErrorContext ctx;
        ctx.component = component;
        ctx.operation = operation;
        ctx.message = std::string("Exception during operation: ") + e.what();
        ctx.file = "unknown";
        ctx.line = 0;
        ctx.function = "unknown";
        ctx.timestamp = std::chrono::system_clock::now();
        
        Error error(ctx, ErrorSeverity::Error, errorCode);
        nlm::getGlobalErrorReporter().reportError(error);
        
        throw error; // Re-throw as NLM error
    }
}

} // namespace nlm