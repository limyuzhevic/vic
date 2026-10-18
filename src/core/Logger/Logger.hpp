#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>

namespace nlm {

/**
 * @brief Log severity levels for structured logging
 * 
 * Defines the severity hierarchy for log messages, from most verbose
 * (Debug) to most severe (Critical).
 */
enum class LogLevel {
    /** @brief Detailed debugging information, very verbose */
    Debug = 0,
    /** @brief General informational messages */
    Info = 1,
    /** @brief Warning messages that don't prevent operation */
    Warning = 2,
    /** @brief Error messages that indicate a failure condition */
    Error = 3,
    /** @brief Critical errors that prevent normal operation */
    Critical = 4
};

// Log entry structure
/**
 * @brief Structure representing a single log entry
 * 
 * Contains all information needed for a log message, including severity,
 * message content, source location, and timestamp.
 */
struct LogEntry {
    /** @brief Severity level of the log message */
    LogLevel level;
    /** @brief The actual log message content */
    std::string message;
    /** @brief Source file where the log occurred (filename) */
    std::string file;
    /** @brief Line number in source file where the log occurred */
    int line;
    /** @brief Function name where the log occurred */
    std::string function;
    /** @brief Timestamp when the log occurred */
    std::chrono::system_clock::time_point timestamp;
};

// Logger interface
/**
 * @brief Abstract base class for all logger implementations
 * 
 * Defines the interface that all logger implementations must provide.
 * This allows for multiple logging backends (console, file, network, etc.)
 * to be used interchangeably.
 */
class ILogger {
public:
    /**
     * @brief Virtual destructor
     * 
     * Ensures proper cleanup of derived class implementations
     */
    virtual ~ILogger() = default;
    
    /**
     * @brief Log a log entry
     * 
     * @param entry The log entry to process
     */
    virtual void log(const LogEntry& entry) = 0;
    
    /**
     * @brief Set the minimum log level for this logger
     * 
     * @param level The minimum level that will be logged
     */
    virtual void setLevel(LogLevel level) = 0;
    
    /**
     * @brief Get the current minimum log level
     * 
     * @return Current minimum log level
     */
    virtual LogLevel getLevel() const = 0;
};

// Console logger implementation
/**
 * @brief Console-based logger implementation
 * 
 * Writes log messages to the standard output (console) with optional
 * color coding based on log level. Can also write to a file in addition
 * to the console. This is the most common logger implementation for
 * debugging and development.
 */
class ConsoleLogger : public ILogger {
public:
    /**
     * @brief Construct a console logger with default settings
     */
    ConsoleLogger();
    
    /**
     * @brief Construct a console logger with specific log level
     * 
     * @param level Initial minimum log level
     */
    explicit ConsoleLogger(LogLevel level);
    
    /**
     * @brief Destructor
     */
    ~ConsoleLogger() override;
    
    /**
     * @brief Log a log entry to console and/or file
     * 
     * @param entry The log entry to process
     */
    void log(const LogEntry& entry) override;
    
    /**
     * @brief Set the minimum log level
     * 
     * @param level New minimum log level
     */
    void setLevel(LogLevel level) override;
    
    /**
     * @brief Get the current minimum log level
     * 
     * @return Current minimum log level
     */
    LogLevel getLevel() const override;
    
    /**
     * @brief Enable or disable color output
     * 
     * @param useColors Whether to use ANSI color codes in output
     */
    void setUseColors(bool useColors);
    
    /**
     * @brief Check if colors are enabled
     * 
     * @return true if colors are enabled, false otherwise
     */
    bool getUseColors() const;
    
    /**
     * @brief Set output file for logging
     * 
     * @param filepath Path to the file where logs should be written
     */
    void setOutputFile(const std::string& filepath);
    
    /**
     * @brief Close the output file
     */
    void closeOutputFile();
    
private:
    /** @brief Private implementation details */
    struct Impl;
    /** @brief Pointer to implementation (Pimpl idiom) */
    std::unique_ptr<Impl> pImpl;
    
    /**
     * @brief Convert log level to string representation
     * 
     * @param level Log level to convert
     * @return String representation of the level
     */
    std::string levelToString(LogLevel level) const;
    
    /**
     * @brief Convert log level to ANSI color code
     * 
     * @param level Log level to convert
     * @return ANSI color code string for the level
     */
    std::string levelToColor(LogLevel level) const;
};

// Main Logger class
/**
 * @brief Central logger class with multiple sinks and convenience methods
 * 
 * The Logger class provides a centralized logging system that can send messages
 * to multiple logger implementations (sinks) simultaneously. It provides both
 * simple string-based logging and stream-based logging for convenience.
 * 
 * Features:
 * - Multiple logging backends (console, file, etc.)
 * - Log level filtering
 * - Thread-safe operation
 * - Rich metadata (file, line, function, timestamp)
 * - Convenience macros for common logging patterns
 * 
 * Usage:
 * @code
 * #include "Logger.hpp"
 * 
 * // Basic logging
 * NLM_LOG_INFO("Starting simulation");
 * 
 * // Stream-style logging
 * NLM_LOG_STREAM(Logger::Level::Info) << "Value: " << 42;
 * 
 * // File/line/function info automatically included
 * NLM_LOG_ERROR("Failed to load: " << filename);
 * @endcode
 */
class Logger {
public:
    /**
     * @brief Construct a logger with default settings
     */
    Logger();
    
    /**
     * @brief Destructor
     */
    ~Logger();
    
    /**
     * @brief Disable copying
     * 
     * Logger instances should be managed through the singleton pattern
     */
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    /**
     * @brief Add a logger sink to this logger
     * 
     * @param logger Shared pointer to a logger implementation
     */
    void addLogger(std::shared_ptr<ILogger> logger);
    
    /**
     * @brief Remove all logger sinks
     */
    void clearLoggers();
    
    /**
     * @brief Set minimum log level for all sinks
     * 
     * @param level Minimum log level that will be processed
     */
    void setLevel(LogLevel level);
    
    /**
     * @brief Log a message with full metadata
     * 
     * @param level Log severity level
     * @param message Log message content
     * @param file Source file (defaults to empty string)
     * @param line Source line number (defaults to 0)
     * @param function Source function name (defaults to empty string)
     */
    void log(LogLevel level, const std::string& message, 
             const std::string& file = "", int line = 0, 
             const std::string& function = "");
    
    /**
     * @brief Log a debug message
     * 
     * @param message Log message content
     * @param file Source file
     * @param line Source line number
     * @param function Source function name
     */
    void debug(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    /**
     * @brief Log an info message
     */
    void info(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    /**
     * @brief Log a warning message
     */
    void warning(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    /**
     * @brief Log an error message
     */
    void error(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    /**
     * @brief Log a critical message
     */
    void critical(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    /**
     * @brief Check if a log level is enabled
     * 
     * @param level Log level to check
     * @return true if the level is enabled, false otherwise
     */
    bool isEnabled(LogLevel level) const;
    
    /**
     * @brief Get the global logger instance
     * 
     * @return Reference to the global logger singleton
     */
    static Logger& getGlobal();
    
    /**
     * @brief Set the global logger instance
     * 
     * @param logger Shared pointer to the global logger
     */
    static void setGlobal(std::shared_ptr<Logger> logger);
    
private:
    /** @brief Private implementation details */
    struct Impl;
    /** @brief Pointer to implementation (Pimpl idiom) */
    std::unique_ptr<Impl> pImpl;
};

// Convenience macro for file/line/function info
#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)

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

} // namespace nlm
