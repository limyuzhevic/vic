/**
 * @file Logger.hpp
 * @brief Multi-level logging system for the NLM framework
 * 
 * This header provides a comprehensive logging system with multiple severity levels,
 * flexible sinks, and thread-safe operation. The logging system is essential for
 * debugging, monitoring, and runtime diagnostics of neural network simulations.
 * 
 * @details The logging system features:
 * - Five severity levels: Debug, Info, Warning, Error, and Critical
 * - Multiple logger sinks (console, file, custom implementations)
 * - Global and per-logger level control
 * - File and function/line number context for log entries
 * - Stream-style and convenience macros for easy logging
 * - Thread-safe operation for concurrent simulation environments
 * 
 * @note The logging system is used throughout the NLM framework for:
 * - Debug output during development
 * - Simulation progress monitoring
 * - Error reporting and troubleshooting
 * - Performance logging
 * - Configuration validation messages
 * 
 * @author NLM Authors
 * @date Created: 2024
 * @version 1.0
 * 
 * @copyright MIT License
 * 
 * @warning Logs may contain sensitive information. Configure logging levels
 * appropriately in production environments.
 * 
 * @see ILogger for abstract logger interface
 * @see ConsoleLogger for console-specific implementation
 * @see LogEntry for log entry structure
 * @see LogLevel for severity levels
 * @see NLM_LOG_* macros for convenience logging
 * 
 * @ingroup CoreComponents
 */
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
 * @brief Defines the severity levels for log entries
 * 
 * LogLevel enumerates the different severity levels used in the logging system,
 * allowing for filtering of log output based on importance. The levels follow
 * a hierarchy where lower values indicate more verbose output.
 * 
 * @note The severity order (from most verbose to least verbose) is:
 * Debug < Info < Warning < Error < Critical
 * 
 * This order determines the default filtering behavior - if the current log level
 * is set to Warning, only Warning, Error, and Critical messages will be logged.
 * 
 * @see LogEntry for log entry structure
 * @see ILogger for logger interface
 * @see Logger for main logger implementation
 */
enum class LogLevel {
    Debug = 0,     ///< Detailed debug information for development
    Info = 1,       ///< General information about simulation progress
    Warning = 2,    ///< Warnings about potential issues or suboptimal behavior
    Error = 3,      ///< Error conditions that affect simulation operation
    Critical = 4    ///< Critical errors that terminate simulation
};

/**
 * @struct LogEntry
 * @brief Represents a single log message with context information
 * 
 * LogEntry encapsulates a log message with associated metadata including
 * severity level, timestamp, and source code location. This structure provides
 * comprehensive context for log entries, making debugging and post-mortem
 * analysis easier.
 * 
 * @details Each log entry contains:
 * - Severity level for filtering and prioritization
 * - The actual message text
 * - File name where the log was generated
 * - Line number in the source file
 * - Function name where the log was generated
 * - Precise timestamp of log generation
 * 
 * LogEntry objects are passed to ILogger implementations for output formatting
 * and storage. The source code location information helps identify the exact
 * location of log generation within the codebase.
 * 
 * @note LogEntry is immutable after creation to ensure thread safety during
 * logging operations. Implementations may convert this to their internal
 * log format as needed.
 * 
 * @see LogLevel for severity enumeration
 * @see ILogger for logger interface
 * @see ConsoleLogger for output formatting
 * 
 * @warning File, line, and function names may contain sensitive information
 * in production environments. Consider filtering these in production.
 */
struct LogEntry {
    LogLevel level;                                   ///< Severity level of the log message
    std::string message;                              ///< Log message text
    std::string file;                                 ///< Source file name (can be empty)
    int line;                                          ///< Line number in source file
    std::string function;                             ///< Function name where log was generated
    std::chrono::system_clock::time_point timestamp;   ///< Timestamp when log was created
};

