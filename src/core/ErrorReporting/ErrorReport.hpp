// ErrorReport.hpp - Error reporting interface for NLM
// This file defines interfaces for error reporting, monitoring, and recovery

#pragma once

#include "Error.hpp"
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <chrono>
#include <mutex>
#include <map>

namespace nlm {

// Error severity levels
enum class ErrorSeverity {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4,
    Fatal = 5
};

// Error category for classification
enum class ErrorCategory {
    None = 0,
    Configuration = 1,
    Memory = 2,
    Brain = 3,
    Logger = 4,
    World = 5,
    Validation = 6,
    System = 7,
    Network = 8,
    IO = 9,
    Plasticity = 10,
    Neuromodulation = 11,
    Cognitive = 12,
    Development = 13,
    Prediction = 14,
    MemorySystem = 15,
    Checkpoint = 16
};

// Detailed error information structure
struct DetailedError {
    // Error identification
    ErrorCategory category;
    std::string code;
    std::string message;
    std::string context;
    
    // Error details
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    
    // Error state
    ErrorSeverity severity;
    std::string stackTrace;
    std::map<std::string, std::string> metadata;
    
    // Recovery information
    bool recoverable;
    std::vector<std::string> suggestedActions;
    double estimatedRecoveryTime;
    
    // Statistics
    uint64_t occurrenceCount;
    std::chrono::system_clock::time_point firstOccurrence;
    std::chrono::system_clock::time_point lastOccurrence;
    
    DetailedError()
        : category(ErrorCategory::None), code(""), message(""), context(""),
          line(0), severity(ErrorSeverity::Info), recoverable(false),
          estimatedRecoveryTime(0.0), occurrenceCount(0) {}
    
    DetailedError(ErrorCategory cat, const std::string& errCode, const std::string& msg,
                 const std::string& ctx = "", ErrorSeverity sev = ErrorSeverity::Error)
        : category(cat), code(errCode), message(msg), context(ctx),
          line(0), severity(sev), recoverable(true), estimatedRecoveryTime(0.0), occurrenceCount(1) {
        timestamp = std::chrono::system_clock::now();
        firstOccurrence = timestamp;
        lastOccurrence = timestamp;
    }
};

// Error report structure for batch reporting
struct ErrorReport {
    std::chrono::system_clock::time_point reportTime;
    std::vector<DetailedError> errors;
    std::vector<DetailedError> warnings;
    std::map<ErrorCategory, uint64_t> errorCounts;
    uint64_t totalErrors;
    uint64_t totalWarnings;
    std::string summary;
    
    ErrorReport()
        : totalErrors(0), totalWarnings(0) {
        reportTime = std::chrono::system_clock::now();
    }
    
    void addError(const DetailedError& error) {
        errors.push_back(error);
        errorCounts[error.category]++;
        totalErrors++;
    }
    
    void addWarning(const DetailedError& warning) {
        warnings.push_back(warning);
        errorCounts[warning.category]++;
        totalWarnings++;
    }
    
    std::string generateSummary() const;
    
private:
    std::string getCategoryString(ErrorCategory category) const {
        static const std::unordered_map<ErrorCategory, std::string> categoryStrings = {
            {ErrorCategory::None, "NONE"},
            {ErrorCategory::Configuration, "CONFIG"},
            {ErrorCategory::Memory, "MEMORY"},
            {ErrorCategory::Brain, "BRAIN"},
            {ErrorCategory::Logger, "LOGGER"},
            {ErrorCategory::World, "WORLD"},
            {ErrorCategory::Validation, "VALIDATION"},
            {ErrorCategory::System, "SYSTEM"},
            {ErrorCategory::Network, "NETWORK"},
            {ErrorCategory::IO, "IO"},
            {ErrorCategory::Plasticity, "PLASTICITY"},
            {ErrorCategory::Neuromodulation, "NEUROMOD"},
            {ErrorCategory::Cognitive, "COGNITIVE"},
            {ErrorCategory::Development, "DEVELOPMENT"},
            {ErrorCategory::Prediction, "PREDICTION"},
            {ErrorCategory::MemorySystem, "MEMSYS"},
            {ErrorCategory::Checkpoint, "CHECKPOINT"}
        };
        
        auto it = categoryStrings.find(category);
        return (it != categoryStrings.end()) ? it->second : "UNKNOWN";
    }
};

// Error reporter interface
class IErrorReporter {
public:
    virtual ~IErrorReporter() = default;
    
    // Report a single error
    virtual void reportError(const DetailedError& error) = 0;
    
    // Report a warning
    virtual void reportWarning(const DetailedError& warning) = 0;
    
