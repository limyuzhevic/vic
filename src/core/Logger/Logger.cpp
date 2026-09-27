/**
 * @file Logger.cpp
 * @brief Implementation file for the Logger and ConsoleLogger classes
 * 
 * This file provides the implementation of the Logger class methods, including
 * the ConsoleLogger implementation, stream-style logging, and global logger management.
 * The implementation covers the complete logging infrastructure from abstract
 * interface to concrete implementation.
 * 
 * @details Implementation features:
 * - ConsoleLogger implementation with colored output support
 * - File output management with append mode
 * - Thread-safe logging with mutex protection
 * - Stream-style logging with RAII pattern
 * - Global logger management with singleton pattern
 * - Log level filtering and management
 * - Log entry creation with timestamp and source location
 * - ANSI color code management for console output
 * - Efficient string formatting and output
 * 
 * @note This implementation provides the concrete logging functionality for
 * the NLM framework, enabling comprehensive logging across all components.
 * 
 * @author NLM Authors
 * @date Created: 2024
 * @version 1.0
 * 
 * @copyright MIT License
 * 
 * @warning Logging may produce significant output in long-running simulations.
 * Configure log levels appropriately to balance debugging needs with performance.
 * 
 * @see Logger.hpp for class interface
 * @see ILogger for abstract logger interface
 * @see LogEntry for log entry structure
 * @see LogLevel for severity levels
 * @see ConsoleLogger for console logger implementation
 * @see NLM_LOG_* macros for convenience logging
 * 
 * @ingroup CoreComponents
 */

#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>

namespace nlm {

/**
 * @brief Implementation details for the ConsoleLogger class
 * 
 * This struct contains the private implementation details of the ConsoleLogger class,
 * following the pImpl idiom for efficient memory management and forward declaration.
 * The implementation includes thread-safe storage for configuration and output state.
 * 
 * @details The implementation uses:
 * - LogLevel for severity level configuration
 * - bool for color output preference
 * - std::ofstream for file output with append mode
 * - std::mutex for thread-safe logging operations
 * - Efficient string conversion functions
 * 
 * @see ConsoleLogger::Impl for public interface
 * @see ConsoleLogger::ConsoleLogger() for constructor
 * @see ConsoleLogger::log() for logging implementation
 */
struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;        ///< Minimum severity level to log
    bool useColors = true;                   ///< Enable/disable colored output
    std::ofstream fileOut;                   ///< Output file stream (append mode)
    std::mutex mutex;                        ///< Thread-safe logging mutex
};

/**
 * @brief Default constructor implementation
 * 
 * Constructs the ConsoleLogger implementation with default configuration.
 * The default configuration includes:
 * - Log level: Info
 * - Colors: Enabled (for terminal output)
 * - File output: Disabled
 * 
 * @note The constructor uses std::make_unique for efficient memory management
 * and automatic cleanup of the implementation structure.
 * 
 * @see ConsoleLogger::ConsoleLogger()
 * @see ConsoleLogger::ConsoleLogger(LogLevel level) for level-specific constructor
 */
ConsoleLogger::ConsoleLogger() : pImpl(std::make_unique<Impl>()) {}

/**
 * @brief Level-specific constructor implementation
 * 
 * Constructs the ConsoleLogger implementation with the specified log level.
 * Other settings use defaults (colors enabled, file output disabled).
 * 
 * @param level Initial minimum log level
 * 
 * @note This constructor allows for easy configuration of the logger's
 * minimum severity level during creation. Other settings can be modified
 * later using the setLevel() and setUseColors() methods.
 * 
 * @see ConsoleLogger() for default constructor
 * @see ConsoleLogger::setLevel() for changing level later
 */
ConsoleLogger::ConsoleLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
}

/**
 * @brief Destructor implementation
 * 
 * The destructor is defaulted, relying on the implicit destruction of the
 * unique_ptr smart pointer that manages the implementation. This ensures
 * proper cleanup of the implementation resources without manual memory management.
 * 
 * @note The default implementation is sufficient for the current ConsoleLogger
 * implementation and provides exception safety.
 * 
 * @see ConsoleLogger::~ConsoleLogger()
 */
ConsoleLogger::~ConsoleLogger() = default;