/**
 * @class ILogger
 * @brief Abstract interface for logger implementations
 * 
 * ILogger defines the interface that all logger implementations must satisfy.
 * This interface provides the core functionality for log entry processing,
 * level management, and output control. Multiple ILogger implementations can
 * be combined to create flexible logging configurations.
 * 
 * @details The ILogger interface supports:
 * - Virtual log entry processing
 * - Level-based filtering
 * - Thread-safe operation (implementation-defined)
 * - Configuration management
 * - Error handling for logging failures
 * 
 * ILogger uses the Bridge pattern to allow different output mechanisms
 * (console, file, network, database) while maintaining a consistent interface.
 * This enables runtime switching of logging destinations without changing
 * application code.
 * 
 * @note ILogger implementations should be thread-safe as logging may occur
 * concurrently from multiple simulation threads. The interface uses virtual
 * functions for extensibility and polymorphism.
 * 
 * @see ConsoleLogger for concrete console logger implementation
 * @see LogEntry for log entry structure
 * @see LogLevel for severity levels
 * @see Logger for main logger manager class
 * 
 * @warning Log implementations should handle exceptions gracefully and
 * continue operating even if individual log messages fail.
 * 
 * @ingroup CoreComponents
 */
class ILogger {
public:
    virtual ~ILogger() = default;
    
    /**
     * @brief Process a log entry
     * 
     * This pure virtual function must be implemented by all ILogger subclasses.
     * It receives a LogEntry object and is responsible for outputting or storing
     * the log message according to the logger's configuration.
     * 
     * @param entry Log entry to process
     * 
     * @note Implementations should handle thread safety, formatting, and
     * output destinations. This function may throw exceptions if logging
     * fails catastrophically.
     * 
     * @see LogEntry for log entry structure
     * @see setLevel() for level filtering
     */
    virtual void log(const LogEntry& entry) = 0;
    
    /**
     * @brief Set the minimum log level for this logger
     * 
     * Configures the minimum severity level that this logger will process.
     * Log entries with level below this threshold are silently discarded.
     * 
     * @param level Minimum severity level to process
     * 
     * @note This is a filtering operation and does not affect the logger's
     * configuration or output destinations. It only determines which messages
     * are passed to the output mechanism.
     * 
     * @see getLevel() for retrieving current level
     * @see LogLevel for severity levels
     * 
     * @par Example
     * @code
     * nlm::ConsoleLogger logger;
     * logger.setLevel(nlm::LogLevel::Warning);
     * // Now only Warning, Error, and Critical messages will be logged
     * @endcode
     */
    virtual void setLevel(LogLevel level) = 0;
    
    /**
     * @brief Get the current minimum log level
     * 
     * Retrieves the minimum severity level configured for this logger.
     * 
     * @return LogLevel Current minimum log level
     * 
     * @see setLevel() for setting the level
     * @see LogLevel for severity levels
     */
    virtual LogLevel getLevel() const = 0;
};

/**
 * @class ConsoleLogger
 * @brief Console-based logger implementation
 * 
 * ConsoleLogger provides a concrete implementation of ILogger that outputs
 * log messages to the standard output stream. It supports colored output,
 * multiple log levels, and additional output destinations like files.
 * 
 * @details ConsoleLogger features:
 * - Color-coded output based on log severity levels
 * - Optional file output in addition to console
 * - Configurable log level filtering
 * - Thread-safe operation with mutex protection
 * - Performance optimizations for different log levels
 * - Timestamp and source location formatting
 * 
 * The logger formats output with severity level, timestamp, source file,
 * line number, and function name. Colors are used to distinguish log levels
 * when output is directed to a terminal.
 * 
 * @note ConsoleLogger is the most commonly used logger in NLM applications.
 * It provides a good balance of functionality, performance, and ease of use.
 * For specialized logging needs (e.g., network, database), implement
 * custom ILogger subclasses.
 * 
 * @see ILogger for abstract interface
 * @see LogLevel for severity levels
 * @see LogEntry for log entry structure
 * @see Logger for logger manager
 * 
 * @warning File output may consume significant disk space. Monitor log file
 * sizes in long-running simulations.
 * 
 * @ingroup CoreComponents
 */
