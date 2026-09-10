#pragma once

/**
 * Enhanced Logger Infrastructure with Comprehensive Error Handling
 * 
 * Phase 2 improvements for NLM logging system:
 * 
 * 1. Thread-safe operations with deadlock protection
 * 2. File I/O error recovery and automatic fallback
 * 3. Color-coded output for different log levels
 * 4. Configurable log levels per logger
 * 5. Exception-safe operations
 * 6. Performance optimizations for high-frequency logging
 * 7. Structured log entries with timestamps and metadata
 * 8. Multiple output targets (console, file, stream)
 * 9. Backpressure handling for slow file systems
 * 10. Memory-efficient string handling
 * 
 * This infrastructure supports the complex logging needs of an
 * integrated artificial brain system with multiple concurrent processes.
 */

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <mutex>
#include <fstream>
#include <iomanip>
#include <cstring>
#include <cerrno>
#include <stdexcept>
#include <atomic>
#include <condition_variable>
#include <queue>

namespace nlm {

// Log severity levels
enum class LogLevel {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4
};

// Log entry structure with full metadata
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    uint64_t threadId;
    uint64_t processId;
    
    LogEntry() : level(LogLevel::Info), line(0), threadId(0), processId(0) {
        timestamp = std::chrono::system_clock::now();
    }
};

// Logger interface with enhanced functionality
class ILogger {
public:
    virtual ~ILogger() = default;
    
    // Core logging interface
    virtual void log(const LogEntry& entry) = 0;
    
    // Configuration interface
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
    
    // Advanced configuration
    virtual void setMaxLogSize(size_t maxSize) {}  // In bytes
    virtual void setLogRotation(bool enableRotation) {}  // Daily rotation
    virtual bool isHealthy() const { return true; }  // Health check for monitoring
    
    // Diagnostic interface
    virtual std::string getError() const { return ""; }
    virtual void clearError() {}
};

// Console logger implementation with error recovery
class ConsoleLogger : public ILogger {
public:
    ConsoleLogger();
    explicit ConsoleLogger(LogLevel level);
    ~ConsoleLogger() override;
    
    // Core interface
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    // Advanced configuration
    void setUseColors(bool useColors) override;
    bool getUseColors() const override;
    
    // File output management
    void setOutputFile(const std::string& filepath) override;
    void closeOutputFile() override;
    void setMaxLogSize(size_t maxSize) override;
    void setLogRotation(bool enableRotation) override;
    
    // Health monitoring
    bool isHealthy() const override;
    std::string getError() const override;
    void clearError() override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
    void writeOutput(const std::string& output);
    void rotateLogFile();
    bool isFileHealthy() const;
};

// File-only logger for systems where console output is not desired
class FileLogger : public ILogger {
public:
    FileLogger(const std::string& filepath);
    ~FileLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    void setOutputFile(const std::string& filepath);
    void closeOutputFile();
    void setMaxLogSize(size_t maxSize) override;
    void setLogRotation(bool enableRotation) override;
    
    bool isHealthy() const override;
    std::string getError() const override;
    void clearError() override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Memory-efficient batch logger for high-frequency logging
class BatchLogger : public ILogger {
public:
    BatchLogger(size_t batchSize = 100);
    ~BatchLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    void setFlushInterval(std::chrono::milliseconds interval);
    void setMaxMemoryUsage(size_t maxMemory);
    
    bool isHealthy() const override;
    std::string getError() const override;
    void clearError() override;
    
    // Flush pending logs
    void flush();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Main Logger class with enhanced error handling and thread safety
class Logger {
public:
    Logger();
    ~Logger();
    
    // Disable copying
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    // Logger sink management
    void addLogger(std::shared_ptr<ILogger> logger);
    
    // Remove all loggers
    void clearLoggers();
    
    // Global configuration
    void setLevel(LogLevel level);
    void setGlobalLevel(LogLevel level);
    
    // Global logging functions with file/line/function info
    void log(LogLevel level, const std::string& message, 
             const std::string& file = "", int line = 0, 
             const std::string& function = "");
    
    // Convenience methods
    void debug(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void info(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void warning(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void error(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void critical(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    // Stream-style logging for complex messages
    class LogStream;
    
    // Check if level is enabled
    bool isEnabled(LogLevel level) const;
    
    // Get global logger instance (singleton pattern)
    static Logger& getGlobal();
    
    // Set global logger (for advanced use cases)
    static void setGlobal(std::shared_ptr<Logger> logger);
    
    // Health monitoring
    bool isHealthy() const;
    std::string getErrors() const;
    void clearErrors();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Convenience macros for file/line/function tracking
#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)

// Enhanced stream-style logging with formatting support
class Logger::LogStream {
public:
    LogStream(LogLevel level, const std::string& file, int line, const std::string& function);
    ~LogStream();
    
    // Template-based value insertion
    template<typename T>
    LogStream& operator<<(const T& value) {
        oss_ << value;
        return *this;
    }
    
    // Support for manipulators
    LogStream& operator<<(std::ostream& (*manip)(std::ostream&)) {
        oss_ << manip;
        return *this;
    }
    
    // Support for iomanip
    LogStream& operator<<(const std::ios_base& (*manip)(std::ios_base&)) {
        oss_ << manip;
        return *this;
    }
    
private:
    LogLevel level_;
    std::ostringstream oss_;
    std::string file_;
    int line_;
    std::string function_;
};

#define NLM_LOG_STREAM(level) nlm::Logger::LogStream(level, __FILE__, __LINE__, __func__)

// Utility functions for logging
namespace LogUtils {
    // Get thread-safe timestamp string
    std::string getTimestamp();
    
    // Get thread ID as string
    std::string getThreadId();
    
    // Get human-readable log level
    std::string levelToString(LogLevel level);
    
    // Format message with optional structured data
    std::string formatMessage(const std::string& message, 
                              const std::string& file = "",
                              int line = 0,
                              const std::string& function = "");
    
    // Escape special characters in messages for safe file output
    std::string escapeForFileOutput(const std::string& message);
}

} // namespace nlm
