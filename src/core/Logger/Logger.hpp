#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>

namespace nlm {

/**
 * @file Logger.hpp
 * @brief Logging system for NLM
 * 
 * This header defines the comprehensive logging system for the Neural Learning Machine (NLM).
 * It provides:
 * - Multiple log levels (Debug, Info, Warning, Error, Critical)
 * - Thread-safe logging with mutex protection
 * - Multiple logger backends (Console, File)
 * - Global logger instance management
 * - Convenient macros for common logging operations
 * - Stream-style logging for complex messages
 * 
 * @version 2.0 - Enhanced with thread safety, file output, and better color support
 */

/**
 * @brief Log severity levels
 * 
 * Log levels determine the importance of log messages:
 * - Debug: Detailed debug information (lowest priority)
 * - Info: General information about system operation
 * - Warning: Potentially problematic situations that don't prevent operation
 * - Error: Error conditions that prevent specific operations
 * - Critical: Critical errors that prevent normal operation (highest priority)
 */
enum class LogLevel {
public:
    Debug = 0,      ///< Detailed debug information
    Info = 1,        ///< General information
    Warning = 2,     ///< Warning conditions
    Error = 3,       ///< Error conditions
    Critical = 4     ///< Critical errors
};

/**
 * @brief Log entry structure
 * 
 * Contains all information for a single log message, including source location
 * and timestamp for debugging purposes.
 */
struct LogEntry {
    LogLevel level;                       ///< Severity level
    std::string message;                   ///< Log message text
    std::string file;                      ///< Source file where log was generated
    int line;                              ///< Line number in source file
    std::string function;                  ///< Function name where log was generated
    std::chrono::system_clock::time_point timestamp;  ///< Time when log was created
};

/**
 * @brief Abstract interface for all logger implementations
 * 
 * Provides the contract for logging implementations. All loggers must implement
 * this interface to be compatible with the NLM logging system.
 */
class ILogger {
public:
    virtual ~ILogger() = default;
    
    /**
     * @brief Process a log entry
     * 
     * @param entry Log entry to process
     */
    virtual void log(const LogEntry& entry) = 0;
    
    /**
     * @brief Set the minimum log level for this logger
     * 
     * @param level Minimum log level (messages below this level are ignored)
     */
    virtual void setLevel(LogLevel level) = 0;
    
    /**
     * @brief Get the current minimum log level
     * 
     * @return Current minimum log level
     */
    virtual LogLevel getLevel() const = 0;
};

/**
 * @brief Console logger implementation
 * 
 * Logs messages to the console with optional colors and file output.
 * Supports multiple log levels and can write to both stdout and log files.
 */
class ConsoleLogger : public ILogger {
public:
    /**
     * @brief Default constructor
     * 
     * Creates a console logger with Info level and colors enabled.
     */
    ConsoleLogger();
    
    /**
     * @brief Constructor with log level
     * 
     * @param level Minimum log level for this logger
     */
    explicit ConsoleLogger(LogLevel level);
    
    /**
     * @brief Destructor
     */
    ~ConsoleLogger() override;
    
    /**
     * @brief Process a log entry
     */
    void log(const LogEntry& entry) override;
    
    /**
     * @brief Set the minimum log level
     */
    void setLevel(LogLevel level) override;
    
    /**
     * @brief Get the current minimum log level
     */
    LogLevel getLevel() const override;
    
    /**
     * @brief Enable or disable colored output
     * 
     * @param useColors Whether to use ANSI color codes
     */
    void setUseColors(bool useColors);
    
    /**
     * @brief Check if colored output is enabled
     * 
     * @return true if colors are enabled, false otherwise
     */
    bool getUseColors() const;
    
    /**
     * @brief Set output file (in addition to console)
     * 
     * @param filepath Path to log file (will be appended to)
     */
    void setOutputFile(const std::string& filepath);
    
    /**
     * @brief Close the output file
     */
    void closeOutputFile();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    /**
     * @brief Convert log level to string for display
     */
    std::string levelToString(LogLevel level) const;
    
    /**
     * @brief Get ANSI color code for log level
     * 
     * @param level Log level
     * @return ANSI color code string or empty string for no color
     */
    std::string levelToColor(LogLevel level) const;
};

/**
 * @brief Main Logger class
 * 
 * Provides the core logging functionality with multiple logger sinks.
 * Manages a collection of ILogger implementations and routes log messages
 * to all registered sinks.
 */
class Logger {
public:
    /**
     * @brief Default constructor
     */
    Logger();
    
    /**
     * @brief Destructor
     */
    ~Logger();
    
    /**
     * @brief Disable copying (non-copyable)
     */
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    /**
     * @brief Add logger sink
     * 
     * @param logger Logger implementation to add
     */
    void addLogger(std::shared_ptr<ILogger> logger);
    
    /**
     * @brief Remove all loggers
     */
    void clearLoggers();
    
    /**
     * @brief Set minimum level for all loggers
     * 
     * @param level Minimum log level for all loggers
     */
    void setLevel(LogLevel level);
    
    /**
     * @brief Log message with all parameters
     * 
     * @param level Log level
     * @param message Log message
     * @param file Source file (defaults to empty string)
     * @param line Line number (defaults to 0)
     * @param function Function name (defaults to empty string)
     */
    void log(LogLevel level, const std::string& message, 
             const std::string& file = "", int line = 0, 
             const std::string& function = "");
    
    /**
     * @brief Convenience method for debug logging
     */
    void debug(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    /**
     * @brief Convenience method for info logging
     */
    void info(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    /**
     * @brief Convenience method for warning logging
     */
    void warning(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    /**
     * @brief Convenience method for error logging
     */
    void error(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    /**
     * @brief Convenience method for critical logging
     */
    void critical(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    /**
     * @brief Check if level is enabled for any logger
     * 
     * @param level Log level to check
     * @return true if any logger has this level enabled
     */
    bool isEnabled(LogLevel level) const;
    
    /**
     * @brief Get global logger instance
     * 
     * @return Reference to global logger instance
     */
    static Logger& getGlobal();
    
    /**
     * @brief Set global logger
     * 
     * @param logger New global logger instance
     */
    static void setGlobal(std::shared_ptr<Logger> logger);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * @brief Convenience macros for file/line/function logging
 * 
 * These macros automatically capture the current file, line number, and
 * function name for better debugging information.
 */
#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)

/**
 * @brief Stream-style logging helper
 * 
 * Allows complex log messages using stream operators for easy construction
 * of formatted log messages.
 */
class LogStream {
public:
    /**
     * @brief Constructor
     */
    LogStream(LogLevel level, const std::string& file, int line, const std::string& function);
    
    /**
     * @brief Destructor (automatically logs the accumulated message)
     */
    ~LogStream();
    
    /**
     * @brief Stream output operator for any type
     */
    template<typename T>
    LogStream& operator<<(const T& value) {
        oss_ << value;
        return *this;
    }
    
private:
    LogLevel level_;              ///< Log level for this stream
    std::ostringstream oss_;      ///< Accumulated log message
    std::string file_;            ///< Source file
    int line_;                    ///< Line number
    std::string function_;        ///< Function name
};

/**
 * @brief Macro for stream-style logging
 */
#define NLM_LOG_STREAM(level) nlm::LogStream(level, __FILE__, __LINE__, __func__)

} // namespace nlm
