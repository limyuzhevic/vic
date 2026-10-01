#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>

namespace nlm {

/**
 * @brief Log severity levels
 * 
 * Defines the hierarchy of log severity levels used throughout the system.
 * Each level includes all levels with equal or higher severity.
 * 
 * @see ILogger Interface for logging methods
 * @see Logger Main logging class with global convenience functions
 */
enum class LogLevel {
public:
    /**
     * @brief Debug level - Detailed information useful for debugging
     * 
     * Provides fine-grained information about internal state, algorithmic steps,
     * and data flow. Useful for understanding and fixing issues in neural network
     * computations and simulations.
     */
    Debug = 0,
    
    /**
     * @brief Info level - General information about system operation
     * 
     * Provides important information about system initialization, major events,
     * and operational status. Useful for monitoring simulation progress.
     */
    Info = 1,
    
    /**
     * @brief Warning level - Potentially problematic situations that are not errors
     * 
     * Indicates conditions that may cause issues or unexpected behavior,
     * such as deprecations, resource warnings, or performance concerns.
     */
    Warning = 2,
    
    /**
     * @brief Error level - Error conditions that prevent functionality
     * 
     * Indicates serious problems that affect system operation or data integrity,
     * such as file I/O errors, memory allocation failures, or calculation errors.
     */
    Error = 3,
    
    /**
     * @brief Critical level - Severe errors that may cause system failure
     * 
     * Indicates catastrophic conditions that prevent normal system operation,
     * such as hardware failures, unrecoverable memory corruption, or fatal
     * computational errors.
     */
    Critical = 4
};

/**
 * @brief Complete log entry containing all information for a log message
 * 
 * Stores all metadata associated with a log event, including the message content,
 * severity level, source location, and timestamp. Designed to be immutable
 * and efficiently copyable.
 * 
 * @tparam LogEntryType Alternative parameter name for the template version
 */
struct LogEntry {
    /**
     * @brief The severity level of this log entry
     * 
     * Determines how important the message is and whether it should be
     * processed based on the current logging threshold.
     */
    LogLevel level;
    
    /**
     * @brief The log message content
     * 
     * The actual log message text. Can contain arbitrary string content
     * including formatting characters, but should avoid excessive verbosity
     * for debug-level logs.
     */
    std::string message;
    
    /**
     * @brief Source file where the log entry was created
     * 
     * Full path to the source file containing the log statement. Empty if
     * not provided. Useful for identifying the source of log messages.
     */
    std::string file;
    
    /**
     * @brief Source line number where the log entry was created
     * 
     * Line number in the source file containing the log statement. 0 if
     * not provided. Used for pinpointing the exact location of log events.
     */
    int line;
    
    /**
     * @brief Name of the function where the log entry was created
     * 
     * Name of the function containing the log statement. Empty if not provided.
     * Useful for understanding which code path generated log messages.
     */
    std::string function;
    
    /**
     * @brief Timestamp when the log entry was created
     * 
     * High-resolution timestamp marking when the log entry was generated,
     * captured when the log entry was constructed.
     */
    std::chrono::system_clock::time_point timestamp;
};

/**
 * @brief Abstract interface for all logger implementations
 * 
 * Defines the contract that all logger sinks must implement to be compatible
 * with the NLM logging system. Supports pluggable logging backends.
 * 
 * @see ConsoleLogger Concrete implementation for console output
 * @see Logger Main logging class that manages multiple loggers
 */
class ILogger {
public:
    /**
     * @brief Virtual destructor
     */
    virtual ~ILogger() = default;
    
    /**
     * @brief Log a complete log entry
     * 
     * Processes a log entry according to the logger's configuration and
     * output destination. Must not throw exceptions.
     * 
     * @param entry The log entry to process
     */
    virtual void log(const LogEntry& entry) = 0;
    
    /**
     * @brief Set the minimum log level for this logger
     * 
     * Configure the logger to only process log entries at or above this level.
     * Entries below this level are silently discarded.
     * 
     * @param level The minimum log level to process
     */
    virtual void setLevel(LogLevel level) = 0;
    