class ConsoleLogger : public ILogger {
public:
    /**
     * @brief Construct a console logger with default settings
     * 
     * Creates a ConsoleLogger with default configuration:
     * - Log level: Info
     * - Colors: Enabled (if output is a terminal)
     * - File output: Disabled
     * 
     * @see ConsoleLogger(LogLevel level) for level-specific constructor
     */
    ConsoleLogger();
    
    /**
     * @brief Construct a console logger with specified log level
     * 
     * Creates a ConsoleLogger with the specified minimum log level.
     * Other settings use defaults (colors enabled, file output disabled).
     * 
     * @param level Initial minimum log level
     * 
     * @see ConsoleLogger() for default constructor
     * @see setLevel() for changing level later
     */
    explicit ConsoleLogger(LogLevel level);
    
    /**
     * @brief Destructor
     * 
     * Properly closes any open file output and cleans up resources.
     */
    ~ConsoleLogger() override;
    
    /**
     * @brief Process a log entry (override)
     * 
     * Implements ILogger::log() by formatting and outputting the log entry
     * to both console and optionally to file.
     * 
     * @param entry Log entry to process
     * 
     * @throws std::runtime_error if output stream operations fail
     * 
     * @see ILogger::log()
     * @see log(const LogEntry& entry) for implementation details
     */
    void log(const LogEntry& entry) override;
    
    /**
     * @brief Set the minimum log level (override)
     * 
     * Configures the minimum severity level for this logger.
     * 
     * @param level Minimum severity level to process
     * 
     * @see ILogger::setLevel()
     * @see getLevel() for retrieving current level
     */
    void setLevel(LogLevel level) override;
    
    /**
     * @brief Get the current minimum log level (override)
     * 
     * @return LogLevel Current minimum log level
     * 
     * @see ILogger::getLevel()
     * @see setLevel() for setting the level
     */
    LogLevel getLevel() const override;
    
    /**
     * @brief Enable or disable colored output
     * 
     * Configures whether log messages should be colored based on severity
     * level. Colors are only applied when output is directed to a terminal
     * (isatty() check).
     * 
     * @param useColors true to enable colored output, false to disable
     * 
     * @note Colors improve readability but may cause issues in certain output
     * environments (e.g., log files, redirected output).
     * 
     * @see getUseColors() for querying current setting
     * 
     * @par Example
     * @code
     * nlm::ConsoleLogger logger;
     * logger.setUseColors(true); // Enable colors
     * // Log messages will now have color codes
     * @endcode
     */
    void setUseColors(bool useColors);
    
    /**
     * @brief Check if colors are enabled
     * 
     * @return true if colors are enabled, false otherwise
     * 
     * @see setUseColors() for setting the setting
     */
    bool getUseColors() const;
    
    /**
     * @brief Set additional output file for log messages
     * 
     * Enables file output in addition to console output. All log messages
     * are written to both the console and the specified file. The file is
     * opened in append mode.
     * 
     * @param filepath Path to the file for log output
     * 
     * @throws std::runtime_error if file cannot be opened
     * 
     * @note Multiple calls to setOutputFile() will replace the previous
     * file output. Use closeOutputFile() to disable file output.
     * 
     * @see setOutputFile() for setting file output
     * @see closeOutputFile() for closing file output
     * @see getUseColors() for querying color setting
     * 
     * @par Example
     * @code
     * nlm::ConsoleLogger logger;
     * logger.setOutputFile("simulation.log");
     * logger.setUseColors(false); // Disable colors in log file
     * // All log messages now go to both console and file
     * @endcode
     */
    void setOutputFile(const std::string& filepath);
    
