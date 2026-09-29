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
 * Defines the severity levels used for logging in NLM.
 * The severity hierarchy is: Debug < Info < Warning < Error < Critical
 */
enum class LogLevel {
    Debug = 0,      ///< Detailed debug information for troubleshooting
    Info = 1,        ///< General information about system operation
    Warning = 2,     ///< Warning conditions that don't prevent operation
    Error = 3,       ///< Error conditions that affect functionality
    Critical = 4     ///< Critical conditions that prevent further operation
};

/**
 * @brief Log entry structure
 * 
 * Contains all information about a single log entry including severity,
 * message, source location, and timestamp.
 */
struct LogEntry {
    LogLevel level;                                     ///< Severity level of the log entry
    std::string message;                                 ///< Log message content
    std::string file;                                    ///< Source file where log was generated
    int line;                                            ///< Line number in source file
    std::string function;                                ///< Function name where log was generated
    std::chrono::system_clock::time_point timestamp;     ///< When the log was generated
};

/**
 * @brief Abstract base class for log implementations
 * 
 * Defines the interface for all logger implementations. Log entries are
 * processed by implementing the log() method.
 */
class ILogger {
public:
    virtual ~ILogger() = default;
    
    /**
     * @brief Log a message
     * 
     * @param entry Log entry to be processed
     */
    virtual void log(const LogEntry& entry) = 0;
    
    /**
     * @brief Set the minimum log level
     * 
     * @param level Minimum severity level to log
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
 * Logs messages to the standard output (console) with optional colors
 * and file output capabilities.
 */
class ConsoleLogger : public ILogger {
public:
    ConsoleLogger();
    explicit ConsoleLogger(LogLevel level);
    ~ConsoleLogger() override;
    
    /**
     * @brief Log a message to console
     * 
     * @param entry Log entry to be processed
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
     * @return Current minimum log level
     */
    LogLevel getLevel() const override;
    
    /**
     * @brief Enable or disable colored output
     * 
     * @param useColors Whether to use colors in console output
     */
    void setUseColors(bool useColors);
    
    /**
     * @brief Check if colors are enabled
     * 
     * @return True if colors are enabled, false otherwise
     */
    bool getUseColors() const;
    
    /**
     * @brief Set output file for logging
     * 
     * @param filepath Path to the output file (logs are appended)
     */
    void setOutputFile(const std::string& filepath);
    
    /**
     * @brief Close the output file
     * 
     * Closes the currently open output file if one is open
     */
    void closeOutputFile();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    /**
     * @brief Convert log level to string representation
     * 
     * @param level Log level to convert
     * @return String representation of the log level
     */
    std::string levelToString(LogLevel level) const;
    
    /**
     * @brief Get ANSI color code for log level
     * 
     * @param level Log level to get color for
     * @return ANSI color code string
     */
    std::string levelToColor(LogLevel level) const;
};

/**
 * @brief Main Logger class
 * 
 * Provides centralized logging functionality for the entire NLM system.
 * The Logger class manages multiple logger implementations and provides
 * convenience methods for logging at different severity levels.
 */
class Logger {
public:
    Logger();
    ~Logger();
    
    // Disable copying to prevent issues with global logger management
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    /**
     * @brief Add a logger implementation
     * 
     * @param logger Shared pointer to a logger implementation
     */
    void addLogger(std::shared_ptr<ILogger> logger);
    
    /**
     * @brief Remove all loggers
     * 
     * Clears all registered logger implementations
     */
    void clearLoggers();
    
    /**
     * @brief Set minimum log level for all loggers
     * 
     * @param level Minimum severity level for all loggers
     */
    void setLevel(LogLevel level);
    
    /**
     * @brief Log a message with full context information
     * 
     * @param level Severity level of the message
     * @param message Log message content
     * @param file Source file where log was generated (optional)
     * @param line Line number in source file (optional)
     * @param function Function name where log was generated (optional)
     */
    void log(LogLevel level, const std::string& message, 
             const std::string& file = "", int line = 0, 
             const std::string& function = "");
    
    /**
     * @brief Log a debug level message
     * 
     * @param message Log message content
     * @param file Source file (optional, uses __FILE__ by default)
     * @param line Line number (optional, uses __LINE__ by default)
     * @param function Function name (optional, uses __func__ by default)
     */
    void debug(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    /**
     * @brief Log an info level message
     * 
     * @param message Log message content
     * @param file Source file (optional, uses __FILE__ by default)
     * @param line Line number (optional, uses __LINE__ by default)
     * @param function Function name (optional, uses __func__ by default)
     */
    void info(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    /**
     * @brief Log a warning level message
     * 
     * @param message Log message content
     * @param file Source file (optional, uses __FILE__ by default)
     * @param line Line number (optional, uses __LINE__ by default)
     * @param function Function name (optional, uses __func__ by default)
     */
    void warning(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    /**
     * @brief Log an error level message
     * 
     * @param message Log message content
     * @param file Source file (optional, uses __FILE__ by default)
     * @param line Line number (optional, uses __LINE__ by default)
     * @param function Function name (optional, uses __func__ by default)
     */
    void error(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    /**
     * @brief Log a critical level message
     * 
     * @param message Log message content
     * @param file Source file (optional, uses __FILE__ by default)
     * @param line Line number (optional, uses __LINE__ by default)
     * @param function Function name (optional, uses __func__ by default)
     */
    void critical(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    /**
     * @brief Check if a log level is enabled
     * 
     * @param level Log level to check
     * @return True if the level is enabled for logging, false otherwise
     */
    bool isEnabled(LogLevel level) const;
    
    /**
     * @brief Get the global logger instance
     * 
     * @return Reference to the global logger instance
     */
    static Logger& getGlobal();
    
    /**
     * @brief Set the global logger
     * 
     * @param logger Shared pointer to the global logger
     */
    static void setGlobal(std::shared_ptr<Logger> logger);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * @brief Convenience macro for debug level logging
 * 
 * Usage: NLM_LOG_DEBUG("Debug message here");
 */
#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)

/**
 * @brief Convenience macro for info level logging
 * 
 * Usage: NLM_LOG_INFO("Info message here");
 */
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__)

/**
 * @brief Convenience macro for warning level logging
 * 
 * Usage: NLM_LOG_WARNING("Warning message here");
 */
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)

/**
 * @brief Convenience macro for error level logging
 * 
 * Usage: NLM_LOG_ERROR("Error message here");
 */
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__)

/**
 * @brief Convenience macro for critical level logging
 * 
 * Usage: NLM_LOG_CRITICAL("Critical message here");
 */
#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)

/**
 * @brief Stream-style logging helper
 * 
 * Enables logging with stream operators for formatted output.
 * Usage: NLM_LOG_STREAM(nlm::LogLevel::Info) << "Stream logged " << value << " " << string;
 */
class LogStream {
public:
    LogStream(LogLevel level, const std::string& file, int line, const std::string& function);
    ~LogStream();
    
    /**
     * @brief Stream operator for logging values
     * 
     * @tparam T Type of value to log
     * @param value Value to log
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
 * @brief Macro for stream-style logging
 * 
 * Usage: NLM_LOG_STREAM(nlm::LogLevel::Info) << "Formatted " << value << " message";
 */
#define NLM_LOG_STREAM(level) nlm::LogStream(level, __FILE__, __LINE__, __func__)

} // namespace nlm
