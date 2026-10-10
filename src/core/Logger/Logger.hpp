#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include "../ErrorHandling.hpp"

namespace nlm {

/**
 * @file Logger.hpp
 * 
 * Comprehensive logging system for NLM
 * 
 * This file provides a flexible, extensible logging system that supports
 * multiple log levels, output destinations, and integration with error
 * handling. The logging system is used throughout the NLM to provide
 * runtime diagnostics, debugging information, and system monitoring.
 * 
 * Key features:
 * - Six severity levels (Debug, Info, Warning, Error, Critical)
 * - Multiple output destinations (console, file)
 * - Customizable formatting and filtering
 * - Integration with error handling system
 * - Stream-style logging for convenient output
 * - Thread-safe operations
 * - Global logger singleton for application-wide logging
 * 
 * @note Log levels are hierarchical: Debug < Info < Warning < Error < Critical
 * @note Only messages at or above the current level are processed
 * @note Used for debugging, monitoring, and system diagnostics
 * 
 * @see ErrorHandling.hpp - Integration with error system
 * @see Config.hpp - Configuration options for logging
 * 
 * @author NLM Development Team
 * @date 2026
 * @version 1.0
 */

/**
 * @brief Log severity level enumeration
 * 
 * Represents the severity of log messages for filtering and display purposes.
 * Log levels are hierarchical - higher severity levels indicate more serious
 * issues. Only messages at or above the current log level are processed.
 * 
 * @note Used by ILogger, ConsoleLogger, and Logger classes
 * @see ILogger - Interface for logging implementations
 * @see LogEntry - Structure containing log entries
 * @see Logger - Main logging class with global functions
 * 
 * @author NLM Development Team
 */
enum class LogLevel {
    /** Debug level for detailed troubleshooting and development */
    Debug = 0,
    /** Informational level for normal operation tracking */
    Info = 1,
    /** Warning level for potentially harmful situations */
    Warning = 2,
    /** Error level for error conditions that don't halt execution */
    Error = 3,
    /** Critical level for severe errors that may halt execution */
    Critical = 4
};

/**
 * @struct LogEntry
 * 
 * Complete log entry containing all information about a logged message.
 * This structure is used by all logging implementations and contains
 * the data needed for formatting, filtering, and display.
 * 
 * @note LogEntry objects are passed to ILogger implementations
 * @see ILogger - Interface for processing log entries
 * @see ConsoleLogger - Console output implementation
 * @see Logger - Main logging class
 * 
 * @author NLM Development Team
 */
struct LogEntry {
    /** Severity level of the log message */
    LogLevel level;
    
    /** The actual log message content */
    std::string message;
    
    /** Source file where the log was generated */
    std::string file;
    
    /** Line number in source file where log was generated */
    int line;
    
    /** Function name where the log was generated */
    std::string function;
    
    /** Timestamp when the log was created */
    std::chrono::system_clock::time_point timestamp;
};

/**
 * @class ILogger
 * 
 * Abstract interface for logging implementations.
 * 
 * This class defines the contract for all logging implementations,
 * ensuring a consistent interface across different output backends.
 * Users can implement custom loggers by inheriting from this interface.
 * 
 * @note Concrete implementations include ConsoleLogger
 * @note Used by Logger class to build logging sinks
 * @note Thread-safe - implementations must be thread-safe
 * 
 * @see ConsoleLogger - Standard console logging implementation
 * @see Logger - Main logging class that uses ILogger implementations
 * 
 * @author NLM Development Team
 */
class ILogger {
public:
    /**
     * @brief Virtual destructor
     * 
     * Ensures proper cleanup of logging implementations.
     */
    virtual ~ILogger() = default;
    
    /**
     * @brief Log a complete log entry
     * 
     * Processes a complete log entry according to the implementation's
     * formatting, filtering, and output requirements.
     * 
     * @param entry Complete log entry to process
     * 
     * @pre entry must be a valid LogEntry object
     * @post Log entry is processed according to implementation
     * 
     * @note This is the primary method for all logging implementations
     * @see LogEntry - Structure of log entries
     * @see setLevel() - For filtering log entries
     */
    virtual void log(const LogEntry& entry) = 0;
    