    /**
     * @brief Close the output file
     * 
     * Closes the file output stream and disables file logging. Console logging
     * continues as configured.
     * 
     * @note This operation is safe to call even if no file is currently open.
     * 
     * @see setOutputFile() for opening a log file
     * 
     * @par Example
     * @code
     * nlm::ConsoleLogger logger;
     * logger.setOutputFile("simulation.log");
     * // ... simulation runs ...
     * logger.closeOutputFile(); // Disable file output
     * @endcode
     */
    void closeOutputFile();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    /**
     * @brief Convert log level to string for display
     * 
     * @param level Log level to convert
     * @return std::string String representation of the level
     * 
     * @note Used for formatting log entries in console output.
     */
    std::string levelToString(LogLevel level) const;
    
    /**
     * @brief Convert log level to color code
     * 
     * @param level Log level to convert
     * @return std::string ANSI color code for the level
     * 
     * @note Returns empty string if colors are disabled or if the level
     * has no color mapping. Used for colored console output.
     */
    std::string levelToColor(LogLevel level) const;
};

/**
 * @class Logger
 * @brief Main logger manager class
 * 
 * Logger provides a comprehensive logging system that manages multiple
 * ILogger implementations (sinks). It serves as the central logging interface
 * for the NLM framework, providing both global and instance-level logging
 * capabilities.
 * 
 * @details Logger features:
 * - Multiple logger sinks for output diversity
 * - Global singleton instance for framework-wide logging
 * - Thread-safe operation for concurrent simulation
 * - Convenience methods for all log levels
 * - Stream-style logging with NLM_LOG_STREAM macro
 * - Global logging functions with file/line/function context
 * 
 * The Logger class uses the Composite pattern to manage multiple ILogger
 * implementations. Each logger can have its own level filtering, and the
 * main Logger provides additional convenience methods and macros for easy
 * logging throughout the codebase.
 * 
 * @note Logger is designed to be both simple to use and powerful enough for
 * complex logging requirements. The global instance (Logger::getGlobal())
 * provides framework-wide logging with minimal setup.
 * 
 * @see ILogger for abstract logger interface
 * @see ConsoleLogger for console output implementation
 * @see LogEntry for log entry structure
 * @see LogLevel for severity levels
 * @see NLM_LOG_* macros for convenience logging
 * 
 * @warning Global logger state is shared across all components. Changes to
 * the global logger affect all logging throughout the framework.
 * 
 * @ingroup CoreComponents
 */
class Logger {
public:
    Logger();
    ~Logger();
    
    /**
     * @brief Copy constructor is deleted
     */
    Logger(const Logger&) = delete;
    
    /**
     * @brief Copy assignment operator is deleted
     */
    Logger& operator=(const Logger&) = delete;
    
    /**
     * @brief Add a logger sink
     * 
     * Adds an ILogger implementation to this logger's composite sink list.
     * The added logger will receive all log messages processed by this Logger.
     * 
     * @param logger Shared pointer to ILogger implementation to add
     * 
     * @note Added loggers retain their own level filtering. Messages pass through
     * all configured loggers regardless of individual settings.
     * 
     * @see addLogger() for adding with move semantics
     * @see clearLoggers() for removing all loggers
     * 
     * @par Example
     * @code
     * nlm::ConsoleLogger consoleLogger;
     * nlm::Logger logger;
     * logger.addLogger(std::make_shared<nlm::ConsoleLogger>(consoleLogger));
     * // Now consoleLogger's messages go through the main logger
     * @endcode
     */
    void addLogger(std::shared_ptr<ILogger> logger);
    
    /**
     * @brief Remove all logger sinks
     * 
     * Removes all ILogger implementations from this logger's composite sink list.
     * After this operation, the logger will not produce any output until
     * additional loggers are added.
     * 
     * @note This is useful for temporarily disabling logging or during testing
     * when you want to verify code paths without log output.
     * 
     * @see addLogger() for adding loggers
     * 
     * @par Example
     * @code
     * nlm::Logger logger;
     * // ... add some loggers ...
     * logger.clearLoggers(); // Disable all logging
     * @endcode
     */
    void clearLoggers();
    
