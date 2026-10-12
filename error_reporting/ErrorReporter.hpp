// Error Reporter Framework for NLM
// This file provides comprehensive error reporting and diagnostics

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <chrono>
#include <mutex>
#include <functional>
#include <sstream>
#include <iomanip>

namespace nlm {

// Error Levels
enum class ErrorLevel {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4,
    Fatal = 5
};

// Error Categories
enum class ErrorCategory {
    System = 0,
    Configuration = 1,
    Resource = 2,
    Network = 3,
    Logic = 4,
    Memory = 5,
    Performance = 6,
    Recovery = 7,
    Validation = 8,
    Custom = 9
};

// Error Information
struct ErrorInfo {
    std::string id;
    ErrorLevel level;
    ErrorCategory category;
    std::string message;
    std::string description;
    std::string sourceFile;
    std::string sourceFunction;
    int lineNumber;
    std::chrono::system_clock::time_point timestamp;
    std::string stackTrace;
    std::unordered_map<std::string, std::string> context;
    std::vector<std::string> suggestions;
    bool isHandled;
    
    ErrorInfo() : level(ErrorLevel::Info), category(ErrorCategory::Custom),
                 lineNumber(0), isHandled(false) {
        timestamp = std::chrono::system_clock::now();
    }
    
    std::string getFormattedMessage() const;
    std::string getLocationInfo() const;
    std::string getTimestampString() const;
    bool isError() const { return level >= ErrorLevel::Warning; }
    bool isCritical() const { return level >= ErrorLevel::Critical; }
};

// Error Reporter Interface
class ErrorReporter {
public:
    virtual ~ErrorReporter() = default;
    
    virtual void reportError(const ErrorInfo& error) = 0;
    virtual void reportError(const std::string& message, ErrorLevel level = ErrorLevel::Error,
                           ErrorCategory category = ErrorCategory::Custom,
                           const std::string& file = "", const std::string& function = "",
                           int line = 0) = 0;
    
    virtual void reportWarning(const std::string& message, ErrorCategory category = ErrorCategory::Custom) = 0;
    virtual void reportInfo(const std::string& message, ErrorCategory category = ErrorCategory::Custom) = 0;
    virtual void reportDebug(const std::string& message, ErrorCategory category = ErrorCategory::Custom) = 0;
    
    virtual std::vector<ErrorInfo> getRecentErrors(size_t count = 100) const = 0;
    virtual std::vector<ErrorInfo> getErrorsByLevel(ErrorLevel level) const = 0;
    virtual std::vector<ErrorInfo> getErrorsByCategory(ErrorCategory category) const = 0;
    virtual std::vector<ErrorInfo> getErrorsBySource(const std::string& file, const std::string& function = "") const = 0;
    
    virtual void clearErrors() = 0;
    virtual void setMaxErrorHistory(size_t maxErrors) = 0;
    virtual size_t getErrorHistorySize() const = 0;
    
    virtual std::string generateErrorReport() const = 0;
    virtual std::string generateErrorSummary() const = 0;
    
    virtual void setErrorCallback(const std::function<void(const ErrorInfo&)>& callback) = 0;
    virtual void setAlertThreshold(ErrorLevel level) = 0;
    
    virtual bool isAlertingEnabled() const = 0;
    virtual void setAlertingEnabled(bool enabled) = 0;
    virtual bool isErrorReportingEnabled() const = 0;
    virtual void setErrorReportingEnabled(bool enabled) = 0;
};

// Default Error Reporter
class DefaultErrorReporter : public ErrorReporter {
private:
    std::vector<ErrorInfo> errorHistory;
    std::mutex mutex;
    std::function<void(const ErrorInfo&)> errorCallback;
    ErrorLevel alertThreshold;
    bool alertingEnabled;
    bool errorReportingEnabled;
    
    static std::string generateErrorId();
    static std::string captureStackTrace();
    std::string formatError(const ErrorInfo& error) const;
    bool shouldAlert(const ErrorInfo& error) const;
    
public:
    DefaultErrorReporter()
        : alertThreshold(ErrorLevel::Warning), alertingEnabled(true),
          errorReportingEnabled(true) {}
    
