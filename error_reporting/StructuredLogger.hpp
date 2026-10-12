// Structured Logger Framework for NLM
// This file provides structured logging with correlation IDs and advanced features

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
#include <json.hpp>

namespace nlm {

using json = nlohmann::json;

// Log Levels
enum class LogLevel {
    Trace = 0,
    Debug = 1,
    Info = 2,
    Notice = 3,
    Warning = 4,
    Error = 5,
    Critical = 6,
    Alert = 7,
    Emergency = 8
};

// Log Categories
enum class LogCategory {
    System = 0,
    Configuration = 1,
    Resource = 2,
    Network = 3,
    Neural = 4,
    Memory = 5,
    Performance = 6,
    Recovery = 7,
    Validation = 8,
    Monitoring = 9,
    Custom = 10
};

// Log Entry
struct LogEntry {
    std::string id;
    std::string correlationId;
    LogLevel level;
    LogCategory category;
    std::chrono::system_clock::time_point timestamp;
    std::string message;
    std::string sourceFile;
    std::string sourceFunction;
    int lineNumber;
    
    std::unordered_map<std::string, std::string> structuredFields;
    std::unordered_map<std::string, double> numericFields;
    std::unordered_map<std::string, std::vector<std::string>> arrayFields;
    
    std::string threadId;
    std::string processId;
    std::string hostName;
    
    std::string getFormattedMessage() const;
    std::string getFormattedTimestamp() const;
    std::string getLocationInfo() const;
    
    json toJson() const;
    static LogEntry fromJson(const json& j);
    
    std::string getLevelString() const;
    std::string getCategoryString() const;
    
    bool isError() const { return level >= LogLevel::Error; }
    bool isWarning() const { return level >= LogLevel::Warning; }
    bool isInfo() const { return level >= LogLevel::Info; }
    bool isDebug() const { return level >= LogLevel::Debug; }
    bool isTrace() const { return level >= LogLevel::Trace; }
};

// Logger Interface
class Logger {
public:
    virtual ~Logger() = default;
    
    virtual void log(const LogEntry& entry) = 0;
    virtual void log(const std::string& message, LogLevel level = LogLevel::Info,
                   LogCategory category = LogCategory::Custom,
                   const std::string& file = "", const std::string& function = "",
                   int line = 0) = 0;
    
    virtual void logTrace(const std::string& message, LogCategory category = LogCategory::Custom) {
        log(message, LogLevel::Trace, category);
    }
    
    virtual void logDebug(const std::string& message, LogCategory category = LogCategory::Custom) {
        log(message, LogLevel::Debug, category);
    }
    
    virtual void logInfo(const std::string& message, LogCategory category = LogCategory::Custom) {
        log(message, LogLevel::Info, category);
    }
    
    virtual void logNotice(const std::string& message, LogCategory category = LogCategory::Custom) {
        log(message, LogLevel::Notice, category);
    }
    
    virtual void logWarning(const std::string& message, LogCategory category = LogCategory::Custom) {
        log(message, LogLevel::Warning, category);
    }
    
    virtual void logError(const std::string& message, LogCategory category = LogCategory::Custom) {
        log(message, LogLevel::Error, category);
    }
    
    virtual void logCritical(const std::string& message, LogCategory category = LogCategory::Custom) {
        log(message, LogLevel::Critical, category);
    }
    
    virtual void logAlert(const std::string& message, LogCategory category = LogCategory::Custom) {
        log(message, LogLevel::Alert, category);
    }
    
    virtual void logEmergency(const std::string& message, LogCategory category = LogCategory::Custom) {
        log(message, LogLevel::Emergency, category);
    }
    
    virtual std::string generateCorrelationId() const;
    virtual std::string generateLogId() const;
    
    virtual void setLogLevel(LogLevel level) = 0;
    virtual LogLevel getLogLevel() const = 0;
    
    virtual void setCategoryEnabled(LogCategory category, bool enabled) = 0;
    virtual bool isCategoryEnabled(LogCategory category) const = 0;
    
    virtual void setFormat(const std::string& format) = 0;
    virtual std::string getFormat() const = 0;
    
    virtual void setStructuredLoggingEnabled(bool enabled) = 0;
    virtual bool isStructuredLoggingEnabled() const = 0;
    
    virtual std::vector<LogEntry> getLogHistory(size_t count = 100) const = 0;
    virtual std::vector<LogEntry> getLogsByLevel(LogLevel level) const = 0;
    virtual std::vector<LogEntry> getLogsByCategory(LogCategory category) const = 0;
    virtual std::vector<LogEntry> getLogsByCorrelationId(const std::string& correlationId) const = 0;
    
