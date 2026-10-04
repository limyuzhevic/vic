#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <map>
#include <vector>
#include <fstream>
#include <mutex>
#include "ErrorHandling/Error.hpp"

namespace nlm {

class EnhancedLogger {
public:
    EnhancedLogger() : logLevel_(LogLevel::Info) {}

    // Enhanced logging with error context
    void logError(const std::string& message,
                  ErrorCategory category,
                  ErrorSeverity severity,
                  const std::string& file = "",
                  int line = 0,
                  const std::string& function = "",
                  const std::map<std::string, std::string>& context = {}) {

        std::lock_guard<std::mutex> lock(mutex_);

        // Create enhanced log entry
        LogEntry entry;
        entry.level = (severity >= ErrorSeverity::Error) ? LogLevel::Error :
                     (severity >= ErrorSeverity::Warning) ? LogLevel::Warning :
                     (severity >= ErrorSeverity::Info) ? LogLevel::Info : LogLevel::Debug;
        entry.timestamp = std::chrono::system_clock::now();

        // Build enhanced message with error details
        std::ostringstream oss;
        oss << "[" << errorCategoryToString(category) << "] ";
        oss << "[" << errorSeverityToString(severity) << "] ";

        if (!file.empty()) {
            oss << "File: " << file;
            if (line > 0) oss << ":" << line;
            oss << " ";
        }

        oss << message;

        // Add context
        for (const auto& [key, value] : context) {
            oss << " | " << key << "=" << value;
        }

        entry.message = oss.str();
        entry.file = file;
        entry.line = line;
        entry.function = function;

        // Log to all registered loggers
        for (const auto& logger : loggers_) {
            logger->log(entry);
        }
    }

    // Convenience method for error logging with exception capture
    template<typename T>
    void logAndRethrow(const std::string& operation,
                      const std::exception& e,
                      ErrorCategory category = ErrorCategory::Unknown,
                      ErrorSeverity severity = ErrorSeverity::Error,
                      const std::string& file = "",
                      int line = 0,
                      const std::string& function = "") {

        std::map<std::string, std::string> context;
        context["operation"] = operation;
        context["exception_type"] = typeid(e).name();
        context["exception_message"] = e.what();

        std::string enhancedMessage = "Operation ";
        enhancedMessage += "'" + operation + "' failed: ";
        enhancedMessage += e.what();

        logError(enhancedMessage, category, severity, file, line, function, context);

        // Re-throw with enhanced context if it's already a NLMError
        if (dynamic_cast<const NLMError*>(&e)) {
            throw;
        } else {
            throw NLMError(enhancedMessage, category, severity, file, line, function);
        }
    }

    // System health monitoring
    struct SystemHealth {
        double cpuUsage;
        double memoryUsage;
        double diskUsage;
        int activeThreads;
        int errorCount;
        double lastErrorTime;
    };

    SystemHealth getSystemHealth() const {
        SystemHealth health;
        // Implement actual health monitoring
        health.cpuUsage = 0.0;
        health.memoryUsage = 0.0;
        health.diskUsage = 0.0;
        health.activeThreads = 0;
        health.errorCount = errorCount_;
        health.lastErrorTime = lastErrorTime_;
        return health;
    }

    // Configuration validation
    struct ValidationResult {
        bool isValid;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        double performanceImpact;
    };

    ValidationResult validateConfiguration() {
        ValidationResult result;
        result.isValid = true;
        result.performanceImpact = 0.0;

        // Implement configuration validation logic
        // Check for invalid settings, missing required values, etc.

        return result;
    }

    // Environment checking
    struct EnvironmentInfo {
        std::string platform;
        std::string architecture;
        double totalMemory;
        double availableMemory;
        std::string compilerVersion;
        std::string buildType;
    };

    EnvironmentInfo getEnvironmentInfo() const {
        EnvironmentInfo env;
        // Implement environment detection
        return env;
    }

    // Set minimum log level
    void setLogLevel(LogLevel level) {
        std::lock_guard<std::mutex> lock(mutex_);
        logLevel_ = level;
    }

    // Get minimum log level
    LogLevel getLogLevel() const {
        return logLevel_;
    }

    // Add logger sink
    void addLogger(std::shared_ptr<ILogger> logger) {
        std::lock_guard<std::mutex> lock(mutex_);
        loggers_.push_back(logger);
        logger->setLevel(logLevel_);
    }

    // Record error for statistics
    void recordError() {
        std::lock_guard<std::mutex> lock(mutex_);
        errorCount_++;
        lastErrorTime_ = std::chrono::system_clock::now().time_since_epoch().count() / 1e9;
    }

    // Get error count
    int getErrorCount() const {
        return errorCount_;
    }

private:
    LogLevel logLevel_;
    std::vector<std::shared_ptr<ILogger>> loggers_;
    mutable std::mutex mutex_;
    int errorCount_ = 0;
    double lastErrorTime_ = 0.0;

    static std::string errorCategoryToString(ErrorCategory category) {
        static const std::map<ErrorCategory, std::string> categoryMap = {
            {ErrorCategory::Configuration, "CONFIG"},
            {ErrorCategory::Validation, "VALID"},
            {ErrorCategory::Memory, "MEM"},
            {ErrorCategory::Neural, "NEURAL"},
            {ErrorCategory::I_O, "IO"},
            {ErrorCategory::System, "SYS"},
            {ErrorCategory::Neuromodulation, "NEUROMOD"},
            {ErrorCategory::Development, "DEV"},
            {ErrorCategory::Sensory, "SENS"},
            {ErrorCategory::Motor, "MOTOR"},
            {ErrorCategory::Unknown, "UNKNOWN"}
        };
        auto it = categoryMap.find(category);
        return it != categoryMap.end() ? it->second : "UNK";
    }

    static std::string errorSeverityToString(ErrorSeverity severity) {
        static const std::map<ErrorSeverity, std::string> severityMap = {
            {ErrorSeverity::Debug, "DEBUG"},
            {ErrorSeverity::Info, "INFO"},
            {ErrorSeverity::Warning, "WARN"},
            {ErrorSeverity::Error, "ERR"},
            {ErrorSeverity::Critical, "CRIT"},
            {ErrorSeverity::Fatal, "FATAL"}
        };
        auto it = severityMap.find(severity);
        return it != severityMap.end() ? it->second : "ERR";
    }
};

} // namespace nlm