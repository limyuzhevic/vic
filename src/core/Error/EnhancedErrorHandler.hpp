// Enhanced Error Handling and Validation System

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <functional>
#include <unordered_map>
#include <stdexcept>
#include "Logger/Logger.hpp"

namespace nlm {

// Enhanced error severity levels with more granularity
enum class ErrorSeverity {
    Debug = 0,           // Information for debugging
    Info = 1,            // General information
    Warning = 2,         // Potentially problematic but recoverable
    Error = 3,          // Error that prevents normal operation
    Critical = 4,       // Serious error, may cause data loss
    Fatal = 5           // Unrecoverable error, system cannot continue
};

// Error categories for better error classification
enum class ErrorCategory {
    None = 0,
    Configuration,        // Configuration loading, parsing errors
    Memory,              // Memory allocation errors
    FileSystem,          // File I/O errors
    Network,             // Network communication errors
    Neural,              // Neural network computation errors
    MemoryManagement,    // Memory management errors
    Resource,            // Resource allocation errors
    Timeout,            // Timeout errors
    Permission,         // Permission/access errors
    Validation,         // Input validation errors
    Algorithm,          // Algorithm-specific errors
    Development,        // Development/debugging errors
    System              // System-level errors
};

// Enhanced error context with additional diagnostic information
struct ErrorContext {
    std::string errorId;              // Unique error identifier
    ErrorSeverity severity;           // Error severity level
    ErrorCategory category;           // Error category
    std::string message;              // Error message
    std::string file;                 // File where error occurred
    int line;                         // Line number where error occurred
    std::string function;             // Function where error occurred
    std::chrono::system_clock::time_point timestamp;  // Error timestamp
    std::vector<std::string> stackTrace;  // Call stack trace
    std::unordered_map<std::string, std::string> metadata;  // Additional metadata
    std::vector<std::string> recoveryActions;  // Possible recovery actions
    bool recovered;                    // Whether error was recovered from
    std::string recoveryAction;        // Actual recovery action taken
    std::string rootCause;            // Root cause of the error
    std::string impact;               // Impact of the error on system operation
    
    ErrorContext()
        : severity(ErrorSeverity::Info)
        , category(ErrorCategory::None)
        , line(0)
        , recovered(false)
        , timestamp(std::chrono::system_clock::now()) {}
    
    ErrorContext(ErrorSeverity severity, const std::string& message, 
                 const std::string& file = "", int line = 0,
                 const std::string& function = "")
        : severity(severity)
        , category(ErrorCategory::None)
        , message(message)
        , file(file)
        , line(line)
        , function(function)
        , recovered(false)
        , timestamp(std::chrono::system_clock::now()) {}
};

// Enhanced exception class with error context
class EnhancedException : public std::runtime_error {
public:
    EnhancedException(const std::string& message,
                     ErrorSeverity severity = ErrorSeverity::Error,
                     const std::string& file = "",
                     int line = 0,
                     const std::string& function = "")
        : std::runtime_error(message)
        , errorContext_(severity, message, file, line, function) {}
    
    EnhancedException(const ErrorContext& context)
        : std::runtime_error(context.message)
        , errorContext_(context) {}
    
    const ErrorContext& getContext() const { return errorContext_; }
    ErrorSeverity getSeverity() const { return errorContext_.severity; }
    ErrorCategory getCategory() const { return errorContext_.category; }
    
    std::string getFullMessage() const {
        std::ostringstream oss;
        oss << "[" << errorContext_.timestamp << "] ";
        oss << "[" << errorContext_.severity << "] ";
        oss << "[" << errorContext_.category << "] ";
        oss << errorContext_.message;
        return oss.str();
    }
    
    std::string getLocationInfo() const {
        if (!errorContext_.file.empty() && errorContext_.line > 0) {
            return errorContext_.file + ":" + std::to_string(errorContext_.line);
        }
        return errorContext_.file.empty() ? "" : errorContext_.file;
    }
    
private:
    ErrorContext errorContext_;
};

// Enhanced error handler with recovery mechanisms
class EnhancedErrorHandler {
public:
    // Singleton pattern
    static EnhancedErrorHandler& getInstance() {
        static EnhancedErrorHandler instance;
        return instance;
    }
    
    // Error registration
    void registerError(const ErrorContext& error);
    void registerError(ErrorSeverity severity, const std::string& message,
                      const std::string& file = "", int line = 0,
                      const std::string& function = "", ErrorCategory category = ErrorCategory::None);
    
    // Error management
    bool hasErrors(ErrorSeverity minSeverity = ErrorSeverity::Error) const;
    bool hasErrors(ErrorCategory category) const;
    bool hasErrors(ErrorSeverity minSeverity, ErrorCategory category) const;
    
    // Error retrieval
    std::vector<ErrorContext> getErrors(ErrorSeverity minSeverity = ErrorSeverity::Error) const;
    std::vector<ErrorContext> getErrors(ErrorCategory category) const;
    std::vector<ErrorContext> getErrors(ErrorSeverity minSeverity, ErrorCategory category) const;
    std::optional<ErrorContext> getLastError(ErrorSeverity minSeverity = ErrorSeverity::Error) const;
    
