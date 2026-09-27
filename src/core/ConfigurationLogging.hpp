#pragma once

/**
 * Configuration Logging
 * 
 * Configuration logging system for detailed logging of configuration changes,
 * providing comprehensive logging of configuration operations and events
 * for the NLM brain simulation project.
 * 
 * Features:
 * - Configuration change logging
 * - Configuration operation logging
 * - Configuration error logging
 * - Configuration performance logging
 * - Configuration audit logging
 * - Log level management
 * - Log file management
 * - Log rotation
 * - Log filtering
 * - Log format customization
 */

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <mutex>
#include <chrono>
#include <algorithm>
#include <unordered_map>
#include <functional>
#include <fstream>
#include <sstream>

namespace nlm {
namespace core {

class ConfigurationManager;

/**
 * Log level
 */
enum class LogLevel {
    TRACE,
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    CRITICAL,
    NONE
};

/**
 * Log entry
 */
struct LogEntry {
    double timestamp;
    LogLevel level;
    std::string source;
    std::string message;
    std::map<std::string, std::string> metadata;
    std::string sessionId;
    
    LogEntry(LogLevel lvl, const std::string& src, const std::string& msg)
        : timestamp(0.0), level(lvl), source(src), message(msg) {}
};

/**
 * Log configuration
 */
struct LogConfig {
    LogLevel level;
    std::string filePath;
    size_t maxFileSize;
    size_t maxFileCount;
    bool enableConsoleOutput;
    bool enableFileOutput;
    std::string logFormat;
    bool enableColors;
    
    LogConfig() : level(LogLevel::INFO), maxFileSize(10 * 1024 * 1024), // 10MB
                  maxFileCount(10), enableConsoleOutput(true), enableFileOutput(true),
                  enableColors(false) {}
};

/**
 * Configuration logger
 */
class ConfigurationLogger {
public:
    ConfigurationLogger();
    ~ConfigurationLogger();
    
    // Disable copying, enable moving
    ConfigurationLogger(const ConfigurationLogger&) = delete;
    ConfigurationLogger& operator=(const ConfigurationLogger&) = delete;
    ConfigurationLogger(ConfigurationLogger&&) noexcept = default;
    ConfigurationLogger& operator=(ConfigurationLogger&&) noexcept = default;
    
    // Log configuration
    void configure(const LogConfig& config);
    LogConfig getConfiguration() const { return config_; }
    
    // Log level management
    void setLogLevel(LogLevel level);
    LogLevel getLogLevel() const { return config_.level; }
    
    // Log entry logging
    void log(LogLevel level, const std::string& source, const std::string& message,
             const std::map<std::string, std::string>& metadata = {});
    void trace(const std::string& source, const std::string& message);
    void debug(const std::string& source, const std::string& message);
    void info(const std::string& source, const std::string& message);
    void warning(const std::string& source, const std::string& message);
    void error(const std::string& source, const std::string& message);
    void critical(const std::string& source, const std::string& message);
    
    // Log filtering
    void addLogFilter(const std::string& source, LogLevel level);
    void removeLogFilter(const std::string& source);
    bool shouldLog(LogLevel level, const std::string& source) const;
    
    // Log entry retrieval
    std::vector<LogEntry> getRecentLogs(size_t limit = 100) const;
    std::vector<LogEntry> getLogsByLevel(LogLevel level, size_t limit = 100) const;
    std::vector<LogEntry> getLogsBySource(const std::string& source, size_t limit = 100) const;
    std::vector<LogEntry> getLogsByTimeRange(double startTime, double endTime) const;
    
    // Log file management
    bool rotateLogs();
    bool clearLogs();
    std::string getLogFilePath() const;
    size_t getLogFileSize() const;
    
    // Log format
    std::string formatLogEntry(const LogEntry& entry) const;
    void setLogFormat(const std::string& format) { config_.logFormat = format; }
    
    // Log statistics
    size_t getTotalLogCount() const { return log_entries_.size(); }
    size_t getLogCountByLevel(LogLevel level) const;
    double getLastLogTime() const { return last_log_time_; }
    
    // Configuration-specific logging
    void logConfigurationChange(const std::string& key, const std::string& oldValue,
                              const std::string& newValue, ConfigSource source);
    void logConfigurationLoad(const std::string& filePath, bool success, const std::string& error = "");
    void logConfigurationValidation(const std::string& component, bool valid,
                                   const std::vector<std::string>& errors);
    void logConfigurationError(const std::string& operation, const std::string& error);
    
    // Thread safety
    void lock() const;
    void unlock() const;
    bool tryLock() const;
    
    // Debug and diagnostics
    std::string dump() const;
    std::string getStatisticsString() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    mutable std::mutex mutex_;
    LogConfig config_;
    double last_log_time_ = 0.0;
    
    // Internal helpers
    void writeLogToFile(const LogEntry& entry);
    void writeLogToConsole(const LogEntry& entry);
    std::string getCurrentTimestamp() const;
    
    friend class ConfigurationManager;
};

} // namespace core
} // namespace nlm
