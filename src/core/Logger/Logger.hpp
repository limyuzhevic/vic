// Copyright (c) 2024+ NLM Project
// All rights reserved.
// 
// This file is part of the Neural Learning Machine (NLM) project.
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
// 3. Neither the name of the copyright holder nor the names of its contributors
//    may be used to endorse or promote products derived from this software
//    without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

/**
 * @file Logger.hpp
 * @brief Logging system for NLM with multiple output handlers and formatting
 * 
 * This file implements a comprehensive logging system for the NLM project,
 * providing logging capabilities with multiple severity levels, output handlers,
 * and formatting options. The Logger class serves as the central logging hub
 * for all components of the NLM system.
 * 
 * The logging system is designed with the following key principles:
 * - Multiple severity levels (Debug, Info, Warning, Error, Critical)
 * - Multiple output handlers (console, file, etc.)
 * - Configurable log levels and filtering
 * - Thread-safe operation for parallel simulation
 * - Rich formatting options with colors and timestamps
 * - Performance optimization to minimize overhead in production
 * - Support for structured logging and custom formatters
 * - Integration with NLM macros for convenient usage
 * 
 * @author NLM Project
 * @version 1.0.0
 * @date 2024-2026
 * @copyright NLM Project (2024+)
 * 
 * @see src/core/Config/Config.hpp - Configuration system for logging settings
 * @see src/brain/Brain.hpp - Brain logging for neural system monitoring
 * @see src/core/Logger/Logger.cpp - Logger implementation
 * 
 * @note This file uses modern C++17 features and follows Doxygen documentation standards.
 *       The logging system supports integration with NLM's macro system for convenient usage.
 */

#pragma once

// NLM filename guard
#ifndef NLM_LOGGER_HPP
#define NLM_LOGGER_HPP

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <mutex>
#include <unordered_map>
#include <ctime>
#include <iomanip>
#include <optional>
#include <algorithm>

namespace nlm {

/**
 * @enum LogLevel
 * @brief Severity levels for log messages
 * 
 * This enumeration defines the different severity levels for log messages,
 * ordered from most verbose to least verbose:
 * - Debug: Detailed debugging information
 * - Info: General information messages
 * - Warning: Warnings about potential issues
 * - Error: Error conditions that prevent normal operation
 * - Critical: Critical errors that may cause system failure
 * 
 * Log levels are used for:
 * - Filtering log output based on severity
 * - Prioritizing log messages
 * - Configurable logging verbosity
 * - Different handling based on severity
 */
enum class LogLevel {
    Debug = 0,    ///< Detailed debugging information
    Info = 1,     ///< General information messages
    Warning = 2,  ///< Warnings about potential issues
    Error = 3,    ///< Error conditions preventing normal operation
    Critical = 4  ///< Critical errors potentially causing system failure
};

/**
 * @struct LogEntry
 * @brief Complete log message with metadata
 * 
 * This structure contains a complete log message with all relevant metadata,
 * enabling comprehensive log analysis and reporting. Each log entry includes:
 * - Severity level for filtering and prioritization
 * - Message content
 * - Source location (file, line, function) for debugging
 * - Timestamp for temporal analysis
 * - Additional metadata (future extension point)
 * 
 * Log entries are used for:
 * - Log output formatting and display
 * - Log analysis and reporting
 * - Log storage and archiving
 * - Log filtering and searching
 */
struct LogEntry {
    LogLevel level;                                   ///< Severity level of the log message
    std::string message;                              ///< Log message content
    std::string file;                                 ///< Source file name (for debugging)
    int line;                                         ///< Source line number (for debugging)
    std::string function;                            ///< Source function name (for debugging)
    std::chrono::system_clock::time_point timestamp;  ///< When the log was created
    
    /**
     * @brief Default constructor
     * 
     * Creates an empty log entry with default values.
     * Useful for creating placeholder log entries.
     */
    LogEntry() : level(LogLevel::Info), line(0) {}
    
    /**
     * @brief Parameterized constructor
     * 
     * @param lvl Severity level
     * @param msg Log message
     * @param file_source Source file name
     * @param line_num Source line number
     * @param function_name Source function name
     */
    LogEntry(LogLevel lvl, const std::string& msg, const std::string& file_source = "",
             int line_num = 0, const std::string& function_name = "")
        : level(lvl)
        , message(msg)
        , file(file_source)
        , line(line_num)
        , function(function_name)
        , timestamp(std::chrono::system_clock::now()) {}
    