    virtual void clearLogHistory() = 0;
    virtual void setMaxLogHistory(size_t maxLogs) = 0;
    virtual size_t getLogHistorySize() const = 0;
    
    virtual std::string generateLogReport() const = 0;
    virtual std::string generateLogSummary() const = 0;
    
    virtual void setLogCallback(const std::function<void(const LogEntry&)>& callback) = 0;
    
    virtual bool isRunning() const = 0;
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual void pause() = 0;
    virtual void resume() = 0;
};

// Default Logger
class DefaultLogger : public Logger {
private:
    std::vector<LogEntry> logHistory;
    std::mutex mutex;
    std::function<void(const LogEntry&)> logCallback;
    
    std::string getCurrentThreadId() const;
    std::string getCurrentProcessId() const;
    std::string getCurrentHostName() const;
    std::chrono::system_clock::time_point getCurrentTime() const;
    
    LogLevel currentLogLevel;
    std::unordered_map<LogCategory, bool> categoryEnabled;
    std::string logFormat;
    bool structuredLoggingEnabled;
    
    std::atomic<size_t> logCount;
    std::chrono::system_clock::time_point lastLogTime;
    
    std::string formatMessage(const LogEntry& entry) const;
    std::string formatJson(const LogEntry& entry) const;
    std::string formatPlainText(const LogEntry& entry) const;
    
    bool shouldLog(const LogEntry& entry) const;
    
public:
    DefaultLogger()
        : currentLogLevel(LogLevel::Info), structuredLoggingEnabled(true),
          logCount(0), lastLogTime(std::chrono::system_clock::now()) {
        
        categoryEnabled[LogCategory::System] = true;
        categoryEnabled[LogCategory::Configuration] = true;
        categoryEnabled[LogCategory::Resource] = true;
        categoryEnabled[LogCategory::Network] = true;
        categoryEnabled[LogCategory::Neural] = true;
        categoryEnabled[LogCategory::Memory] = true;
        categoryEnabled[LogCategory::Performance] = true;
        categoryEnabled[LogCategory::Recovery] = true;
        categoryEnabled[LogCategory::Validation] = true;
        categoryEnabled[LogCategory::Monitoring] = true;
        
        logFormat = "%(timestamp)s [%(level)s] %(category)s: %(message)s";
    }
    
    // Logging
    void log(const LogEntry& entry) override;
    void log(const std::string& message, LogLevel level = LogLevel::Info,
             LogCategory category = LogCategory::Custom,
             const std::string& file = "", const std::string& function = "",
             int line = 0) override;
    
    // Convenience methods
    void logTrace(const std::string& message, LogCategory category = LogCategory::Custom) override {
        log(message, LogLevel::Trace, category);
    }
    
    void logDebug(const std::string& message, LogCategory category = LogCategory::Custom) override {
        log(message, LogLevel::Debug, category);
    }
    
    void logInfo(const std::string& message, LogCategory category = LogCategory::Custom) override {
        log(message, LogLevel::Info, category);
    }
    
    void logNotice(const std::string& message, LogCategory category = LogCategory::Custom) override {
        log(message, LogLevel::Notice, category);
    }
    
    void logWarning(const std::string& message, LogCategory category = LogCategory::Custom) override {
        log(message, LogLevel::Warning, category);
    }
    
    void logError(const std::string& message, LogCategory category = LogCategory::Custom) override {
        log(message, LogLevel::Error, category);
    }
    
    void logCritical(const std::string& message, LogCategory category = LogCategory::Custom) override {
        log(message, LogLevel::Critical, category);
    }
    
    void logAlert(const std::string& message, LogCategory category = LogCategory::Custom) override {
        log(message, LogLevel::Alert, category);
    }
    
    void logEmergency(const std::string& message, LogCategory category = LogCategory::Custom) override {
        log(message, LogLevel::Emergency, category);
    }
    
    // Correlation ID support
    std::string generateCorrelationId() const override {
        return "CORR_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
    }
    
    std::string generateLogId() const override {
        return "LOG_" + std::to_string(logCount.fetch_add(1));
    }
    
    // Configuration
    void setLogLevel(LogLevel level) override { currentLogLevel = level; }
    LogLevel getLogLevel() const override { return currentLogLevel; }
    
    void setCategoryEnabled(LogCategory category, bool enabled) override {
        categoryEnabled[category] = enabled;
    }
    
    bool isCategoryEnabled(LogCategory category) const override {
        auto it = categoryEnabled.find(category);
        return it != categoryEnabled.end() && it->second;
    }
    
    void setFormat(const std::string& format) override { logFormat = format; }
    std::string getFormat() const override { return logFormat; }
    
