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
    Trace = 0,
    Debug = 1,
    Info = 2,
    Warning = 3,
    Error = 4,
    Critical = 5
};

// Log entry structure with extended metadata
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    std::string threadId;
    uint64_t processId;
    std::map<std::string, std::string> context;
};

// Log formatter interface
class LogFormatter {
public:
    virtual ~LogFormatter() = default;
    virtual std::string format(const LogEntry& entry) = 0;
    virtual std::string formatMinimal(const LogEntry& entry) = 0;
    virtual std::string formatJson(const LogEntry& entry) = 0;
};

// Color-coded log formatter
class ColorFormatter : public LogFormatter {
public:
    std::string format(const LogEntry& entry) override;
    std::string formatMinimal(const LogEntry& entry) override;
    std::string formatJson(const LogEntry& entry) override;
    void setUseColors(bool useColors);
    bool getUseColors() const;
    void setTimeFormat(const std::string& format);
    std::string getTimeFormat() const;
    void setIncludeContext(bool includeContext);
    bool getIncludeContext() const;
private:
    bool useColors_ = true;
    std::string timeFormat_ = "%Y-%m-%d %H:%M:%S";
    bool includeContext_ = true;
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
};

// Simple text formatter
class SimpleFormatter : public LogFormatter {
public:
    std::string format(const LogEntry& entry) override;
    std::string formatMinimal(const LogEntry& entry) override;
    std::string formatJson(const LogEntry& entry) override;
    void setIncludeContext(bool includeContext);
    bool getIncludeContext() const;
private:
    bool includeContext_ = true;
};

// Log destination interface (for file output)
class LogDestination {
public:
    virtual ~LogDestination() = default;
    virtual void write(const std::string& formattedMessage) = 0;
    virtual void flush() = 0;
    virtual void close() = 0;
    virtual bool isOpen() const = 0;
};

// File log destination with rotation
class RotatingFileDestination : public LogDestination {
public:
    RotatingFileDestination(const std::string& filepath, size_t maxFileSize = 10 * 1024 * 1024, // 10MB
                           int maxFiles = 5, const std::string& pattern = "{}.log");
    ~RotatingFileDestination() override;
    void write(const std::string& formattedMessage) override;
    void flush() override;
    void close() override;
    bool isOpen() const override;
    void setLevel(LogLevel level);
    LogLevel getLevel() const;
    void setFormat(std::shared_ptr<LogFormatter> formatter);
    std::shared_ptr<LogFormatter> getFormat() const;
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Logger interface
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
    virtual void setFormatter(std::shared_ptr<LogFormatter> formatter) = 0;
    virtual std::shared_ptr<LogFormatter> getFormatter() const = 0;
};

// Async logger for high-performance logging
class AsyncLogger : public ILogger {
public:
    AsyncLogger(size_t queueSize = 8192, size_t workerThreads = 2);
    ~AsyncLogger() override;
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    void setFormatter(std::shared_ptr<LogFormatter> formatter) override;
    std::shared_ptr<LogFormatter> getFormatter() const override;
    void flush();
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Main Logger class with advanced features
class Logger {
public:
    Logger();
    ~Logger();
    
    // Disable copying, enable moving
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    Logger(Logger&&) noexcept;
    Logger& operator=(Logger&&) noexcept;
    
    // Add logger sink (async or sync)
    void addLogger(std::shared_ptr<ILogger> logger);
    
    // Remove all loggers
    void clearLoggers();
    
    // Set minimum level for all loggers
    void setLevel(LogLevel level);
    
    // Set formatter for all loggers
    void setGlobalFormatter(std::shared_ptr<LogFormatter> formatter);
    
    // Log with full context
    void log(LogLevel level, const std::string& message, 
             const std::string& file = "", int line = 0, 
             const std::string& function = "");
    
    // Add context to log entries
    void addContext(const std::string& key, const std::string& value);
    void clearContext();
    
    // Convenience methods
    void trace(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
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
    struct PerformanceStats {
        uint64_t totalLogs = 0;
        uint64_t droppedLogs = 0;
        uint64_t logSizeBytes = 0;
        double avgLogTime = 0.0;
        std::chrono::system_clock::time_point lastLogTime;
    };
    
    PerformanceStats getStats() const;
    void resetStats();
    
    // Log formatting shortcuts
    template<typename... Args>
    void logf(LogLevel level, const std::string& format, Args&&... args) {
        std::string message = formatString(format, std::forward<Args>(args)...);
        log(level, message);
    }
    
    template<typename... Args>
    void infof(const std::string& format, Args&&... args) {
        logf(LogLevel::Info, format, std::forward<Args>(args)...);
    }
    
    template<typename... Args>
    void errorf(const std::string& format, Args&&... args) {
        logf(LogLevel::Error, format, std::forward<Args>(args)...);
    }
    
    template<typename... Args>
    void warningf(const std::string& format, Args&&... args) {
        logf(LogLevel::Warning, format, std::forward<Args>(args)...);
    }
private:
    std::string formatString(const std::string& format, ...);
    
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Convenience macro for file/line/function info
#define NLM_LOG_TRACE(msg) nlm::Logger::getGlobal().trace(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)

// Stream-style logging helper with context
class LogStream {
public:
    LogStream(LogLevel level, const std::string& file, int line, const std::string& function);
    ~LogStream();
    
    template<typename T>
    LogStream& operator<<(const T& value) {
        oss_ << value;
        return *this;
    }
    
    // Add context to the stream
    LogStream& withContext(const std::string& key, const std::string& value);
    
private:
    LogLevel level_;
    std::ostringstream oss_;
    std::string file_;
    int line_;
    std::string function_;
    std::map<std::string, std::string> context_;
};

#define NLM_LOG_STREAM(level) nlm::LogStream(level, __FILE__, __LINE__, __func__)

// Logger instance management
extern std::shared_ptr<Logger> globalLogger;

} // namespace nlm