    /**
     * @brief Get formatted timestamp string
     * 
     * @return std::string Formatted timestamp
     */
    std::string getFormattedTimestamp() const;
    
    /**
     * @brief Get source location string
     * 
     * @return std::string Formatted source location
     */
    std::string getSourceLocation() const;
};

/**
 * @class ILogger
 * @brief Abstract interface for log handlers
 * 
 * This class defines the interface for log handlers, which are responsible
 * for processing and outputting log messages. Implementations can write to
 * different destinations (console, file, network, etc.) and format messages
 * as needed.
 * 
 * ILogger is designed for:
 * - Pluggable log handlers
 * - Multiple output destinations
 * - Custom formatting and filtering
 * - Extension points for new log destinations
 * 
 * Implementation notes:
 * - Implementations should be thread-safe
 * - Should minimize performance overhead
 * - Should handle log filtering internally
 * - Should provide proper resource management
 */
class ILogger {
public:
    virtual ~ILogger() = default;
    
    /**
     * @brief Process and output a log entry
     * 
     * @param entry Log entry to process
     * 
     * This method processes the log entry and outputs it to the appropriate
     * destination. Implementations should handle:
     * - Message formatting
     * - Output destination
     * - Thread safety
     * - Resource management
     * 
     * @note This method should be thread-safe for concurrent logging.
     */
    virtual void log(const LogEntry& entry) = 0;
    
    /**
     * @brief Set the minimum log level for this handler
     * 
     * @param level Minimum log level (only messages >= this level will be logged)
     * 
     * This method configures which log messages will be processed and output.
     * For example, if the level is set to Warning, only Warning, Error, and
     * Critical messages will be logged.
     */
    virtual void setLevel(LogLevel level) = 0;
    
    /**
     * @brief Get the current minimum log level
     * 
     * @return LogLevel Current minimum log level
     */
    virtual LogLevel getLevel() const = 0;
    
    /**
     * @brief Check if this handler is enabled for a specific log level
     * 
     * @param level Log level to check
     * @return bool true if handler is enabled for this level
     * 
     * This method checks whether the handler is configured to process
     * log messages of the specified level.
     */
    virtual bool isEnabled(LogLevel level) const {
        return level >= getLevel();
    }
}; // class ILogger

/**
 * @class ConsoleLogger
 * @brief Console-based log handler with color support
 * 
 * This class implements a log handler that outputs messages to the console
 * with color coding based on severity levels. It provides:
 * - Color-coded output for different log levels
 * - Configurable color usage
 * - Optional file output in addition to console
 * - Efficient console formatting
 * 
 * ConsoleLogger is ideal for:
 * - Interactive debugging
 * - Development environments
 * - Real-time monitoring
 * - Error reporting with visual distinction
 * 
 * Color scheme:
 * - Debug: Gray
 * - Info: Green
 * - Warning: Yellow
 * - Error: Red
 * - Critical: Bold Red
 */
class ConsoleLogger : public ILogger {
public:
    /**
     * @brief Constructor with default log level
     * 
     * Creates a console logger with default settings:
     * - Log level: Info
     * - Colors: Enabled (if terminal supports it)
     * - File output: Disabled
     */
    ConsoleLogger();
    
    /**
     * @brief Constructor with specified log level
     * 
     * @param level Initial log level for this handler
     */
    explicit ConsoleLogger(LogLevel level);
    
    /**
     * @brief Destructor
     * 
     * Cleans up resources including file handles and terminal state.
     */
    ~ConsoleLogger() override;
    
    /**
     * @brief Process and output a log entry to console
     * 
     * @param entry Log entry to output
     * 
     * This method formats and outputs the log entry to the console,
     * including timestamp, source location, and message with appropriate
     * color coding based on severity level.
     */
    void log(const LogEntry& entry) override;
    
    /**
     * @brief Set the log level for this handler
     * 
     * @param level New log level
     */
    void setLevel(LogLevel level) override;
    
    /**
     * @brief Get the current log level
     * 
     * @return LogLevel Current log level
     */
    LogLevel getLevel() const override;
    
    /**
     * @brief Enable/disable color output
     * 
     * @param useColors Whether to use color coding
     * 
     * This method enables or disables color output based on terminal support
     * and user preference. Colors are only used when output is directed to
     * a terminal that supports them.
     */
    void setUseColors(bool useColors);
    
    /**
     * @brief Check if color output is enabled
     * 
     * @return bool true if color output is enabled
     */
    bool getUseColors() const;
    