    /**
     * @brief Get the current minimum log level for this logger
     * 
     * @return LogLevel The current minimum log level
     */
    virtual LogLevel getLevel() const = 0;
};

/**
 * @brief Console logger implementation using standard output
 * 
 * Writes log entries to the console with optional color formatting.
 * Provides support for both stderr and stdout, and can write to a file
 * in addition to or instead of the console.
 * 
 * @see ILogger Abstract interface for all loggers
 * @see Logger Main logging class that manages multiple loggers
 */
class ConsoleLogger : public ILogger {
public:
    /**
     * @brief Default constructor
     * 
     * Creates a console logger with Debug level enabled and colors enabled.
     */
    ConsoleLogger();
    
    /**
     * @brief Constructor with level specification
     * 
     * Creates a console logger with the specified minimum log level.
     * Colors are enabled by default.
     * 
     * @param level The minimum log level to process
     */
    explicit ConsoleLogger(LogLevel level);
    
    /**
     * @brief Destructor
     */
    ~ConsoleLogger() override;
    
    /**
     * @brief Log a complete log entry
     * 
     * Writes the log entry to the console with appropriate formatting.
     * Applies color formatting based on log level if enabled.
     * 
     * @param entry The log entry to log
     */
    void log(const LogEntry& entry) override;
    
    /**
     * @brief Set the minimum log level for this logger
     * 
     * @param level The minimum log level to process
     */
    void setLevel(LogLevel level) override;
    
    /**
     * @brief Get the current minimum log level for this logger
     * 
     * @return LogLevel The current minimum log level
     */
    LogLevel getLevel() const override;
    
    /**
     * @brief Enable or disable color formatting
     * 
     * Controls whether log entries are colorized based on their severity level.
     * Colors are enabled by default if the output supports them.
     * 
     * @param useColors True to enable color formatting, false to disable
     */
    void setUseColors(bool useColors);
    
    /**
     * @brief Check if color formatting is enabled
     * 
     * @return bool True if color formatting is enabled
     */
    bool getUseColors() const;
    
    /**
     * @brief Set output file for logging (in addition to console)
     * 
     * Configures the logger to write log entries to both the console and
     * the specified file. Useful for redirecting logs to files for analysis.
     * 
     * @param filepath Path to the file for log output
     * @throws std::runtime_error if the file cannot be opened
     */
    void setOutputFile(const std::string& filepath);
    
    /**
     * @brief Close the output file
     * 
     * Closes the output file and returns to console-only logging.
     */
    void closeOutputFile();
    
private:
    /**
     * @brief Internal implementation details (Pimpl idiom)
     */
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    /**
     * @brief Convert log level to human-readable string
     * 
     * @param level The log level to convert
     * @return std::string String representation of the log level
     */
    std::string levelToString(LogLevel level) const;
    
    /**
     * @brief Get ANSI color code for log level
     * 
     * @param level The log level to get color for
     * @return std::string ANSI color code for the log level
     */
    std::string levelToColor(LogLevel level) const;
};

/**
 * @brief Main logging class managing multiple logger sinks
 * 
 * Provides a centralized logging infrastructure with support for multiple
 * logger implementations. Routes log messages to all registered logger sinks,
 * each with its own configuration. Manages the global logger instance.
 * 
 * @see ILogger Abstract interface for logging backends
 * @see ConsoleLogger Example console logger implementation
 */
class Logger {
public:
    /**
     * @brief Constructor
     * 
     * Creates a new Logger instance with default configuration.
     * The instance is not automatically registered as the global logger.
     */
    Logger();
    
    /**
     * @brief Destructor
     */
    ~Logger();
    
    /**
     * @brief Copy constructor (deleted)
     * 
     * Logger objects cannot be copied to prevent resource management issues.
     */
    Logger(const Logger&) = delete;
    
