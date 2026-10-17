#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <mutex>
#include <unordered_map>
#include <map>
#include <algorithm>
#include <fstream>
#include <filesystem>
#include <iomanip>
#include <ctime>
#include <numeric>
#include <queue>

namespace nlm {

// Enhanced log severity levels with categories
enum class LogLevel {
    // Trace level (most verbose)
    Trace = 0,
    
    // Debug level  
    Debug = 1,
    
    // Informational level
    Info = 2,
    
    // Warning level - potential issues
    Warning = 3,
    
    // Error level - errors that don't stop execution
    Error = 4,
    
    // Critical level - serious errors
    Critical = 5,
    
    // Fatal level - program should terminate
    Fatal = 6
};

// Log categories for organization
enum class LogCategory {
    System = 0,          // Core system operations
    Brain = 1,           // Neural network operations
    Memory = 2,          // Memory management
    Plasticity = 3,      // Synaptic plasticity
    Neuromodulation = 4, // Dopamine, curiosity, etc.
    Sensory = 5,         // Sensory processing
    Motor = 6,           // Motor control
    Learning = 7,        // Learning algorithms
    Development = 8,     // Development processes
    Prediction = 9,      // Predictive coding
    Cognition = 10,      // Higher-level cognition
    Experiment = 11,     // Experimental results
    Performance = 12,    // Performance metrics
    Configuration = 13,  // Configuration and setup
    Network = 14,        // Network topology
    ErrorHandling = 15,  // Error recovery and handling
    User = 16,           // User interactions
    Interface = 17,      // API/interface operations
    Debug = 18           // Low-level debugging
};

// Log entry with enhanced metadata
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    LogCategory category;
    std::string threadId;
    std::vector<std::string> tags;
    std::map<std::string, std::string> structuredData;
    std::string sessionId;
    size_t sequenceNumber;
    
    LogEntry() : level(LogLevel::Info), message(), file(), line(0), function(), 
                 timestamp(std::chrono::system_clock::now()), category(LogCategory::System),
                 threadId(), tags(), structuredData(), sessionId(), sequenceNumber(0) {}
    
    LogEntry(LogLevel lvl, const std::string& msg, const std::string& filePath = "", 
             int lineNum = 0, const std::string& func = "", LogCategory cat = LogCategory::System)
        : level(lvl), message(msg), file(filePath), line(lineNum), function(func),
          timestamp(std::chrono::system_clock::now()), category(cat),
          threadId(), tags(), structuredData(), sessionId(), sequenceNumber(0) {
        // Generate thread ID
        std::ostringstream oss;
        oss << std::this_thread::get_id();
        threadId = oss.str();
        
        // Generate session ID if not exists
        if (sessionId.empty()) {
            auto now = std::chrono::system_clock::now();
            auto time = std::chrono::system_clock::to_time_t(now);
            std::ostringstream sessionOss;
            sessionOss << "session_" << std::hex << std::hash<std::thread::id>{}(std::this_thread::get_id())
                       << "_" << time;
            sessionId = sessionOss.str();
        }
    }
};

// Log statistics with performance metrics
struct AdvancedLogStatistics {
    // Count by level
    std::map<LogLevel, size_t> countByLevel;
    
    // Count by category
    std::map<LogCategory, size_t> countByCategory;
    
    // Count by tags
    std::map<std::string, size_t> countByTag;
    
    // Performance metrics
    struct PerformanceMetrics {
        size_t totalLogs = 0;
        double averageLogTime = 0.0;  // microseconds
        double minLogTime = 0.0;
        double maxLogTime = 0.0;
        double standardDeviation = 0.0;
        size_t logsPerSecond = 0;
        std::chrono::system_clock::time_point startTime;
        std::chrono::system_clock::time_point endTime;
        
        void reset() {
            totalLogs = 0;
            averageLogTime = 0.0;
            minLogTime = 0.0;
            maxLogTime = 0.0;
            standardDeviation = 0.0;
            logsPerSecond = 0;
        }
    } performance;
    
    // Memory usage
    size_t estimatedMemoryUsage = 0;
    
    // Rate limiting statistics
    struct RateLimitStats {
        size_t totalAllowed = 0;
        size_t totalDenied = 0;
        double denialRate = 0.0;
    } rateLimit;
    
    // Error statistics
    struct ErrorStats {
        size_t totalErrors = 0;
        std::map<std::string, size_t> errorsByType;
        std::chrono::system_clock::time_point lastErrorTime;
        double errorRate = 0.0;
    } errors;
    
