#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <unordered_map>

namespace nlm {

// Log severity levels - used for filtering and filtering log output
enum class LogLevel {
    Debug = 0,      // Detailed debugging information
    Info = 1,        // General information messages
    Warning = 2,     // Warning conditions that don't prevent operation
    Error = 3,       // Error conditions that prevent specific operations
    Critical = 4     // Critical errors that prevent continued operation
};

// Log entry structure containing all information about a log message
struct LogEntry {
    LogLevel level;                             // Severity level of the log entry
    std::string message;                        // The actual log message
    std::string file;                           // Source file where log was generated
    int line;                                   // Line number in source file
    std::string function;                       // Function name where log was generated
    std::chrono::system_clock::time_point timestamp;  // When the log was generated
    
    // Constructor for easy creation of log entries
    LogEntry(LogLevel lvl, const std::string& msg, 
             const std::string& file = "", int line = 0,
             const std::string& function = "")
        : level(lvl), message(msg), file(file), line(line), 
          function(function), timestamp(std::chrono::system_clock::now()) {}
};

// Abstract interface for log sinks (destinations for log messages)
class ILogger {
public:
    virtual ~ILogger() = default;
    
    // Process a log entry
    virtual void log(const LogEntry& entry) = 0;
    
    // Set minimum log level for this sink
    virtual void setLevel(LogLevel level) = 0;
    
    // Get current minimum log level for this sink
    virtual LogLevel getLevel() const = 0;
    
    // Check if a specific level is enabled for this sink
    virtual bool isEnabled(LogLevel level) const {
        return level >= getLevel();
    }
};

// Console logger implementation - outputs to standard error with colors
class ConsoleLogger : public ILogger {
public:
    ConsoleLogger();
    explicit ConsoleLogger(LogLevel level);
    ~ConsoleLogger() override;
    
    // Process a log entry (outputs to console)
    void log(const LogEntry& entry) override;
    
    // Set minimum log level for this logger
    void setLevel(LogLevel level) override;
    
    // Get current minimum log level
    LogLevel getLevel() const override;
    
    // Enable/disable colored output for different log levels
    void setUseColors(bool useColors);
    bool getUseColors() const;
    
    // Set output file (in addition to console) - enables log file output
    void setOutputFile(const std::string& filepath);
    void closeOutputFile();
    
    // Check if file output is currently enabled
    bool hasOutputFile() const;
    
    // Get current log file name
    std::string getOutputFile() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Convert log level enum to human-readable string
    std::string levelToString(LogLevel level) const;
    
    // Get ANSI color code for a log level (for console output)
    std::string levelToColor(LogLevel level) const;
};

// Main Logger class - manages multiple log sinks and provides global access
class Logger {
public:
    Logger();
    ~Logger();
    
    // Disable copying, enable moving
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    Logger(Logger&&) noexcept;
    Logger& operator=(Logger&&) noexcept;
    
    // Add a log sink to this logger
    void addLogger(std::shared_ptr<ILogger> logger);
    
    // Remove all log sinks
    void clearLoggers();
    
    // Set minimum log level for all loggers
    void setLevel(LogLevel level);
    
    // Process a log entry through all registered loggers
    void log(LogLevel level, const std::string& message, 
             const std::string& file = "", int line = 0, 
             const std::string& function = "");
    
    // Convenience methods for different log levels
    void debug(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void info(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void warning(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void error(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void critical(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    // Check if a specific level is enabled by any logger
    bool isEnabled(LogLevel level) const;
    
    // Get global logger instance (singleton pattern)
    static Logger& getGlobal();
    
    // Set global logger (replaces the default global logger)
    static void setGlobal(std::shared_ptr<Logger> logger);
    
    // Get list of registered loggers
    std::vector<std::shared_ptr<ILogger>> getLoggers() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Convenience macro for file/line/function info in log messages
#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)

// Stream-style logging helper - allows chaining values into log messages
class LogStream {
public:
    LogStream(LogLevel level, const std::string& file, int line, const std::string& function);
    ~LogStream();
    
    // Stream insertion operator for various types
    template<typename T>
    LogStream& operator<<(const T& value) {
        oss_ << value;
        return *this;
    }
    
    // String stream access for advanced formatting
    std::ostringstream& getStream() { return oss_; }
    
private:
    LogLevel level_;
    std::ostringstream oss_;
    std::string file_;
    int line_;
    std::string function_;
};

// Macro for stream-style logging with automatic level detection
#define NLM_LOG_STREAM(level) nlm::LogStream(level, __FILE__, __LINE__, __func__)

// Additional logging utilities
namespace LogUtils {
    // Format a log entry as a human-readable string
    std::string formatEntry(const LogEntry& entry);
    
    // Check if a log level string is valid
    bool isValidLogLevel(const std::string& levelStr);
    
    // Convert log level string to enum
    LogLevel parseLogLevel(const std::string& levelStr);
    
    // Get all available log level strings
    std::vector<std::string> getAvailableLogLevels();
}

} // namespace nlm
