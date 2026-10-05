#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <thread>
#include <mutex>
#include <unordered_map>
#include <set>
#include <algorithm>

namespace nlm {

// Enhanced Log severity levels with more granularity
enum class LogLevel {
    TRACE = 0,
    DEBUG = 1,
    INFO = 2,
    NOTICE = 3,
    WARNING = 4,
    ERROR = 5,
    CRITICAL = 6,
    FATAL = 7
};

// Enhanced Log entry with additional metadata
struct EnhancedLogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    std::thread::id threadId;
    uint64_t sequenceNumber;
    
    // Additional context for better debugging
    std::string module;              // "brain", "world", "agent", etc.
    std::string experiment;          // "phase3", "phase4", "phase6", etc.
    std::unordered_map<std::string, std::string> metadata;
    std::vector<std::string> tags;   // For categorizing logs
    
    // Performance tracking
    std::chrono::microseconds processingTime;
};

// Enhanced logger interface
class IEnhancedLogger {
public:
    virtual ~IEnhancedLogger() = default;
    
    // Set minimum log level
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
    
    // Set filtering rules
    virtual void setFilter(std::function<bool(const EnhancedLogEntry&)>
                          filter) = 0;
    
    // Add tags to filter logs
    virtual void setRequiredTags(const std::set<std::string>& tags) = 0;
    virtual void setExcludeTags(const std::set<std::string>& tags) = 0;
    
    // Set module filter
    virtual void setModuleFilter(const std::set<std::string>& modules) = 0;
    
    // Set experiment filter
    virtual void setExperimentFilter(const std::set<std::string>& experiments) = 0;
    
    // Enable/disable structured logging
    virtual void setStructuredLogging(bool enabled) = 0;
    virtual bool getStructuredLogging() const = 0;
    
    // Enable/disable performance tracking
    virtual void setPerformanceTracking(bool enabled) = 0;
    virtual bool getPerformanceTracking() const = 0;
    
    // Set log format
    virtual void setFormat(const std::string& format) = 0;
    virtual std::string getFormat() const = 0;
    
    // Log the entry
    virtual void log(const EnhancedLogEntry& entry) = 0;
    
    // Clear any buffered logs
    virtual void flush() = 0;
    
    // Get log statistics
    virtual struct LogStatistics {
        uint64_t totalLogs;
        std::unordered_map<LogLevel, uint64_t> byLevel;
        std::unordered_map<std::string, uint64_t> byModule;
        std::unordered_map<std::string, uint64_t> byTag;
        uint64_t errors;
        uint64_t warnings;
        double avgProcessingTime;
    } getStatistics() const = 0;
};

// Enhanced Console logger implementation
class EnhancedConsoleLogger : public IEnhancedLogger {
public:
    EnhancedConsoleLogger();
    explicit EnhancedConsoleLogger(LogLevel level);
    ~EnhancedConsoleLogger() override;
    
    // IEnhancedLogger interface
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    void setFilter(std::function<bool(const EnhancedLogEntry&)>
                  filter) override;
    
    void setRequiredTags(const std::set<std::string>& tags) override;
    void setExcludeTags(const std::set<std::string>& tags) override;
    
    void setModuleFilter(const std::set<std::string>& modules) override;
    void setExperimentFilter(const std::set<std::string>& experiments) override;
    
    void setStructuredLogging(bool enabled) override;
    bool getStructuredLogging() const override;
    
    void setPerformanceTracking(bool enabled) override;
    bool getPerformanceTracking() const override;
    
    void setFormat(const std::string& format) override;
    std::string getFormat() const override;
    
    void log(const EnhancedLogEntry& entry) override;
    
    void flush() override;
    
    struct LogStatistics getStatistics() const override;
    
    // Enhanced ConsoleLogger specific methods
    void setUseColors(bool useColors);
    bool getUseColors() const;
    
    void setOutputFile(const std::string& filepath);
    void closeOutputFile();
    
    // Set custom formatters
    void setMessageFormatter(std::function<std::string(const EnhancedLogEntry&)> formatter);
    void setTimestampFormatter(std::function<std::string(const EnhancedLogEntry&)> formatter);
    
    // Add context information
    void setDefaultModule(const std::string& module);
    void setDefaultExperiment(const std::string& experiment);
    void addMetadata(const std::string& key, const std::string& value);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string formatMessage(const EnhancedLogEntry& entry) const;
    std::string formatTimestamp(const EnhancedLogEntry& entry) const;
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
};

// Enhanced main Logger class with backward compatibility
class EnhancedLogger {
public:
    EnhancedLogger();
    ~EnhancedLogger();
    
    // Disable copying
    EnhancedLogger(const EnhancedLogger&) = delete;
    EnhancedLogger& operator=(const EnhancedLogger&) = delete;
    
    // Add enhanced logger sink
    void addEnhancedLogger(std::shared_ptr<IEnhancedLogger> logger);
    
    // Add console logger for backward compatibility
    void addConsoleLogger(LogLevel level = LogLevel::INFO);
    
    // Remove all loggers
    void clearLoggers();
    
    // Set minimum level for all loggers
    void setLevel(LogLevel level);
    