    // Log levels configuration
    struct LevelConfig {
        LogLevel level;
        bool enabled = true;
        std::string colorCode;
        std::string name;
        std::string description;
    };
    
    std::map<LogLevel, LevelConfig> levelConfigs;
    
    // AdvancedLogStatistics constructor
    AdvancedLogStatistics() {
        startDefaultConfigurations();
        performance.startTime = std::chrono::system_clock::now();
        performance.endTime = performance.startTime;
        errors.lastErrorTime = performance.startTime;
    }
    
private:
    void startDefaultConfigurations() {
        levelConfigs[LogLevel::Trace] = {LogLevel::Trace, false, "\033[90m", "TRACE", "Detailed debugging information"};
        levelConfigs[LogLevel::Debug] = {LogLevel::Debug, false, "\033[36m", "DEBUG", "Debug information"};
        levelConfigs[LogLevel::Info] = {LogLevel::Info, true, "\033[32m", "INFO", "Informational messages"};
        levelConfigs[LogLevel::Warning] = {LogLevel::Warning, true, "\033[33m", "WARN", "Warning messages"};
        levelConfigs[LogLevel::Error] = {LogLevel::Error, true, "\033[31m", "ERROR", "Error messages"};
        levelConfigs[LogLevel::Critical] = {LogLevel::Critical, true, "\033[35m", "CRIT", "Critical error messages"};
        levelConfigs[LogLevel::Fatal] = {LogLevel::Fatal, true, "\033[1;31m", "FATAL", "Fatal error messages"};
    }
};

// Log filter for selective logging
struct LogFilter {
    std::vector<LogLevel> excludedLevels;
    std::vector<LogCategory> excludedCategories;
    std::vector<std::string> includedTags;
    std::vector<std::string> excludedTags;
    std::string messagePattern;
    std::string filePattern;
    bool matchAllTags = false;
    bool matchAnyTag = false;
    
    bool matches(const LogEntry& entry) const {
        // Check level
        if (std::find(excludedLevels.begin(), excludedLevels.end(), entry.level) != excludedLevels.end()) {
            return false;
        }
        
        // Check category
        if (std::find(excludedCategories.begin(), excludedCategories.end(), entry.category) != excludedCategories.end()) {
            return false;
        }
        
        // Check tags
        if (!includedTags.empty()) {
            bool tagMatches = false;
            for (const auto& tag : includedTags) {
                if (std::find(entry.tags.begin(), entry.tags.end(), tag) != entry.tags.end()) {
                    tagMatches = true;
                    break;
                }
            }
            if (matchAllTags) {
                // Check if all included tags are present
                for (const auto& tag : includedTags) {
                    if (std::find(entry.tags.begin(), entry.tags.end(), tag) == entry.tags.end()) {
                        return false;
                    }
                }
            } else if (!matchAnyTag && !tagMatches) {
                return false;
            }
        }
        
        for (const auto& tag : excludedTags) {
            if (std::find(entry.tags.begin(), entry.tags.end(), tag) != entry.tags.end()) {
                return false;
            }
        }
        
        // Check message pattern
        if (!messagePattern.empty()) {
            if (entry.message.find(messagePattern) == std::string::npos) {
                return false;
            }
        }
        
        // Check file pattern
        if (!filePattern.empty()) {
            if (entry.file.find(filePattern) == std::string::npos) {
                return false;
            }
        }
        
        return true;
    }
}; 

// Logger interface with enhanced functionality
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
    
    // Enhanced pattern matching for log levels and categories
    virtual bool shouldLog(LogLevel level, LogCategory category = LogCategory::System, 
                          const std::vector<std::string>& tags = {}) const {
        return level >= getLevel();
    }
    
    // Enhanced filtering
    virtual bool matchesFilter(const LogFilter& filter, const LogEntry& entry) const {
        return filter.matches(entry);
    }
    
    // Configure formatting
    virtual void setFormat(const std::string& format) = 0;
    virtual std::string getFormat() const = 0;
    
    // Thread safety
    virtual void setThreadSafe(bool threadSafe) = 0;
    virtual bool getThreadSafe() const = 0;
    
    // Statistics
    virtual AdvancedLogStatistics getStatistics() const = 0;
    
    // Clear statistics
    virtual void resetStatistics() = 0;
    
    // Add tag to logger
    virtual void addTag(const std::string& tag) = 0;
    
    // Remove tag from logger
    virtual void removeTag(const std::string& tag) = 0;
    
    // Set category filter
    virtual void setCategoryFilter(const std::vector<LogCategory>& categories) = 0;
    
    // Set level filter
    virtual void setLevelFilter(const std::vector<LogLevel>& levels) = 0;
};