    /**
     * @brief Set output file (in addition to console)
     * 
     * @param filepath Path to the output file
     * 
     * This method enables logging to both console and a file. The file will
     * receive the same formatted output as the console.
     */
    void setOutputFile(const std::string& filepath);
    
    /**
     * @brief Close output file
     * 
     * This method closes the output file, flushing any pending writes.
     * After this call, logging will go only to console.
     */
    void closeOutputFile();
    
    /**
     * @brief Get the output file path
     * 
     * @return std::string Output file path, or empty string if not set
     */
    std::string getOutputFile() const;
    
private:
    /**
     * @brief Implementation details structure
     * 
     * Contains the actual implementation details using the Pimpl idiom.
     * This allows efficient compilation and reduces coupling between
     * header and implementation.
     */
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    /**
     * @brief Convert log level to string representation
     * 
     * @param level Log level to convert
     * @return std::string String representation of log level
     * 
     * This method returns the string representation of the log level
     * for display purposes.
     */
    std::string levelToString(LogLevel level) const;
    
    /**
     * @brief Convert log level to ANSI color code
     * 
     * @param level Log level to convert
     * @return std::string ANSI color code for the level
     * 
     * This method returns the ANSI escape code for coloring output
     * based on the log level, if colors are enabled.
     */
    std::string levelToColor(LogLevel level) const;
}; // class ConsoleLogger

/**
 * @class Logger
 * @brief Central logging manager for NLM
 * 
 * The Logger class provides a comprehensive logging system for the NLM project,
 * serving as the central hub that manages multiple log handlers and provides
 * a unified interface for logging messages. It supports:
 * - Multiple log handlers (console, file, etc.)
 * - Configurable log levels and filtering
 * - Thread-safe operation for parallel simulation
 * - Convenient macros for common logging operations
 * - Stream-based logging for formatted output
 * - Global logger instance for convenience
 * 
 * Key Features:
 * - Thread-safe operation with mutex protection
 * - Multiple output handlers
 * - Configurable log levels
 * - Convenient macros and stream interface
 * - Global logger instance
 * - Performance optimization
 * 
 * Logger is designed for:
 * - Debugging and troubleshooting
 * - Runtime monitoring
 * - Error reporting
 * - Performance analysis
 * - System health monitoring
 * 
 * Usage Examples:
 * @code
 * #include "src/core/Logger/Logger.hpp"
 * 
 * // Basic logging
 * nlm::Logger::getGlobal().info("Simulation started");
 * nlm::Logger::getGlobal().error("Failed to load model", "model.cpp", 42, "loadModel");
 * 
 * // Colored logging
 * NLM_LOG_DEBUG("Debug message");
 * NLM_LOG_INFO("Information message");
 * NLM_LOG_WARNING("Warning message");
 * NLM_LOG_ERROR("Error message");
 * NLM_LOG_CRITICAL("Critical error message");
 * 
 * // Stream logging
 * NLM_LOG_STREAM(nlm::LogLevel::Info) << "Streamed " << 42 << " values";
 * 
 * // Conditional logging
 * if (nlm::Logger::getGlobal().isEnabled(nlm::LogLevel::Debug)) {
 *     nlm::Logger::getGlobal().debug("Detailed debug info");
 * }
 * @endcode
 * 
 * @note This class uses the Pimpl idiom for efficient compilation and reduced
 *       coupling. The actual implementation is in Logger.cpp.
 */
class Logger {
public:
    /**
     * @brief Default constructor
     * 
     * Creates a logger with default configuration:
     * - No log handlers
     * - Default log level: Info
     * - Thread-safe operation enabled
     */
    Logger();
    
    /**
     * @brief Destructor
     * 
     * Cleans up all allocated resources including log handlers
     * and mutexes. Ensures proper cleanup and prevents resource leaks.
     */
    ~Logger();
    
    /**
     * @brief Disable copy construction
     * 
     * Loggers cannot be copied because they contain:
     * - Dynamic resources requiring unique ownership
     * - Mutexes that should not be shared
     * - Log handlers that would become invalid
     * - Thread-local state that requires coordination
     */
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    /**
     * @brief Enable move construction
     * 
     * Allows efficient transfer of logger ownership from one instance
     * to another. After move construction, the source logger is in a
     * valid but unspecified state.
     */
    Logger(Logger&&) noexcept;
    Logger& operator=(Logger&&) noexcept;
    
    /**
     * @brief Add a log handler to the logger
     * 
     * @param logger Handler to add (shared pointer)
     * 
     * This method adds a log handler to the logger. The logger will forward
     * all log messages to this handler. Multiple handlers can be added.
     * 
     * @note Handlers are shared, so they can be shared between multiple loggers.
     */
    void addLogger(std::shared_ptr<ILogger> logger);
    
