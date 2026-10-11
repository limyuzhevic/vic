#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <system_error>
#include "../Error/Error.hpp"

namespace nlm {

/**
 * ILogger interface
 * @brief Abstract base class for logging implementations
 * @details Defines the interface for all logging sinks:
 *         - Message logging with level filtering
 *         - Log level configuration
 *         - Thread-safe operation
 *         
 * Thread safety: All methods must be thread-safe for concurrent logging
 * 
 * @warning Implementations must handle exceptions gracefully
 * @note Sink implementations should be lightweight and fast
 * @see ConsoleLogger for concrete implementation
 */
class ILogger {
public:
    virtual ~ILogger() = default;
    
    /**
     * @brief Log a message entry
     * @param entry LogEntry containing message and metadata
     * @post Message logged to sink
     * @details Entry includes:
     *         - Log level (Debug, Info, Warning, Error, Critical)
     *         - Message text
     *         - File and line information (if available)
     *         - Function name (if available)
     *         - Timestamp
     * 
     * @warning LogEntry contains runtime information; don't modify
     * @throws std::exception if logging fails
     */
    virtual void log(const LogEntry& entry) = 0;
    
    /**
     * @brief Set minimum log level
     * @param level Minimum log level to process
     * @post Log level threshold updated
     * @details Messages below this level will be filtered out:
     *         - Debug (0) is most verbose
     *         - Critical (4) is most severe
     *         - Default typically Info (1) or Warning (2)
     * 
     * @warning Thread-safe implementation required
     */
    virtual void setLevel(LogLevel level) = 0;
    
    /**
     * @brief Get current minimum log level
     * @return Current log level threshold
     * @post None
     * @note Used for filtering and query operations
     */
    virtual LogLevel getLevel() const = 0;
};

/**
 * ConsoleLogger - Console output logging implementation
 * @brief Concrete ILogger implementation for console output
 * @details High-performance console logging with:
 *         - Color-coded output for different log levels
 *         - File output in addition to console
 *         - Thread-safe operation
 *         - Exception handling and error recovery
 *         
 * Color scheme:
 * - Debug: Cyan
 * - Info: Green
 * - Warning: Yellow
 * - Error: Red
 * - Critical: Magenta
 * 
 * Memory model: Uses pointer-to-implementation (PIMPL) for stable ABI.
 * Thread safety: Thread-safe with mutex protection.
 * 
 * @warning File operations may fail (permissions, disk full, etc.)
 * @note Console output may fail if stdout redirected or closed
 * @see ILogger for interface documentation
 */
class ConsoleLogger : public ILogger {
public:
    /**
     * @brief Constructor - creates console logger with default settings
     * @post Logger initialized with:
     *       - Log level: Info
     *       - Colors: Enabled
     *       - File output: Disabled
     *       - Error state: Healthy
     */
    ConsoleLogger();
    
    /**
     * @brief Constructor with log level specification
     * @param level Initial log level threshold
     * @post Logger initialized with specified level
     * @warning Setting level too high may miss important messages
     */
    explicit ConsoleLogger(LogLevel level);
    
    /**
     * @brief Destructor - cleans up resources
     * @post File closed if open; mutex released
     */
    ~ConsoleLogger() override;
    
    /**
     * @brief Get console log level
     * @return Current log level
     * @post None
     */
    LogLevel getLevel() const override;
    
    /**
     * @brief Set log level
     * @param level New log level threshold
     * @post Log level updated
     */
    void setLevel(LogLevel level) override;
    
    /**
     * @brief Enable/disable colored output
     * @param useColors Whether to use ANSI color codes
     * @post Color setting updated
     * @note Colors disabled on Windows consoles by default
     */
    void setUseColors(bool useColors);
    
    /**
     * @brief Get color setting
     * @return true if colors enabled, false otherwise
     * @post None
     */
    bool getUseColors() const;
    
    /**
     * @brief Set output file (in addition to console)
     * @param filepath Path to log file
     * @post File opened for appending
     * @warning File must be writable
     * @warning Opens in append mode; does not truncate existing content
     * @see closeOutputFile() for file closing
     */
    void setOutputFile(const std::string& filepath);
    
    /**
     * @brief Close output file
     * @post File closed if open
     * @note Safe to call if file not open
     */
    void closeOutputFile();
    
    /**
     * @brief Check if file error occurred
     * @return true if file error, false otherwise
     * @post None
     * @note Indicates if log file operations failed
     */
    bool isFileError() const;
    
    /**
     * @brief Get last file error message
     * @return Error message string
     * @post None
     * @note Empty string if no error
     */
    std::string getLastFileError() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
};

/**
 * Logger - Main logging interface
 * @brief Central logging manager supporting multiple sinks
 * @details Provides comprehensive logging services:
 *         - Multiple log sinks (ConsoleLogger, file loggers, etc.)
 *         - Thread-safe operation with mutex protection
 *         - Log level filtering per sink
 *         - Error recovery and health monitoring
 *         - Global logger instance management
 *         
 * Architecture:
 * - Global logger instance (singleton pattern)
 * - Multiple sink support (delegation pattern)
 * - Hierarchical log level filtering
 * - Exception handling and error recovery
 * 
 * Memory model: Uses pointer-to-implementation (PIMPL) for stable ABI.
 * Thread safety: Thread-safe with mutex protection.
 * 
 * @warning Not thread-safe during construction/destruction
 * @note Use global logger via getGlobal() for most applications
 * @see ILogger for sink interface
 * @see ConsoleLogger for console logging
 */
class Logger {
public:
    /**
     * @brief Constructor - creates empty logger
     * @post Logger created with:
     *       - No sinks
     *       - Healthy state
     *       - Empty error list
     */
    Logger();
    