    /**
     * @brief Copy assignment operator (deleted)
     */
    Logger& operator=(const Logger&) = delete;
    
    /**
     * @brief Add a logger sink to this logger
     * 
     * Registers a logger implementation to receive log messages. The logger
     * will be called for all log entries at or above the configured level.
     * 
     * @param logger Shared pointer to the logger implementation
     */
    void addLogger(std::shared_ptr<ILogger> logger);
    
    /**
     * @brief Remove all logger sinks
     * 
     * Clears all registered logger sinks, effectively disabling logging
     * for this Logger instance.
     */
    void clearLoggers();
    
    /**
     * @brief Set the minimum log level for all logger sinks
     * 
     * Configures all registered logger sinks to only process log entries
     * at or above this level. Can be overridden on individual sinks.
     * 
     * @param level The minimum log level for all sinks
     */
    void setLevel(LogLevel level);
    
    /**
     * @brief Log a message through all logger sinks
     * 
     * Creates a log entry with the specified level and message, then routes
     * it to all registered logger sinks. Includes optional source location
     * information for debugging.
     * 
     * @param level The severity level of the log message
     * @param message The log message content
     * @param file Source file (defaults to empty string)
     * @param line Source line number (defaults to 0)
     * @param function Source function name (defaults to empty string)
     */
    void log(LogLevel level, const std::string& message, 
             const std::string& file = "", int line = 0, 
             const std::string& function = "");
    