    /**
     * @brief Set minimum log level for all loggers
     * 
     * Configures the minimum severity level for all logger sinks. Log entries
     * with level below this threshold are filtered out before being passed to
     * individual loggers.
     * 
     * @param level Minimum severity level to process
     * 
     * @note This provides a global filtering mechanism. Individual loggers can
     * have their own level settings that are applied after this global filter.
     * 
     * @see setLevel() for convenience method
     * @see isEnabled() for checking if level is enabled
     
     * @par Example
     * @code
     * nlm::Logger logger;
     * logger.setLevel(nlm::LogLevel::Warning);
     * // Now only Warning, Error, and Critical messages are processed
     * @endcode
     */
    void setLevel(LogLevel level);
    
    /**
     * @brief Global logging function
     * 
     * Logs a message with specified severity level. This function provides
     * source code location information using the current __FILE__, __LINE__,
     * and __func__ macros, making it easy to log with proper context.
     * 
     * @param level Log severity level
     * @param message Log message text
     * @param file Source file name (default: "")
     * @param line Line number in source file (default: 0)
     * @param function Function name where log was generated (default: "")
     * 
     * @throws std::runtime_error if no loggers are configured
     * 
     * @note This function is thread-safe and can be called from any context.
     * The source parameters default to the macros that expand to the current
     * file, line, and function, providing automatic location information.
     * 
     * @see debug() for Debug level convenience
     * @see info() for Info level convenience
     * @see warning() for Warning level convenience
     * @see error() for Error level convenience
     * @see critical() for Critical level convenience
     * @see isEnabled() for checking if level is enabled
     
     * @par Example
     * @code
     * void myFunction() {
     *     nlm::Logger::getGlobal().log(
     *         nlm::LogLevel::Info,
     *         "Starting neural network simulation",
     *         __FILE__, __LINE__, __func__
     *     );
     * }
     * @endcode
     */
    void log(LogLevel level, const std::string& message,
             const std::string& file = "", int line = 0,
             const std::string& function = "");
    
    /**
     * @brief Debug level logging convenience method
     * 
     * Logs a debug-level message with automatic source code context. Debug
     * messages are typically used during development and debugging.
     * 
     * @param message Log message text
     * @param file Source file name (default: "")
     * @param line Line number in source file (default: 0)
     * @param function Function name where log was generated (default: "")
     * 
     * @see log() for general logging function
     * @see NLM_LOG_DEBUG macro for stream-style debug logging
     * 
     * @par Example
     * @code
     * // Debug logging with automatic context
     * nlm::Logger::getGlobal().debug("Neuron initialized with ID: 42");
     * 
     * // Debug logging with manual context
     * nlm::Logger::getGlobal().debug(
     *     "Processing synaptic input",
     *     __FILE__, __LINE__, __func__
     * );
     * @endcode
     */
    void debug(const std::string& message,
               const std::string& file = "", int line = 0,
               const std::string& function = "");
    
    /**
     * @brief Info level logging convenience method
     * 
     * Logs an informational message with automatic source code context.
     * Info messages provide general information about simulation progress
     * and important events.
     * 
     * @param message Log message text
     * @param file Source file name (default: "")
     * @param line Line number in source file (default: 0)
     * @param function Function name where log was generated (default: "")
     * 
     * @see log() for general logging function
     * @see NLM_LOG_INFO macro for stream-style info logging
     * 
     * @par Example
     * @code
     * // Info logging
     * nlm::Logger::getGlobal().info("Simulation step completed");
     * @endcode
     */
    void info(const std::string& message,
              const std::string& file = "", int line = 0,
              const std::string& function = "");
    
