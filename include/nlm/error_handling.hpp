// Error handling and logging enhancements for NLM

#ifndef NLM_ERROR_HANDLING_HPP
#define NLM_ERROR_HANDLING_HPP

#include "core/Logger/Logger.hpp"
#include <string>
#include <map>
#include <chrono>
#include <mutex>
#include <stdexcept>
#include <memory>
#include <unordered_map>

namespace nlm {

// Error categories
enum class ErrorCategory {
    System,           // System-level errors
    Configuration,    // Configuration errors
    Memory,           // Memory allocation errors
    Network,          // Network communication errors
    File,             // File I/O errors
    Validation,       // Input validation errors
    Integration,      // Integration system errors
    Plasticity,       // Plasticity rule errors
    Learning,         // Learning process errors
    Development,      // Development stage errors
    Unknown           // Unknown error type
};

// Error severity levels
enum class ErrorSeverity {
    Debug,
    Info,
    Warning,
    Error,
    Critical,
    Fatal
};

// Error codes for specific error types
enum class ErrorCode {
    // System errors
    Success = 0,
    SystemNotInitialized = 1,
    InvalidParameter = 2,
    OutOfMemory = 3,
    FileNotFound = 4,
    PermissionDenied = 5,
    
    // Configuration errors
    ConfigFileNotFound = 100,
    InvalidConfigValue = 101,
    ConfigValidationFailed = 102,
    MissingRequiredConfig = 103,
    
    // Memory errors
    MemoryAllocationFailed = 200,
    MemoryCorruption = 201,
    MemoryLimitExceeded = 202,
    
    // Integration errors
    IntegrationFailed = 300,
    SystemNotConnected = 301,
    CommunicationError = 302,
    Timeout = 303,
    
    // Plasticity errors
    PlasticityRuleNotFound = 400,
    PlasticityParameterInvalid = 401,
    STDPTimingError = 402,
    
    // Learning errors
    LearningNotConverged = 500,
    LearningRateTooHigh = 501,
    LearningRateTooLow = 502,
    
    // Development errors
    DevelopmentStageError = 600,
    CriticalPeriodMissed = 601,
    MaturationError = 602,
    
    // General errors
    UnknownError = 999,
    NotImplemented = 1000,
    OperationCancelled = 1001,
    ResourceUnavailable = 1002
};

// Exception base class
class NLMException : public std::runtime_error {
public:
    NLMException(ErrorCode code, const std::string& message, 
                 ErrorCategory category = ErrorCategory::Unknown,
                 ErrorSeverity severity = ErrorSeverity::Error,
                 const std::string& file = "", int line = 0,
                 const std::string& function = "")
        : std::runtime_error(message), code_(code), category_(category),
          severity_(severity), file_(file), line_(line), function_(function),
          timestamp_(std::chrono::system_clock::now()) {}
    
    ErrorCode getCode() const { return code_; }
    ErrorCategory getCategory() const { return category_; }
    ErrorSeverity getSeverity() const { return severity_; }
    const std::string& getFile() const { return file_; }
    int getLine() const { return line_; }
    const std::string& getFunction() const { return function_; }
    const std::chrono::system_clock::time_point& getTimestamp() const { return timestamp_; }
    
    std::string getFullMessage() const;
    std::string getLocationInfo() const;
    std::string getSeverityString() const;
    std::string getCategoryString() const;
    
