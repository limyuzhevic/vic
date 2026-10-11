// Logger.hpp - Enhanced version with error handling
// This file defines the Logger class with comprehensive error handling,
// validation, and robustness features

#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <mutex>
#include <fstream>
#include <iomanip>
#include "Error.hpp"
#include "ErrorReport.hpp"

namespace nlm {

// Log severity levels
enum class LogLevel {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4
};

// Log entry structure with error context
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    std::string errorContext; // Additional error context from error reporting
    ErrorSeverity errorSeverity; // Map to error severity for error reporting
    
    LogEntry()
        : level(LogLevel::Info), line(0), errorSeverity(ErrorSeverity::Info) {}
    
    LogEntry(LogLevel lvl, const std::string& msg, const std::string& file = "", 
             int lineNum = 0, const std::string& func = "")
        : level(lvl), message(msg), file(file), line(lineNum), function(func),
          timestamp(std::chrono::system_clock::now()), errorSeverity(ErrorSeverity::Info) {}
};

// Logger interface
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
    virtual bool isEnabled(LogLevel level) const = 0;
};

// Enhanced console logger with error handling and file output
class ConsoleLogger : public ILogger {
public:
    ConsoleLogger();
    explicit ConsoleLogger(LogLevel level);
    ~ConsoleLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    bool isEnabled(LogLevel level) const override;
    
    // Enable/disable colors
    void setUseColors(bool useColors);
    bool getUseColors() const;
    
    // Set output file (in addition to console) with error handling
    void setOutputFile(const std::string& filepath);
    void closeOutputFile();
    
    // Error handling for file operations
    bool isFileOpen() const;
    std::string getOutputFilePath() const;
    
    // Flush and error recovery
    void flush();
    bool recoverFromError();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Helper methods
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
    std::string getFormattedTimestamp(std::chrono::system_clock::time_point timestamp) const;
    void logToFile(const LogEntry& entry);
    void logToConsole(const LogEntry& entry);
    
    // Error recovery and validation
    bool validateFilePath(const std::string& filepath) const;
    bool ensureFileCanWrite(const std::string& filepath);
};

// Error-aware logger wrapper for error reporting integration
class ErrorAwareLogger : public ILogger {
public:
    ErrorAwareLogger(std::shared_ptr<ILogger> underlyingLogger);
    ~ErrorAwareLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    bool isEnabled(LogLevel level) const override;
    
    // Error reporting integration
    void setErrorReporter(std::shared_ptr<IErrorReporter> errorReporter);
    std::shared_ptr<IErrorReporter> getErrorReporter() const;
    
    // Error context management
    void setErrorContext(const std::string& context);
    std::string getErrorContext() const;
    
    // Filter errors based on severity
    void setErrorSeverityFilter(ErrorSeverity minSeverity, ErrorSeverity maxSeverity);
    void setErrorCategoryFilter(ErrorCategory category);
    std::vector<ErrorCategory> getErrorCategories() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Error reporting integration
    void reportAsError(const LogEntry& entry);
    void reportAsWarning(const LogEntry& entry);
    bool shouldReportAsError(const LogEntry& entry) const;
    
    // Error context and filtering
    std::string errorContext_;
    ErrorSeverity minErrorSeverity_;
    ErrorSeverity maxErrorSeverity_;
    std::vector<ErrorCategory> errorCategories_;
    
    // Error statistics
    mutable std::mutex statsMutex_;
    uint64_t totalErrorsReported_;
    uint64_t totalWarningsReported_;
    std::map<ErrorCategory, uint64_t> errorsByCategory_;
    std::map<ErrorCategory, uint64_t> warningsByCategory_;
};

// Main Logger class with thread safety and error handling
class Logger {
public:
    Logger();
    ~Logger();
    
    // Disable copying, enable moving with error handling
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    Logger(Logger&&) noexcept;
    Logger& operator=(Logger&&) noexcept;
    
