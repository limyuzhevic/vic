#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <mutex>
#include <atomic>
#include <fstream>
#include <map>
#include <unordered_map>

namespace nlm {

// Log severity levels
enum class LogLevel {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4
};

// Log format options
enum class LogFormat {
    Simple,      // Simple text format
    Detailed,    // Detailed format with file/line/function
    Structured,  // JSON-like structured format
    Colored      // Colored output when supported
};

// Log entry structure
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    
    // Additional fields for structured logging
    std::map<std::string, std::string> fields;
    
    LogEntry() : level(LogLevel::Info), line(0), timestamp(std::chrono::system_clock::now()) {}
};

// Logger statistics
struct LoggerStatistics {
    std::atomic<uint64_t> totalMessages{0};
    std::atomic<uint64_t> messagesByLevel[5];  // Indexed by LogLevel
    std::chrono::system_clock::time_point firstMessage;
    std::chrono::system_clock::time_point lastMessage;
    
    LoggerStatistics() {
        for (int i = 0; i < 5; ++i) {
            messagesByLevel[i] = 0;
        }
        firstMessage = std::chrono::system_clock::now();
        lastMessage = std::chrono::system_clock::now();
    }
    
    void increment(LogLevel level) {
        totalMessages++;
        messagesByLevel[static_cast<int>(level)]++;
        lastMessage = std::chrono::system_clock::now();
    }
};

// File rotation strategy
enum class RotationStrategy {
    None,        // No rotation
    Size,        // Rotate when file reaches size limit
    Time,        // Rotate based on time (hourly/daily)
    SizeAndTime // Combine both strategies
};

// Logger configuration for advanced users
struct LoggerConfig {
    LogLevel minimumLevel = LogLevel::Info;           // Minimum log level to output
    LogFormat format = LogFormat::Detailed;         // Log format
    bool enableColors = true;                       // Enable colored output
    bool enableFileOutput = false;                   // Enable file output
    std::string filePath = "nlm.log";               // Log file path
    size_t maxFileSize = 10 * 1024 * 1024;          // 10MB max file size
    RotationStrategy rotationStrategy = RotationStrategy::Size;
    size_t maxBackupFiles = 5;                       // Number of backup files to keep
    bool enablePerformanceStats = false;             // Enable performance statistics
    size_t flushInterval = 1000;                     // Flush interval in bytes
    bool threadSafe = true;                          // Enable thread safety
    
    // Filtering
    std::vector<std::string> filterPattern;         // Only log messages matching this pattern
    bool excludeFiltered = false;                    // If true, exclude filtered messages instead of including
    
    // Custom formatting
    std::string customFormat;                        // Custom format string (placeholders: {level}, {time}, {message}, etc.)
    
    // Log level thresholds for filtering
    bool filterDebug = true;                         // Include debug messages
    bool filterInfo = true;                          // Include info messages
    bool filterWarning = true;                       // Include warning messages
    bool filterError = true;                         // Include error messages
    bool filterCritical = true;                      // Include critical messages
};

// Logger interface
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
    virtual void setConfig(const LoggerConfig& config) = 0;
    virtual LoggerConfig getConfig() const = 0;
    virtual std::string formatEntry(const LogEntry& entry) const = 0;
};

// Performance logger with thread safety and advanced features
class PerformanceLogger : public ILogger {
public:
    PerformanceLogger();
    explicit PerformanceLogger(const LoggerConfig& config);
    ~PerformanceLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    void setConfig(const LoggerConfig& config) override;
    std::string formatEntry(const LogEntry& entry) const override;
    
    // File management
    void rotateFile();
    bool shouldRotate() const;
    void flush();
    
    // Statistics
    LoggerStatistics getStatistics() const { return stats_; }
    void resetStatistics();
    
    // Performance monitoring
    uint64_t getTotalLogCount() const { return stats_.totalMessages; }
    uint64_t getLogCount(LogLevel level) const { return stats_.messagesByLevel[static_cast<int>(level)]; }
    
    // Advanced features
    void enablePerformanceMonitoring(bool enable) { config_.enablePerformanceStats = enable; }
    void setFilterPattern(const std::vector<std::string>& patterns);
    bool matchesFilter(const std::string& message) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Helper functions
    std::string formatDetailed(const LogEntry& entry) const;
    std::string formatSimple(const LogEntry& entry) const;
    std::string formatColored(const LogEntry& entry) const;
    std::string formatStructured(const LogEntry& entry) const;
    std::string applyCustomFormat(const LogEntry& entry) const;
    bool shouldLogLevel(LogLevel level) const;
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
    std::string timestampToString(const std::chrono::system_clock::time_point& tp) const;
    
    LoggerConfig config_;
    LoggerStatistics stats_;
    mutable std::mutex mutex_;
    std::ofstream fileStream_;
    std::string currentFileName_;
    uint64_t currentFileSize_;
    std::chrono::system_clock::time_point lastRotationTime_;
};

// Main Logger class with enhanced features
class Logger {
public:
    Logger();
    explicit Logger(const LoggerConfig& config);
    ~Logger();
    
    // Disable copying
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    // Add logger sink
    void addLogger(std::shared_ptr<ILogger> logger);
    
    // Remove specific logger
    void removeLogger(ILogger* logger);
    
    // Remove all loggers
    void clearLoggers();
    
    // Set minimum level for all loggers
    void setLevel(LogLevel level);
    
    // Configure all loggers
    void setConfig(const LoggerConfig& config);
    
    // Global logging functions
    void log(LogLevel level, const std::string& message, 
             const std::string& file = "", int line = 0, 
             const std::string& function = "");
    
    // Convenience methods
    void debug(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void info(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void warning(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void error(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void critical(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    // Stream-style logging helper
    class LogStream {
    public:
        LogStream(LogLevel level, const std::string& file, int line, const std::string& function);
        ~LogStream();
        
        template<typename T>
        LogStream& operator<<(const T& value) {
            oss_ << value;
            return *this;
        }
        
    private:
        LogLevel level_;
        std::ostringstream oss_;
        std::string file_;
        int line_;
        std::string function_;
    };
    
    // Get stream for logging
    static LogStream createStream(LogLevel level, const std::string& file, int line, const std::string& function);
    
    // Check if level is enabled
    bool isEnabled(LogLevel level) const;
    
    // Get global logger instance
    static Logger& getGlobal();
    
    // Set global logger
    static void setGlobal(std::shared_ptr<Logger> logger);
    
    // Get global logger config
    static LoggerConfig getGlobalConfig();
    static void setGlobalConfig(const LoggerConfig& config);
    
    // Logger statistics
    static LoggerStatistics getGlobalStatistics();
    static void resetGlobalStatistics();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Convenience macros for file/line/function info
#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)

// Stream-style macros
#define NLM_LOG_STREAM(level) nlm::Logger::createStream(level, __FILE__, __LINE__, __func__)

} // namespace nlm