    // Convert to log level for logging
    LogLevel getLogLevel() const {
        switch (severity_) {
            case ErrorSeverity::Debug: return LogLevel::Debug;
            case ErrorSeverity::Info: return LogLevel::Info;
            case ErrorSeverity::Warning: return LogLevel::Warning;
            case ErrorSeverity::Error: return LogLevel::Error;
            case ErrorSeverity::Critical: return LogLevel::Critical;
            case ErrorSeverity::Fatal: return LogLevel::Critical;
            default: return LogLevel::Error;
        }
    }
    
private:
    ErrorCode code_;
    ErrorCategory category_;
    ErrorSeverity severity_;
    std::string file_;
    int line_;
    std::string function_;
    std::chrono::system_clock::time_point timestamp_;
};

// Specific exception types
class SystemException : public NLMException {
public:
    SystemException(ErrorCode code, const std::string& message,
                    const std::string& file = "", int line = 0,
                    const std::string& function = "")
        : NLMException(code, message, ErrorCategory::System, ErrorSeverity::Error,
                       file, line, function) {}
};

class ConfigurationException : public NLMException {
public:
    ConfigurationException(ErrorCode code, const std::string& message,
                           const std::string& file = "", int line = 0,
                           const std::string& function = "")
        : NLMException(code, message, ErrorCategory::Configuration, ErrorSeverity::Warning,
                       file, line, function) {}
};

class MemoryException : public NLMException {
public:
    MemoryException(ErrorCode code, const std::string& message,
                    const std::string& file = "", int line = 0,
                    const std::string& function = "")
        : NLMException(code, message, ErrorCategory::Memory, ErrorSeverity::Critical,
                       file, line, function) {}
};

class IntegrationException : public NLMException {
public:
    IntegrationException(ErrorCode code, const std::string& message,
                        const std::string& file = "", int line = 0,
                        const std::string& function = "")
        : NLMException(code, message, ErrorCategory::Integration, ErrorSeverity::Error,
                       file, line, function) {}
};

class ValidationException : public NLMException {
public:
    ValidationException(ErrorCode code, const std::string& message,
                       const std::string& file = "", int line = 0,
                       const std::string& function = "")
        : NLMException(code, message, ErrorCategory::Validation, ErrorSeverity::Warning,
                       file, line, function) {}
};

// Error reporting and handling utilities
class ErrorHandler {
public:
    static ErrorHandler& getInstance() {
        static ErrorHandler instance;
        return instance;
    }
    
    // Report an exception with logging
    void reportError(const NLMException& error);
    
    // Handle a specific error type
    bool handleError(const NLMException& error);
    
    // Get error statistics
    struct ErrorStats {
        int totalErrors = 0;
        int errorsByCategory(int category) const;
        int errorsBySeverity(int severity) const;
        double errorRate() const;
    };
    
    ErrorStats getErrorStats() const { return stats_; }
    
    // Clear statistics
    void clearStats() { stats_ = ErrorStats(); }
    
    // Set error callback
    using ErrorCallback = std::function<void(const NLMException&)>
    void setErrorCallback(ErrorCallback callback) {
        errorCallback_ = callback;
    }
    
    // Enable/disable error reporting
    void setReportingEnabled(bool enabled) {
        reportingEnabled_ = enabled;
    }
    
    // Get error message by code
    static std::string getErrorMessage(ErrorCode code);
    
    // Create exception from code
    static NLMException createException(ErrorCode code, const std::string& message,
                                         const std::string& file = "", int line = 0,
                                         const std::string& function = "");
    
private:
    ErrorHandler() : stats_(), reportingEnabled_(true) {}
    
    ErrorStats stats_;
    bool reportingEnabled_;
    ErrorCallback errorCallback_;
    mutable std::mutex mutex_;
};

// Enhanced logging with error context
class EnhancedLogger {
public:
    struct LogContext {
        std::string operation;
        std::string component;
        std::chrono::system_clock::time_point startTime;
        std::chrono::system_clock::time_point endTime;
        int severityLevel;
        std::string details;
    };
    
    // Log with context
    static void logWithContext(LogLevel level, const std::string& message,
                              const std::string& operation = "",
                              const std::string& component = "",
                              const std::string& file = "", int line = 0,
                              const std::string& function = "");
    
    // Log operation start
    static void logOperationStart(const std::string& operation,
                                 const std::string& component = "",
                                 const std::string& file = "", int line = 0);
    
    // Log operation end
    static void logOperationEnd(const std::string& operation,
                               const std::string& result = "success",
                               const std::string& details = "",
                               const std::string& file = "", int line = 0);
    
    // Log performance metrics
    static void logPerformance(const std::string& operation,
                              double duration,
                              size_t memoryUsed = 0,
                              const std::string& component = "");
    
    // Get operation logs
    static std::vector<LogContext> getOperationLogs(const std::string& operation = "",
                                                   const std::string& component = "");
    
    // Clear operation logs
    static void clearOperationLogs();
    
    // Enable/disable performance logging
    static void setPerformanceLoggingEnabled(bool enabled) {
        performanceLoggingEnabled_ = enabled;
    }
    
private:
    static std::vector<LogContext> operationLogs_;
    static bool performanceLoggingEnabled_;
    static std::mutex logsMutex_;
};

// Error recovery utilities
class ErrorRecovery {
public:
    enum class RecoveryStrategy {
        Retry,
        Fallback,
        DisableFeature,
        RestoreDefault,
        Abort
    };
    