    /**
     * @brief Remove all log handlers
     * 
     * This method removes all log handlers from the logger, stopping all
     * log output. The logger can be reused by adding handlers again.
     */
    void clearLoggers();
    
    /**
     * @brief Set the minimum log level for all handlers
     * 
     * @param level Minimum log level (default: Info)
     * 
     * This method sets the minimum log level for all log handlers attached
     * to this logger. Only messages with level >= this level will be
     * processed by handlers.
     */
    void setLevel(LogLevel level);
    
    /**
     * @brief Log a message with specified level
     * 
     * @param level Severity level of the log message
     * @param message Log message content
     * @param file Source file name (optional, for debugging)
     * @param line Source line number (optional, for debugging)
     * @param function Source function name (optional, for debugging)
     * 
     * This method logs a message with the specified severity level.
     * The message is formatted with timestamp, source location, and
     * output to all registered log handlers.
     */
    void log(LogLevel level, const std::string& message,
             const std::string& file = "", int line = 0,
             const std::string& function = "");
    
    /**
     * @brief Log a debug message
     * 
     * @param message Log message
     * @param file Source file name (optional)
     * @param line Source line number (optional)
     * @param function Source function name (optional)
     * 
     * This method logs a debug-level message. Use this method for
     * detailed debugging information that should only appear when
     * debug logging is enabled.
     */
    void debug(const std::string& message, const std::string& file = "",
               int line = 0, const std::string& function = "");
    
    /**
     * @brief Log an info message
     * 
     * @param message Log message
     * @param file Source file name (optional)
     * @param line Source line number (optional)
     * @param function Source function name (optional)
     * 
     * This method logs an info-level message. Use this method for
     * general informational messages about normal system operation.
     */
    void info(const std::string& message, const std::string& file = "",
              int line = 0, const std::string& function = "");
    
    /**
     * @brief Log a warning message
     * 
     * @param message Log message
     * @param file Source file name (optional)
     * @param line Source line number (optional)
     * @param function Source function name (optional)
     * 
     * This method logs a warning-level message. Use this method for
     * warnings about potential issues or unexpected conditions.
     */
    void warning(const std::string& message, const std::string& file = "",
                 int line = 0, const std::string& function = "");
    
    /**
     * @brief Log an error message
     * 
     * @param message Log message
     * @param file Source file name (optional)
     * @param line Source line number (optional)
     * @param function Source function name (optional)
     * 
     * This method logs an error-level message. Use this method for
     * errors that prevent normal operation but may not be fatal.
     */
    void error(const std::string& message, const std::string& file = "",
               int line = 0, const std::string& function = "");
    
    /**
     * @brief Log a critical message
     * 
     * @param message Log message
     * @param file Source file name (optional)
     * @param line Source line number (optional)
     * @param function Source function name (optional)
     * 
     * This method logs a critical-level message. Use this method for
     * critical errors that may cause system failure.
     */
    void critical(const std::string& message, const std::string& file = "",
                  int line = 0, const std::string& function = "");
    
    /**
     * @brief Check if logging is enabled for a specific level
     * 
     * @param level Log level to check
     * @return bool true if logging is enabled for this level
     * 
     * This method checks whether the logger has any handlers configured
     * and whether those handlers are enabled for the specified log level.
     */
    bool isEnabled(LogLevel level) const;
    
    /**
     * @brief Get the global logger instance
     * 
     * @return Logger& Global logger instance
     * 
     * This method returns the global logger instance, which can be used
     * for convenience logging from anywhere in the code. The global logger
     * is a singleton that is shared across all components.
     */
    static Logger& getGlobal();
    
    /**
     * @brief Set the global logger instance
     * 
     * @param logger Global logger instance
     * 
     * This method sets the global logger instance. The previous global
     * logger (if any) is replaced with the new one.
     */
    static void setGlobal(std::shared_ptr<Logger> logger);
    
    /**
     * @brief Get the current configuration file path
     * 
     * @return std::string Path to configuration file, or empty string if not set
     */
    std::string getConfigFilePath() const;
    
    /**
     * @brief Set the configuration file path
     * 
     * @param path Path to configuration file
     * 
     * This method sets the configuration file path for the logger,
     * allowing the logger to load configuration from files.
     */
    void setConfigFilePath(const std::string& path);
    
    /**
     * @brief Load configuration from file
     * 
     * @param path Path to configuration file
     * @return bool true if configuration was loaded successfully
     */
    bool loadFromFile(const std::string& path);
    
