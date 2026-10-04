#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <mutex>
#include <map>
#include <memory>
#include <iomanip>
#include <optional>

namespace nlm {

// Log severity levels with enhanced functionality
enum class LogLevel {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4,
    // Enhanced levels for more granular control
    Trace = -1,
    Notice = 5
};

// Log entry structure with enhanced metadata
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    
    // Enhanced fields for better debugging
    std::string threadName;
    std::string processId;
    
    LogEntry() 
        : level(LogLevel::Info), line(0), threadName(), processId() {}
    
    LogEntry(LogLevel lvl, const std::string& msg, const std::string& file = "", int line = 0, 
             const std::string& function = "", const std::string& thread = "")
        : level(lvl), message(msg), file(file), line(line), function(function), 
          timestamp(std::chrono::system_clock::now()), threadName(thread) {}
};

// Formatter for log entries
class LogFormatter {
public:
    virtual ~LogFormatter() = default;
    virtual std::string format(const LogEntry& entry) = 0;
    virtual void setFormat(const std::string& format) = 0;
};

// Simple text formatter
class TextLogFormatter : public LogFormatter {
public:
    TextLogFormatter();
    TextLogFormatter(const std::string& format);
    
    std::string format(const LogEntry& entry) override;
    void setFormat(const std::string& format) override;
    
private:
    std::string formatPattern;
    bool useColors;
    
    std::string formatTimestamp(const LogEntry& entry) const;
    std::string formatLevel(const LogEntry& entry) const;
    std::string formatThread(const LogEntry& entry) const;
    std::string formatLocation(const LogEntry& entry) const;
};

// Logger interface with enhanced functionality
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
    
    // Enhanced functionality
    virtual void setFormatter(std::unique_ptr<LogFormatter> formatter) = 0;
    virtual void setMinLevel(LogLevel level) = 0;
    virtual LogLevel getMinLevel() const = 0;
    
    // Async logging support
    virtual void logAsync(const LogEntry& entry) = 0;
    
    // Log counting and statistics
    virtual size_t getLogCount(LogLevel level) const = 0;
    virtual void resetLogCount() = 0;
};

// Enhanced console logger implementation
class ConsoleLogger : public ILogger {
public:
    ConsoleLogger();
    explicit ConsoleLogger(LogLevel level);
    ~ConsoleLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    // Enhanced functionality
    void setFormatter(std::unique_ptr<LogFormatter> formatter) override;
    void setMinLevel(LogLevel level) override;
    LogLevel getMinLevel() const override;
    void logAsync(const LogEntry& entry) override;
    
    // Color and formatting
    void setUseColors(bool useColors);
    bool getUseColors() const;
    void setOutputFile(const std::string& filepath);
    void closeOutputFile();
    
    // Statistics
    size_t getLogCount(LogLevel level) const override;
    void resetLogCount() override;
    
    // Log filtering
    void addFilter(std::function<bool(const LogEntry&)> filter);
    void removeFilter(std::function<bool(const LogEntry&)> filter);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
    bool shouldLog(const LogEntry& entry) const;
};

// File logger implementation
class FileLogger : public ILogger {
public:
    FileLogger(const std::string& filepath);
    ~FileLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    // Enhanced functionality
    void setFormatter(std::unique_ptr<LogFormatter> formatter) override;
    void setMinLevel(LogLevel level) override;
    LogLevel getMinLevel() const override;
    void logAsync(const LogEntry& entry) override;
    
    // File management
    void setMaxFileSize(size_t maxSize);
    void setMaxFiles(size_t maxFiles);
    void rotateFiles();
    
    // Statistics
    size_t getLogCount(LogLevel level) const override;
    void resetLogCount() override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Main enhanced Logger class
class Logger {
public:
    Logger();
    ~Logger();
    
    // Disable copying
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    // Enhanced logger management
    void addLogger(std::shared_ptr<ILogger> logger);
    void removeLogger(const std::string& name = ""); // Remove by name if set, otherwise last added
    void clearLoggers();
    
    // Set minimum level for all loggers
    void setLevel(LogLevel level);
    
    // Global logging functions with enhanced parameters
    void log(LogLevel level, const std::string& message, 
             const std::string& file = "", int line = 0, 
             const std::string& function = "", const std::string& thread = "");
    
    // Convenience methods with enhanced signatures
    void debug(const std::string& message, const std::string& file = "", int line = 0, 
               const std::string& function = "", const std::string& thread = "");
    void info(const std::string& message, const std::string& file = "", int line = 0, 
              const std::string& function = "", const std::string& thread = "");
    void warning(const std::string& message, const std::string& file = "", int line = 0, 
                 const std::string& function = "", const std::string& thread = "");
    void error(const std::string& message, const std::string& file = "", int line = 0, 
               const std::string& function = "", const std::string& thread = "");
    void critical(const std::string& message, const std::string& file = "", int line = 0, 
                  const std::string& function = "", const std::string& thread = "");
    
    // Enhanced convenience macros (same as before for backward compatibility)
    void debug(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "") {
        debug(message, file, line, function, "");
    }
    void info(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "") {
        info(message, file, line, function, "");
    }
    void warning(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "") {
        warning(message, file, line, function, "");
    }
    void error(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "") {
        error(message, file, line, function, "");
    }
    void critical(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "") {
        critical(message, file, line, function, "");
    }
    
    // Check if level is enabled
    bool isEnabled(LogLevel level) const;
    
    // Get global logger instance
    static Logger& getGlobal();
    
    // Set global logger
    static void setGlobal(std::shared_ptr<Logger> logger);
    
    // Global statistics and configuration
    static size_t getTotalLogCount(LogLevel level);
    static void resetTotalLogCount();
    static void setGlobalLogLevel(LogLevel level);
    static LogLevel getGlobalLogLevel();
    
    // Log level string conversion
    static std::string logLevelToString(LogLevel level);
    
    // Initialize with default configuration
    static void initializeDefaultLogger(const std::string& logFile = "", LogLevel level = LogLevel::Info);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Enhanced stream-style logging helper
class LogStream {
public:
    LogStream(LogLevel level, const std::string& file, int line, const std::string& function);
    LogStream(LogLevel level, const std::string& file, int line, const std::string& function, const std::string& thread);
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
    
private:
    LogLevel level_;
    std::ostringstream oss_;
    std::string file_;
    int line_;
    std::string function_;
    std::string thread_;
};

// Enhanced macro system
#define NLM_LOG_TRACE(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_NOTICE(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)

// Enhanced stream macro
#define NLM_LOG_STREAM(level) nlm::LogStream(level, __FILE__, __LINE__, __func__)

// Thread identification helper
inline std::string getCurrentThreadName() {
    // Implementation would depend on the platform
    return "main_thread";
}

inline std::string getCurrentProcessId() {
    // Implementation would depend on the platform
    return "PID_1234";
}

} // namespace nlm