// Enhanced console logger with advanced formatting
class ConsoleLogger : public ILogger {
public:
    ConsoleLogger();
    explicit ConsoleLogger(LogLevel level, LogCategory category = LogCategory::System);
    ~ConsoleLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    // Enhanced color support
    void setUseColors(bool useColors);
    bool getUseColors() const;
    
    // Advanced formatting
    void setFormat(const std::string& format) override;
    std::string getFormat() const override;
    
    // Custom timestamp format
    void setTimestampFormat(const std::string& format);
    std::string getTimestampFormat() const;
    
    // Thread safety
    void setThreadSafe(bool threadSafe) override;
    bool getThreadSafe() const override;
    
    // Statistics
    AdvancedLogStatistics getStatistics() const override;
    void resetStatistics() override;
    
    // Tag management
    void addTag(const std::string& tag) override;
    void removeTag(const std::string& tag) override;
    
    // Category filter
    void setCategoryFilter(const std::vector<LogCategory>& categories) override;
    
    // Level filter
    void setLevelFilter(const std::vector<LogLevel>& levels) override;
    
    // Performance monitoring
    void enablePerformanceMonitoring(bool enable);
    bool isPerformanceMonitoringEnabled() const;
    
    // Log level configuration
    void configureLevel(LogLevel level, bool enabled, const std::string& colorCode = "", 
                      const std::string& name = "", const std::string& description = "");
    void configureLevel(LogLevel level, bool enabled, const std::string& name, 
                      const std::string& description);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
    std::string formatTimestamp(const std::chrono::system_clock::time_point& timestamp) const;
    std::string formatMessage(const LogEntry& entry) const;
    std::string applyFormatting(const std::string& message, const LogEntry& entry) const;
};

// Enhanced file logger with rotation and backup
class FileLogger : public ILogger {
public:
    FileLogger();
    explicit FileLogger(const std::string& filepath, LogLevel level = LogLevel::Info,
                       LogCategory category = LogCategory::System);
    ~FileLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    // Advanced file rotation
    void setMaxFileSize(size_t maxBytes);
    void setMaxFiles(size_t maxFiles);
    void enableRotation(bool enable);
    bool isRotating() const;
    
    // Time-based rotation
    void setMaxFileAge(std::chrono::hours maxAge);
    void setRotationInterval(std::chrono::hours interval);
    
    // Compression
    void enableCompression(bool enable, const std::string& compressionTool = "gzip");
    bool isCompressionEnabled() const;
    
    // Get current log file path
    std::string getCurrentFilePath() const;
    
    // Backup management
    void setBackupCount(size_t count);
    void createBackup(const std::string& filename);
    std::vector<std::string> getBackupFiles() const;
    
    // Statistics
    AdvancedLogStatistics getStatistics() const override;
    void resetStatistics() override;
    
    // Tag management
    void addTag(const std::string& tag) override;
    void removeTag(const std::string& tag) override;
    
    // Category filter
    void setCategoryFilter(const std::vector<LogCategory>& categories) override;
    
    // Level filter
    void setLevelFilter(const std::vector<LogLevel>& levels) override;
    
    // File management
    bool rotateLogFile();
    bool compressOldFiles();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Main Logger class with advanced features
class Logger {
public:
    Logger();
    ~Logger();
    
    // Disable copying
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    // Add logger sink
    void addLogger(std::shared_ptr<ILogger> logger);
    
    // Remove all loggers
    void clearLoggers();
    
    // Remove specific logger
    void removeLogger(std::shared_ptr<ILogger> logger);
    
    // Set minimum level for all loggers
    void setLevel(LogLevel level);
    
    // Set level for specific logger
    void setLevel(std::shared_ptr<ILogger> logger, LogLevel level);
    
    // Get minimum level for all loggers
    LogLevel getLevel() const;
    
    // Global logging functions with enhanced parameters
    void log(LogLevel level, const std::string& message, 
             const std::string& file = "", int line = 0, 
             const std::string& function = "", 
             LogCategory category = LogCategory::System,
             const std::vector<std::string>& tags = {});
    
    // Enhanced stream-style logging helper
    class LogStream {
    public:
        LogStream(LogLevel level, const std::string& file, int line, 
                 const std::string& function, LogCategory category = LogCategory::System,
                 const std::vector<std::string>& tags = {});
        ~LogStream();
        