/**
 * @brief Log entry processing implementation
 * 
 * Implements the ILogger::log() method by formatting and outputting the log entry
 * to both console and optionally to file. This method handles level filtering,
 * thread safety, formatting, and output redirection.
 * 
 * @param entry Log entry to process
 * 
 * @throws std::runtime_error if output stream operations fail (uncaught)
 * 
 * @note This implementation performs:
 * - Level filtering (skip if entry.level < pImpl->level)
 * - Thread-safe locking (std::lock_guard)
 * - Timestamp formatting
 * - Console output with optional colors
 * - File output if file stream is open
 * - Automatic flushing for file output
 * 
 * @see ILogger::log(const LogEntry& entry) for interface
 * @see ConsoleLogger::setLevel() for level filtering
 * @see ConsoleLogger::setUseColors() for color control
 * @see ConsoleLogger::setOutputFile() for file output
 * @see ConsoleLogger::levelToString() for level conversion
 * @see ConsoleLogger::levelToColor() for color conversion
 */
void ConsoleLogger::log(const LogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "[" << levelToString(entry.level) << "] ";
    
    // Add timestamp
    auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    
    oss << " " << entry.message;
    
    std::string output = oss.str();
    
    // Console output
    if (pImpl->useColors) {
        std::cout << levelToColor(entry.level) << output << "\033[0m\n";
    } else {
        std::cout << output << "\n";
    }
    
    // File output if set
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut << output << "\n";
        pImpl->fileOut.flush();
    }
}

/**
 * @brief Set minimum log level implementation
 * 
 * Configures the minimum severity level for this logger. Log entries with
 * level below this threshold are silently discarded.
 * 
 * @param level Minimum severity level to process
 * 
 * @note This is a thread-safe operation using std::lock_guard for mutex protection.
 * The level is stored in the implementation and used for filtering in the log() method.
 * 
 * @see ConsoleLogger::setLevel(LogLevel level)
 * @see ConsoleLogger::getLevel() for retrieving current level
 * @see LogLevel for severity levels
 */
void ConsoleLogger::setLevel(LogLevel level) {
    pImpl->level = level;
}

/**
 * @brief Get current minimum log level implementation
 * 
 * @return LogLevel Current minimum log level
 * 
 * @note This implementation provides O(1) access to the level configuration
 * stored in the implementation structure.
 * 
 * @see ConsoleLogger::setLevel() for setting the level
 */
LogLevel ConsoleLogger::getLevel() const {
    return pImpl->level;
}

/**
 * @brief Enable/disable colored output implementation
 * 
 * Configures whether log messages should be colored based on severity level.
 * Colors are only applied when output is directed to a terminal (isatty() check).
 * 
 * @param useColors true to enable colored output, false to disable
 * 
 * @note This implementation simply stores the preference for later use
 * in the log() method. Colors are applied at output time, not at storage time.
 * 
 * @see ConsoleLogger::getUseColors() for querying current setting
 * @see ConsoleLogger::log() for color application
 * @see ConsoleLogger::levelToColor() for color mapping
 */
void ConsoleLogger::setUseColors(bool useColors) {
    pImpl->useColors = useColors;
}

/**
 * @brief Check if colors are enabled implementation
 * 
 * @return bool true if colors are enabled, false otherwise
 * 
 * @see ConsoleLogger::setUseColors() for setting the setting
 */
bool ConsoleLogger::getUseColors() const {
    return pImpl->useColors;
}

/**
 * @brief Set additional output file implementation
 * 
 * Enables file output in addition to console output. All log messages
 * are written to both the console and the specified file. The file is
 * opened in append mode.
 * 
 * @param filepath Path to the file for log output
 * 
 * @note This implementation uses std::ofstream::open with std::ios::app
 * mode to ensure logs are appended rather than overwritten. The file stream
 * is stored in the implementation for subsequent writes.
 * 
 * @see ConsoleLogger::closeOutputFile() for closing file output
 * @see ConsoleLogger::log() for file output usage
 * @see std::ofstream for file stream details
 */
void ConsoleLogger::setOutputFile(const std::string& filepath) {
    pImpl->fileOut.open(filepath, std::ios::app);
}

/**
 * @brief Close the output file implementation
 * 
 * Closes the file output stream and disables file logging. Console logging
 * continues as configured.
 * 
 * @note This implementation safely closes the file stream only if it is
 * currently open. It uses std::ofstream::is_open() to check stream status.
 * 
 * @see ConsoleLogger::setOutputFile() for opening a log file
 * @see ConsoleLogger::log() for file output usage
 */
void ConsoleLogger::closeOutputFile() {
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
}

