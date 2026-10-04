#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <unordered_map>

namespace nlm {

// Enhanced log severity levels
enum class LogLevel {
    Trace = 0,        // Very detailed debugging info
    Debug = 1,        // General debugging info
    Info = 2,         // General information
    Notice = 3,       // Important events
    Warning = 4,      // Warning conditions
    Error = 5,        // Error conditions
    Critical = 6,     // Critical errors
    Alert = 7,        // Action must be taken immediately
    Emergency = 8     // System is unusable
};

// Enhanced log entry structure with additional metadata
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    
    // Additional metadata for enhanced logging
    std::unordered_map<std::string, std::string> metadata;
    bool includeStackTrace;
    size_t threadId;
    
    LogEntry()
        : level(LogLevel::Info), line(0), includeStackTrace(false), threadId(0) {}
    
    LogEntry(LogLevel lvl, const std::string& msg, const std::string& file = "", 
             int line = 0, const std::string& function = "")
        : level(lvl), message(msg), file(file), line(line), function(function),
          includeStackTrace(false), threadId(0) {
        timestamp = std::chrono::system_clock::now();
    }
};

// Logger interface with enhanced functionality
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
    
    // Filter messages by tag/category
    virtual bool shouldLog(const std::string& category) const {
        return true; // Default: log everything
    }
    
    // Format log entry to string
    virtual std::string formatEntry(const LogEntry& entry) const {
        return entry.message;
    }
};

// Enhanced Console logger implementation
class ConsoleLogger : public ILogger {
public:
    ConsoleLogger();
    explicit ConsoleLogger(LogLevel level, LogLevel minLevel = LogLevel::Debug);
    ~ConsoleLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    // Enable/disable colors
    void setUseColors(bool useColors);
    bool getUseColors() const;
    
    // Set output file (in addition to console)
    void setOutputFile(const std::string& filepath);
    void closeOutputFile();
    
    // Configure output format
    void setFormat(const std::string& format);
    std::string getFormat() const;
    
    // Configure categories to log
    void setLogCategory(const std::string& category, bool enable);
    bool shouldLogCategory(const std::string& category) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
    std::string formatEntry(const LogEntry& entry) const;
};

// Main Logger class with enhanced functionality
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
    
    // Set minimum level for all loggers
    void setLevel(LogLevel level);
    
    // Configure global logging
    void setValidationEnabled(bool enable);
    bool isValidationEnabled() const;
    void setLogErrors(bool enable);
    bool getLogErrors() const;
    void setPerformanceMonitoring(bool enable);
    bool isPerformanceMonitoringEnabled() const;
    
    // Enhanced global logging functions
    void log(LogLevel level, const std::string& message, 
             const std::string& file = "", int line = 0, 
             const std::string& function = "");
    
    void logWithMetadata(LogLevel level, const std::string& message,
                        const std::string& file = "", int line = 0,
                        const std::string& function = "",
                        const std::unordered_map<std::string, std::string>& metadata = {});
    
    // Convenience methods
    void trace(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void debug(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void info(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void notice(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void warning(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void error(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void critical(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void alert(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void emergency(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    // Check if level is enabled
    bool isEnabled(LogLevel level) const;
    
    // Get global logger instance
    static Logger& getGlobal();
    
    // Set global logger
    static void setGlobal(std::shared_ptr<Logger> logger);
    
    // Performance monitoring
    void startTiming(const std::string& operation);
    void stopTiming(const std::string& operation);
    void logPerformanceStats() const;
    
    // Configuration validation
    void validateConfiguration() const;
    
    // Error recovery
    void setRecoveryEnabled(bool enable);
    bool isRecoveryEnabled() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Enhanced convenience macros for file/line/function info
#define NLM_LOG_TRACE(msg) nlm::Logger::getGlobal().trace(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_NOTICE(msg) nlm::Logger::getGlobal().notice(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ALERT(msg) nlm::Logger::getGlobal().alert(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_EMERGENCY(msg) nlm::Logger::getGlobal().emergency(msg, __FILE__, __LINE__, __func__)

// Enhanced stream-style logging helper
class LogStream {
public:
    LogStream(LogLevel level, const std::string& file, int line, const std::string& function);
    ~LogStream();
    
    template<typename T>
    LogStream& operator<<(const T& value) {
        oss_ << value;
        return *this;
    }
    
    // Support for string streams
    LogStream& operator<<(const std::string& value) {
        oss_ << value;
        return *this;
    }
    
    // Support for character streams
    LogStream& operator<<(char value) {
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

#define NLM_LOG_STREAM(level) nlm::LogStream(level, __FILE__, __LINE__, __func__)

// Category-based logging macros
#define NLM_LOG_PERF(msg) nlm::Logger::getGlobal().info("[PERFORMANCE] " + msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_MEM(msg) nlm::Logger::getGlobal().info("[MEMORY] " + msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_NET(msg) nlm::Logger::getGlobal().info("[NETWORK] " + msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_FILE(msg) nlm::Logger::getGlobal().info("[FILE] " + msg, __FILE__, __LINE__, __func__)

// Exception-safe logging helper
class SafeLogger {
public:
    SafeLogger(LogLevel level, const std::string& operation, const std::string& file = "", 
              int line = 0, const std::string& function = "");
    ~SafeLogger();
    
    template<typename T>
    SafeLogger& operator<<(const T& value) {
        stream_ << value;
        return *this;
    }
    
    void logSuccess();
    void logFailure(const std::string& error);
    
private:
    LogLevel level_;
    std::string operation_;
    std::ostringstream stream_;
    bool successLogged_;
    bool failureLogged_;
};

#define NLM_LOG_SAFE(level, operation) nlm::SafeLogger(level, operation, __FILE__, __LINE__, __func__)

} // namespace nlm
