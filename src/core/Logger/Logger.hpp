#pragma once

/**
 * Enhanced Logging System for NLM (Neural Learning Machine)
 * 
 * This file defines an improved logging system for the NLM neural simulation framework.
 * The system provides:
 * - Multi-level logging (Debug, Info, Warning, Error, Critical)
 * - Multiple output destinations (console, file, stream)
 * - Thread-safe logging with mutex protection
 * - Color-coded console output with optional formatting
 * - Global and per-instance logging management
 * - Log level filtering and categorization
 * - Enhanced error handling and recovery
 * - Performance optimizations for high-frequency logging
 * 
 * @author NLM Development Team
 * @version 2.0
 * @date 2026
 */

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <atomic>
#include <thread>
#include <map>

namespace nlm {

/**
 * Enhanced log severity levels with additional categories.
 * 
 * Expanded severity levels for more granular logging:
 * Debug = 0: Detailed debug information for development
 * Info = 1: General information messages
 * Warning = 2: Potentially harmful situations
 * Error = 3: Error conditions that should be handled
 * Critical = 4: Critical conditions requiring immediate attention
 * System = 5: System-level events and state changes
 * Performance = 6: Performance metrics and benchmarks
 * Memory = 7: Memory management events
 * Neural = 8: Neural network specific events
 * 
 * @note Higher levels imply lower priority (Error > Info)
 */
enum class LogLevel {
public:
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4,
    System = 5,
    Performance = 6,
    Memory = 7,
    Neural = 8
};

/**
 * Enhanced log entry structure with metadata.
 * 
 * Extended log entry with additional metadata for better log analysis:
 * level: Severity level of the log message
 * message: Log message content
 * file: Source file where log was generated
 * line: Line number in source file
 * function: Function name where log was generated
 * threadId: ID of thread that generated the log
 * processId: ID of process that generated the log
 * category: Log category for filtering and filtering
 * timestamp: Exact timestamp with nanosecond precision
 * 
 * @note Thread and process IDs enable better debugging in multi-threaded
 * environments and distributed systems.
 */
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    std::thread::id threadId;
    std::string category;
    
    LogEntry() : level(LogLevel::Info), line(0), 
                 threadId(std::this_thread::get_id()), category("general") {}
    
    LogEntry(LogLevel lvl, const std::string& msg, const std::string& file = "", int line = 0, 
             const std::string& function = "", const std::string& category = "general")
        : level(lvl), message(msg), file(file), line(line), function(function),
          timestamp(std::chrono::system_clock::now()), 
          threadId(std::this_thread::get_id()), category(category) {}
};

/**
 * Enhanced logger interface with additional features.
 * 
 * Extends basic ILogger with enhanced capabilities:
 * - Asynchronous logging support
 * - Log filtering by category and level
 * - Performance metrics and timing
 * - Log buffer management
 * - Error recovery mechanisms
 * 
 * @see ILogger for basic logging interface
 */
class ILogger {
public:
    virtual ~ILogger() = default;
    
    /**
     * Log a message with enhanced error handling.
     * 
     * @param entry Log entry to be processed
     * @return True if log was successful, false otherwise
     */
    virtual bool log(const LogEntry& entry) = 0;
    
    /**
     * Set minimum log level for this logger.
     * 
     * @param level Minimum log level (higher values = less verbose)
     */
    virtual void setLevel(LogLevel level) = 0;
    
    /**
     * Get current log level.
     * 
     * @return Current minimum log level
     */
    virtual LogLevel getLevel() const = 0;
    
    /**
     * Set log category filter.
     * 
     * @param categories Comma-separated list of categories to include
     */
    virtual void setCategories(const std::string& categories) = 0;
    
    /**
     * Check if category is enabled.
     * 
     * @param category Category to check
     * @return True if category is enabled
     */
    virtual bool isCategoryEnabled(const std::string& category) const = 0;
    
    /**
     * Flush all buffered logs.
     * 
     * Useful for ensuring logs are written before program exit.
     */
    virtual void flush() = 0;
    
    /**
     * Get logging statistics.
     * 
     * @return String containing logging statistics
     */
    virtual std::string getStatistics() const = 0;
};

/**
 * Enhanced console logger with advanced features.
 * 
 * Provides console logging with:
 * - Color-coded output with customizable themes
 * - Thread-safe concurrent logging
 * - Multiple file output support
 * - Advanced formatting and timestamps
 * - Performance monitoring
 * - Error recovery for file operations
 * 
 * @see ILogger for base interface
 */
class ConsoleLogger : public ILogger {
public:
    ConsoleLogger();
    explicit ConsoleLogger(LogLevel level);
    ~ConsoleLogger() override;
    
    bool log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    // Enhanced features
    void setCategories(const std::string& categories) override;
    bool isCategoryEnabled(const std::string& category) const override;
    void flush() override;
    std::string getStatistics() const override;
    
    // Enable/disable colors
    void setUseColors(bool useColors);
    bool getUseColors() const;
    
    // Set output file (in addition to console)
    void setOutputFile(const std::string& filepath);
    void closeOutputFile();
    