    /**
     * @brief Set the minimum log level for this logger
     * 
     * Configures the minimum severity level that this logger will
     * process. Messages below this level are silently discarded.
     * 
     * @param level Minimum severity level to process
     * 
     * @pre level must be a valid LogLevel value
     * @post Only messages at or above level are processed
     * 
     * @see LogLevel - Enumeration of severity levels
     * @see getLevel() - For retrieving current level
     */
    virtual void setLevel(LogLevel level) = 0;
    
    /**
     * @brief Get the current minimum log level
     * 
     * Retrieves the current minimum severity level configured for
     * this logger.
     * 
     * @return Current minimum log level
     * 
     * @see setLevel() - For setting the minimum level
     */
    virtual LogLevel getLevel() const = 0;
};

/**
 * @class ConsoleLogger
 * 
 * Standard console logger implementation.
 * 
 * This class provides logging output to the standard console (stdout/stderr)
 * with optional formatting features including color coding and file output.
 * It is the most commonly used logger in NLM applications.
 * 
 * Features:
 * - Color-coded output for different log levels
 * - Optional file output in addition to console
 * - Customizable log level filtering
 * - Thread-safe operations
 * 
 * @note Uses ANSI escape codes for color on supported terminals
 * @note Can be configured to write to both console and file simultaneously
 * @note Used as the default logger in Logger::getGlobal()
 * 
 * @see ILogger - Interface this class implements
 * @see Logger - Main logging class that manages ConsoleLogger instances
 * 
 * @author NLM Development Team
 */
class ConsoleLogger : public ILogger {
public:
    /**
     * @brief Default constructor
     * 
     * Creates a console logger with default settings:
     * - Log level: Info
     * - Colors: Enabled
     * - File output: Disabled
     * 
     * @post Logger created with default configuration
     */
    ConsoleLogger();
    
    /**
     * @brief Constructor with log level
     * 
     * Creates a console logger with specified minimum log level.
     * 
     * @param level Minimum severity level to log
     * 
     * @post Logger created with specified log level
     * @see setLevel() - For changing level after creation
     */
    explicit ConsoleLogger(LogLevel level);
    
    /**
     * @brief Destructor
     * 
     * Cleans up console logger resources and closes any open file.
     */
    ~ConsoleLogger() override;
    
    // Disable copying - PImpl idiom
    ConsoleLogger(const ConsoleLogger&) = delete;
    ConsoleLogger& operator=(const ConsoleLogger&) = delete;
    ConsoleLogger(ConsoleLogger&&) noexcept;
    ConsoleLogger& operator=(ConsoleLogger&&) noexcept;
    
    /**
     * @brief Process a log entry to console
     * 
     * Formats and outputs the log entry to the console (and optionally
     * to file). Supports color coding based on log level.
     * 
     * @param entry Complete log entry to process
     * 
     * @pre entry must be a valid LogEntry object
     * @post Log entry displayed to console (and file if configured)
     * 
     * @note Formats output with timestamp, level, and message
     * @see LogEntry - Structure of log entries
     * @see setUseColors() - For color formatting
     */
    void log(const LogEntry& entry) override;
    
    /**
     * @brief Set minimum log level
     * 
     * Changes the minimum severity level for this console logger.
     * 
     * @param level Minimum severity level
     * 
     * @see getLevel() - For current level
     * @see LogLevel - Enumeration of levels
     */
    void setLevel(LogLevel level) override;
    
    /**
     * @brief Get current minimum log level
     * 
     * @return Current minimum log level
     * 
     * @see setLevel() - For setting the level
     */
    LogLevel getLevel() const override;
    
    /**
     * @brief Enable or disable color output
     * 
     * Configures whether log messages should include ANSI color codes
     * based on log level. Colors improve readability but may interfere
     * with log parsing or non-terminal output.
     * 
     * @param useColors true to enable colors, false to disable
     * 
     * @post Color output setting updated
     * 
     * @note Colors are only used on terminal emulators that support ANSI codes
     * @see getUseColors() - For current setting
     */
    void setUseColors(bool useColors);
    