    /**
     * @brief Destructor - cleans up resources
     * @post All sinks removed; mutex released
     */
    ~Logger();
    
    /**
     * @brief Copy constructor - deleted to prevent copying
     * @details Loggers should not be copied to maintain consistency
     * @warning Attempting to copy will result in compilation error
     */
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    /**
     * @brief Move constructor - enables efficient logger transfer
     * @param other Logger to move from (will be invalidated)
     * @post this->pImpl = other->pImpl, other->pImpl = nullptr
     * @note Move semantics provide efficient transfer without deep copying
     */
    Logger(Logger&&) noexcept;
    
    /**
     * @brief Move assignment operator - enables efficient logger reassignment
     * @param other Logger to move from (will be invalidated)
     * @post this->pImpl = other->pImpl, other->pImpl = nullptr
     * @return Reference to this logger
     * @note Move assignment provides efficient transfer without deep copying
     */
    Logger& operator=(Logger&&) noexcept;
    
    /**
     * @brief Add logger sink
     * @param logger Shared pointer to ILogger implementation
     * @post Sink added to logger
     * @warning Logger cannot be null
     * @warning Sink ownership transferred to logger
     * @see clearLoggers() for removing all sinks
     */
    void addLogger(std::shared_ptr<ILogger> logger);
    
    /**
     * @brief Remove all logger sinks
     * @post All sinks removed; health issues cleared
     * @warning Resets logger to empty state
     * @see addLogger() for adding sinks
     */
    void clearLoggers();
    
    /**
     * @brief Set minimum log level for all sinks
     * @param level Minimum log level
     * @post All sinks set to specified level
     * @warning May filter out important messages
     */
    void setLevel(LogLevel level);
    
    /**
     * @brief Log message with full metadata
     * @param level Log level
     * @param message Message text
     * @param file Source file name
     * @param line Source line number
     * @param function Source function name
     * @post Message logged to all sinks
     * @details Metadata includes source location for debugging
     * 
     * @warning File and line parameters should be from __FILE__ and __LINE__
     * @warning Function parameter should be from __func__ or equivalent
     */
    void log(LogLevel level, const std::string& message,
             const std::string& file = "", int line = 0,
             const std::string& function = "");
    
    /**
     * @brief Log debug message
     * @param message Debug message
     * @param file Source file name
     * @param line Source line number
     * @param function Source function name
     * @post Message logged if level permits
     * @note Use NLM_LOG_DEBUG macro for easy usage
     */
    void debug(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    /**
     * @brief Log info message
     * @param message Info message
     * @param file Source file name
     * @param line Source line number
     * @param function Source function name
     * @post Message logged if level permits
     * @note Use NLM_LOG_INFO macro for easy usage
     */
    void info(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    /**
     * @brief Log warning message
     * @param message Warning message
     * @param file Source file name
     * @param line Source line number
     * @param function Source function name
     * @post Message logged if level permits
     * @note Use NLM_LOG_WARNING macro for easy usage
     */
    void warning(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    /**
     * @brief Log error message
     * @param message Error message
     * @param file Source file name
     * @param line Source line number
     * @param function Source function name
     * @post Message logged if level permits
     * @note Use NLM_LOG_ERROR macro for easy usage
     */
    void error(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    /**
     * @brief Log critical message
     * @param message Critical message
     * @param file Source file name
     * @param line Source line number
     * @param function Source function name
     * @post Message logged if level permits
     * @note Use NLM_LOG_CRITICAL macro for easy usage
     */
    void critical(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    /**
     * @brief Check if log level is enabled
     * @param level Log level to check
     * @return true if level enabled, false otherwise
     * @post None
     * @details Checks all sinks:
     *         - Returns true if any sink allows this level
     *         - Considers sink-specific thresholds
     */
    bool isEnabled(LogLevel level) const;
    
    /**
     * @brief Get global logger instance
     * @return Reference to global logger
     * @post Global logger created if not exists
     * @note Most applications use this global logger
     * @see setGlobal() for custom global logger
     */
    static Logger& getGlobal();
    
    /**
     * @brief Set global logger
     * @param logger Shared pointer to logger
     * @post Global logger replaced
     * @warning Used for advanced logging configurations
     * @note Most applications use getGlobal()
     */
    static void setGlobal(std::shared_ptr<Logger> logger);
    
    /**
     * @brief Check logger health
     * @return true if healthy, false otherwise
     * @post None
     * @details Health depends on:
     *         - All sinks operational
     *         - No unrecovered errors
     *         - Configuration valid
     */
    bool isHealthy() const;
    
    /**
     * @brief Get health issues
     * @return Vector of health issue strings
     * @post None
     * @note Empty if healthy
     */
    std::vector<std::string> getHealthIssues() const;
    
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
    
    // Get the message (for error reporting)
    std::string getMessage() const { return oss_.str(); }
    
private:
    LogLevel level_;
    std::ostringstream oss_;
    std::string file_;
    int line_;
    std::string function_;
};

#define NLM_LOG_STREAM(level) nlm::LogStream(level, __FILE__, __LINE__, __func__)

} // namespace nlm