    // Error clearing
    void clearErrors(ErrorSeverity minSeverity = ErrorSeverity::Error);
    void clearErrors(ErrorCategory category);
    void clearErrors(ErrorSeverity minSeverity, ErrorCategory category);
    void clearAllErrors();
    
    // Error analysis
    size_t getErrorCount(ErrorSeverity minSeverity = ErrorSeverity::Error) const;
    size_t getErrorCount(ErrorCategory category) const;
    size_t getErrorCount(ErrorSeverity minSeverity, ErrorCategory category) const;
    
    // Error recovery
    bool attemptRecovery(const ErrorContext& error);
    bool attemptRecovery(ErrorSeverity severity, const std::string& message);
    bool recoverAllErrors();
    
    // Error reporting
    std::string getErrorReport(ErrorSeverity minSeverity = ErrorSeverity::Error) const;
    std::string getErrorReport(ErrorCategory category) const;
    std::string getErrorReport(ErrorSeverity minSeverity, ErrorCategory category) const;
    
    // Error validation
    bool validateInput(const std::string& input, const std::string& context = "") const;
    bool validateMemoryAccess(const void* pointer, size_t size, const std::string& context = "") const;
    bool validateFilePath(const std::string& path, bool requireExists = false, const std::string& context = "") const;
    bool validateConfiguration(const std::string& key, const std::string& value, const std::string& context = "") const;
    
    // Error prevention
    void setupErrorPrevention();
    void setupErrorRecovery() { setupErrorPrevention(); }
    void setupErrorReporting();
    
    // Configuration
    void setMaxErrors(size_t maxErrors) { maxErrors_ = maxErrors; }
    void setErrorSeverityThreshold(ErrorSeverity threshold) { severityThreshold_ = threshold; }
    void setAutoRecovery(bool autoRecovery) { autoRecovery_ = autoRecovery; }
    void setErrorReporting(bool errorReporting) { errorReporting_ = errorReporting; }
    
    // Statistics
    std::chrono::system_clock::time_point getLastErrorTime() const { return lastErrorTime_; }
    std::chrono::system_clock::time_point getFirstErrorTime() const { return firstErrorTime_; }
    double getErrorRate() const;  // Errors per minute
    
    // Utility functions
    static std::string severityToString(ErrorSeverity severity);
    static std::string categoryToString(ErrorCategory category);
    static std::string formatErrorContext(const ErrorContext& error);
    static ErrorSeverity stringToSeverity(const std::string& str);
    static ErrorCategory stringToCategory(const std::string& str);
    
    // Global error handling functions
    static void logError(ErrorSeverity severity, const std::string& message,
                        const std::string& file = "", int line = 0,
                        const std::string& function = "", ErrorCategory category = ErrorCategory::None);
    
    static void logWarning(const std::string& message, const std::string& file = "", int line = 0,
                          const std::string& function = "", ErrorCategory category = ErrorCategory::None);
    
    static void logInfo(const std::string& message, const std::string& file = "", int line = 0,
                       const std::string& function = "", ErrorCategory category = ErrorCategory::None);
    
    static void handleCriticalError(const std::string& message,
                                   const std::string& file = "", int line = 0,
                                   const std::string& function = "", ErrorCategory category = ErrorCategory::None);

private:
    EnhancedErrorHandler();
    ~EnhancedErrorHandler();
    
    // Disable copying
    EnhancedErrorHandler(const EnhancedErrorHandler&) = delete;
    EnhancedErrorHandler& operator=(const EnhancedErrorHandler&) = delete;
    
    // Error recovery actions
    bool recoverMemoryError(const ErrorContext& error);
    bool recoverFileSystemError(const ErrorContext& error);
    bool recoverConfigurationError(const ErrorContext& error);
    bool recoverNetworkError(const ErrorContext& error);
    bool recoverValidationError(const ErrorContext& error);
    
    // Error analysis
    ErrorCategory analyzeErrorCategory(const ErrorContext& error) const;
    std::string analyzeRootCause(const ErrorContext& error) const;
    std::string analyzeImpact(const ErrorContext& error) const;
    
    // Error filtering
    bool shouldFilterError(const ErrorContext& error) const;
    bool isRecoverableError(const ErrorContext& error) const;
    
    // Error reporting
    void reportError(const ErrorContext& error);
    void reportErrorGroup(const std::vector<ErrorContext>& errors);
    
    // Statistics tracking
    struct Statistics {
        size_t totalErrors = 0;
        size_t errorsBySeverity[6] = {0};  // Array indexed by severity
        size_t errorsByCategory[12] = {0};  // Array indexed by category
        size_t recoveredErrors = 0;
        size_t autoRecoveredErrors = 0;
        std::chrono::system_clock::time_point firstErrorTime;
        std::chrono::system_clock::time_point lastErrorTime;
    };
    
    Statistics stats_;
    std::vector<ErrorContext> errors_;
    
    size_t maxErrors_;
    ErrorSeverity severityThreshold_;
    bool autoRecovery_;
    bool errorReporting_;
    