    /**
     * @brief Check if colors are enabled
     * 
     * @return true if colors are enabled, false otherwise
     * 
     * @see setUseColors() - For setting color mode
     */
    bool getUseColors() const;
    
    /**
     * @brief Set output file for additional logging
     * 
     * Configures the console logger to write log messages to both
     * console and a specified file. This is useful for capturing
     * logs during long-running processes or for file analysis.
     * 
     * @param filepath Path to file for log output
     * 
     * @pre filepath must be writable
     * @post Log messages written to both console and file
     * @post Previous file (if any) is closed
     * 
     * @note File output is appended to existing file
     * @see closeOutputFile() - For closing file output
     */
    void setOutputFile(const std::string& filepath);
    
    /**
     * @brief Close the output file
     * 
     * Closes the file used for additional log output. The console
     * output continues normally after file closure.
     * 
     * @post File output is disabled
     * @post Console output remains active
     * 
     * @see setOutputFile() - For opening file output
     */
    void closeOutputFile();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    /**
     * @brief Convert log level to string representation
     * 
     * Helper function for formatting log entries. Converts a
     * LogLevel enum to a string for display in log messages.
     * 
     * @param level Log level to convert
     * @return String representation of log level
     * 
     * @note Used internally for log formatting
     */
    std::string levelToString(LogLevel level) const;
    
    /**
     * @brief Convert log level to ANSI color code
     * 
     * Helper function for color formatting. Returns ANSI escape
     * sequence for the specified log level, or empty string if
     * colors are disabled or not supported.
     * 
     * @param level Log level to convert
     * @return ANSI color code string or empty string
     * 
     * @note Only meaningful when colors are enabled
     */
    std::string levelToColor(LogLevel level) const;
};

/**
 * @class Logger
 * 
 * Central logging manager for the NLM system.
 * 
 * This is the main logging class that provides a unified interface for
 * logging throughout the NLM application. It manages multiple logger
 * sinks (implementations of ILogger), provides global logging functions,
 * and integrates with the error handling system.
 * 
 * Key features:
 * - Multiple logger sinks (console, file, custom)
 * - Global singleton instance (Logger::getGlobal())
 * - Integration with error handling system
 * - Convenience methods for each log level
 * - Thread-safe operations
 * - Stream-style logging support
 * 
 * @note This class follows the Composite pattern for multiple loggers
 * @note Logger instances can be added/removed at runtime
 * @note Global functions provide easy access to logging
 * @see ILogger - Interface for logger sinks
 * @see ConsoleLogger - Standard logger implementation
 * 
 * @author NLM Development Team
 */
class Logger {
public:
    /**
     * @brief Default constructor
     * 
     * Creates a new Logger instance with default configuration.
     * The logger starts with no sinks and will not log anything
     * unless sinks are added.
     * 
     * @post Logger created with no sinks
     * @see addLogger() - For adding sinks
     * @see setGlobal() - For global logger setup
     */
    Logger();
    
    /**
     * @brief Destructor
     * 
     * Cleans up logger resources and removes all sinks.
     */
    ~Logger();
    
    // Disable copying for PImpl idiom
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    Logger(Logger&&) noexcept;
    Logger& operator=(Logger&&) noexcept;
    
    /**
     * @brief Add a logger sink to this instance
     * 
     * Adds a logger implementation (ILogger) to the list of sinks
     * that receive log messages from this Logger instance. Log messages
     * are processed by all registered sinks.
     * 
     * @param logger Shared pointer to logger implementation
     * 
     * @pre logger must be a valid shared pointer to ILogger
     * @post Logger processes messages from this sink
     * @post Messages are forwarded to all registered sinks
     * 
     * @note Logger takes ownership of the shared pointer
     * @see ILogger - Interface for logger implementations
     * @see clearLoggers() - For removing all sinks
     */
    void addLogger(std::shared_ptr<ILogger> logger);
    