    // Report an error with automatic context
    virtual void reportError(ErrorCategory category, const std::string& code,
                            const std::string& message, const std::string& context = "",
                            ErrorSeverity severity = ErrorSeverity::Error,
                            const std::string& file = "", int line = 0,
                            const std::string& function = "") {
        DetailedError error(category, code, message, context, severity);
        error.file = file;
        error.line = line;
        error.function = function;
        reportError(error);
    }
    
    // Report a warning with automatic context
    virtual void reportWarning(ErrorCategory category, const std::string& code,
                              const std::string& message, const std::string& context = "",
                              const std::string& file = "", int line = 0,
                              const std::string& function = "") {
        DetailedError warning(category, code, message, context, ErrorSeverity::Warning);
        warning.file = file;
        warning.line = line;
        warning.function = function;
        reportWarning(warning);
    }
    
    // Query error statistics
    virtual uint64_t getErrorCount(ErrorCategory category = ErrorCategory::None) const = 0;
    virtual uint64_t getWarningCount(ErrorCategory category = ErrorCategory::None) const = 0;
    virtual std::chrono::system_clock::time_point getLastErrorTime(ErrorCategory category = ErrorCategory::None) const = 0;
    
    // Get recent errors
    virtual std::vector<DetailedError> getRecentErrors(size_t maxCount = 100) const = 0;
    virtual std::vector<DetailedError> getRecentWarnings(size_t maxCount = 100) const = 0;
    
    // Clear error history
    virtual void clear() = 0;
    
    // Export error report
    virtual ErrorReport generateReport() const = 0;
    
    // Configure reporter
    virtual void configure(const std::map<std::string, std::string>& config) = 0;
};

// Error handler interface for recovery
class IErrorHandler {
public:
    virtual ~IErrorHandler() = default;
    
    // Handle an error
    virtual bool handleError(const DetailedError& error) = 0;
    
    // Handle a warning
    virtual bool handleWarning(const DetailedError& warning) = 0;
    
    // Check if this handler can handle the error
    virtual bool canHandle(ErrorCategory category, ErrorSeverity severity) const = 0;
    
    // Get handler priority
    virtual int getPriority() const = 0;
    
    // Get handler name
    virtual std::string getName() const = 0;
};

// Error monitoring interface
class IErrorMonitor {
public:
    virtual ~IErrorMonitor() = default;
    
    // Start monitoring
    virtual void startMonitoring() = 0;
    
    // Stop monitoring
    virtual void stopMonitoring() = 0;
    
    // Check system health
    virtual bool isHealthy() const = 0;
    
    // Get health report
    virtual std::string getHealthReport() const = 0;
    
    // Set health check threshold
    virtual void setErrorThreshold(ErrorSeverity severity, uint64_t maxErrors) = 0;
    
    // Get health status
    virtual bool isErrorThresholdExceeded(ErrorCategory category) const = 0;
};

// Error manager for coordinating reporters, handlers, and monitors
class ErrorManager {
public:
    static ErrorManager& getInstance() {
        static ErrorManager instance;
        return instance;
    }
    
    // Add an error reporter
    void addErrorReporter(std::unique_ptr<IErrorReporter> reporter);
    
    // Remove an error reporter
    void removeErrorReporter(IErrorReporter* reporter);
    
    // Add an error handler
    void addErrorHandler(std::unique_ptr<IErrorHandler> handler);
    
    // Remove an error handler
    void removeErrorHandler(IErrorHandler* handler);
    
    // Add an error monitor
    void addErrorMonitor(std::unique_ptr<IErrorMonitor> monitor);
    
    // Remove an error monitor
    void removeErrorMonitor(IErrorMonitor* monitor);
    
    // Report an error
    void reportError(const DetailedError& error);
    
    // Report a warning
    void reportWarning(const DetailedError& warning);
    
    // Report an error with automatic context
    void reportError(ErrorCategory category, const std::string& code,
                    const std::string& message, const std::string& context = "",
                    ErrorSeverity severity = ErrorSeverity::Error,
                    const std::string& file = "", int line = 0,
                    const std::string& function = "");
    
    // Report a warning with automatic context
    void reportWarning(ErrorCategory category, const std::string& code,
                      const std::string& message, const std::string& context = "",
                      const std::string& file = "", int line = 0,
                      const std::string& function = "");
    
    // Get error report
    ErrorReport getErrorReport() const;
    
    // Clear all errors
    void clearErrors();
    
    // Configure error manager
    void configure(const std::map<std::string, std::string>& config);
    
    // Get error statistics
    uint64_t getTotalErrorCount() const;
    uint64_t getTotalWarningCount() const;
    uint64_t getErrorCount(ErrorCategory category) const;
    uint64_t getWarningCount(ErrorCategory category) const;
    
    // Check system health
    bool isSystemHealthy() const;
    
    // Get health report
    std::string getHealthReport() const;
    
    // Start/stop monitoring
    void startMonitoring();
    void stopMonitoring();
    