/**
 * @brief Convert log level to string implementation
 * 
 * Converts a log level to its string representation for display purposes.
 * This implementation provides short, readable abbreviations for log levels.
 * 
 * @param level Log level to convert
 * @return std::string String representation of the level
 * 
 * @note This implementation uses a switch statement to provide consistent
 * abbreviations: DEBUG, INFO, WARN, ERROR, CRIT, UNKNOWN. These abbreviations
 * are optimized for compact log output.
 * 
 * @see ConsoleLogger::log() for string usage
 * @see ConsoleLogger::levelToColor() for color mapping
 * @see LogLevel for available level values
 */
std::string ConsoleLogger::levelToString(LogLevel level) const {
    switch (level) {
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Info: return "INFO";
        case LogLevel::Warning: return "WARN";
        case LogLevel::Error: return "ERROR";
        case LogLevel::Critical: return "CRIT";
        default: return "UNKNOWN";
    }
}

/**
 * @brief Convert log level to color code implementation
 * 
 * Converts a log level to ANSI color codes for console output. This implementation
 * provides consistent color mapping for different severity levels.
 * 
 * @param level Log level to convert
 * @return std::string ANSI color code for the level
 * 
 * @note This implementation uses standard ANSI color codes:
 * - Cyan (36m) for DEBUG
 * - Green (32m) for INFO
 * - Yellow (33m) for WARNING
 * - Red (31m) for ERROR
 * - Magenta (35m) for CRITICAL
 * - Default (0m) for unknown levels
 * 
 * @see ConsoleLogger::log() for color application
 * @see ConsoleLogger::setUseColors() for color control
 * @see LogLevel for available level values
 * @see ANSI escape codes for color details
 */
std::string ConsoleLogger::levelToColor(LogLevel level) const {
    switch (level) {
        case LogLevel::Debug: return "\033[36m";    // Cyan
        case LogLevel::Info: return "\033[32m";      // Green
        case LogLevel::Warning: return "\033[33m";   // Yellow
        case LogLevel::Error: return "\033[31m";     // Red
        case LogLevel::Critical: return "\033[35m"; // Magenta
        default: return "\033[0m";
    }
}

/**
 * @brief Logger class implementation details
 * 
 * This struct contains the private implementation details of the Logger class,
 * following the pImpl idiom for efficient memory management and forward declaration.
 * The implementation manages multiple ILogger implementations (sinks) and provides
 * thread-safe access to them.
 * 
 * @details The implementation uses:
 * - std::vector<std::shared_ptr<ILogger>> for storing logger sinks
 * - std::mutex for thread-safe logger management
 * - Singleton pattern for global logger access
 * - Efficient iteration over logger sinks
 * 
 * @see Logger::Impl for public interface
 * @see Logger::Logger() for constructor
 * @see Logger::log() for logging implementation
 */
struct Logger::Impl {
    std::vector<std::shared_ptr<ILogger>> loggers;  ///< Vector of logger sinks
    std::mutex mutex;                               ///< Thread-safe logging mutex
};

/**
 * @brief Default constructor implementation
 * 
 * Constructs the Logger implementation with an empty logger sink list.
 * The Logger starts with no sinks, meaning no output will occur until
 * loggers are added.
 * 
 * @note The constructor uses std::make_unique for efficient memory management
 * and automatic cleanup of the implementation structure.
 * 
 * @see Logger::Logger()
 * @see Logger::addLogger() for adding sinks
 * @see Logger::getGlobal() for global logger access
 */
Logger::Logger() : pImpl(std::make_unique<Impl>()) {}

/**
 * @brief Destructor implementation
 * 
 * The destructor is defaulted, relying on the implicit destruction of the
 * unique_ptr smart pointer that manages the implementation. This ensures
 * proper cleanup of the implementation resources without manual memory management.
 * 
 * @note The default implementation is sufficient for the current Logger
 * implementation and provides exception safety.
 * 
 * @see Logger::~Logger()
 */
Logger::~Logger() = default;

/**
 * @brief Add a logger sink implementation
 * 
 * Adds an ILogger implementation to this logger's composite sink list.
 * The added logger will receive all log messages processed by this Logger.
 * 
 * @param logger Shared pointer to ILogger implementation to add
 * 
 * @note This implementation is thread-safe, using std::lock_guard for mutex protection.
 * The logger is added to the end of the vector for efficient appending.
 * 
 * @see Logger::addLogger(std::shared_ptr<ILogger> logger)
 * @see Logger::clearLoggers() for removing sinks
 * @see std::shared_ptr for smart pointer details
 */
