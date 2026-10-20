#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>

namespace nlm {

// Log severity levels
enum class LogLevel {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4
};

// Log entry structure
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
};

/**
 * @class ILogger
 * @brief Abstract interface for logging implementations
 * @details Defines the contract for logging systems in the nlm framework.
 * All concrete logging implementations must inherit from this interface
 * and provide implementations for the three core operations: logging messages,
 * setting log level, and retrieving current log level.
 * 
 * Thread Safety: Implementations should be thread-safe for concurrent logging
 * operations. The interface methods should handle synchronization internally.
 * 
 * Performance: Log implementations should be optimized for minimal overhead
 * when logging is disabled (filtered by level). Critical implementations should
 * buffer logs when disabled to avoid unnecessary computation.
 * 
 * Usage Example:
 * @code
 * class MyLogger : public ILogger {
 * public:
 *     void log(const LogEntry& entry) override {
 *         // Implementation-specific logging
 *         std::cout << formatEntry(entry);
 *     }
 *     // ... other required methods
 * };
 * @endcode
 */
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
};

/**
 * @class ConsoleLogger
 * @brief Console-based logger implementation with color support and file output
 * @details Provides a configurable console logger that can output to both console
 * and optional log files. Supports colored output based on log levels and can
 * filter messages by severity level. The implementation uses a pImpl pattern
 * for efficient implementation hiding and thread-safe operations.
 * 
 * Features:
 * - Color-coded output for different log levels (ANSI escape sequences)
 * - Optional file logging in addition to console
 * - Configurable minimum log level
 * - Thread-safe concurrent logging
 * 
 * Thread Safety: This implementation is thread-safe for concurrent logging
 * operations. Multiple threads can call log() simultaneously without race conditions.
 * 
 * Performance: Uses buffered writing for console output. File I/O operations
 * are synchronized to prevent corruption when writing from multiple threads.
 * Color support adds minimal overhead when enabled.
 * 
 * Error Handling: If file output fails, the logger continues functioning
 * with console-only output and optionally logs the error to console.
 * 
 * Usage Example:
 * @code
 * // Create logger with info level and colors
 * ConsoleLogger logger(LogLevel::Info);
 * logger.setUseColors(true);
 * 
 * // Log a warning
 * logger.log(LogLevel::Warning, "Disk space low", "disk.cpp", 42, "checkDiskSpace");
 * 
 * // Set file output
 * logger.setOutputFile("/var/log/nlm.log");
 * @endcode
 * 
 * @see ILogger
 * @see LogEntry
 * @see LogLevel
 */
class ConsoleLogger : public ILogger {
public:
    ConsoleLogger();
    explicit ConsoleLogger(LogLevel level);
    ~ConsoleLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    // Enable/disable colors
    void setUseColors(bool useColors);
    bool getUseColors() const;
    
    // Set output file (in addition to console)
    void setOutputFile(const std::string& filepath);
    void closeOutputFile();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
};

/**
 * @class Logger
 * @brief Global logging manager and multi-sink logger implementation
 * @details The Logger class serves as the central logging coordinator, managing
 * multiple logger sinks and providing global logging functionality. It implements
 * the core logging interface with support for level filtering, multiple output
 * destinations, and thread-safe operations. This class follows the Singleton
 * pattern through getGlobal()/setGlobal() methods for global access.
 * 
 * Architecture:
 * - Manages multiple ILogger implementations as sinks
 * - Provides level-based filtering at the global level
 * - Supports both global and instance-specific configuration
 * - Offers convenience macros and stream-style logging
 * 
 * Thread Safety: This class is thread-safe for concurrent logging operations.
 * Multiple threads can call log() and other methods simultaneously without issues.
 * All internal data structures use appropriate synchronization.
 * 
 * Performance: Uses observer pattern for extensibility. Each log message goes
 * through all enabled sinks. When disabled (filtered by level), minimal overhead
 * is incurred with early filtering.
 * 
 * Memory Management: Logger uses shared_ptr for managing ILogger implementations,
 * ensuring proper cleanup and preventing dangling references.
 * 
 * Configuration Patterns:
 * @code
 * // Global configuration
 * Logger::getGlobal().setLevel(LogLevel::Info);
 * 
 * // Add file logger
 * auto fileLogger = std::make_shared<ConsoleLogger>(LogLevel::Debug);
 * fileLogger->setOutputFile("debug.log");
 * Logger::getGlobal().addLogger(fileLogger);
 * 
 * // Log with location info
 * NLM_LOG_INFO("Processing completed successfully");
 * @endcode
 * 
 * Error Handling: If any logger sink fails, the failure is logged but
 * doesn't prevent other sinks from functioning.
 * 
 * @see ILogger
 * @see LogLevel
 * @see LogEntry
 * @see NLM_LOG_DEBUG
 * @see NLM_LOG_INFO
 * @see NLM_LOG_WARNING
 * @see NLM_LOG_ERROR
 * @see NLM_LOG_CRITICAL
 * @see LogStream
 */
class Logger {
public:
    Logger();
    ~Logger();
    
    // Disable copying
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    // Add logger sink
    void addLogger(std::shared_ptr<ILogger> logger);
    
    // Remove all loggers
    void clearLoggers();
    
    // Set minimum level for all loggers
    void setLevel(LogLevel level);
    
    // Global logging functions
    void log(LogLevel level, const std::string& message, 
             const std::string& file = "", int line = 0, 
             const std::string& function = "");
    
    // Convenience methods
    void debug(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void info(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void warning(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void error(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void critical(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    // Check if level is enabled
    bool isEnabled(LogLevel level) const;
    
    // Get global logger instance
    static Logger& getGlobal();
    
    // Set global logger
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

/**
 * @class LogStream
 * @brief Stream-style logging helper for formatted output
 * @details Provides a convenient stream interface for constructing log messages
 * with automatic file/line/function information. This class simplifies logging
 * by allowing arbitrary object output through stream operators, similar to
 * std::cout but with integrated log metadata.
 * 
 * Usage:
 * @code
 * NLM_LOG_INFO << "Processing item " << itemId << " with value " << value;
 * @endcode
 * 
 * Thread Safety: Thread-safe for concurrent use. Each LogStream instance is
 * temporary and used only within its destructor, so no shared state.
 * 
 * Performance: Minimal overhead compared to formatted string construction.
 * Stream operators are efficient for multiple insertions.
 * 
 * Memory: Stream buffers are allocated on the stack and released immediately.
 * 
 * @see Logger
 * @see NLM_LOG_STREAM
 * @see NLM_LOG_DEBUG
 * @see NLM_LOG_INFO
 */
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