    // Get active monitors
    std::vector<IErrorMonitor*> getActiveMonitors() const;
    
private:
    ErrorManager() = default;
    ~ErrorManager() = default;
    ErrorManager(const ErrorManager&) = delete;
    ErrorManager& operator=(const ErrorManager&) = delete;
    
    std::vector<std::unique_ptr<IErrorReporter>> errorReporters_;
    std::vector<std::unique_ptr<IErrorHandler>> errorHandlers_;
    std::vector<std::unique_ptr<IErrorMonitor>> errorMonitors_;
    mutable std::mutex mutex_;
    
    // Default error reporter
    std::unique_ptr<IErrorReporter> defaultErrorReporter_;
    
    // Error filtering
    bool shouldReport(const DetailedError& error) const;
    bool shouldHandle(const DetailedError& error) const;
};

// Convenience functions for error reporting
namespace ErrorReporting {
    // Initialize error reporting system
    void initialize();
    
    // Shutdown error reporting system
    void shutdown();
    
    // Report functions with automatic context
    void report(ErrorCategory category, const std::string& code,
                const std::string& message, const std::string& context = "",
                ErrorSeverity severity = ErrorSeverity::Error,
                const std::string& file = "", int line = 0,
                const std::string& function = "");
    
    void reportWarning(ErrorCategory category, const std::string& code,
                      const std::string& message, const std::string& context = "",
                      const std::string& file = "", int line = 0,
                      const std::string& function = "");
    
    // Query functions
    uint64_t getErrorCount(ErrorCategory category = ErrorCategory::None);
    uint64_t getWarningCount(ErrorCategory category = ErrorCategory::None);
    ErrorReport getErrorReport();
    bool isSystemHealthy();
    std::string getHealthReport();
    
    // Configuration
    void configure(const std::map<std::string, std::string>& config);
    
    // Error management
    void clearErrors();
}

// Macros for error reporting with automatic context
#define REPORT_ERROR(category, code, message, context) \
    nlm::ErrorReporting::report(category, code, message, context, nlm::ErrorSeverity::Error, __FILE__, __LINE__, __func__)

#define REPORT_WARNING(category, code, message, context) \
    nlm::ErrorReporting::reportWarning(category, code, message, context, __FILE__, __LINE__, __func__)

#define REPORT_BRAIN_ERROR(code, message, context) \
    REPORT_ERROR(nlm::ErrorCategory::Brain, code, message, context)

#define REPORT_CONFIG_ERROR(code, message, context) \
    REPORT_ERROR(nlm::ErrorCategory::Configuration, code, message, context)

#define REPORT_MEMORY_ERROR(code, message, context) \
    REPORT_ERROR(nlm::ErrorCategory::Memory, code, message, context)

#define REPORT_LOGGER_ERROR(code, message, context) \
    REPORT_ERROR(nlm::ErrorCategory::Logger, code, message, context)

#define REPORT_WORLD_ERROR(code, message, context) \
    REPORT_ERROR(nlm::ErrorCategory::World, code, message, context)

#define REPORT_VALIDATION_ERROR(code, message, context) \
    REPORT_ERROR(nlm::ErrorCategory::Validation, code, message, context)

} // namespace nlm

// Helper function for creating detailed errors from NLMError
inline nlm::DetailedError createDetailedError(const nlm::NLMError& error) {
    nlm::ErrorCategory category = nlm::ErrorCategory::None;
    nlm::ErrorSeverity severity = nlm::ErrorSeverity::Error;
    
    // Map error type to category
    if (dynamic_cast<const nlm::ConfigError*>(&error)) {
        category = nlm::ErrorCategory::Configuration;
    } else if (dynamic_cast<const nlm::MemoryError*>(&error)) {
        category = nlm::ErrorCategory::Memory;
    } else if (dynamic_cast<const nlm::BrainError*>(&error)) {
        category = nlm::ErrorCategory::Brain;
    } else if (dynamic_cast<const nlm::LoggerError*>(&error)) {
        category = nlm::ErrorCategory::Logger;
    } else if (dynamic_cast<const nlm::WorldError*>(&error)) {
        category = nlm::ErrorCategory::World;
    } else if (dynamic_cast<const nlm::ValidationError*>(&error)) {
        category = nlm::ErrorCategory::Validation;
    }
    
    // Map error code to severity
    if (error.getCode() >= 4000) {  // Critical errors
        severity = nlm::ErrorSeverity::Critical;
    } else if (error.getCode() >= 3000) {  // High severity
        severity = nlm::ErrorSeverity::Error;
    } else if (error.getCode() >= 2000) {  // Medium severity
        severity = nlm::ErrorSeverity::Warning;
    } else {  // Low severity
        severity = nlm::ErrorSeverity::Info;
    }
    
    return nlm::DetailedError(
        category,
        std::to_string(error.getCode()),
        error.getMessage(),
        error.getContext(),
        severity
    );
}