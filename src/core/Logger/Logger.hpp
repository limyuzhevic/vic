#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>

namespace nlm {

// Log severity levels for controlling output verbosity
// Debug: Detailed diagnostic information (finest granularity)
// Info: General information about system operation
// Warning: Potentially harmful situations that don't prevent operation
// Error: Error conditions that may still allow operation
// Critical: Severe errors that prevent further operation
enum class LogLevel {
    Debug = 0,    // Detailed diagnostic information
    Info = 1,      // General operational information
    Warning = 2,   // Potentially harmful situations
    Error = 3,     // Error conditions that may allow continued operation
    Critical = 4   // Severe errors that prevent further operation
};

// Log entry structure containing complete logging information
// Includes all metadata needed for log analysis and presentation
struct LogEntry {
    LogLevel level;                                           // Severity level
    std::string message;                                       // Log message text
    std::string file;                                         // Source file where logged
    int line;                                                  // Line number in source file
    std::string function;                                      // Function name where logged
    std::chrono::system_clock::time_point timestamp;          // Precise timestamp
};

// Abstract base class for all logger implementations
// Defines the interface for log sink implementations
class ILogger {
public:
    virtual ~ILogger() = default;
    
    // Core logging method
    // Processes a complete log entry
    virtual void log(const LogEntry& entry) = 0;
    
    // Level filtering
    // Sets the minimum log level this logger will process
    virtual void setLevel(LogLevel level) = 0;
    
    // Level retrieval
    // Returns the current minimum log level for this logger
    virtual LogLevel getLevel() const = 0;
};

// Console logger implementation for standard output
// Provides formatted colored console output with file/line/function info
class ConsoleLogger : public ILogger {
public:
    ConsoleLogger();
    explicit ConsoleLogger(LogLevel level);
    ~ConsoleLogger() override;
    
    // Core logging implementation
    void log(const LogEntry& entry) override;
    
    // Level control
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    // Output formatting
    // Enables/disables color output for better readability
    void setUseColors(bool useColors);
    bool getUseColors() const;
    
    // Output redirection
    // Directs log output to file in addition to console
    void setOutputFile(const std::string& filepath);
    void closeOutputFile();
    
private:
    // Pimpl idiom for implementation hiding
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal formatting helpers
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
};

// Main Logger class providing centralized logging management
// Manages multiple logger implementations and provides global logging utilities
class Logger {
public:
    Logger();
    ~Logger();
    
    // Disable copying for memory safety
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    // Logger sink management
    // Adds a new logger implementation to the logging system
    void addLogger(std::shared_ptr<ILogger> logger);
    
    // Removes all logger sinks
    void clearLoggers();
    
    // Global level control
    // Sets minimum level for all registered loggers
    void setLevel(LogLevel level);
    
    // Core logging function with full metadata
    // Supports automatic file/line/function extraction
    void log(LogLevel level, const std::string& message, 
             const std::string& file = "", int line = 0, 
             const std::string& function = "");
    
    // Convenience methods with automatic metadata
    void debug(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void info(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void warning(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void error(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void critical(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    // Level checking
    // Returns true if messages at this level will be logged
    bool isEnabled(LogLevel level) const;
    
    // Global logger instance management
    // Gets the singleton global logger instance
    static Logger& getGlobal();
    
    // Custom global logger setup
    // Allows replacing the default global logger
    static void setGlobal(std::shared_ptr<Logger> logger);
    
private:
    // Pimpl idiom for implementation hiding
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Convenience macros for easy logging with automatic metadata
// These macros automatically capture file, line, and function information
#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)

// Stream-style logging helper for complex message construction
// Allows building log messages using stream operators for better performance
class LogStream {
public:
    LogStream(LogLevel level, const std::string& file, int line, const std::string& function);
    ~LogStream();
    
    // Stream operator for building log messages
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

// Stream-style logging macro
#define NLM_LOG_STREAM(level) nlm::LogStream(level, __FILE__, __LINE__, __func__)

} // namespace nlm
