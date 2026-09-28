/*
 * nlm/LoggerWrapper.hpp - Enhanced logging with error levels for the NLM system
 * Wraps the existing Logger class with enhanced error level support and context
 */

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <mutex>
#include <map>
#include "nlm/Exceptions.hpp"
#include "nlm/ErrorReporter.hpp"

namespace nlm {

// Enhanced logging levels
enum class LogLevel {
    Trace = 0,
    Debug = 1,
    Info = 2,
    Warning = 3,
    Error = 4,
    Critical = 5,
    Fatal = 6
};

// Log entry with enhanced context
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    std::map<std::string, std::string> context;
    std::string threadId;
    
    LogEntry(LogLevel lvl, const std::string& msg,
             const std::string& file = "", int line = 0,
             const std::string& function = "")
        : level(lvl), message(msg), file(file), line(line), function(function),
          timestamp(std::chrono::system_clock::now()) {}
    
    void addContext(const std::string& key, const std::string& value) {
        context[key] = value;
    }
    
    std::string getFormattedTimestamp() const {
        auto time = std::chrono::system_clock::to_time_t(timestamp);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
};

// Log entry formatter
class LogFormatter {
public:
    virtual ~LogFormatter() = default;
    virtual std::string format(const LogEntry& entry) = 0;
};

// Console log formatter
class ConsoleLogFormatter : public LogFormatter {
public:
    std::string format(const LogEntry& entry) override;
};

// JSON log formatter
class JSONLogFormatter : public LogFormatter {
public:
    std::string format(const LogEntry& entry) override;
};

// Enhanced logger interface
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
    virtual void setFormatter(std::shared_ptr<LogFormatter> formatter) = 0;
    virtual void addContext(const std::string& key, const std::string& value) = 0;
};

// Enhanced logger implementation
class LoggerWrapper : public ILogger {
public:
    LoggerWrapper();
    ~LoggerWrapper() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    void setFormatter(std::shared_ptr<LogFormatter> formatter) override;
    void addContext(const std::string& key, const std::string& value) override;
    
    // Convenience methods
    void trace(const std::string& message,
               const std::string& file = "", int line = 0,
               const std::string& function = "");
    
    void debug(const std::string& message,
               const std::string& file = "", int line = 0,
               const std::string& function = "");
    
    void info(const std::string& message,
              const std::string& file = "", int line = 0,
              const std::string& function = "");
    
    void warning(const std::string& message,
                 const std::string& file = "", int line = 0,
                 const std::string& function = "");
    
    void error(const std::string& message,
               const std::string& file = "", int line = 0,
               const std::string& function = "");
    
    void critical(const std::string& message,
                  const std::string& file = "", int line = 0,
                  const std::string& function = "");
    
    void fatal(const std::string& message,
               const std::string& file = "", int line = 0,
               const std::string& function = "");
    
    // Log NLM exception
    void logException(const NLMException& e);
    
    // Get global logger wrapper instance
    static LoggerWrapper& getGlobal();
    
    // Set global logger wrapper
    static void setGlobal(std::shared_ptr<LoggerWrapper> logger);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Log level conversion utilities
inline LogLevel errorToLogLevel(ErrorSeverity severity) {
    switch (severity) {
        case ErrorSeverity::Debug: return LogLevel::Debug;
        case ErrorSeverity::Info: return LogLevel::Info;
        case ErrorSeverity::Warning: return LogLevel::Warning;
        case ErrorSeverity::Error: return LogLevel::Error;
        case ErrorSeverity::Critical: return LogLevel::Critical;
        case ErrorSeverity::Fatal: return LogLevel::Fatal;
        default: return LogLevel::Error;
    }
}

inline ErrorSeverity logLevelToError(ErrorSeverity severity) {
    return severity;
}

} // namespace nlm