    /**
     * @brief Remove all logger sinks
     * 
     * Clears all registered logger sinks, preventing any further logging
     * from this Logger instance. This is useful for temporarily disabling
     * logging or during configuration changes.
     * 
     * @post All logger sinks removed
     * @post Logger will not process any messages
     * 
     * @see addLogger() - For adding sinks
     */
    void clearLoggers();
    
    /**
     * @brief Set minimum log level for all sinks
     * 
     * Configures the minimum severity level for all registered
     * logger sinks. Messages below this level are filtered out.
     * 
     * @param level Minimum severity level for all sinks
     * 
     * @post All sinks use the new minimum level
     * 
     * @note Changes apply to all registered sinks
     * @see LogLevel - Enumeration of severity levels
     */
    void setLevel(LogLevel level);
    
    /**
     * @brief Log a message with level and source information
     * 
     * Primary logging method that formats and forwards a message
     * to all registered logger sinks. This method includes file,
     * line, and function information for better debugging.
     * 
     * @param level Severity level of the message
     * @param message Log message content
     * @param file Source file name (defaults to empty string)
     * @param line Line number in source file (defaults to 0)
     * @param function Function name (defaults to empty string)
     * 
     * @pre level must be a valid LogLevel
     * @post Message logged to all sinks with source information
     * 
     * @note Source information is used for debugging and filtering
     * @see LogEntry - Complete log entry structure
     */
    void log(LogLevel level, const std::string& message,
             const std::string& file = "", int line = 0,
             const std::string& function = "");
    
    /**
     * @brief Log an error from the error handling system
     * 
     * Convenience method for logging NLMError objects. Extracts
     * relevant information from the error and formats it for logging.
     * 
     * @param error Complete error object to log
     * 
     * @pre error must be a valid NLMError
     * @post Error logged to all sinks with full details
     * 
     * @note Includes error code, message, and source location
     * @see NLMError - Error structure
     * @see logError() - Alternative overload
     */
    void logError(const NLMError& error);
    
    /**
     * @brief Log an error with code and message
     * 
     * Alternative logging method for errors that don't require
     * a full NLMError object. Provides source location information
     * for better debugging.
     * 
     * @param code Error code
     * @param message Error message
     * @param file Source file name (defaults to "")
     * @param line Line number (defaults to 0)
     * @param function Function name (defaults to "")
     * 
     * @pre code must be a valid ErrorCode
     * @post Error logged to all sinks
     * 
     * @see logError(const NLMError&) - For error object logging
     */
    void logError(ErrorCode code, const std::string& message,
                  const std::string& file = "", int line = 0,
                  const std::string& function = "");
    
    /**
     * @brief Convenience method for debug-level logging
     * 
     * Logs a message at the Debug level with source location information.
     * This is the most detailed level of logging.
     * 
     * @param message Log message
     * @param file Source file name
     * @param line Line number
     * @param function Function name
     * 
     * @see debug() - With source information
     * @see LogLevel::Debug - Log level
     */
    void debug(const std::string& message, const std::string& file = "",
               int line = 0, const std::string& function = "");
    
    /**
     * @brief Convenience method for info-level logging
     * 
     * Logs a message at the Info level with source location information.
     * This is used for general information and status messages.
     * 
     * @param message Log message
     * @param file Source file name
     * @param line Line number
     * @param function Function name
     * 
     * @see info() - With source information
     * @see LogLevel::Info - Log level
     */
    void info(const std::string& message, const std::string& file = "",
              int line = 0, const std::string& function = "");
    
    /**
     * @brief Convenience method for warning-level logging
     * 
     * Logs a message at the Warning level with source location information.
     * This is used for potentially harmful situations.
     * 
     * @param message Log message
     * @param file Source file name
     * @param line Line number
     * @param function Function name
     * 
     * @see warning() - With source information
     * @see LogLevel::Warning - Log level
     */
    void warning(const std::string& message, const std::string& file = "",
                 int line = 0, const std::string& function = "");
    
