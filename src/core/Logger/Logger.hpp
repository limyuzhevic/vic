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
#include <map>
#include <ctime>

namespace nlm {

// Enhanced log severity levels with additional levels
enum class LogLevel {
    Off = -1,           // No logging
    Emergency = 0,       // System is unusable
    Alert = 1,           // Action must be taken immediately
    Critical = 2,        // Critical conditions
    Error = 3,           // Error conditions
    Warning = 4,         // Warning conditions
    Notice = 5,          // Normal but significant conditions
    Info = 6,            // General information
    Debug = 7,           // Detailed debugging information
    Trace = 8            // Extremely detailed debugging information
};

// Enhanced log entry structure with additional metadata
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    std::string threadName;
    uint64_t threadId;
    
    // Performance tracking
    std::chrono::high_resolution_clock::duration duration;
    
    LogEntry() 
        : level(LogLevel::Info), line(0), threadId(0) {}
    
    LogEntry(LogLevel lvl, const std::string& msg, const std::string& file = "", 
             int lineNum = 0, const std::string& function = "")
        : level(lvl), message(msg), file(file), line(lineNum), function(function),
          threadId(std::hash<std::thread::id>{}(std::this_thread::get_id())) {
        timestamp = std::chrono::system_clock::now();
    }
};

// Enhanced ILogger interface with more capabilities
class ILogger {
public:
    virtual ~ILogger() = default;
    
    // Basic logging interface
    virtual void log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
    
    // Enhanced capabilities
    virtual void setPattern(const std::string& pattern) {}  // Custom formatting
    virtual void setFormatter(std::function<std::string(const LogEntry&)> formatter) {}
    virtual void enableAsyncLogging(bool enable) {}
    virtual void setMaxFileSize(size_t maxBytes) {}
    virtual void setMaxFiles(int maxCount) {}
    virtual void rotate() {}  // Force log rotation
    virtual bool isFileLoggingEnabled() const { return false; }
    virtual bool isConsoleLoggingEnabled() const { return false; }
    
    // Performance monitoring
    virtual double getAverageLogTime() const { return 0.0; }
    virtual uint64_t getTotalLogCount() const { return 0; }
};

// Enhanced Console logger with more features
class ConsoleLogger : public ILogger {
public:
    // Configuration options
    struct ConsoleConfig {
        LogLevel level = LogLevel::Info;
        bool useColors = true;
        bool useTimestamp = true;
        bool useThreadInfo = true;
        bool useFunctionInfo = true;
        std::string timestampFormat = "%Y-%m-%d %H:%M:%S";
        std::string pattern = "[%L] %T %M";
        bool async = false;
        size_t bufferSize = 1024;
    };
    
    explicit ConsoleLogger(const ConsoleConfig& config = ConsoleConfig{});
    explicit ConsoleLogger(LogLevel level);
    ~ConsoleLogger() override;
    
    // Core interface
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    // Enhanced configuration methods
    void setPattern(const std::string& pattern) override;
    void setUseColors(bool useColors);
    void setUseTimestamp(bool useTimestamp);
    void setUseThreadInfo(bool useThreadInfo);
    void setUseFunctionInfo(bool useFunctionInfo);
    void setTimestampFormat(const std::string& format);
    void enableAsyncLogging(bool enable) override;
    
    // File logging methods
    void setOutputFile(const std::string& filepath);
    void setMaxFileSize(size_t maxBytes);
    void setMaxFiles(int maxCount);
    void setFilePattern(const std::string& pattern);
    void closeOutputFile();
    bool isFileLoggingEnabled() const override;
    
    // Performance and statistics
    double getAverageLogTime() const override;
    uint64_t getTotalLogCount() const override;
    uint64_t getErrorCount() const;
    uint64_t getWarningCount() const;
    
    // Advanced features
    void flush();
    void setBufferSize(size_t size);
    void enableColors(bool enable) { setUseColors(enable); }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helper methods
    std::string formatMessage(const LogEntry& entry) const;
    std::string applyPattern(const std::string& pattern, const LogEntry& entry) const;
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
    std::string formatTimestamp(const std::chrono::system_clock::time_point& timestamp) const;
    std::string getCurrentThreadName() const;
};