    /**
     * @brief Log a debug-level message (convenience method)
     * 
     * Convenience wrapper for log(LogLevel::Debug, ...).
     * 
     * @param message The debug message
     * @param file Source file (defaults to empty string)
     * @param line Source line number (defaults to 0)
     * @param function Source function name (defaults to empty string)
     */
    void debug(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    /**
     * @brief Log an info-level message (convenience method)
     * 
     * Convenience wrapper for log(LogLevel::Info, ...).
     * 
     * @param message The info message
     * @param file Source file (defaults to empty string)
     * @param line Source line number (defaults to 0)
     * @param function Source function name (defaults to empty string)
     */
    void info(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    /**
     * @brief Log a warning-level message (convenience method)
     * 
     * Convenience wrapper for log(LogLevel::Warning, ...).
     * 
     * @param message The warning message
     * @param file Source file (defaults to empty string)
     * @param line Source line number (defaults to 0)
     * @param function Source function name (defaults to empty string)
     */
    void warning(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    /**
     * @brief Log an error-level message (convenience method)
     * 
     * Convenience wrapper for log(LogLevel::Error, ...).
     * 
     * @param message The error message
     * @param file Source file (defaults to empty string)
     * @param line Source line number (defaults to 0)
     * @param function Source function name (defaults to empty string)
     */
    void error(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    /**
     * @brief Log a critical-level message (convenience method)
     * 
     * Convenience wrapper for log(LogLevel::Critical, ...).
     * 
     * @param message The critical message
     * @param file Source file (defaults to empty string)
     * @param line Source line number (defaults to 0)
     * @param function Source function name (defaults to empty string)
     */
    void critical(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    /**
     * @brief Check if a log level is enabled
     * 
     * Determines whether log messages at the specified level would be processed
     * by the logger's configured sinks. Considers both the global and sink-specific
     * minimum log levels.
     * 
     * @param level The log level to check
     * @return bool True if log messages at this level would be processed
     */
    bool isEnabled(LogLevel level) const;
    
    /**
     * @brief Get the global logger instance
     * 
     * Returns the singleton global logger instance. If no global logger has been
     * set via setGlobal(), returns a default ConsoleLogger instance.
     * 
     * @return Logger& Reference to the global logger instance
     */
    static Logger& getGlobal();
    
    /**
     * @brief Set the global logger instance
     * 
     * Replaces the current global logger with a new instance. Useful for
     * configuring logging output in applications.
     * 
     * @param logger Shared pointer to the new global logger
     */
    static void setGlobal(std::shared_ptr<Logger> logger);
    
private:
    /**
     * @brief Internal implementation details (Pimpl idiom)
     */
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * @brief Stream-style logging helper for convenient message construction
 * 
 * Provides a stream-like interface for constructing log messages with multiple
 * values of different types. The stream object is destroyed at the end of
 * the statement, automatically logging the message.
 * 
 * @code
 * NLM_LOG_STREAM(nlm::LogLevel::Error) << "Failed to load config from " << filepath << ": " << error;
 * @endcode
 * 
 * @see NLM_LOG_STREAM Macro for usage examples
 */
class LogStream {
public:
    /**
     * @brief Constructor for stream-style logging
     * 
     * Creates a log stream with the specified log level and source location.
     * The log entry is automatically created and logged when the stream
     * object is destroyed (at the end of the statement).
     * 
     * @param level The log level for the message
     * @param file Source file where the log call occurs
     * @param line Source line number where the log call occurs
     * @param function Source function where the log call occurs
     */
    LogStream(LogLevel level, const std::string& file, int line, const std::string& function);
    
    /**
     * @brief Destructor - logs the accumulated message
     */
    ~LogStream();
    
    /**
     * @brief Template operator<< for stream-style message construction
     * 
     * Appends a value of any type to the log message stream. Supports
     * all types that can be streamed to std::ostringstream.
     * 
     * @tparam T Type of the value to append
     * @param value The value to append to the log message
     * @return LogStream& Reference to this stream for further chaining
     */
    template<typename T>
    LogStream& operator<<(const T& value) {
        oss_ << value;
        return *this;
    }
    
private:
    /**
     * @brief Log level for this stream
     */
    LogLevel level_;
    
    /**
     * @brief String stream for building the log message
     */
    std::ostringstream oss_;
    
    /**
     * @brief Source file for this log entry
     */
    std::string file_;
    
    /**
     * @brief Source line number for this log entry
     */
    int line_;
    
    /**
     * @brief Source function name for this log entry
     */
    std::string function_;
};

/**
 * @brief Macro for creating stream-style log messages at a specific level
 * 
 * Creates a LogStream object with the specified log level and source location,
 * enabling stream-style logging. The stream is destroyed at the end of the
 * statement, automatically logging the accumulated message.
 * 
 * @param level The log level for the message
 * 
 * @code
 * NLM_LOG_STREAM(nlm::LogLevel::Error) << "Failed to load config from " << filepath << ": " << error;
 * @endcode
 * 
 * @see LogStream Stream-style logging class
 */
#define NLM_LOG_STREAM(level) nlm::LogStream(level, __FILE__, __LINE__, __func__)

/**
 * @brief Convenience macro for debug-level logging
 * 
 * Expands to a call to Logger::getGlobal().debug() with file, line, and function
 * information automatically included.
 * 
 * @param msg The debug message to log
 * 
 * @code
 * NLM_LOG_DEBUG("Failed to load configuration from file");
 * NLM_LOG_DEBUG("Neuron count: " << neuronCount);
 * @endcode
 * 
 * @see NLM_LOG_STREAM Stream-style logging macro
 */
#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)

/**
 * @brief Convenience macro for info-level logging
 * 
 * Expands to a call to Logger::getGlobal().info() with file, line, and function
 * information automatically included.
 * 
 * @param msg The info message to log
 */
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__)

/**
 * @brief Convenience macro for warning-level logging
 * 
 * Expands to a call to Logger::getGlobal().warning() with file, line, and function
 * information automatically included.
 * 
 * @param msg The warning message to log
 */
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)

/**
 * @brief Convenience macro for error-level logging
 * 
 * Expands to a call to Logger::getGlobal().error() with file, line, and function
 * information automatically included.
 * 
 * @param msg The error message to log
 */
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__)

/**
 * @brief Convenience macro for critical-level logging
 * 
 * Expands to a call to Logger::getGlobal().critical() with file, line, and function
 * information automatically included.
 * 
 * @param msg The critical message to log
 */
#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)

} // namespace nlm