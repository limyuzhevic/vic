#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>

namespace nlm {

// Log severity levels
/**
 * @enum LogLevel
 * @brief Enumeration of log severity levels for filtering messages
 *
 * These levels determine which log messages are displayed or stored based on their importance
 *
 * @var LogLevel::Debug     Most detailed messages for debugging purposes
 * @var LogLevel::Info      General information messages
 * @var LogLevel::Warning   Warning messages indicating potential issues
 * @var LogLevel::Error     Error messages indicating failures or errors
 * @var LogLevel::Critical  Critical messages indicating system-critical failures
 */
enum class LogLevel {
    Debug = 0,     ///< Detailed debugging information
    Info = 1,       ///< General informational messages
    Warning = 2,    ///< Warning messages
    Error = 3,      ///< Error messages
    Critical = 4    ///< Critical system failure messages
};

// Log entry structure
/**
 * @struct LogEntry
 * @brief Container for log message information
 *
 * Holds all metadata associated with a log message including timestamp, severity,
 * file location, and the message itself
 */
struct LogEntry {
    LogLevel level;                      ///< Severity level of the log message
    std::string message;                  ///< The actual log message content
    std::string file;                     ///< Source file where log was generated
    int line;                             ///< Line number in source file
    std::string function;                 ///< Function name where log was generated
    std::chrono::system_clock::time_point timestamp; ///< Time when log was created
};

// Logger interface for logging systems
/**
 * @class ILogger
 * @brief Abstract interface for all logger implementations
 *
 * Defines the contract for logging implementations. Concrete loggers must implement
 * all pure virtual methods to provide specific logging functionality.
 *
 * @note This interface uses the Bridge pattern, allowing different logging backends
 * (console, file, network) to be plugged in at runtime.
 */
class ILogger {
public:
    virtual ~ILogger() = default;
    
    /**
     * @brief Log a message entry
     *
     * Processes and outputs/stores the log entry according to the logger's implementation
     *
     * @param entry The log entry containing message and metadata
     */
    virtual void log(const LogEntry& entry) = 0;
    
    /**
     * @brief Set the minimum log level for this logger
     *
     * Messages below this level will be filtered out and not processed
     *
     * @param level The minimum LogLevel to accept (messages below this level are ignored)
     */
    virtual void setLevel(LogLevel level) = 0;
    
    /**
     * @brief Get the current minimum log level
     *
     * @return The current minimum LogLevel
     */
    virtual LogLevel getLevel() const = 0;
};

// Console logger implementation
/**
 * @class ConsoleLogger
 * @brief Logger implementation that outputs to console with colors
 *
 * Provides thread-safe console logging with optional color coding based on log level.
 * Can also write to a file in addition to console output.
 *
 * @note Uses pImpl pattern for implementation hiding and thread safety
 */
class ConsoleLogger : public ILogger {
public:
    /**
     * @brief Default constructor with default log level
     *
     * Creates a logger that outputs to console with default log level (Info)
     */
    ConsoleLogger();
    
    /**
     * @brief Constructor with specified log level
     *
     * @param level Initial log level for filtering messages
     */
    explicit ConsoleLogger(LogLevel level);
    
    /**
     * @brief Destructor
     */
    ~ConsoleLogger() override;
    
    /**
     * @brief Log a message entry to console
     *
     * Processes and outputs the log entry to console with optional color coding
     *
     * @param entry The log entry to log
     */
    void log(const LogEntry& entry) override;
    
    /**
     * @brief Set the minimum log level for this logger
     *
     * @param level The minimum LogLevel to accept
     */
    void setLevel(LogLevel level) override;
    
    /**
     * @brief Get the current minimum log level
     *
     * @return The current minimum LogLevel
     */
    LogLevel getLevel() const override;
    
    /**
     * @brief Enable or disable color output
     *
     * @param useColors True to enable color coding in console output
     */
    void setUseColors(bool useColors);
    
    /**
     * @brief Check if colors are enabled
     *
     * @return True if color output is enabled
     */
    bool getUseColors() const;
    
    /**
     * @brief Set output file for log messages
     *
     * Enables logging to both console and the specified file
     *
     * @param filepath Path to the output file (appends to file)
     */
    void setOutputFile(const std::string& filepath);
    
    /**
     * @brief Close the output file
     *
     * Stops logging to the file and closes it. Console logging continues.
     */
    void closeOutputFile();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    /**
     * @brief Convert log level to human-readable string
     *
     * @param level The LogLevel to convert
     * @return String representation of the log level
     */
    std::string levelToString(LogLevel level) const;
    