    /**
     * @brief Warning level logging convenience method
     * 
     * Logs a warning message with automatic source code context. Warning
     * messages indicate potential issues or suboptimal behavior that doesn't
     * prevent operation.
     * 
     * @param message Log message text
     * @param file Source file name (default: "")
     * @param line Line number in source file (default: 0)
     * @param function Function name where log was generated (default: "")
     * 
     * @see log() for general logging function
     * @see NLM_LOG_WARNING macro for stream-style warning logging
     * 
     * @par Example
     * @code
     * // Warning logging
     * nlm::Logger::getGlobal().warning("High memory usage detected");
     * @endcode
     */
    void warning(const std::string& message,
                 const std::string& file = "", int line = 0,
                 const std::string& function = "");
    
    /**
     * @brief Error level logging convenience method
     * 
     * Logs an error message with automatic source code context. Error
     * messages indicate problems that affect simulation operation.
     * 
     * @param message Log message text
     * @param file Source file name (default: "")
     * @param line Line number in source file (default: 0)
     * @param function Function name where log was generated (default: "")
     * 
     * @see log() for general logging function
     * @see NLM_LOG_ERROR macro for stream-style error logging
     * 
     * @par Example
     * @code
     * // Error logging
     * nlm::Logger::getGlobal().error("Failed to load configuration file");
     * @endcode
     */
    void error(const std::string& message,
               const std::string& file = "", int line = 0,
               const std::string& function = "");
    
    /**
     * @brief Critical level logging convenience method
     * 
     * Logs a critical message with automatic source code context. Critical
     * messages indicate severe errors that typically terminate the simulation.
     * 
     * @param message Log message text
     * @param file Source file name (default: "")
     * @param line Line number in source file (default: 0)
     * @param function Function name where log was generated (default: "")
     * 
     * @see log() for general logging function
     * @see NLM_LOG_CRITICAL macro for stream-style critical logging
     * 
     * @par Example
     * @code
     * // Critical logging
     * nlm::Logger::getGlobal().critical("Simulation terminated due to error");
     * @endcode
     */
    void critical(const std::string& message,
                  const std::string& file = "", int line = 0,
                  const std::string& function = "");
    
    /**
     * @brief Check if a log level is enabled
     * 
     * Determines if messages of the specified severity level will be processed
     * by any configured logger sink. This considers both the global filter
     * and individual logger level settings.
     * 
     * @param level Log level to check
     * @return true if messages of this level will be logged, false otherwise
     * 
     * @note This function is useful for conditional logging to avoid the
     * overhead of formatting messages that won't be output.
     * 
     * @see setLevel() for setting the minimum level
     * @see LogLevel for severity levels
     * 
     * @par Example
     * @code
     * nlm::Logger logger;
     * if (logger.isEnabled(nlm::LogLevel::Debug)) {
     *     logger.debug("Expensive debug calculation");
     * }
     * @endcode
     */
    bool isEnabled(LogLevel level) const;
    
    /**
     * @brief Get the global logger instance
     * 
     * Returns the global singleton logger instance used throughout the NLM
     * framework. This provides a convenient way to access the main logging
     * system without creating multiple logger instances.
     * 
     * @return Logger& Reference to the global logger instance
     * 
     * @note The global logger is created on first access and persists for
     * the lifetime of the program. Changes to the global logger affect all
     * logging throughout the framework.
     * 
     * @see setGlobal() for replacing the global logger
     * 
     * @par Example
     * @code
     * // Using the global logger (most common usage)
     * nlm::Logger::getGlobal().info("Starting simulation");
     * 
     * // Custom logger for specific components
     * nlm::ConsoleLogger customLogger;
     * customLogger.setLevel(nlm::LogLevel::Warning);
     * 
     * nlm::Logger componentLogger;
     * componentLogger.addLogger(std::make_shared<nlm::ConsoleLogger>(customLogger));
     * @endcode
     */
    static Logger& getGlobal();
    