    // Add logger sink with error handling
    void addLogger(std::shared_ptr<ILogger> logger);
    
    // Remove all loggers
    void clearLoggers();
    
    // Set minimum level for all loggers with validation
    void setLevel(LogLevel level);
    
    // Global logging functions with error context
    void log(LogLevel level, const std::string& message, 
             const std::string& file = "", int line = 0, 
             const std::string& function = "", const std::string& errorContext = "");
    
    // Convenience methods with error context
    void debug(const std::string& message, const std::string& file = "", int line = 0, 
               const std::string& function = "", const std::string& errorContext = "");
    void info(const std::string& message, const std::string& file = "", int line = 0, 
              const std::string& function = "", const std::string& errorContext = "");
    void warning(const std::string& message, const std::string& file = "", int line = 0, 
                 const std::string& function = "", const std::string& errorContext = "");
    void error(const std::string& message, const std::string& file = "", int line = 0, 
               const std::string& function = "", const std::string& errorContext = "");
    void critical(const std::string& message, const std::string& file = "", int line = 0, 
                  const std::string& function = "", const std::string& errorContext = "");
    
    // Check if level is enabled
    bool isEnabled(LogLevel level) const;
    
    // Get global logger instance
    static Logger& getGlobal();
    
    // Set global logger with error handling
    static void setGlobal(std::shared_ptr<Logger> logger);
    
    // Error reporting integration
    void setErrorReporter(std::shared_ptr<IErrorReporter> errorReporter);
    std::shared_ptr<IErrorReporter> getErrorReporter() const;
    
    // Error statistics
    uint64_t getTotalErrorCount() const;
    uint64_t getTotalWarningCount() const;
    uint64_t getErrorCount(ErrorCategory category) const;
    uint64_t getWarningCount(ErrorCategory category) const;
    std::string getErrorStatistics() const;
    
    // Get recent errors and warnings
    std::vector<DetailedError> getRecentErrors(size_t maxCount = 100) const;
    std::vector<DetailedError> getRecentWarnings(size_t maxCount = 100) const;
    
    // Error management
    void clearErrors();
    bool isSystemHealthy() const;
    std::string getHealthReport() const;
    
    // Error recovery
    bool attemptRecovery(const std::string& operation, float maxRetries = 3.0f);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Thread safety for logger operations
    mutable std::mutex logMutex_;
    mutable std::mutex errorReporterMutex_;
    
    // Error reporting integration
    void logAsError(const LogEntry& entry);
    void logAsWarning(const LogEntry& entry);
    bool shouldLogAsError(const LogEntry& entry) const;
    
    // Error statistics
    void incrementErrorCount(ErrorCategory category);
    void incrementWarningCount(ErrorCategory category);
    void updateErrorStatistics();
    
    // Error recovery utilities
    bool isOperationCritical(const std::string& operation) const;
    double getRetryDelay(const std::string& operation) const;
};

// Convenience macro for file/line/function info
#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)

// Error reporting macros with context
#define NLM_REPORT_ERROR(category, code, message, context) \
    nlm::Logger::getGlobal().error(std::string("[Error ") + std::to_string(static_cast<int>(category)) + " " + code + "] " + message + " (Context: " + context + ")", __FILE__, __LINE__, __func__)

#define NLM_REPORT_WARNING(category, code, message, context) \
    nlm::Logger::getGlobal().warning(std::string("[Warning ") + std::to_string(static_cast<int>(category)) + " " + code + "] " + message + " (Context: " + context + ")", __FILE__, __LINE__, __func__)

#define NLM_REPORT_BRAIN_ERROR(code, message, context) \
    NLM_REPORT_ERROR(nlm::ErrorCategory::Brain, code, message, context)

#define NLM_REPORT_CONFIG_ERROR(code, message, context) \
    NLM_REPORT_ERROR(nlm::ErrorCategory::Configuration, code, message, context)

