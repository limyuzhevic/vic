#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <atomic>

namespace nlm {

// Log severity levels
enum class LogLevel {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4
};

// Log entry structure
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    
    LogEntry() : level(LogLevel::Info), message(), file(), line(0), function(), timestamp() {}
    LogEntry(LogLevel lvl, const std::string& msg, const std::string& file = "", 
             int lineNum = 0, const std::string& func = "")
        : level(lvl), message(msg), file(file), line(lineNum), function(func), 
          timestamp(std::chrono::system_clock::now()) {}
};

// Logger interface
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
};

// Console logger implementation with advanced features
class ConsoleLogger : public ILogger {
public:
    ConsoleLogger();
    explicit ConsoleLogger(LogLevel level);
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
    
    // Performance optimization: batch logging
    void enableBatchLogging(bool enable);
    bool isBatchLoggingEnabled() const;
    void flushBatchLogs();
    
    // Get log statistics
    size_t getTotalLogCount() const;
    double getLogRate() const; // logs per second
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
};

// Main Logger class with improved thread safety and performance
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
    
    // Set log level for specific logger
    void setLoggerLevel(size_t index, LogLevel level);
    
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
    
    // Component-based logging
    void debugWithComponent(const std::string& component, const std::string& message, 
                           const std::string& file = "", int line = 0, const std::string& function = "");
    void infoWithComponent(const std::string& component, const std::string& message,
                          const std::string& file = "", int line = 0, const std::string& function = "");
    void warningWithComponent(const std::string& component, const std::string& message,
                             const std::string& file = "", int line = 0, const std::string& function = "");
    void errorWithComponent(const std::string& component, const std::string& message,
                           const std::string& file = "", int line = 0, const std::string& function = "");
    void criticalWithComponent(const std::string& component, const std::string& message,
                              const std::string& file = "", int line = 0, const std::string& function = "");
    
    // Check if level is enabled
    bool isEnabled(LogLevel level) const;
    
    // Get global logger instance
    static Logger& getGlobal();
    
    // Set global logger (replaces current global logger)
    static void setGlobal(std::shared_ptr<Logger> logger);
    
    // Global log level control
    static void setGlobalLevel(LogLevel level);
    static LogLevel getGlobalLevel();
    
    // Global log filtering for specific components
    void addLogFilter(const std::string& component, LogLevel level);
    void removeLogFilter(const std::string& component);
    bool isFiltered(const std::string& component, LogLevel level) const;
    
    // Performance monitoring
    double getAverageLogLatency() const;
    size_t getLogCountSinceReset() const;
    void resetLogCounters();
    
    // Async logging support
    void enableAsyncLogging(bool enable);
    bool isAsyncLoggingEnabled() const;
    
    // Get logger count
    size_t getLoggerCount() const;
    
    // Get logger info for debugging
    std::string getLoggerInfo() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Advanced logging macros with component filtering
#define NLM_LOG_DEBUG_COMPONENT(component, msg) nlm::Logger::getGlobal().debugWithComponent(component, msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO_COMPONENT(component, msg) nlm::Logger::getGlobal().infoWithComponent(component, msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING_COMPONENT(component, msg) nlm::Logger::getGlobal().warningWithComponent(component, msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR_COMPONENT(component, msg) nlm::Logger::getGlobal().errorWithComponent(component, msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_CRITICAL_COMPONENT(component, msg) nlm::Logger::getGlobal().criticalWithComponent(component, msg, __FILE__, __LINE__, __func__)

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
    
    // Set component for this log message
    LogStream& component(const std::string& component);
    
private:
    LogLevel level_;
    std::ostringstream oss_;
    std::string file_;
    int line_;
    std::string function_;
    std::string component_;
};

#define NLM_LOG_STREAM(level) nlm::LogStream(level, __FILE__, __LINE__, __func__)

// Performance timing utilities
class LogTimer {
public:
    LogTimer(const std::string& component = "default");
    ~LogTimer();
    
    LogTimer& operation(const std::string& op);
    
private:
    std::string component_;
    std::string operation_;
    std::chrono::high_resolution_clock::time_point startTime_;
};

#define NLM_LOG_TIME(component) nlm::LogTimer timer(component)
#define NLM_LOG_TIME_OP(component, op) { nlm::LogTimer scopedTimer(component); scopedTimer.operation(op); }

} // namespace nlm