    /**
     * @brief Set the global logger instance
     * 
     * Replaces the current global logger instance with a new one. This allows
     * customizing the logging system for specific applications or testing
     * scenarios.
     * 
     * @param logger Shared pointer to the new global logger instance
     * 
     * @note This function is useful for:
     * - Testing with custom logger implementations
     * - Redirecting output to different destinations
     * - Applying application-specific logging configurations
     * 
     * @see getGlobal() for retrieving the global logger
     * 
     * @par Example
     * @code
     * nlm::ConsoleLogger logger;
     * logger.setLevel(nlm::LogLevel::Error);
     * 
     * nlm::Logger::setGlobal(std::make_shared<nlm::Logger>(logger));
     * // Now all framework logging uses this custom configuration
     * @endcode
     */
    static void setGlobal(std::shared_ptr<Logger> logger);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Convenience macro for file/line/function info
/**
 * @def NLM_LOG_DEBUG(msg)
 * @brief Macro for debug-level logging with automatic context
 * 
 * This macro expands to a call to the global logger's debug() method with
 * automatic file, line, and function information using the __FILE__, __LINE__,
 * and __func__ macros. This provides easy, consistent debug logging throughout
 * the codebase.
 * 
 * @param msg Log message text
 * 
 * @note The macro automatically includes source code location information,
 * making debugging easier. For performance-critical code, use the NLM_LOG_STREAM
 * macro or check isEnabled() before logging.
 * 
 * @see NLM_LOG_INFO Macro for info-level logging
 * @see NLM_LOG_WARNING Macro for warning-level logging
 * @see NLM_LOG_ERROR Macro for error-level logging
 * @see NLM_LOG_CRITICAL Macro for critical-level logging
 * @see NLM_LOG_STREAM Macro for stream-style logging
 * 
 * @par Example
 * @code
 * // Debug logging with macro
 * NLM_LOG_DEBUG("Entering neuron update step");
 * @endcode
 */
#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)

/**
 * @def NLM_LOG_INFO(msg)
 * @brief Macro for info-level logging with automatic context
 * 
 * This macro expands to a call to the global logger's info() method with
 * automatic file, line, and function information using the __FILE__, __LINE__,
 * and __func__ macros.
 * 
 * @param msg Log message text
 * 
 * @see NLM_LOG_DEBUG Macro for debug-level logging
 * @see NLM_LOG_WARNING Macro for warning-level logging
 * @see NLM_LOG_ERROR Macro for error-level logging
 * @see NLM_LOG_CRITICAL Macro for critical-level logging
 * @see NLM_LOG_STREAM Macro for stream-style logging
 */
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__)

/**
 * @def NLM_LOG_WARNING(msg)
 * @brief Macro for warning-level logging with automatic context
 * 
 * This macro expands to a call to the global logger's warning() method with
 * automatic file, line, and function information.
 * 
 * @param msg Log message text
 * 
 * @see NLM_LOG_DEBUG Macro for debug-level logging
 * @see NLM_LOG_INFO Macro for info-level logging
 * @see NLM_LOG_ERROR Macro for error-level logging
 * @see NLM_LOG_CRITICAL Macro for critical-level logging
 * @see NLM_LOG_STREAM Macro for stream-style logging
 */
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)

/**
 * @def NLM_LOG_ERROR(msg)
 * @brief Macro for error-level logging with automatic context
 * 
 * This macro expands to a call to the global logger's error() method with
 * automatic file, line, and function information.
 * 
 * @param msg Log message text
 * 
 * @see NLM_LOG_DEBUG Macro for debug-level logging
 * @include NLM_LOG_INFO Macro for info-level logging
 * @see NLM_LOG_WARNING Macro for warning-level logging
 * @see NLM_LOG_CRITICAL Macro for critical-level logging
 * @see NLM_LOG_STREAM Macro for stream-style logging
 */
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__)