    /**
     * @brief Convenience method for error-level logging
     * 
     * Logs a message at the Error level with source location information.
     * This is used for error conditions that don't halt execution.
     * 
     * @param message Log message
     * @param file Source file name
     * @param line Line number
     * @param function Function name
     * 
     * @see error() - With source information
     * @see LogLevel::Error - Log level
     */
    void error(const std::string& message, const std::string& file = "",
               int line = 0, const std::string& function = "");
    
    /**
     * @brief Convenience method for critical-level logging
     * 
     * Logs a message at the Critical level with source location information.
     * This is used for severe errors that may halt execution.
     * 
     * @param message Log message
     * @param file Source file name
     * @param line Line number
     * @param function Function name
     * 
     * @see critical() - With source information
     * @see LogLevel::Critical - Log level
     */
    void critical(const std::string& message, const std::string& file = "",
                  int line = 0, const std::string& function = "");
    
    /**
     * @brief Check if a log level is enabled
     * 
     * Determines whether messages at the specified log level would
     * be processed by this logger instance. This is useful for
     * avoiding expensive formatting operations when logging might
     * be disabled.
     * 
     * @param level Log level to check
     * @return true if level is enabled, false otherwise
     * 
     * @see LogLevel - Enumeration of severity levels
     */
    bool isEnabled(LogLevel level) const;
    
    /**
     * @brief Get global logger instance (singleton)
     * 
     * Returns the global logger instance used throughout the NLM
     * application. This is the primary logging interface for most
     * applications. The global logger is created on first access
     * and persists for the lifetime of the application.
     * 
     * @return Reference to global logger instance
     * 
     * @note Call setGlobal() to replace the global logger
     * @note Global logger is typically a ConsoleLogger
     * 
     * @see setGlobal() - For replacing global logger
     * @see ILogger - Interface for logging
     */
    static Logger& getGlobal();
    
    /**
     * @brief Set global logger instance
     * 
     * Replaces the global logger instance used throughout the NLM
     * application. This allows custom logging implementations to
     * be used instead of the default ConsoleLogger.
     * 
     * @param logger Shared pointer to new global logger
     * 
     * @pre logger must be a valid shared pointer
     * * @post All future getGlobal() calls return new logger
     * @post Existing global logger is replaced
     * 
     * @note Useful for custom logging implementations
     * @note Only one global logger can exist at a time
     */
    static void setGlobal(std::shared_ptr<Logger> logger);
    
    /**
     * @brief Get error handler instance
     * 
     * Returns the global error handler instance. This provides
     * integration between logging and error handling systems.
     * 
     * @return Reference to global error handler
     * 
     * @see setErrorCallback() - For configuring error handling
     * @see ErrorHandling.hpp - For error handling details
     */
    static ErrorHandler& getErrorHandler();
    
    /**
     * @brief Set global error callback
     * 
     * Configures the global error callback function that is invoked
     * when errors occur. This enables integration between error handling
     * and custom error processing.
     * 
     * @param callback Function to call on errors
     * 
     * @see getErrorHandler() - For accessing error handler
     */
    static void setErrorCallback(ErrorHandler::ErrorCallback callback);
    
    /**
     * @brief Handle error through logging system
     * 
     * Processes an error through the logging system, ensuring that
     * errors are properly logged and can be captured by custom
     * logging implementations.
     * 
     * @param error Error to process
     * 
     * @pre error must be a valid NLMError
     * @post Error is logged and handled by error system
     */
    static void handleError(const NLMError& error);
    
    /**
     * @brief Handle error through logging system (alternative)
     * 
     * Alternative error handling method that accepts error code
     * and message directly, without requiring a full NLMError object.
     * 
     * @param code Error code
     * @param message Error message
     * @param file Source file name
     * @param line Line number
     * @param function Function name
     * 
     * @see handleError(const NLMError&) - For error object handling
     */
    static void handleError(ErrorCode code, const std::string& message,
                           const std::string& file = "", int line = 0,
                           const std::string& function = "");
    
