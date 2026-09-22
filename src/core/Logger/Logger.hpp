#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>

namespace nlm {

/**
 * @brief NLM Logging Framework
 * 
 * The logging framework provides a flexible, type-safe logging system for the
 * NLM (Neural Learning Machine) brain simulation. It supports multiple log levels,
 * multiple output destinations (console, files), color-coded output, and thread-safe
 * operations.
 * 
 * Key features:
 * - Six log levels: Debug, Info, Warning, Error, Critical
 * - Multiple logger sinks (ConsoleLogger, FileLogger, etc.)
 * - Thread-safe logging operations
 * - Color-coded terminal output
 * - Structured log entries with file/line/function location
 * - Global logger singleton for easy use
 * - Stream-style logging macros
 */

/**
 * @brief Enumeration of log severity levels
 * 
 * Log levels determine which messages are actually logged, based on the current
 * minimum log level of active loggers. Higher severity levels are:
 * Critical > Error > Warning > Info > Debug.
 */
enum class LogLevel {
    Debug = 0,      ///< Detailed debugging information (verbose)
    Info = 1,       ///< General informational messages
    Warning = 2,    ///< Warnings about potential issues
    Error = 3,      ///< Errors that prevent normal operation
    Critical = 4    ///< Critical errors that may cause system failure
};

/**
 * @brief Structure for log entries
 * 
 * Contains all information about a single log message, including the message
 * content, severity level, and source location (file, line, function) for
 * easy debugging.
 */
struct LogEntry {
    LogLevel level;                                    ///< Severity level of the message
    std::string message;                                ///< Log message content
    std::string file;                                   ///< Source file where log was created
    int line;                                           ///< Line number in source file
    std::string function;                               ///< Function name where log was created
    std::chrono::system_clock::time_point timestamp;    ///< Time when log was created

    /**
     * @brief Default constructor
     */
    LogEntry() : level(LogLevel::Info), message(), file(), line(0), function(), timestamp() {}

    /**
     * @brief Parameterized constructor
     * 
     * @param lvl Severity level
     * @param msg Message content
     * @param f Source file
     * @param l Line number
     * @param func Function name
     */
    LogEntry(LogLevel lvl, const std::string& msg, const std::string& f = "", 
             int l = 0, const std::string& func = "")
        : level(lvl), message(msg), file(f), line(l), function(func), 
          timestamp(std::chrono::system_clock::now()) {}
};

/**
 * @brief Abstract interface for log message handlers
 * 
 * ILogger is the abstract base class for all logger implementations. It defines
 * the interface for handling log messages and managing log levels.
 * 
 * Implementations should be thread-safe when implementing the log() method.
 */
class ILogger {
public:
    virtual ~ILogger() = default;

    /**
     * @brief Process a log entry
     * 
     * Implementations should handle the log entry according to their specific
     * requirements (console output, file writing, network transmission, etc.).
     * 
     * @param entry Log entry to process
     */
    virtual void log(const LogEntry& entry) = 0;

    /**
     * @brief Set minimum log level for this logger
     * 
     * Messages with level lower than this will be ignored.
     * 
     * @param level Minimum log level
     */
    virtual void setLevel(LogLevel level) = 0;

    /**
     * @brief Get current minimum log level
     * 
     * @return Current minimum log level
     */
    virtual LogLevel getLevel() const = 0;

    /**
     * @brief Virtual equality operator
     * 
     * Allows ILogger objects to be compared in algorithms that require equality checks.
     * Default implementation compares dynamic type.
     */
    virtual bool operator==(const ILogger& other) const { return typeid(*this) == typeid(other); }
};

/**
 * @brief Console logger implementation
 * 
 * ConsoleLogger writes log messages to the standard output (console) with
 * optional color coding based on log level. It can also write to a file
 * in addition to the console.
 * 
 * The console logger is thread-safe and uses mutexes to prevent output
 * interleaving when multiple threads log simultaneously.
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
     * @brief Parameterized constructor
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
     * 
     * Writes the log entry to console (and optionally to file) with
     * appropriate formatting and color coding.
     * 
     * @param entry Log entry to process
     */
    void log(const LogEntry& entry) override;

    /**
     * @brief Set minimum log level
     * 
     * @param level Minimum log level
     */
    void setLevel(LogLevel level) override;

    /**
     * @brief Get current minimum log level
     * 
     * @return Current minimum log level
     */
    LogLevel getLevel() const override;

    /**
     * @brief Enable or disable color output
     * 
     * @param useColors Whether to use ANSI color codes in console output
     */
    void setUseColors(bool useColors);

    /**
     * @brief Check if colors are enabled
     * 
     * @return true if colors are enabled, false otherwise
     */
    bool getUseColors() const;

