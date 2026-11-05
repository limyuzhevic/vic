#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>

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
};

// Logger interface
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
};

// Console logger implementation
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
    
    // File management
    void setMaxFileSize(size_t maxSizeBytes);
    void setMaxFileCount(size_t maxFiles);
    size_t getLogFileSize() const;
    std::string getLogFilePath() const;
    
    // Performance monitoring
    void enablePerformanceLogging(bool enable);
    bool isPerformanceLoggingEnabled() const;
    
    // Structured logging
    void logStructured(const std::string& category, const std::string& event,
                      const std::string& message, LogLevel level = LogLevel::Info);
    
    // System state logging
    void enableSystemStateLogging(bool enable);
    bool isSystemStateLoggingEnabled() const;
    
    // Log filtering
    void addLogFilter(const std::string& pattern);
    void clearLogFilters();
    bool shouldLog(const std::string& message, LogLevel level) const;
    
    // Metrics collection
    void incrementMetric(const std::string& name);
    void setMetric(const std::string& name, double value);
    double getMetric(const std::string& name) const;
    std::map<std::string, double> getAllMetrics() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
};

// Main Logger class
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
    
    // Check if level is enabled
    bool isEnabled(LogLevel level) const;
    
    // Get global logger instance
    static Logger& getGlobal();
    
    // Set global logger
    static void setGlobal(std::shared_ptr<Logger> logger);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Main Logger class
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
    
    // Check if level is enabled
    bool isEnabled(LogLevel level) const;
    
    // Get global logger instance
    static Logger& getGlobal();
    
    // Set global logger
    static void setGlobal(std::shared_ptr<Logger> logger);
    
    // Performance monitoring
    void enablePerformanceLogging(bool enable);
    bool isPerformanceLoggingEnabled() const;
    
    // System state logging
    void enableSystemStateLogging(bool enable);
    bool isSystemStateLoggingEnabled() const;
    
    // Metrics collection
    void incrementMetric(const std::string& name);
    void setMetric(const std::string& name, double value);
    double getMetric(const std::string& name) const;
    std::map<std::string, double> getAllMetrics() const;
    
    // Structured logging
    void logStructured(const std::string& category, const std::string& event,
                      const std::string& message, LogLevel level = LogLevel::Info,
                      const std::string& file = "", int line = 0, const std::string& function = "");
    
    // Log filtering
    void addLogFilter(const std::string& pattern);
    void clearLogFilters();
    bool shouldLog(const std::string& message, LogLevel level) const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

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

#define NLM_LOG_STREAM(level) nlm::LogStream(level, __FILE__, __LINE__, __func__)

// Enhanced convenience macros for structured logging
#define NLM_LOG_STRUCTURED(category, event, msg) \
    nlm::Logger::getGlobal().logStructured(category, event, msg, nlm::LogLevel::Info, __FILE__, __LINE__, __func__)

#define NLM_LOG_PERFORMANCE(event, duration_ms) \
    do { \
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start_time).count(); \
        nlm::Logger::getGlobal().logStructured("performance", event, \
            "Duration: " + std::to_string(duration) + "ms", nlm::LogLevel::Info); \
    } while(0)

// Performance timing helper
class PerformanceTimer {
public:
    PerformanceTimer(const std::string& eventName);
    ~PerformanceTimer();
    
    template<typename T>
    PerformanceTimer& operator<<(const T& value) {
        oss_ << value;
        return *this;
    }
    
private:
    std::string eventName_;
    std::chrono::steady_clock::time_point startTime_;
    std::ostringstream oss_;
};

#define NLM_PERF(event_name) nlm::PerformanceTimer perf_timer(event_name)

} // namespace nlm