/**
 * @def NLM_LOG_CRITICAL(msg)
 * @brief Macro for critical-level logging with automatic context
 * 
 * This macro expands to a call to the global logger's critical() method with
 * automatic file, line, and function information.
 * 
 * @param msg Log message text
 * 
 * @see NLM_LOG_DEBUG Macro for debug-level logging
 * @see NLM_LOG_INFO Macro for info-level logging
 * @see NLM_LOG_WARNING Macro for warning-level logging
 * @see NLM_LOG_ERROR Macro for error-level logging
 * @see NLM_LOG_STREAM Macro for stream-style logging
 */
#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)

/**
 * @class LogStream
 * @brief Stream-style logging helper for efficient logging
 * 
 * LogStream provides a stream interface for logging messages, similar to
 * std::cout. It formats multiple values and operators (<<) efficiently
 * without the overhead of string formatting.
 * 
 * @details LogStream features:
 * - Stream-style interface with << operators
 * - Automatic severity level and context
 * - Efficient string building
 * - RAII pattern for automatic log flushing
 * - Thread-safe operation
 * 
 * This class is useful for logging complex data structures and
 * performance-sensitive code where string formatting overhead is a concern.
 * 
 * @note LogStream is designed for use with the NLM_LOG_STREAM macro.
 * It automatically logs when the stream is destroyed.
 * 
 * @see NLM_LOG_STREAM Macro for usage example
 * @see ILogger for abstract logger interface
 * @see Logger for main logger implementation
 * 
 * @par Example
 * @code
 * // Stream-style logging
 * NLM_LOG_STREAM(nlm::LogLevel::Debug)
 *     << "Neuron state: " << neuron.getMembranePotential()
 *     << "mV, rate: " << neuron.getFiringRate() << "Hz";
 * @endcode
 */
class LogStream {
public:
    /**
     * @brief Construct a log stream with context
     * 
     * @param level Log severity level
     * @param file Source file name
     * @param line Line number in source file
     * @param function Function name where log was generated
     */
    LogStream(LogLevel level, const std::string& file, int line, const std::string& function);
    
    /**
     * @brief Destructor - automatically logs the accumulated message
     * 
     * The destructor automatically logs the accumulated message to the
     * global logger. This ensures that log messages are flushed even if
     * exceptions occur within the stream.
     */
    ~LogStream();
    
    /**
     * @brief Stream operator for appending values
     * 
     * @tparam T Type of value to append
     * @param value Value to append to the log message
     * @return LogStream& Reference to this stream for chaining
     */
    template<typename T>
    LogStream& operator<<(const T& value) {
        oss_ << value;
        return *this;
    }
    
private:
    LogLevel level_;           ///< Log severity level
    std::ostringstream oss_;   ///< Output string stream
    std::string file_;         ///< Source file name
    int line_;                 ///< Line number in source file
    std::string function_;     ///< Function name where log was generated
};

/**
 * @def NLM_LOG_STREAM(level)
 * @brief Macro for stream-style logging with automatic context
 * 
 * This macro expands to construction of a LogStream object with the specified
 * level and current source code context. The stream provides a convenient
 * interface for logging complex messages with multiple values.
 * 
 * @param level Log severity level for the message
 * 
 * @note The stream automatically logs when it is destroyed (RAII pattern).
 * This is more efficient than concatenating strings for complex messages.
 * 
 * @see LogStream for stream class
 * @see NLM_LOG_DEBUG Macro for debug-level macro
 * @see NLM_LOG_INFO Macro for info-level macro
 * @see NLM_LOG_WARNING Macro for warning-level macro
 * @see NLM_LOG_ERROR Macro for error-level macro
 * @see NLM_LOG_CRITICAL Macro for critical-level macro
 * 
 * @par Example
 * @code
 * // Stream-style logging with multiple values
 * NLM_LOG_STREAM(nlm::LogLevel::Info)
 *     << "Simulation completed in " << steps << " steps, "
 *     << "average rate: " << avgRate << "Hz";
 * @endcode
 */
#define NLM_LOG_STREAM(level) nlm::LogStream(level, __FILE__, __LINE__, __func__)

} // namespace nlm
