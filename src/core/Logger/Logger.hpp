#pragma once
/**
 * @file Logger.hpp
 * @brief NLM Logging System
 * 
 * This file provides a comprehensive logging system for NLM with multiple
 * logging levels, sinks, and output formats. The logging system is designed
 * to be thread-safe and efficient, with minimal overhead when logging is
 * disabled.
 * 
 * Features:
 * - Multiple log levels (Debug, Info, Warning, Error, Critical)
 * - Multiple log sinks (console, file, custom)
 * - Thread-safe operation
 * - Format customization
 * - Performance optimizations
 * - Global logger instance for convenience
 * 
 * @author NLM Research Team
 * @date Phase 6 Integration
 * @version 0.1.0
 */

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <mutex>
#include <unordered_map>

namespace nlm {

/**
 * @enum class LogLevel
 * @brief Log severity levels
 * 
 * The log level determines the importance and urgency of a log message.
 * Messages below the current threshold level are filtered out.
 */
enum class LogLevel {
public:
    /** Very detailed debug information, only in development */
    Debug = 0,
    /** General information about program execution */
    Info = 1,
    /** Warnings that don't prevent program execution */
    Warning = 2,
    /** Error conditions that may affect functionality */
    Error = 3,
    /** Critical errors that may cause program termination */
    Critical = 4
};

/**
 * @struct LogEntry
 * @brief A single log message with metadata
 * 
 * Each log entry contains the actual message, its severity level, and
 * metadata for context and debugging purposes.
 */
struct LogEntry {
    LogLevel level;                                     ///< Severity level
    std::string message;                                ///< Log message
    std::string file;                                   ///< Source file
    int line;                                           ///< Line number in file
    std::string function;                               ///< Function name
    std::chrono::system_clock::time_point timestamp;    ///< When log was created
    
    /**
     * @brief Construct a log entry with current timestamp
     */
    LogEntry() : level(LogLevel::Info), message(), file(), line(0), 
                 function(), timestamp(std::chrono::system_clock::now()) {}
    
    /**
     * @brief Construct a log entry with all fields
     */
    LogEntry(LogLevel lvl, const std::string& msg, const std::string& file = "",
             int line = 0, const std::string& function = "")
        : level(lvl), message(msg), file(file), line(line), function(function),
          timestamp(std::chrono::system_clock::now()) {}
    
    /**
     * @brief Get log level as string
     */
    static std::string levelToString(LogLevel level) {
        switch (level) {
            case LogLevel::Debug: return "DEBUG";
            case LogLevel::Info: return "INFO";
            case LogLevel::Warning: return "WARNING";
            case LogLevel::Error: return "ERROR";
            case LogLevel::Critical: return "CRITICAL";
            default: return "UNKNOWN";
        }
    }
    
    /**
     * @brief Get color code for terminal output (ANSI escape sequences)
     */
    std::string getColorCode() const {
        switch (level) {
            case LogLevel::Debug: return "\033[36m";      // Cyan
            case LogLevel::Info: return "\033[32m";       // Green
            case LogLevel::Warning: return "\033[33m";    // Yellow
            case LogLevel::Error: return "\033[31m";      // Red
            case LogLevel::Critical: return "\033[41;37m"; // Red background, white text
            default: return "\033[0m";
        }
    }
    
    /**
     * @brief Get reset code for terminal output
     */
    static std::string getResetCode() { return "\033[0m"; }
};

/**
 * @class ILogger
 * @brief Abstract interface for log sinks
 * 
 * This is the interface that all log sinks must implement. A sink is any
 * destination for log messages (console, file, network, etc.).
 */
class ILogger {
public:
    /**
     * @brief Virtual destructor
     */
    virtual ~ILogger() = default;
    
    /**
     * @brief Process a log entry
     * 
     * @param entry Log entry to process
     */
    virtual void log(const LogEntry& entry) = 0;
    
    /**
     * @brief Set the minimum log level for this sink
     */
    virtual void setLevel(LogLevel level) = 0;
    