void Logger::addLogger(std::shared_ptr<ILogger> logger) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loggers.push_back(logger);
}

/**
 * @brief Remove all logger sinks implementation
 * 
 * Removes all ILogger implementations from this logger's composite sink list.
 * After this operation, the logger will not produce any output until
 * additional loggers are added.
 * 
 * @note This implementation is thread-safe and efficiently clears the vector
 * using std::vector::clear, which preserves capacity for future additions.
 * 
 * @see Logger::addLogger() for adding sinks
 * @see std::vector::clear for implementation details
 */
void Logger::clearLoggers() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loggers.clear();
}

/**
 * @brief Set minimum log level for all loggers implementation
 * 
 * Configures the minimum severity level for all logger sinks. Log entries
 * with level below this threshold are filtered out before being passed to
 * individual loggers.
 * 
 * @param level Minimum severity level to process
 * 
 * @note This implementation iterates over all loggers and sets their level
 * individually. It is thread-safe, using std::lock_guard for mutex protection.
 * The level filtering occurs in individual logger implementations.
 * 
 * @see Logger::setLevel() for convenience method
 * @see ILogger::setLevel() for individual logger level setting
 */
void Logger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        logger->setLevel(level);
    }
}

/**
 * @brief Global logging function implementation
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
 * @throws std::runtime_error if no loggers are configured (uncaught)
 * 
 * @note This implementation creates a LogEntry with the provided information
 * and passes it to all configured loggers. It is thread-safe, using
 * std::lock_guard for mutex protection. The timestamp is captured at
 * processing time.
 * 
 * @see Logger::log(LogLevel level, const std::string& message,
 *                 const std::string& file = "", int line = 0,
 *                 const std::string& function = "")
 * @see LogEntry for log entry structure
 * @see ILogger::log() for individual logger processing
 * @see std::chrono::system_clock::now() for timestamp
 */
void Logger::log(LogLevel level, const std::string& message,
                const std::string& file, int line, const std::string& function) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.timestamp = std::chrono::system_clock::now();
    
    for (auto& logger : pImpl->loggers) {
        logger->log(entry);
    }
}

/**
 * @brief Debug level logging convenience method implementation
 * 
 * Logs a debug-level message with automatic source code context. Debug
 * messages are typically used during development and debugging.
 * 
 * @param message Log message text
 * @param file Source file name (default: "")
 * @param line Line number in source file (default: 0)
 * @param function Function name where log was generated (default: "")
 * 
 * @see Logger::log() for general logging function
 * @see NLM_LOG_DEBUG macro for stream-style debug logging
 * @see LogLevel::Debug for level definition
 */
void Logger::debug(const std::string& message,
                   const std::string& file, int line,
                   const std::string& function) {
    log(LogLevel::Debug, message, file, line, function);
}

/**
 * @brief Info level logging convenience method implementation
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
 * @see Logger::log() for general logging function
 * @see NLM_LOG_INFO macro for stream-style info logging
 * @see LogLevel::Info for level definition
 */
void Logger::info(const std::string& message,
                  const std::string& file, int line,
                  const std::string& function) {
    log(LogLevel::Info, message, file, line, function);
}

/**
 * @brief Warning level logging convenience method implementation
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
 * @see Logger::log() for general logging function
 * @see NLM_LOG_WARNING macro for stream-style warning logging
 * @see LogLevel::Warning for level definition
 */
void Logger::warning(const std::string& message,
                     const std::string& file, int line,
                     const std::string& function) {
    log(LogLevel::Warning, message, file, line, function);
}

/**
 * @brief Error level logging convenience method implementation
 * 
 * Logs an error message with automatic source code context. Error
 * messages indicate problems that affect simulation operation.
 * 
 * @param message Log message text
 * @param file Source file name (default: "")
 * @param line Line number in source file (default: 0)
 * @param function Function name where log was generated (default: "")
 * 
 * @see Logger::log() for general logging function
 * @see NLM_LOG_ERROR macro for stream-style error logging
 * @see LogLevel::Error for level definition
 */
void Logger::error(const std::string& message,
                   const std::string& file, int line,
                   const std::string& function) {
    log(LogLevel::Error, message, file, line, function);
}