    struct RecoveryConfig {
        RecoveryStrategy strategy;
        int maxRetries;
        std::chrono::milliseconds retryDelay;
        std::string fallbackValue;
        bool enableLogging;
    };
    
    // Execute operation with error recovery
    template<typename Func>
    static auto executeWithRecovery(Func&& operation, const RecoveryConfig& config,
                                   const std::string& operationName = "") -> decltype(operation()) {
        int retryCount = 0;
        
        while (true) {
            try {
                auto start = std::chrono::high_resolution_clock::now();
                auto result = operation();
                auto end = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
                
                if (config.enableLogging) {
                    EnhancedLogger::logPerformance(operationName, duration.count());
                }
                
                return result;
            } catch (const NLMException& e) {
                retryCount++;
                
                if (retryCount > config.maxRetries) {
                    EnhancedLogger::logWithContext(LogLevel::Error, 
                                                 "Operation failed after " + std::to_string(retryCount) + " retries: " + e.what(),
                                                 operationName);
                    throw;
                }
                
                EnhancedLogger::logWithContext(LogLevel::Warning,
                                             "Operation failed, retrying... (Attempt " + std::to_string(retryCount) + ")",
                                             operationName);
                
                std::this_thread::sleep_for(config.retryDelay);
            } catch (const std::exception& e) {
                EnhancedLogger::logWithContext(LogLevel::Error,
                                             "Unexpected exception in operation: " + std::string(e.what()),
                                             operationName);
                throw SystemException(ErrorCode::UnknownError, e.what());
            }
        }
    }
    
    // Handle error with recovery
    static bool handleError(const NLMException& error, const RecoveryConfig& config);
    
    // Set default recovery configuration
    static void setDefaultRecoveryConfig(const RecoveryConfig& config) {
        defaultConfig_ = config;
    }
    
private:
    static RecoveryConfig defaultConfig_;
    static std::mutex mutex_;
};

// Error reporting for external systems
class ExternalErrorReporter {
public:
    struct ReportConfig {
        std::string endpoint;
        std::string apiKey;
        bool enableReporting;
        bool reportOnlyCritical;
        std::map<std::string, std::string> metadata;
    };
    
    // Send error report to external system
    static bool reportError(const NLMException& error, const ReportConfig& config);
    
    // Enable/disable error reporting
    static void setReportingEnabled(bool enabled) {
        reportingEnabled_ = enabled;
    }
    
    // Add metadata to reports
    static void addMetadata(const std::string& key, const std::string& value) {
        metadata_[key] = value;
    }
    
private:
    static bool reportingEnabled_;
    static std::map<std::string, std::string> metadata_;
    static std::mutex mutex_;
};

// Macro for enhanced error handling
#define NLM_TRY_CATCH(...) \
    try { \
        __VA_ARGS__ \
    } catch (const NLMException& e) { \
        ErrorHandler::getInstance().reportError(e); \
        throw; \
    } catch (const std::exception& e) { \
        ErrorHandler::getInstance().reportError( \
            ErrorHandler::getInstance().createException( \
                ErrorCode::UnknownError, std::string(e.what()), __FILE__, __LINE__, __func__ \
            ) \
        ); \
        throw; \
    }

#define NLM_THROW(code, message) throw ErrorHandler::getInstance().createException( \
    code, message, __FILE__, __LINE__, __func__)

#define NLM_THROW_IF(condition, code, message) \
    if (condition) { \
        NLM_THROW(code, message); \
    }

#define NLM_THROW_UNLESS(condition, code, message) \
    if (!(condition)) { \
        NLM_THROW(code, message); \
    }

#define NLM_LOG_ERROR_WITH_CONTEXT(message, operation, component) \
    EnhancedLogger::logWithContext(LogLevel::Error, message, operation, component, __FILE__, __LINE__)

#define NLM_LOG_WARNING_WITH_CONTEXT(message, operation, component) \
    EnhancedLogger::logWithContext(LogLevel::Warning, message, operation, component, __FILE__, __LINE__)

// Utility functions
namespace ErrorUtils {
    std::string formatError(const NLMException& error);
    std::string errorCategoryToString(ErrorCategory category);
    std::string errorSeverityToString(ErrorSeverity severity);
    std::string errorCodeToString(ErrorCode code);
    bool isRecoverable(ErrorCode code);
    ErrorSeverity mapToSeverity(ErrorCode code);
}

} // namespace nlm

#endif // NLM_ERROR_HANDLING_HPP