    // Advanced formatting
    void setTimestampFormat(const std::string& format);
    void setIncludeThreadId(bool include);
    void setIncludeCategory(bool include);
    
    // Performance monitoring
    void enablePerformanceMonitoring(bool enable);
    bool isPerformanceMonitoringEnabled() const;
    double getAverageLogTime() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
    bool shouldLog(const LogEntry& entry) const;
    std::string formatMessage(const LogEntry& entry) const;
};

/**
 * Main enhanced Logger class with comprehensive features.
 * 
 * Provides centralized logging management with:
 * - Multiple logger sinks (console, file, network, etc.)
 * - Thread-safe concurrent logging
 * - Advanced log filtering and categorization
 * - Performance monitoring and metrics
 * - Error recovery and resilience
 * - Configuration management
 * - Log buffer management
 * 
 * @see ILogger for base interface
 */
class Logger {
public:
    Logger();
    ~Logger();
    
    // Disable copying
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    // Enhanced logging management
    void addLogger(std::shared_ptr<ILogger> logger);
    void addConsoleLogger(LogLevel level = LogLevel::Info);
    void addFileLogger(const std::string& filepath, LogLevel level = LogLevel::Info);
    void removeLogger(std::shared_ptr<ILogger> logger);
    void clearLoggers();
    
    // Enhanced level management
    void setLevel(LogLevel level);
    void setLevelForLogger(std::shared_ptr<ILogger> logger, LogLevel level);
    LogLevel getLevelForLogger(std::shared_ptr<ILogger> logger) const;
    
    // Enhanced global logging functions
    bool log(LogLevel level, const std::string& message, 
             const std::string& file = "", int line = 0, 
             const std::string& function = "", const std::string& category = "general");
    
    // Enhanced convenience methods
    void debug(const std::string& message, const std::string& file = "", int line = 0, 
               const std::string& function = "", const std::string& category = "debug");
    void info(const std::string& message, const std::string& file = "", int line = 0, 
             const std::string& function = "", const std::string& category = "info");
    void warning(const std::string& message, const std::string& file = "", int line = 0, 
                 const std::string& function = "", const std::string& category = "warning");
    void error(const std::string& message, const std::string& file = "", int line = 0, 
               const std::string& function = "", const std::string& category = "error");
    void critical(const std::string& message, const std::string& file = "", int line = 0, 
                  const std::string& function = "", const std::string& category = "critical");
    
    // System-specific logging methods
    void neuralEvent(const std::string& message, const std::string& source = "");
    void performanceMetric(const std::string& name, double value, const std::string& unit = "");
    void memoryEvent(const std::string& message, size_t memoryBytes = 0);
    void systemEvent(const std::string& message);
    
    // Check if level is enabled
    bool isEnabled(LogLevel level) const;
    bool isEnabledForCategory(LogLevel level, const std::string& category) const;
    
    // Enhanced global logger management
    static Logger& getGlobal();
    static void setGlobal(std::shared_ptr<Logger> logger);
    
    // Statistics and monitoring
    std::string getGlobalStatistics();
    void exportLogs(const std::string& filepath);
    void importLogs(const std::string& filepath);
    
    // Performance optimization
    void enableAsyncLogging(bool enable);
    bool isAsyncLoggingEnabled() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * Enhanced stream-style logging helper with advanced features.
 * 
 * Provides stream-style logging with:
 * - Multi-level logging support
 * - Enhanced formatting and serialization
 * - Performance optimization
 * - Error recovery
 * - Thread safety
 * 
 * @see Logger for basic logging interface
 */
class LogStream {
public:
    LogStream(LogLevel level, const std::string& file, int line, const std::string& function, 
              const std::string& category = "general");
    ~LogStream();
    
    template<typename T>
    LogStream& operator<<(const T& value) {
        oss_ << value;
        return *this;
    }
    
    // Enhanced stream operations
    LogStream& operator<<(const std::string& value);
    LogStream& operator<<(const char* value);
    LogStream& operator<<(int value);
    LogStream& operator<<(double value);
    LogStream& operator<<(bool value);
    
    // Special formatting
    LogStream& operator<<(const std::chrono::system_clock::time_point& timepoint);
    LogStream& operator<<(std::thread::id threadId);
    
private:
    LogLevel level_;
    std::ostringstream oss_;
    std::string file_;
    int line_;
    std::string function_;
    std::string category_;
};

// Enhanced logging macros for convenience
#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)

// Enhanced stream macros
#define NLM_LOG_STREAM(level) nlm::LogStream(level, __FILE__, __LINE__, __func__)
#define NLM_LOG_STREAM_CATEGORY(level, category) nlm::LogStream(level, __FILE__, __LINE__, __func__, category)

// System event macros
#define NLM_NEURAL_EVENT(msg) nlm::Logger::getGlobal().neuralEvent(msg)
#define NLM_PERFORMANCE_METRIC(name, value, unit) nlm::Logger::getGlobal().performanceMetric(name, value, unit)
#define NLM_MEMORY_EVENT(msg, bytes) nlm::Logger::getGlobal().memoryEvent(msg, bytes)
#define NLM_SYSTEM_EVENT(msg) nlm::Logger::getGlobal().systemEvent(msg)

} ; // namespace nlm