    /**
     * @brief Set output file path
     * 
     * Enables file output in addition to console output. Messages are
     * appended to the specified file.
     * 
     * @param filepath Path to the output file
     */
    void setOutputFile(const std::string& filepath);

    /**
     * @brief Close the output file
     * 
     * Closes the output file and removes the file output destination.
     */
    void closeOutputFile();

    /**
     * @brief Convert log level to string representation
     * 
     * @param level Log level to convert
     * @return String representation (e.g., "DEBUG", "INFO", etc.)
     */
    std::string levelToString(LogLevel level) const;

    /**
     * @brief Get ANSI color code for log level
     * 
     * Returns the ANSI escape sequence for coloring console output
     * based on log level. Only valid when colors are enabled.
     * 
     * @param level Log level
     * @return ANSI color code string
     */
    std::string levelToColor(LogLevel level) const;

    /**
     * @brief Check if file output is enabled
     * 
     * @return true if file output is enabled and file is open, false otherwise
     */
    bool hasFileOutput() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * @brief Main Logger class
 * 
 * Logger is the primary interface for logging in the NLM system. It manages
 * multiple ILogger sinks and provides a unified interface for logging messages
 * at different severity levels.
 * 
 * The Logger class uses the pImpl pattern and is thread-safe. It provides:
 * - Multiple logger sinks (ConsoleLogger, FileLogger, etc.)
 * - Global logger singleton for easy use
 * - Convenience methods for common log levels
 * - Stream-style logging macros
 * - Log level filtering
 * 
 * Thread safety: Logger operations are thread-safe. Multiple threads can call
 * logging methods simultaneously without causing data races.
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
     * @brief Move constructor
     * 
     * @param other Logger object to move from
     */
    Logger(Logger&&) noexcept;

    /**
     * @brief Move assignment operator
     * 
     * @param other Logger object to move from
     * @return Reference to this object
     */
    Logger& operator=(Logger&&) noexcept;

    /**
     * @brief Add a logger sink
     * 
     * Adds a new ILogger sink to the logging system. The logger will
     * receive all log messages that pass the level filter.
     * 
     * @param logger Shared pointer to logger sink to add
     */
    void addLogger(std::shared_ptr<ILogger> logger);

    /**
     * @brief Remove all logger sinks
     * 
     * Clears all logger sinks, disabling logging until new sinks are added.
     */
    void clearLoggers();

    /**
     * @brief Set minimum log level for all loggers
     * 
     * Sets the minimum log level for all registered logger sinks.
     * Messages with level lower than this will be ignored by all loggers.
     * 
     * @param level Minimum log level for all loggers
     */
    void setLevel(LogLevel level);

    /**
     * @brief Log a message with specified level
     * 
     * Creates a LogEntry with the specified level and processes it through
     * all registered logger sinks. File and function information are automatically
     * included if available.
     * 
     * @param level Log level
     * @param message Log message content
     * @param file Source file (defaults to empty string, will be filled by macros)
     * @param line Source line number (defaults to 0, will be filled by macros)
     * @param function Function name (defaults to empty string, will be filled by macros)
     */
    void log(LogLevel level, const std::string& message,
             const std::string& file = "", int line = 0,
             const std::string& function = "");

    /**
     * @brief Log debug-level message
     * 
     * Convenience method for logging debug messages. Debug messages are typically
     * used for detailed tracing information.
     * 
     * @param message Debug message content
     * @param file Source file (defaults to empty string)
     * @param line Source line number (defaults to 0)
     * @param function Function name (defaults to empty string)
     */
    void debug(const std::string& message, const std::string& file = "",
               int line = 0, const std::string& function = "");

    /**
     * @brief Log info-level message
     * 
     * Convenience method for logging informational messages. Info messages are
     * typically used for general status updates and progress reports.
     * 
     * @param message Info message content
     * @param file Source file (defaults to empty string)
     * @param line Source line number (defaults to 0)
     * @param function Function name (defaults to empty string)
     */
    void info(const std::string& message, const std::string& file = "",
              int line = 0, const std::string& function = "");

    /**
     * @brief Log warning-level message
     * 
     * Convenience method for logging warning messages. Warning messages are
     * typically used for potential issues that don't prevent normal operation.
     * 
     * @param message Warning message content
     * @param file Source file (defaults to empty string)
     * @param line Source line number (defaults to 0)
     * @param function Function name (defaults to empty string)
     */
    void warning(const std::string& message, const std::string& file = "",
                 int line = 0, const std::string& function = "");

    /**
     * @brief Log error-level message
     * 
     * Convenience method for logging error messages. Error messages are
     * typically used for errors that prevent normal operation.
     * 
     * @param message Error message content
     * @param file Source file (defaults to empty string)
     * @param line Source line number (defaults to 0)
     * @param function Function name (defaults to empty string)
     */
    void error(const std::string& message, const std::string& file = "",
               int line = 0, const std::string& function = "");