    /**
     * @brief Save configuration to file
     * 
     * @param path Path where configuration will be saved
     * @return bool true if configuration was saved successfully
     */
    bool saveToFile(const std::string& path) const;

private:
    /**
     * @brief Implementation details structure
     * 
     * Contains the actual implementation details using the Pimpl idiom.
     * This allows efficient compilation and reduces coupling between
     * header and implementation.
     */
    struct Impl;
    
    Impl* pImpl; ///< Pointer to implementation details
}; // class Logger

/**
 * @brief Get formatted timestamp string
 * 
 * @param time_point Time point to format
 * @return std::string Formatted timestamp string
 * 
 * This function formats a time point as a human-readable string
 * with date and time information.
 */
std::string formatTimestamp(const std::chrono::system_clock::time_point& time_point);

/**
 * @brief Get log level color code for terminal output
 * 
 * @param level Log level
 * @param use_colors Whether to use color codes
 * @return std::string ANSI color code string
 * 
 * This function returns the ANSI color code for the specified log level,
 * if colors are enabled.
 */
std::string getLogLevelColor(LogLevel level, bool use_colors = true);

/**
 * @brief Get log level name string
 * 
 * @param level Log level
 * @return std::string Human-readable log level name
 * 
 * This function returns the human-readable name of the log level.
 */
std::string getLogLevelName(LogLevel level);

// Convenience macros for file/line/function info (to be defined in implementation)
// #define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)
// #define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__)
// #define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)
// #define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__)
// #define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)

// Stream-style logging helper
class LogStream {
public:
    /**
     * @brief Constructor
     * 
     * @param level Log level
     * @param file Source file name
     * @param line Source line number
     * @param function Source function name
     */
    LogStream(LogLevel level, const std::string& file, int line, const std::string& function);
    
    /**
     * @brief Destructor
     * 
     * This method flushes the stream and logs the message.
     */
    ~LogStream();
    
    /**
     * @brief Stream insertion operator for any type
     * 
     * @tparam T Type of value to insert
     * @param value Value to insert into the stream
     * @return LogStream& Reference to this stream
     * 
     * This method inserts a value into the stream for later logging.
     */
    template<typename T>
    LogStream& operator<<(const T& value) {
        oss_ << value;
        return *this;
    }
    
    /**
     * @brief Stream insertion operator for strings
     * 
     * @param str String to insert
     * @return LogStream& Reference to this stream
     */
    LogStream& operator<<(const std::string& str);
    
    /**
     * @brief Stream insertion operator for string literals
     * 
     * @param str String literal to insert
     * @return LogStream& Reference to this stream
     */
    LogStream& operator<<(const char* str);
    
    /**
     * @brief Stream insertion operator for integer types
     * 
     * @tparam T Integer type
     * @param value Integer value to insert
     * @return LogStream& Reference to this stream
     */
    template<typename T>
    LogStream& operator<<(T value) requires std::is_integral_v<T> {
        oss_ << value;
        return *this;
    }
    
    /**
     * @brief Stream insertion operator for floating-point types
     * 
     * @tparam T Floating-point type
     * @param value Floating-point value to insert
     * @return LogStream& Reference to this stream
     */
    template<typename T>
    LogStream& operator<<(T value) requires std::is_floating_point_v<T> {
        oss_ << value;
        return *this;
    }
    
    /**
     * @brief Stream insertion operator for boolean values
     * 
     * @param value Boolean value to insert
     * @return LogStream& Reference to this stream
     */
    LogStream& operator<<(bool value);
    
    /**
     * @brief Stream insertion operator for pointers
     * 
     * @tparam T Pointer type
     * @param ptr Pointer value to insert
     * @return LogStream& Reference to this stream
     */
    template<typename T>
    LogStream& operator<<(T* ptr) {
        oss_ << static_cast<const void*>(ptr);
        return *this;
    }

private:
    LogLevel level_;           ///< Log level for this stream
    std::ostringstream oss_;  ///< Output string stream for message construction
    std::string file_;        ///< Source file name
    int line_;                ///< Source line number
    std::string function_;    ///< Source function name
    std::chrono::system_clock::time_point timestamp_;  ///< Timestamp when stream was created
}; // class LogStream

// Convenience macro for stream logging
// #define NLM_LOG_STREAM(level) nlm::LogStream(level, __FILE__, __LINE__, __func__)

} // namespace nlm

// Close NLM filename guard
#endif // NLM_LOGGER_HPP