    /**
     * @brief Get the current minimum log level for this sink
     */
    virtual LogLevel getLevel() const = 0;
    
    /**
     * @brief Check if this sink can handle messages at the given level
     */
    virtual bool canHandle(LogLevel level) const {
        return static_cast<int>(level) >= static_cast<int>(getLevel());
    }
};

/**
 * @class ConsoleLogger
 * @brief Console output log sink
 * 
 * This is the default logger sink that outputs messages to the console
 * with optional colors and file redirection.
 */
class ConsoleLogger : public ILogger {
public:
    /**
     * @brief Construct a console logger with default settings
     */
    ConsoleLogger();
    
    /**
     * @brief Construct a console logger with custom minimum level
     */
    explicit ConsoleLogger(LogLevel level);
    
    /**
     * @brief Virtual destructor
     */
    ~ConsoleLogger() override;
    
    // Delete copy operations
    ConsoleLogger(const ConsoleLogger&) = delete;
    ConsoleLogger& operator=(const ConsoleLogger&) = delete;
    
    // Enable move operations
    ConsoleLogger(ConsoleLogger&&) noexcept;
    ConsoleLogger& operator=(ConsoleLogger&&) noexcept;
    
    /**
     * @brief Process a log entry by outputting to console
     */
    void log(const LogEntry& entry) override;
    
    /**
     * @brief Set the minimum log level for this console logger
     */
    void setLevel(LogLevel level) override;
    
    /**
     * @brief Get the current minimum log level
     */
    LogLevel getLevel() const override;
    
    /**
     * @brief Enable or disable colored output
     * 
     * @param useColors true to enable colors, false to disable
     */
    void setUseColors(bool useColors);
    
    /**
     * @brief Check if colors are enabled
     */
    bool getUseColors() const;
    
    /**
     * @brief Redirect output to a file (in addition to console)
     * 
     * @param filepath Path to file for log output
     */
    void setOutputFile(const std::string& filepath);
    
    /**
     * @brief Close the output file
     */
    void closeOutputFile();
    
    /**
     * @brief Check if there's an open output file
     */
    bool hasOutputFile() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    /**
     * @brief Convert log level to string for display
     */
    std::string levelToString(LogLevel level) const;
    
    /**
     * @brief Get ANSI color code for log level
     */
    std::string levelToColor(LogLevel level) const;
    
    /**
     * @brief Format log entry as string
     */
    std::string formatEntry(const LogEntry& entry) const;
};

/**
 * @class Logger
 * @brief Main logging manager
 * 
 * This class manages all log sinks and provides the global logging interface.
 * It maintains a list of active log sinks and routes messages to them.
 */
class Logger {
public:
    /**
     * @brief Construct a new logger
     */
    Logger();
    
    /**
     * @brief Destroy the logger
     */
    ~Logger();
    
    // Delete copy operations
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    // Enable move operations
    Logger(Logger&&) noexcept;
    Logger& operator=(Logger&&) noexcept;
    
    /**
     * @brief Add a log sink
     * 
     * @param logger Shared pointer to logger sink
     */
    void addLogger(std::shared_ptr<ILogger> logger);
    
    /**
     * @brief Remove all log sinks
     */
    void clearLoggers();
    
    /**
     * @brief Set minimum log level for all sinks
     */
    void setLevel(LogLevel level);
    
    /**
     * @brief Log a message
     * 
     * This is the main logging method. Messages are sent to all registered
     * sinks that can handle the message level.
     * 
     * @param level Log level
     * @param message Log message
     * @param file Source file (optional, for debugging)
     * @param line Line number in source file (optional)
     * @param function Function name (optional)
     */
    void log(LogLevel level, const std::string& message, 
             const std::string& file = "", int line = 0, 
             const std::string& function = "");
    
    // =====================================================================
    // Convenience Methods
    // =====================================================================
    