// Enhanced Main Logger class with better management
class Logger {
public:
    // Configuration options
    struct LoggerConfig {
        LogLevel level = LogLevel::Info;
        bool threadSafe = true;
        bool asyncLogging = false;
        size_t logBufferSize = 4096;
        std::string defaultPattern = "[%L] %T %M";
        bool enablePerformanceMonitoring = false;
    };
    
    Logger(const LoggerConfig& config = LoggerConfig{});
    ~Logger();
    
    // Disable copying, enable moving
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    Logger(Logger&&) noexcept;
    Logger& operator=(Logger&&) noexcept;
    
    // Logger management
    void addLogger(std::shared_ptr<ILogger> logger);
    void removeLogger(ILogger* logger);  // Remove specific logger
    void clearLoggers();
    void setLevel(LogLevel level);
    void setGlobalLevel(LogLevel level);  // Set level for all loggers
    
    // Core logging interface
    void log(LogLevel level, const std::string& message, 
             const std::string& file = "", int line = 0, 
             const std::string& function = "");
    
    // Enhanced logging interface
    void log(const LogEntry& entry);
    
    // Convenience methods with enhanced context
    void emergency(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void alert(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void critical(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void error(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void warning(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void notice(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void info(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void debug(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void trace(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    // Stream-style logging helper (enhanced)
    class LogStream {
    public:
        LogStream(LogLevel level, const std::string& file, int line, const std::string& function);
        ~LogStream();
        
        template<typename T>
        LogStream& operator<<(const T& value) {
            oss_ << value;
            return *this;
        }
        
        // Support for different stream types
        LogStream& operator<<(std::ostream& (*manip)(std::ostream&)) {
            oss_ << manip;
            return *this;
        }
        
    private:
        LogLevel level_;
        std::ostringstream oss_;
        std::string file_;
        int line_;
        std::string function_;
    };
    
    LogStream logStream(LogLevel level) {
        return LogStream(level, "", 0, "");
    }
    
    // Check if level is enabled
    bool isEnabled(LogLevel level) const;
    
    // Global logger instance management
    static Logger& getGlobal();
    static void setGlobal(std::shared_ptr<Logger> logger);
    
    // Performance monitoring and statistics
    double getAverageLogTime() const;
    uint64_t getTotalLogCount() const;
    uint64_t getErrorCount() const;
    uint64_t getWarningCount() const;
    std::string getStatistics() const;
    
    // Configuration
    void setDefaultPattern(const std::string& pattern);
    void setPerformanceMonitoring(bool enable);
    void flushAll();
    
    // File logging configuration (global)
    void configureFileLogging(const std::string& filepath, size_t maxSize = 10 * 1024 * 1024, int maxFiles = 5);
    void disableFileLogging();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    std::string getCurrentThreadName() const;
    uint64_t getCurrentThreadId() const;
    std::string formatTimestamp(const std::chrono::system_clock::time_point& timestamp) const;
};

// Enhanced convenience macros with better context
#define NLM_LOG_EMERGENCY(msg) nlm::Logger::getGlobal().emergency(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ALERT(msg) nlm::Logger::getGlobal().alert(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_NOTICE(msg) nlm::Logger::getGlobal().notice(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_TRACE(msg) nlm::Logger::getGlobal().trace(msg, __FILE__, __LINE__, __func__)

// Enhanced stream-style logging macros
#define NLM_LOG_STREAM(severity) nlm::Logger::getGlobal().logStream(severity)

// Utility functions for logging configuration
namespace LogUtils {
    // Pattern tokens for custom formatting
    static constexpr const char* PATTERN_TOKENS = "[%L] %T %M %F:%l %f()"; // Level, Timestamp, Message, File:line, Function
    
    // Common patterns
    const std::string BASIC_PATTERN = "[%L] %M";
    const std::string DETAILED_PATTERN = "[%L] %T [%t] %M";
    const std::string FULL_PATTERN = "[%L] %T [%t] %M (%f:%l)";
    const std::string PERFORMANCE_PATTERN = "[%L] %T %M [%.3fms]";
    
    // Pattern helper functions
    std::string escapePattern(const std::string& pattern);
    bool validatePattern(const std::string& pattern);
    std::vector<std::string> extractTokens(const std::string& pattern);
}

} // namespace nlm