    void setStructuredLoggingEnabled(bool enabled) override { structuredLoggingEnabled = enabled; }
    bool isStructuredLoggingEnabled() const override { return structuredLoggingEnabled; }
    
    // Log retrieval
    std::vector<LogEntry> getLogHistory(size_t count = 100) const override;
    std::vector<LogEntry> getLogsByLevel(LogLevel level) const override;
    std::vector<LogEntry> getLogsByCategory(LogCategory category) const override;
    std::vector<LogEntry> getLogsByCorrelationId(const std::string& correlationId) const override;
    
    // Management
    void clearLogHistory() override;
    void setMaxLogHistory(size_t maxLogs) override;
    size_t getLogHistorySize() const override;
    
    // Reporting
    std::string generateLogReport() const override;
    std::string generateLogSummary() const override;
    
    // Callback
    void setLogCallback(const std::function<void(const LogEntry&)>& callback) override {
        logCallback = callback;
    }
    
    // Status
    bool isRunning() const override { return true; }
    void start() override {}
    void stop() override {}
    void pause() override {}
    void resume() override {}
    
    // Convenience methods for common log types
    void logSystemInfo(const std::string& message);
    void logSystemWarning(const std::string& message);
    void logSystemError(const std::string& message);
    
    void logConfigurationInfo(const std::string& message);
    void logConfigurationWarning(const std::string& message);
    void logConfigurationError(const std::string& message);
    
    void logResourceInfo(const std::string& message);
    void logResourceWarning(const std::string& message);
    void logResourceError(const std::string& message);
    
    void logNetworkInfo(const std::string& message);
    void logNetworkWarning(const std::string& message);
    void logNetworkError(const std::string& message);
    
    void logNeuralInfo(const std::string& message);
    void logNeuralWarning(const std::string& message);
    void logNeuralError(const std::string& message);
    
    void logMemoryInfo(const std::string& message);
    void logMemoryWarning(const std::string& message);
    void logMemoryError(const std::string& message);
    
    void logPerformanceInfo(const std::string& message);
    void logPerformanceWarning(const std::string& message);
    void logPerformanceError(const std::string& message);
    
    void logRecoveryInfo(const std::string& message);
    void logRecoveryWarning(const std::string& message);
    void logRecoveryError(const std::string& message);
    
    void logValidationInfo(const std::string& message);
    void logValidationWarning(const std::string& message);
    void logValidationError(const std::string& message);
    
    void logMonitoringInfo(const std::string& message);
    void logMonitoringWarning(const std::string& message);
    void logMonitoringError(const std::string& message);
};

// Global Logger Instance
inline Logger& getDefaultLogger() {
    static DefaultLogger logger;
    return logger;
}

// Convenience Functions
inline void log(const std::string& message, LogLevel level = LogLevel::Info,
               LogCategory category = LogCategory::Custom,
               const std::string& file = "", const std::string& function = "",
               int line = 0) {
    getDefaultLogger().log(message, level, category, file, function, line);
}

inline void logTrace(const std::string& message, LogCategory category = LogCategory::Custom) {
    getDefaultLogger().logTrace(message, category);
}

inline void logDebug(const std::string& message, LogCategory category = LogCategory::Custom) {
    getDefaultLogger().logDebug(message, category);
}

inline void logInfo(const std::string& message, LogCategory category = LogCategory::Custom) {
    getDefaultLogger().logInfo(message, category);
}

inline void logNotice(const std::string& message, LogCategory category = LogCategory::Custom) {
    getDefaultLogger().logNotice(message, category);
}

inline void logWarning(const std::string& message, LogCategory category = LogCategory::Custom) {
    getDefaultLogger().logWarning(message, category);
}

inline void logError(const std::string& message, LogCategory category = LogCategory::Custom) {
    getDefaultLogger().logError(message, category);
}

inline void logCritical(const std::string& message, LogCategory category = LogCategory::Custom) {
    getDefaultLogger().logCritical(message, category);
}

inline void logAlert(const std::string& message, LogCategory category = LogCategory::Custom) {
    getDefaultLogger().logAlert(message, category);
}

inline void logEmergency(const std::string& message, LogCategory category = LogCategory::Custom) {
    getDefaultLogger().logEmergency(message, category);
}

inline std::string generateCorrelationId() {
    return getDefaultLogger().generateCorrelationId();
}

inline std::vector<LogEntry> getLogHistory(size_t count = 100) {
    return getDefaultLogger().getLogHistory(count);
}

inline std::string generateLogReport() {
    return getDefaultLogger().generateLogReport();
}

} // namespace nlm