    /**
     * @brief Log a debug message
     */
    void debug(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    /**
     * @brief Log an info message
     */
    void info(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    /**
     * @brief Log a warning message
     */
    void warning(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    /**
     * @brief Log an error message
     */
    void error(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    /**
     * @brief Log a critical message
     */
    void critical(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    // =====================================================================
    // Query Methods
    // =====================================================================
    
    /**
     * @brief Check if a log level is enabled
     * 
     * @param level Log level to check
     * @return true if messages at this level will be processed
     */
    bool isEnabled(LogLevel level) const;
    
    /**
     * @brief Get the number of active log sinks
     */
    size_t getLoggerCount() const;
    
    /**
     * @brief Get the global logger instance
     */
    static Logger& getGlobal();
    
    /**
     * @brief Set the global logger instance
     */
    static void setGlobal(std::shared_ptr<Logger> logger);
    
    /**
     * @brief Get the name of a log level
     */
    static std::string levelToString(LogLevel level);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Friendship declarations
    friend class ILogger;
    friend class ConsoleLogger;
};

// =====================================================================
// Convenience Macros
// =====================================================================

/**
 * @def NLM_LOG_DEBUG(msg)
 * @brief Debug log macro with file/line/function info
 */
#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)

/**
 * @def NLM_LOG_INFO(msg)
 * @brief Info log macro with file/line/function info
 */
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__)

/**
 * @def NLM_LOG_WARNING(msg)
 * @brief Warning log macro with file/line/function info
 */
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)

/**
 * @def NLM_LOG_ERROR(msg)
 * @brief Error log macro with file/line/function info
 */
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__)

/**
 * @def NLM_LOG_CRITICAL(msg)
 * @brief Critical log macro with file/line/function info
 */
#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)

// =====================================================================
// Stream-style Logging Helper
// =====================================================================

/**
 * @class LogStream
 * @brief Stream-style logging helper for complex messages
 * 
 * This class allows logging messages using the C++ stream operator<<
 * pattern, making it easy to format complex log messages.
 */
class LogStream {
public:
    /**
     * @brief Construct a log stream
     */
    LogStream(LogLevel level, const std::string& file, int line, const std::string& function);
    
    /**
     * @brief Destructor - logs the accumulated message
     */
    ~LogStream();
    
    /**
     * @brief Stream operator for adding values
     */
    template<typename T>
    LogStream& operator<<(const T& value) {
        oss_ << value;
        return *this;
    }
    
    /**
     * @brief Stream operator for adding strings
     */
    LogStream& operator<<(const std::string& value);
    
    /**
     * @brief Stream operator for adding C-style strings
     */
    LogStream& operator<<(const char* value);
    
    /**
     * @brief Stream operator for adding stream manipulator
     */
    template<typename T>
    LogStream& operator<<(std::ios_base& (*manip)(std::ios_base&)) {
        oss_ << manip;
        return *this;
    }

private:
    LogLevel level_;
    std::ostringstream oss_;
    std::string file_;
    int line_;
    std::string function_;
    std::shared_ptr<Logger> logger_;
};

/**
 * @def NLM_LOG_STREAM(level)
 * @brief Stream-style log macro
 */
#define NLM_LOG_STREAM(level) nlm::LogStream(level, __FILE__, __LINE__, __func__)

} // namespace nlm

// =====================================================================
// Convenience Function
// =====================================================================

/**
 * @brief Convenience function for global logging
 */
inline void nlmLog(nlm::LogLevel level, const std::string& message, 
                   const std::string& file = "", int line = 0,
                   const std::string& function = "") {
    nlm::Logger::getGlobal().log(level, message, file, line, function);
}

/**
 * @brief Convenience macros for easy logging
 */
#define NLM_LOG(level, msg) nlm::Logger::getGlobal().log(level, msg, __FILE__, __LINE__, __func__)

#define NLM_LOG_L(level, msg, file, line, func) nlm::Logger::getGlobal().log(level, msg, file, line, func)