    /**
     * @brief Log critical-level message
     * 
     * Convenience method for logging critical messages. Critical messages are
     * typically used for critical errors that may cause system failure.
     * 
     * @param message Critical message content
     * @param file Source file (defaults to empty string)
     * @param line Source line number (defaults to 0)
     * @param function Function name (defaults to empty string)
     */
    void critical(const std::string& message, const std::string& file = "",
                  int line = 0, const std::string& function = "");

    /**
     * @brief Check if log level is enabled
     * 
     * Checks if any registered logger has a minimum level that allows
     * the specified level to be logged.
     * 
     * @param level Log level to check
     * @return true if level is enabled (will be logged), false otherwise
     */
    bool isEnabled(LogLevel level) const;

    /**
     * @brief Get global logger instance
     * 
     * @return Reference to the global logger singleton
     */
    static Logger& getGlobal();

    /**
     * @brief Set global logger instance
     * 
     * Sets the global logger instance. Note: This method is primarily
     * for advanced use cases. Most applications should use getGlobal()
     * to access the logger.
     * 
     * @param logger Shared pointer to logger instance to set as global
     */
    static void setGlobal(std::shared_ptr<Logger> logger);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * @brief Convenience macro for debug-level logging
 * 
 * Use NLM_LOG_DEBUG(msg) to log a debug message. The macro automatically
 * includes file, line, and function information.
 * 
 * Example:
 * @code
 * NLM_LOG_DEBUG("Processing neuron " + std::to_string(neuronId));
 * @endcode
 */
#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)

/**
 * @brief Convenience macro for info-level logging
 * 
 * Use NLM_LOG_INFO(msg) to log an info message. The macro automatically
 * includes file, line, and function information.
 * 
 * Example:
 * @code
 * NLM_LOG_INFO("Neuron " + std::to_string(neuronId) + " fired");
 * @endcode
 */
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__)

/**
 * @brief Convenience macro for warning-level logging
 * 
 * Use NLM_LOG_WARNING(msg) to log a warning message. The macro automatically
 * includes file, line, and function information.
 * 
 * Example:
 * @code
 * NLM_LOG_WARNING("High memory usage detected");
 * @endcode
 */
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)

/**
 * @brief Convenience macro for error-level logging
 * 
 * Use NLM_LOG_ERROR(msg) to log an error message. The macro automatically
 * includes file, line, and function information.
 * 
 * Example:
 * @code
 * NLM_LOG_ERROR("Failed to initialize brain: " + errorMessage);
 * @endcode
 */
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__)

/**
 * @brief Convenience macro for critical-level logging
 * 
 * Use NLM_LOG_CRITICAL(msg) to log a critical message. The macro automatically
 * includes file, line, and function information.
 * 
 * Example:
 * @code
 * NLM_LOG_CRITICAL("System failure - attempting recovery");
 * @endcode
 */
#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)

/**
 * @brief Stream-style logging helper class
 * 
 * LogStream allows for streaming log messages similar to std::cout, providing
 * a convenient way to log formatted messages:
 * 
 * Example:
 * @code
 * NLM_LOG_STREAM(nlm::LogLevel::Info) << "Neuron " << neuronId << " fired with rate " << rate;
 * @endcode
 * 
 * The LogStream class is created by the NLM_LOG_STREAM macro and automatically
 * logs the message when destroyed.
 */
class LogStream {
public:
    /**
     * @brief Constructor
     * 
     * @param level Log level for the message
     * @param file Source file
     * @param line Source line number
     * @param function Function name
     */
    LogStream(LogLevel level, const std::string& file, int line, const std::string& function);

    /**
     * @brief Destructor
     * 
     * Logs the accumulated message when the stream is destroyed.
     */
    ~LogStream();

    /**
     * @brief Stream output operator
     * 
     * Adds values to the log message stream.
     * 
     * @tparam T Type of value to add
     * @param value Value to add to the message
     * @return Reference to this LogStream for chaining
     */
    template<typename T>
    LogStream& operator<<(const T& value) {
        oss_ << value;
        return *this;
    }

private:
    LogLevel level_;      ///< Log level for the message
    std::ostringstream oss_; ///< String stream for accumulating message
    std::string file_;    ///< Source file
    int line_;           ///< Source line number
    std::string function_; ///< Function name
};

/**
 * @brief Convenience macro for stream-style logging
 * 
 * Use NLM_LOG_STREAM(level) to create a stream-style log message.
 * 
 * Example:
 * @code
 * NLM_LOG_STREAM(nlm::LogLevel::Info) << "Neuron " << neuronId << " processed";
 * @endcode
 */
#define NLM_LOG_STREAM(level) nlm::LogStream(level, __FILE__, __LINE__, __func__)

} // namespace nlm

#endif // NLM_LOGGER_HPP