/**
 * @brief Critical level logging convenience method implementation
 * 
 * Logs a critical message with automatic source code context. Critical
 * messages indicate severe errors that typically terminate the simulation.
 * 
 * @param message Log message text
 * @param file Source file name (default: "")
 * @param line Line number in source file (default: 0)
 * @param function Function name where log was generated (default: "")
 * 
 * @see Logger::log() for general logging function
 * @see NLM_LOG_CRITICAL macro for stream-style critical logging
 * @see LogLevel::Critical for level definition
 */
void Logger::critical(const std::string& message,
                      const std::string& file, int line,
                      const std::string& function) {
    log(LogLevel::Critical, message, file, line, function);
}

/**
 * @brief Check if a log level is enabled implementation
 * 
 * Determines if messages of the specified severity level will be processed
 * by any configured logger sink. This considers both the global filter
 * and individual logger level settings.
 * 
 * @param level Log level to check
 * @return bool true if messages of this level will be logged, false otherwise
 * 
 * @note This implementation is thread-safe, using std::lock_guard for mutex protection.
 * It checks each logger's level and returns true if any logger would accept the message.
 * 
 * @see Logger::setLevel() for setting the minimum level
 * @see ILogger::getLevel() for individual logger level retrieval
 * @see LogLevel for severity levels
 */
bool Logger::isEnabled(LogLevel level) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        if (logger->getLevel() <= level) {
            return true;
        }
    }
    return false;
}

/**
 * @brief Get the global logger instance implementation
 * 
 * Returns the global singleton logger instance used throughout the NLM
 * framework. This provides a convenient way to access the main logging
 * system without creating multiple logger instances.
 * 
 * @return Logger& Reference to the global logger instance
 * 
 * @note The global logger is created on first access and persists for
 * the lifetime of the program. This implementation uses the static variable
 * pattern for singleton creation, which is thread-safe in C++11 and later.
 * 
 * @see Logger::getGlobal() for public interface
 * @see Logger::setGlobal() for replacing the global logger
 */
Logger& Logger::getGlobal() {
    static Logger globalLogger;
    return globalLogger;
}

/**
 * @brief Set the global logger instance implementation
 * 
 * Replaces the current global logger instance with a new one. This allows
 * customizing the logging system for specific applications or testing
 * scenarios.
 * 
 * @param logger Shared pointer to the new global logger instance
 * 
 * @note This implementation is a placeholder for advanced global logger management.
 * In practice, the getGlobal() method provides the singleton logger instance.
 * This function could be extended for more sophisticated global logger management
 * in future phases.
 * 
 * @see Logger::getGlobal() for retrieving the global logger
 * @see std::shared_ptr for smart pointer details
 */
void Logger::setGlobal(std::shared_ptr<Logger> logger) {
    // This is a placeholder for advanced global logger management
    // In practice, we just use getGlobal() which is already static
}

/**
 * @brief LogStream constructor implementation
 * 
 * Constructs a LogStream object with the specified level and source code context.
 * The stream is initialized for building log messages using the stream operator.
 * 
 * @param level Log severity level
 * @param file Source file name
 * @param line Line number in source file
 * @param function Function name where log was generated
 * 
 * @note This implementation initializes the stream components for later
 * message building and automatic logging when the stream is destroyed.
 * 
 * @see LogStream::LogStream(LogLevel level, const std::string& file,
 *                         int line, const std::string& function)
 * @see LogStream::~LogStream() for destructor
 * @see Logger::log() for logging function
 */
LogStream::LogStream(LogLevel level, const std::string& file, int line, const std::string& function)
    : level_(level), file_(file), line_(line), function_(function) {}

/**
 * @brief LogStream destructor implementation
 * 
 * The destructor automatically logs the accumulated message to the global logger.
 * This ensures that log messages are flushed even if exceptions occur within
 * the stream, providing robust logging with the RAII pattern.
 * 
 * @note This implementation uses the global logger (Logger::getGlobal()) to
 * ensure consistent logging behavior across the application. The stream
 * message is logged using the same interface as regular logging functions.
 * 
 * @see LogStream::LogStream(LogLevel level, const std::string& file,
 *                         int line, const std::string& function)
 * @see Logger::log() for logging function
 * @see NLM_LOG_STREAM macro for usage example
 */
LogStream::~LogStream() {
    Logger::getGlobal().log(level_, oss_.str(), file_, line_, function_);
}

} // namespace nlm