    /**
     * @brief Log status of the logging system
     * 
     * Outputs diagnostic information about the current state of
     * the logging system, including registered sinks, log levels,
     * and configuration. Useful for debugging logging issues.
     * 
     * @see debug() - For debug-level status
     */
    void logStatus() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * @brief Stream-style logging helper class
 * 
 * Provides stream-style logging interface for convenient and
 * efficient logging of complex data structures. This class allows
 * logging of multiple values using the `<<` operator, similar to
 * standard output streams.
 * 
 * @note Used with NLM_LOG_STREAM macro for convenient logging
 * @see NLM_LOG_STREAM - Macro for creating log streams
 * @see ILogger - Interface for logging implementations
 * 
 * @author NLM Development Team
 */
class LogStream {
public:
    /**
     * @brief Constructor for log stream
     * 
     * Creates a log stream with the specified log level and source
     * information. The stream is immediately written to the global
     * logger when destroyed (destructor is called).
     * 
     * @param level Log severity level
     * @param file Source file name
     * @param line Line number
     * @param function Function name
     * 
     * @pre level must be a valid LogLevel
     * @post Log stream created, ready for output
     */
    LogStream(LogLevel level, const std::string& file, int line, const std::string& function);
    
    /**
     * @brief Destructor writes accumulated stream content
     * 
     * Writes the accumulated stream content to the global logger
     * and cleans up the stream resources.
     * 
     * @post Stream content written to global logger
     * @post Stream destroyed
     */
    ~LogStream();
    
    /**
     * @brief Stream insertion operator for logging
     * 
     * Allows streaming multiple values to the log stream using
     * the `<<` operator, similar to std::cout.
     * 
     * @tparam T Type of value to stream
     * @param value Value to add to stream
     * @return Reference to this stream for further chaining
     * 
     * @post Value added to stream buffer
     * @return *this for method chaining
     */
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

/**
 * @brief Macro for stream-style logging
 * 
 * Convenience macro for creating log streams with source location
 * information. This macro captures file, line, and function
 * automatically when logging.
 * 
 * @param level Log severity level
 * 
 * @note Use like: NLM_LOG_STREAM(nlm::LogLevel::Debug) << "message" << value;
 * @see LogStream - Stream logging class
 * @see NLM_LOG_DEBUG - For debug-level logging
 */
#define NLM_LOG_STREAM(level) nlm::LogStream(level, __FILE__, __LINE__, __func__)

/**
 * @brief Convenience macro for debug-level logging
 * 
 * Macro for logging debug messages with automatic source location
 * information. More efficient than calling Logger::debug() directly.
 * 
 * @param msg Message to log
 * 
 * @note Captures __FILE__, __LINE__, and __func__ automatically
 * @see NLM_LOG_STREAM - For stream-style logging
 * @see Logger::debug() - Function call version
 */
#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)

/**
 * @brief Convenience macro for info-level logging
 * 
 * Macro for logging info messages with automatic source location
 * information.
 * 
 * @param msg Message to log
 * 
 * @see NLM_LOG_STREAM - For stream-style logging
 * @see Logger::info() - Function call version
 */
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__)

/**
 * @brief Convenience macro for warning-level logging
 * 
 * Macro for logging warning messages with automatic source location
 * information.
 * 
 * @param msg Message to log
 * 
 * @see NLM_LOG_STREAM - For stream-style logging
 * @see Logger::warning() - Function call version
 */
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)

/**
 * @brief Convenience macro for error-level logging
 * 
 * Macro for logging error messages with automatic source location
 * information.
 * 
 * @param msg Message to log
 * 
 * @see NLM_LOG_STREAM - For stream-style logging
 * @see Logger::error() - Function call version
 */
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__)

/**
 * @brief Convenience macro for critical-level logging
 * 
 * Macro for logging critical messages with automatic source location
 * information.
 * 
 * @param msg Message to log
 * 
 * @see NLM_LOG_STREAM - For stream-style logging
 * @see Logger::critical() - Function call version
 */
#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)

} // namespace nlm