    // Enhanced logging with more context
    void log(LogLevel level, const std::string& message,
             const std::string& file = "", int line = 0,
             const std::string& function = "",
             const std::string& module = "",
             const std::string& experiment = "");
    
    // Log with metadata
    void logWithMetadata(LogLevel level, const std::string& message,
                        const std::unordered_map<std::string, std::string>& metadata,
                        const std::string& file = "", int line = 0,
                        const std::string& function = "",
                        const std::string& module = "",
                        const std::string& experiment = "");
    
    // Convenience methods with more levels
    void trace(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void debug(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void info(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void notice(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void warning(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void error(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void critical(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void fatal(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    // Performance tracking
    void logWithPerformance(LogLevel level, const std::string& message,
                          const std::chrono::microseconds& processingTime,
                          const std::string& file = "", int line = 0,
                          const std::string& function = "");
    
    // Check if level is enabled
    bool isEnabled(LogLevel level) const;
    
    // Get global logger instance
    static EnhancedLogger& getGlobal();
    
    // Set global logger
    static void setGlobal(std::shared_ptr<EnhancedLogger> logger);
    
    // Export logs to file
    void exportLogs(const std::string& filepath, LogLevel minLevel = LogLevel::Info);
    
    // Search logs
    std::vector<EnhancedLogEntry> searchLogs(
        std::function<bool(const EnhancedLogEntry&)> predicate) const;
    
    // Get statistics
    struct Statistics {
        uint64_t totalLogs;
        std::unordered_map<LogLevel, uint64_t> byLevel;
        std::unordered_map<std::string, uint64_t> byModule;
        std::unordered_map<std::string, uint64_t> byTag;
        uint64_t errors;
        uint64_t warnings;
        double avgProcessingTime;
        std::chrono::system_clock::time_point firstLog;
        std::chrono::system_clock::time_point lastLog;
    };
    
    Statistics getStatistics() const;
    
    // Clear statistics
    void clearStatistics();
    
    // Set up default formatters for different modules
    void setupConsoleColors(bool enabled);
    void setupStructuredLogging(bool enabled);
    void setupPerformanceLogging(bool enabled);
    
    // Add tag to log entry
    static void addTag(EnhancedLogEntry& entry, const std::string& tag);
    
    // Create log entry with context
    static EnhancedLogEntry createLogEntry(LogLevel level, const std::string& message,
                                          const std::string& file = "", int line = 0,
                                          const std::string& function = "",
                                          const std::string& module = "",
                                          const std::string& experiment = "");

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Convenience macros for enhanced logging
#define NLM_LOG_TRACE(msg) nlm::EnhancedLogger::getGlobal().trace(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_DEBUG(msg) nlm::EnhancedLogger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO(msg) nlm::EnhancedLogger::getGlobal().info(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_NOTICE(msg) nlm::EnhancedLogger::getGlobal().notice(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING(msg) nlm::EnhancedLogger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR(msg) nlm::EnhancedLogger::getGlobal().error(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_CRITICAL(msg) nlm::EnhancedLogger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_FATAL(msg) nlm::EnhancedLogger::getGlobal().fatal(msg, __FILE__, __LINE__, __func__)

// Performance tracking macro
#define NLM_LOG_PERF(level, msg) { \
    auto start = std::chrono::high_resolution_clock::now(); \
    /* Perform operation */ \
    auto end = std::chrono::high_resolution_clock::now(); \
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start); \
    nlm::EnhancedLogger::getGlobal().logWithPerformance(level, msg, duration, __FILE__, __LINE__, __func__); \
}

// Error with context macro
#define NLM_LOG_ERROR_WITH_CONTEXT(errorMsg, contextKey, contextValue) { \
    auto entry = nlm::EnhancedLogger::createLogEntry(nlm::LogLevel::Error, errorMsg, __FILE__, __LINE__, __func__); \
    entry.metadata[contextKey] = contextValue; \
    nlm::EnhancedLogger::getGlobal().logWithMetadata(nlm::LogLevel::Error, errorMsg, entry.metadata, __FILE__, __LINE__, __func__); \
}

// Module-specific logging
#define NLM_BRAIN_LOG(level, msg) nlm::EnhancedLogger::getGlobal().log(level, msg, __FILE__, __LINE__, __func__, "brain")
#define NLM_WORLD_LOG(level, msg) nlm::EnhancedLogger::getGlobal().log(level, msg, __FILE__, __LINE__, __func__, "world")
#define NLM_AGENT_LOG(level, msg) nlm::EnhancedLogger::getGlobal().log(level, msg, __FILE__, __LINE__, __func__, "agent")
#define NLM_EXPERIMENT_LOG(level, msg, experiment) nlm::EnhancedLogger::getGlobal().log(level, msg, __FILE__, __LINE__, __func__, "", experiment)

// Backward compatibility macros (map to enhanced logger)
#define NLM_LOG_DEBUG(msg) NLM_LOG_DEBUG(msg)
#define NLM_LOG_INFO(msg) NLM_LOG_INFO(msg)
#define NLM_LOG_WARNING(msg) NLM_LOG_WARNING(msg)
#define NLM_LOG_ERROR(msg) NLM_LOG_ERROR(msg)
#define NLM_LOG_CRITICAL(msg) NLM_LOG_CRITICAL(msg)

} // namespace nlm