    // Error recovery strategies
    enum class RecoveryStrategy {
        None,
        Retry,
        Fallback,
        RestoreDefaults,
        Shutdown,
        Emergency,
        Custom
    };
    
    struct RecoveryAction {
        std::string name;
        RecoveryStrategy strategy;
        std::function<bool(const ErrorContext&)> condition;
        std::function<bool(const ErrorContext&)> action;
        std::string description;
    };
    
    std::vector<RecoveryAction> recoveryActions_;
};

// Global error handler instance
inline EnhancedErrorHandler& GetErrorHandler() {
    return EnhancedErrorHandler::getInstance();
}

// Convenience error handling functions
inline void LogError(ErrorSeverity severity, const std::string& message,
                    const std::string& file = "", int line = 0,
                    const std::string& function = "", ErrorCategory category = ErrorCategory::None) {
    GetErrorHandler().logError(severity, message, file, line, function, category);
}

inline void LogWarning(const std::string& message, const std::string& file = "", int line = 0,
                      const std::string& function = "", ErrorCategory category = ErrorCategory::None) {
    GetErrorHandler().logWarning(message, file, line, function, category);
}

inline void LogInfo(const std::string& message, const std::string& file = "", int line = 0,
                   const std::string& function = "", ErrorCategory category = ErrorCategory::None) {
    GetErrorHandler().logInfo(message, file, line, function, category);
}

inline void HandleCriticalError(const std::string& message,
                               const std::string& file = "", int line = 0,
                               const std::string& function = "", ErrorCategory category = ErrorCategory::None) {
    GetErrorHandler().handleCriticalError(message, file, line, function, category);
}

inline bool HasErrors(ErrorSeverity minSeverity = ErrorSeverity::Error) {
    return GetErrorHandler().hasErrors(minSeverity);
}

inline std::vector<ErrorContext> GetErrors(ErrorSeverity minSeverity = ErrorSeverity::Error) {
    return GetErrorHandler().getErrors(minSeverity);
}

inline void ClearErrors(ErrorSeverity minSeverity = ErrorSeverity::Error) {
    GetErrorHandler().clearErrors(minSeverity);
}

inline std::string GetErrorReport(ErrorSeverity minSeverity = ErrorSeverity::Error) {
    return GetErrorHandler().getErrorReport(minSeverity);
}

inline void AttemptRecovery(ErrorSeverity severity, const std::string& message) {
    GetErrorHandler().attemptRecovery(severity, message);
}

// Enhanced error checking macros
#define NLM_CHECK(condition, message, severity) \
    if (!(condition)) { \
        nlm::GetErrorHandler().logError(severity, message, __FILE__, __LINE__, __func__); \
        return false; \
    }

#define NLM_CHECK_WITH_CONTEXT(condition, message, context, severity) \
    if (!(condition)) { \
        nlm::GetErrorHandler().logError(severity, message + " (Context: " + context + ")", __FILE__, __LINE__, __func__); \
        return false; \
    }

#define NLM_THROW_ERROR(message, severity) \
    throw nlm::EnhancedException(severity, message, __FILE__, __LINE__, __func__)

#define NLM_THROW_ERROR_WITH_CATEGORY(message, severity, category) \
    throw nlm::EnhancedException(severity, message, __FILE__, __LINE__, __func__)

#define NLM_THROW_ERROR_CONTEXT(context) \
    throw nlm::EnhancedException(context)

// Validation macros
#define NLM_VALIDATE_INPUT(input, validation_func, error_message) \
    do { \
        if (!(validation_func(input))) { \
            nlm::GetErrorHandler().logError(nlm::ErrorSeverity::Error, error_message, __FILE__, __LINE__, __func__); \
            return false; \
        } \
    } while(0)

#define NLM_VALIDATE_FILE_PATH(path, require_exists) \
    do { \
        if (!nlm::GetErrorHandler().validateFilePath(path, require_exists, "File validation")) { \
            nlm::GetErrorHandler().logError(nlm::ErrorSeverity::Error, "Invalid file path: " + path, __FILE__, __LINE__, __func__); \
            return false; \
        } \
    } while(0)

#define NLM_VALIDATE_CONFIGURATION(key, value) \
    do { \
        if (!nlm::GetErrorHandler().validateConfiguration(key, value, "Configuration validation")) { \
            nlm::GetErrorHandler().logError(nlm::ErrorSeverity::Error, "Invalid configuration: " + key + " = " + value, __FILE__, __LINE__, __func__); \
            return false; \
        } \
    } while(0)

// Error handling utilities
inline bool IsRecoverableError(const ErrorContext& error) {
    return GetErrorHandler().isRecoverableError(error);
}

inline bool AttemptErrorRecovery(const ErrorContext& error) {
    return GetErrorHandler().attemptRecovery(error);
}

inline std::string FormatErrorReport(const std::vector<ErrorContext>& errors) {
    std::ostringstream report;
    report << "=== Error Report ===" << std::endl;
    for (const auto& error : errors) {
        report << nlm::EnhancedErrorHandler::formatErrorContext(error) << std::endl;
    }
    return report.str();
}

// Namespace declaration
} // namespace nlm

// Include implementation files
#include "EnhancedErrorHandler.cpp"