    // Error reporting
    void reportError(const ErrorInfo& error) override;
    void reportError(const std::string& message, ErrorLevel level = ErrorLevel::Error,
                   ErrorCategory category = ErrorCategory::Custom,
                   const std::string& file = "", const std::string& function = "",
                   int line = 0) override;
    
    // Convenience methods
    void reportWarning(const std::string& message, ErrorCategory category = ErrorCategory::Custom) override {
        reportError(message, ErrorLevel::Warning, category);
    }
    
    void reportInfo(const std::string& message, ErrorCategory category = ErrorCategory::Custom) override {
        reportError(message, ErrorLevel::Info, category);
    }
    
    void reportDebug(const std::string& message, ErrorCategory category = ErrorCategory::Custom) override {
        reportError(message, ErrorLevel::Debug, category);
    }
    
    // Error retrieval
    std::vector<ErrorInfo> getRecentErrors(size_t count = 100) const override;
    std::vector<ErrorInfo> getErrorsByLevel(ErrorLevel level) const override;
    std::vector<ErrorInfo> getErrorsByCategory(ErrorCategory category) const override;
    std::vector<ErrorInfo> getErrorsBySource(const std::string& file, const std::string& function = "") const override;
    
    // Management
    void clearErrors() override;
    void setMaxErrorHistory(size_t maxErrors) override;
    size_t getErrorHistorySize() const override;
    
    // Reporting
    std::string generateErrorReport() const override;
    std::string generateErrorSummary() const override;
    
    // Configuration
    void setErrorCallback(const std::function<void(const ErrorInfo&)>& callback) override {
        errorCallback = callback;
    }
    
    void setAlertThreshold(ErrorLevel level) override {
        alertThreshold = level;
    }
    
    bool isAlertingEnabled() const override { return alertingEnabled; }
    void setAlertingEnabled(bool enabled) override { alertingEnabled = enabled; }
    bool isErrorReportingEnabled() const override { return errorReportingEnabled; }
    void setErrorReportingEnabled(bool enabled) override { errorReportingEnabled = enabled; }
    
    // Convenience methods for common error types
    void reportSystemError(const std::string& message, const std::string& component = "System");
    void reportConfigurationError(const std::string& message, const std::string& key = "");
    void reportResourceError(const std::string& message, const std::string& resource = "");
    void reportNetworkError(const std::string& message, const std::string& host = "");
    void reportMemoryError(const std::string& message, size_t allocationSize = 0);
    void reportPerformanceError(const std::string& message, const std::string& metric = "");
    void reportRecoveryError(const std::string& message, const std::string& operation = "");
    void reportValidationError(const std::string& message, const std::string& field = "");
};

// Global Error Reporter Instance
inline ErrorReporter& getDefaultErrorReporter() {
    static DefaultErrorReporter reporter;
    return reporter;
}

// Convenience Functions
inline void reportError(const std::string& message, ErrorLevel level = ErrorLevel::Error,
                       ErrorCategory category = ErrorCategory::Custom,
                       const std::string& file = "", const std::string& function = "",
                       int line = 0) {
    getDefaultErrorReporter().reportError(message, level, category, file, function, line);
}

inline void reportWarning(const std::string& message, ErrorCategory category = ErrorCategory::Custom) {
    getDefaultErrorReporter().reportWarning(message, category);
}

inline void reportInfo(const std::string& message, ErrorCategory category = ErrorCategory::Custom) {
    getDefaultErrorReporter().reportInfo(message, category);
}

inline void reportDebug(const std::string& message, ErrorCategory category = ErrorCategory::Custom) {
    getDefaultErrorReporter().reportDebug(message, category);
}

inline std::vector<ErrorInfo> getRecentErrors(size_t count = 100) {
    return getDefaultErrorReporter().getRecentErrors(count);
}

inline std::string generateErrorReport() {
    return getDefaultErrorReporter().generateErrorReport();
}

} // namespace nlm