        template<typename T>
        LogStream& operator<<(const T& value) {
            oss_ << value;
            return *this;
        }
        
        template<typename T>
        LogStream& operator<<(const std::vector<T>& values) {
            oss_ << "[";
            for (size_t i = 0; i < values.size(); ++i) {
                if (i > 0) oss_ << ", ";
                oss_ << values[i];
            }
            oss_ << "]";
            return *this;
        }
        
        // Structured logging
        LogStream& operator<<(const std::pair<std::string, std::string>& pair) {
            oss_ << "{" << pair.first << ": " << pair.second << "}";
            return *this;
        }
        
        LogStream& operator<<(const std::map<std::string, std::string>& data) {
            oss_ << "{";
            bool first = true;
            for (const auto& pair : data) {
                if (!first) oss_ << ", ";
                oss_ << pair.first << ": " << pair.second;
                first = false;
            }
            oss_ << "}";
            return *this;
        }
        
    private:
        LogLevel level_;
        std::ostringstream oss_;
        std::string file_;
        int line_;
        std::string function_;
        LogCategory category_;
        std::vector<std::string> tags_;
    };
    
    #define NLM_LOG_STREAM(level) nlm::Logger::LogStream(level, __FILE__, __LINE__, __func__)
    
    // Check if level is enabled
    bool isEnabled(LogLevel level) const;
    
    // Check if specific logger is enabled
    bool isLoggerEnabled(std::shared_ptr<ILogger> logger, LogLevel level) const;
    
    // Get global logger instance
    static Logger& getGlobal();
    
    // Set global logger
    static void setGlobal(std::shared_ptr<Logger> logger);
    
    // Configuration
    void enableColors(bool enable);
    bool isColorsEnabled() const;
    
    // Enhanced statistics
    AdvancedLogStatistics getStatistics() const;
    void resetStatistics();
    
    // Performance monitoring
    struct SystemPerformanceMetrics {
        double cpuUsage = 0.0;           // Percentage
        double memoryUsage = 0.0;        // Percentage
        size_t totalLogEvents = 0;
        double averageEventProcessingTime = 0.0; // microseconds
        size_t activeLoggers = 0;
        std::chrono::system_clock::time_point uptime;
    };
    
    SystemPerformanceMetrics getSystemPerformanceMetrics() const;
    
    // Log filtering and search
    void addGlobalFilter(std::shared_ptr<ILogger> logger, const LogFilter& filter);
    void removeGlobalFilter(std::shared_ptr<ILogger> logger);
    std::vector<LogEntry> searchLogs(const std::string& pattern, 
                                     LogLevel minLevel = LogLevel::Trace,
                                     LogCategory category = LogCategory::System,
                                     const std::string& startTime = "",
                                     const std::string& endTime = "") const;
    
    // Log export
    bool exportLogs(const std::string& filepath, LogLevel minLevel = LogLevel::Trace,
                   const std::vector<LogCategory>& categories = {}) const;
    
    // Configuration presets
    void loadLogConfiguration(const std::string& presetName);
    void saveLogConfiguration(const std::string& presetName) const;
    static std::vector<std::string> getAvailableLogConfigurations();
    
    // Convenience methods
    void trace(const std::string& message, const std::string& file = "", int line = 0, 
               const std::string& function = "", LogCategory category = LogCategory::Debug);
    void debug(const std::string& message, const std::string& file = "", int line = 0, 
               const std::string& function = "", LogCategory category = LogCategory::Debug);
    void info(const std::string& message, const std::string& file = "", int line = 0, 
              const std::string& function = "", LogCategory category = LogCategory::System);
    void warning(const std::string& message, const std::string& file = "", int line = 0, 
                 const std::string& function = "", LogCategory category = LogCategory::Warning);
    void error(const std::string& message, const std::string& file = "", int line = 0, 
               const std::string& function = "", LogCategory category = LogCategory::Error);
    void critical(const std::string& message, const std::string& file = "", int line = 0, 
                  const std::string& function = "", LogCategory category = LogCategory::Critical);
    void fatal(const std::string& message, const std::string& file = "", int line = 0, 
               const std::string& function = "", LogCategory category = LogCategory::Error);
    
    // Enhanced stream-style macros with categories
    #define NLM_LOG_STREAM_CAT(level, category) nlm::Logger::LogStream(level, __FILE__, __LINE__, __func__, category)
    #define NLM_LOG_STREAM_TAGS(level, tags) nlm::Logger::LogStream(level, __FILE__, __LINE__, __func__, nlm::LogCategory::System, tags)
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};