#define NLM_REPORT_MEMORY_ERROR(code, message, context) \
    NLM_REPORT_ERROR(nlm::ErrorCategory::Memory, code, message, context)

#define NLM_REPORT_LOGGER_ERROR(code, message, context) \
    NLM_REPORT_ERROR(nlm::ErrorCategory::Logger, code, message, context)

#define NLM_REPORT_WORLD_ERROR(code, message, context) \
    NLM_REPORT_ERROR(nlm::ErrorCategory::World, code, message, context)

#define NLM_REPORT_VALIDATION_ERROR(code, message, context) \
    NLM_REPORT_ERROR(nlm::ErrorCategory::Validation, code, message, context)

// Stream-style logging helper with error context
class ErrorAwareLogStream {
public:
    ErrorAwareLogStream(LogLevel level, const std::string& file, int line, 
                       const std::string& function, const std::string& errorContext = "");
    ~ErrorAwareLogStream();
    
    template<typename T>
    ErrorAwareLogStream& operator<<(const T& value) {
        oss_ << value;
        return *this;
    }
    
    // Set error context for the log message
    ErrorAwareLogStream& withContext(const std::string& context) {
        errorContext_ = context;
        return *this;
    }
    
    // Set error severity for the log message
    ErrorAwareLogStream& withSeverity(ErrorSeverity severity) {
        errorSeverity_ = severity;
        return *this;
    }
    
    // Set error category for the log message
    ErrorAwareLogStream& withCategory(ErrorCategory category) {
        errorCategory_ = category;
        return *this;
    }
    
private:
    LogLevel level_;
    std::ostringstream oss_;
    std::string file_;
    int line_;
    std::string function_;
    std::string errorContext_;
    ErrorSeverity errorSeverity_;
    ErrorCategory errorCategory_;
};

#define NLM_LOG_STREAM(level) nlm::ErrorAwareLogStream(level, __FILE__, __LINE__, __func__)

} // namespace nlm

// Helper function to create DetailedError from LogEntry
inline nlm::DetailedError createDetailedError(const nlm::LogEntry& entry) {
    nlm::ErrorCategory category = nlm::ErrorCategory::None;
    nlm::ErrorSeverity severity = nlm::ErrorSeverity::Info;
    
    // Map log level to error severity
    if (entry.level >= nlm::LogLevel::Error) {
        severity = (entry.level >= nlm::LogLevel::Critical) ? 
                   nlm::ErrorSeverity::Critical : nlm::ErrorSeverity::Error;
    } else if (entry.level == nlm::LogLevel::Warning) {
        severity = nlm::ErrorSeverity::Warning;
    } else if (entry.level == nlm::LogLevel::Info) {
        severity = nlm::ErrorSeverity::Info;
    } else { // Debug
        severity = nlm::ErrorSeverity::Debug;
    }
    
    // Determine error category based on log content and context
    // This is a simple heuristic - can be enhanced with more sophisticated classification
    std::string message = entry.message;
    if (message.find("[Error ") != std::string::npos) {
        category = nlm::ErrorCategory::Brain;
    } else if (message.find("[Config ") != std::string::npos) {
        category = nlm::ErrorCategory::Configuration;
    } else if (message.find("[Memory ") != std::string::npos) {
        category = nlm::ErrorCategory::Memory;
    } else if (message.find("[Logger ") != std::string::npos) {
        category = nlm::ErrorCategory::Logger;
    } else if (message.find("[World ") != std::string::npos) {
        category = nlm::ErrorCategory::World;
    } else if (message.find("[Validation ") != std::string::npos) {
        category = nlm::ErrorCategory::Validation;
    }
    
    // Create DetailedError from LogEntry
    nlm::DetailedError error(category, "LOG_" + std::to_string(static_cast<int>(severity)),
                            message, entry.errorContext, severity);
    error.file = entry.file;
    error.line = entry.line;
    error.function = entry.function;
    error.timestamp = entry.timestamp;
    error.errorSeverity = severity;
    
    return error;
}