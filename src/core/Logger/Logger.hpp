#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>

namespace nlm {

/**
 * @brief Log severity levels for NLM
 * 
 * Defines the severity levels used throughout the NLM system for
 * logging and error reporting. This provides a standardized way to
 * categorize log messages from least to most severe.
 */
enum class LogLevel {
    Debug = 0,      // Detailed debugging information for development
    Info = 1,        // General informational messages about system operation
    Warning = 2,     // Warnings about potential issues that don't prevent operation
    Error = 3,       // Errors that affect system functionality but allow continuation
    Critical = 4     // Critical errors that prevent normal system operation
};

/**
 * @brief Log entry structure for NLM
 * 
 * Contains complete information about a log entry including level,
 * message, source location, and timestamp. This structure provides
 * comprehensive context for log messages to aid in debugging and
 * system monitoring.
 */
struct LogEntry {
    LogLevel level;                                   // Severity level
    std::string message;                              // Log message
    std::string file;                                 // Source file name
    int line;                                         // Line number in file
    std::string function;                            // Function name
    std::chrono::system_clock::time_point timestamp;  // Timestamp when logged
};

/**
 * @brief Logger interface for NLM
 * 
 * Abstract base class for all logger implementations in NLM.
 * This interface defines the contract for log output sinks and
 * provides methods for level filtering and configuration.
 */
class ILogger {
public:
    virtual ~ILogger() = default;
    
    /**
     * @brief Log a message
     * 
     * Outputs a log entry to the logger's configured output.
     * 
     * @param entry The log entry to output
     */
    virtual void log(const LogEntry& entry) = 0;
    
    /**
     * @brief Set the minimum log level
     * 
     * Configures the logger to ignore messages below this level.
     * 
     * @param level Minimum severity level to log
     */
    virtual void setLevel(LogLevel level) = 0;
    
    /**
     * @brief Get the current minimum log level
     * 
     * @return Current minimum severity level
     */
    virtual LogLevel getLevel() const = 0;
};

/**
 * @brief Console logger implementation for NLM
 * 
 * Implementation of ILogger that outputs log messages to the console
 * with optional color coding and file output. This is the primary
 * logger used by default in NLM for real-time monitoring and debugging.
 */
class ConsoleLogger : public ILogger {
public:
    ConsoleLogger();
    explicit ConsoleLogger(LogLevel level);
    ~ConsoleLogger() override;
    
    /**
     * @brief Log a message to console
     * 
     * @param entry The log entry to output
     */
    void log(const LogEntry& entry) override;
    
    /**
     * @brief Set the minimum log level
     * 
     * @param level Minimum severity level to log
     */
    void setLevel(LogLevel level) override;
    
    /**
     * @brief Get the current minimum log level
     * 
     * @return Current minimum severity level
     */
    LogLevel getLevel() const override;
    
    /**
     * @brief Enable/disable colored output
     * 
     * Colors are useful for differentiating log levels in terminal output,
     * but may not work in all environments (e.g., log files, some IDEs).
     * 
     * @param useColors Whether to use color codes
     */
    void setUseColors(bool useColors);
    
    /**
     * @brief Check if colors are enabled
     * 
     * @return true if colors are enabled, false otherwise
     */
    bool getUseColors() const;
    
    /**
     * @brief Set output file for log messages
     * 
     * In addition to console output, messages can also be written to a file.
     * The file is opened in append mode.
     * 
     * @param filepath Path to the output file
     */
    void setOutputFile(const std::string& filepath);
    
    /**
     * @brief Close the output file
     * 
     * Closes the file previously opened with setOutputFile().
     */
    void closeOutputFile();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    /**
     * @brief Convert log level to string representation
     * 
     * @param level The log level to convert
     * @return String representation of the level
     */
    std::string levelToString(LogLevel level) const;
    