    /**
     * @brief Get color code for a log level
     *
     * @param level The LogLevel to get color for
     * @return ANSI color code string or empty if no color
     */
    std::string levelToColor(LogLevel level) const;
};

// Main Logger class with multiple logger sinks
/**
 * @class Logger
 * @brief Thread-safe logger with multiple output sinks and global configuration
 *
 * This is the primary logging interface for the NLM system. It implements the
 * Logger pattern with support for multiple logger backends, thread-safe operations,
 * and global configuration.
 *
 * @note Uses singleton pattern for global access via getGlobal()
 * @warning This class should only be used through the global instance or by
 *          specifically adding loggers via addLogger()
 */
class Logger {
public:
    /**
     * @brief Default constructor
     *
     * Creates a logger with default configuration. Typically, users should use
     * the global logger instance via Logger::getGlobal() instead.
     */
    Logger();
    
    /**
     * @brief Destructor
     */
    ~Logger();
    
    /**
     * @brief Disable copying (Rule of Five)
     *
     * Logger objects should not be copied. Use the global instance instead.
     */
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    /**
     * @brief Add a logger sink to the system
     *
     * Appends a new logger to the collection of active loggers. The new logger
     * will receive all log messages (subject to its own level filtering).
     *
     * @param logger Shared pointer to the logger implementation to add
     */
    void addLogger(std::shared_ptr<ILogger> logger);
    
    /**
     * @brief Remove all logger sinks
     *
     * Clears all registered logger sinks, disabling all logging output.
     * Note: This does not affect the global logger configuration.
     */
    void clearLoggers();
    
    /**
     * @brief Set minimum log level for all loggers
     *
     * Sets the minimum severity level that will be processed by all loggers.
     * Messages below this level are filtered out before reaching individual loggers.
     *
     * @param level The minimum LogLevel to accept globally
     */
    void setLevel(LogLevel level);
    
    /**
     * @brief Log a message with full metadata
     *
     * Core logging function that processes a message at the specified level
     * with file, line, and function information. Routes to all registered loggers.
     *
     * @param level Log severity level
     * @param message The log message content
     * @param file Source file (defaults to empty string)
     * @param line Line number in source file (defaults to 0)
     * @param function Function name (defaults to empty string)
     */
    void log(LogLevel level, const std::string& message, 
             const std::string& file = "", int line = 0, 
             const std::string& function = "");
    
    /**
     * @brief Convenience method for Debug level logging
     *
     * @param message The debug message
     * @param file Source file (defaults to __FILE__)
     * @param line Line number (defaults to __LINE__)
     * @param function Function name (defaults to __func__)
     */
    void debug(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    /**
     * @brief Convenience method for Info level logging
     *
     * @param message The info message
     * @param file Source file (defaults to __FILE__)
     * @param line Line number (defaults to __LINE__)
     * @param function Function name (defaults to __func__)
     */
    void info(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    /**
     * @brief Convenience method for Warning level logging
     *
     * @param message The warning message
     * @param file Source file (defaults to __FILE__)
     * @param line Line number (defaults to __LINE__)
     * @param function Function name (defaults to __func__)
     */
    void warning(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    /**
     * @brief Convenience method for Error level logging
     *
     * @param message The error message
     * @param file Source file (defaults to __FILE__)
     * @param line Line number (defaults to __LINE__)
     * @param function Function name (defaults to __func__)
     */
    void error(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    /**
     * @brief Convenience method for Critical level logging
     *
     * @param message The critical message
     * @param file Source file (defaults to __FILE__)
     * @param line Line number (defaults to __LINE__)
     * @param function Function name (defaults to __func__)
     */
    void critical(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    /**
     * @brief Check if a log level is enabled
     *
     * Determines whether messages at the specified level will be processed
     * based on the current logging configuration.
     *
     * @param level The LogLevel to check
     * @return True if messages at this level will be logged, false otherwise
     */
    bool isEnabled(LogLevel level) const;
    
    /**
     * @brief Get the global logger instance (Singleton)
     *
     * @return Reference to the global Logger instance
     */
    static Logger& getGlobal();
    
    /**
     * @brief Set the global logger instance
     *
     * Replaces the default global logger with a custom implementation.
     * Use with caution as this affects all code using Logger::getGlobal().
     *
     * @param logger Shared pointer to the new global logger
     */
    static void setGlobal(std::shared_ptr<Logger> logger);
    
private:
    struct Impl;
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