    /**
     * @brief Get color code for log level
     * 
     * Returns ANSI escape code for coloring terminal output.
     * 
     * @param level The log level
     * @return Color code string or empty if no colors
     */
    std::string levelToColor(LogLevel level) const;
};

/**
 * @brief Main Logger class for NLM
 * 
 * Centralized logging manager that coordinates multiple logger sinks.
 * This class provides the primary interface for logging in NLM and
 * manages global configuration for all loggers in the system.
 */
class Logger {
public:
    Logger();
    ~Logger();
    
    // Disable copying
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    /**
     * @brief Add a logger sink
     * 
     * Adds a new logger to the system. Messages are distributed to all
     * registered loggers. This enables multiple output destinations
     * (console, file, network, etc.) simultaneously.
     * 
     * @param logger Shared pointer to the logger to add
     */
    void addLogger(std::shared_ptr<ILogger> logger);
    
    /**
     * @brief Remove all logger sinks
     * 
     * Clears all registered loggers. After this call, no log messages
     * will be output until new loggers are added.
     */
    void clearLoggers();
    
    /**
     * @brief Set the minimum log level for all loggers
     * 
     * Configures all registered loggers to ignore messages below this level.
     * This provides a global filter for log verbosity.
     * 
     * @param level Minimum severity level to log
     */
    void setLevel(LogLevel level);
    
    /**
     * @brief Log a message with full context
     * 
     * Logs a message with complete source location information (file, line,
     * function). This is the core logging method used throughout NLM.
     * 
     * @param level Log severity level
     * @param message Log message content
     * @param file Source file name (defaults to "")
     * @param line Line number in file (defaults to 0)
     * @param function Function name (defaults to "")
     */
    void log(LogLevel level, const std::string& message,
             const std::string& file = "", int line = 0,
             const std::string& function = "");
    
    /**
     * @brief Convenience method for debug-level logging
     */
    void debug(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    /**
     * @brief Convenience method for info-level logging
     */
    void info(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    /**
     * @brief Convenience method for warning-level logging
     */
    void warning(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    /**
     * @brief Convenience method for error-level logging
     */
    void error(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    /**
     * @brief Convenience method for critical-level logging
     */
    void critical(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    /**
     * @brief Check if a log level is enabled
     * 
     * Determines whether log messages at the specified level would be
     * output by any configured logger.
     * 
     * @param level The log level to check
     * @return true if any logger is enabled for this level
     */
    bool isEnabled(LogLevel level) const;
    
    /**
     * @brief Get the global logger instance
     * 
     * Returns the singleton instance of the global logger. This is the
     * primary logger used throughout NLM for all log output.
     * 
     * @return Reference to the global logger
     */
    static Logger& getGlobal();
    
    /**
     * @brief Set the global logger
     * 
     * Replaces the global logger instance. This is an advanced operation
     * that should be used with caution as it affects all logging in the system.
     * 
     * @param logger New global logger instance
     */
    static void setGlobal(std::shared_ptr<Logger> logger);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * @brief Convenience macros for NLM logging
 * 
 * These macros provide easy-to-use logging throughout the codebase.
 * They automatically capture file, line, and function information.
 * 
 * Usage: NLM_LOG_INFO("Neuron count: " << neuronCount);
 */
#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)

/**
 * @brief Stream-style logging helper for complex messages
 * 
 * Enables stream-style logging similar to std::cout, allowing you to
 * build complex log messages using operator<<. The message is logged
 * when the LogStream object is destroyed.
 * 
 * Usage: NLM_LOG_STREAM(nlm::LogLevel::Error) << "Error processing neuron " << id << ": " << errorMsg;
 */
class LogStream {
public:
    LogStream(LogLevel level, const std::string& file, int line, const std::string& function);
    ~LogStream();
    
    /**
     * @brief Stream insertion operator
     * 
     * Appends a value to the log message stream.
     * 
     * @tparam T Type of value to append
     * @param value Value to append
     * @return Reference to this LogStream for chaining
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
 * @brief Stream logging macro
 * 
 * Usage: NLM_LOG_STREAM(nlm::LogLevel::Warning) << "Processing neuron " << id;
 */
#define NLM_LOG_STREAM(level) nlm::LogStream(level, __FILE__, __LINE__, __func__)

} // namespace nlm
