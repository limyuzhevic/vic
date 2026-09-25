#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <fstream>
#include <mutex>
#include <queue>
#include <thread>
#include <atomic>
#include <iomanip>
#include <algorithm>
#include <stdexcept>

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
    
    LogEntry() : level(LogLevel::Info), message(), file(), line(0), function(), timestamp() {}
    LogEntry(LogLevel l, const std::string& msg, const std::string& file = "", int line = 0, const std::string& function = "")
        : level(l), message(msg), file(file), line(line), function(function), 
          timestamp(std::chrono::system_clock::now()) {}
    
    // Format timestamp for display
    std::string formatTimestamp() const {
        auto time = std::chrono::system_clock::to_time_t(timestamp);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
};

// Logger interface
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
    
    // Check if this logger is for a specific level
    virtual bool isEnabled(LogLevel level) const {
        return level >= getLevel();
    }
    
    // Virtual destructor for proper cleanup
    virtual void shutdown() {}
};

// Base logger implementation with common functionality
class BaseLogger : public ILogger {
protected:
    LogLevel level_;
    mutable std::mutex mutex_;
    
public:
    BaseLogger(LogLevel level = LogLevel::Info) : level_(level) {}
    
    void setLevel(LogLevel level) override {
        std::lock_guard<std::mutex> lock(mutex_);
        level_ = level;
    }
    
    LogLevel getLevel() const override {
        std::lock_guard<std::mutex> lock(mutex_);
        return level_;
    }
    
    bool isEnabled(LogLevel level) const override {
        std::lock_guard<std::mutex> lock(mutex_);
        return level >= level_;
    }
};

// Console logger implementation
class ConsoleLogger : public BaseLogger {
public:
    ConsoleLogger(LogLevel level = LogLevel::Info, bool useColors = true);
    ~ConsoleLogger() override;
    
    void log(const LogEntry& entry) override;
    
    // Enable/disable colors
    void setUseColors(bool useColors);
    bool getUseColors() const;
    
    // Set output file (in addition to console)
    void setOutputFile(const std::string& filepath);
    void closeOutputFile();
    bool hasOutputFile() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
    std::string formatEntry(const LogEntry& entry) const;
};

// Async logger implementation with queue and worker threads
class AsyncLogger : public BaseLogger {
public:
    AsyncLogger(size_t maxQueueSize = 10000, size_t numThreads = 2,
                LogLevel level = LogLevel::Info);
    ~AsyncLogger() override;
    
    void log(const LogEntry& entry) override;
    
    // Wait for all queued logs to be processed
    void flush();
    
    // Stop the logging threads
    void shutdown() noexcept override;
    
    // Check if logger is shutting down
    bool isStopping() const noexcept;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// File logger implementation with rotation
class FileLogger : public BaseLogger {
public:
    FileLogger(const std::string& filepath, bool enableRotation = true, 
               size_t maxFileSize = 10 * 1024 * 1024, size_t maxFiles = 5,
               LogLevel level = LogLevel::Info);
    ~FileLogger() override;
    
    void log(const LogEntry& entry) override;
    
    // Enable/disable colors in file output
    void setUseColors(bool useColors);
    bool getUseColors() const;
    
    // Close current file and reopen
    void rotate();
    
    // Get current log file path
    std::string getCurrentFilePath() const;
    
    // Set file append mode
    void setAppendMode(bool append);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string levelToString(LogLevel level) const;
    std::string formatEntry(const LogEntry& entry) const;
    std::string generateFileName(const std::string& basePath, int index) const;
    bool shouldRotate(size_t currentSize) const;
};

// Multi-logger that can handle multiple sinks
class MultiLogger : public BaseLogger {
public:
    MultiLogger(LogLevel level = LogLevel::Info);
    ~MultiLogger() override;
    
    void log(const LogEntry& entry) override;
    
    // Add console logger
    void addConsoleLogger(LogLevel level = LogLevel::Info, bool useColors = true);
    
    // Add file logger
    void addFileLogger(const std::string& filepath, bool enableRotation = true,
                       size_t maxFileSize = 10 * 1024 * 1024, size_t maxFiles = 5,
                       LogLevel level = LogLevel::Info);
    
    // Add async logger
    void addAsyncLogger(size_t maxQueueSize = 10000, size_t numThreads = 2,
                        LogLevel level = LogLevel::Info);
    
    // Remove all loggers
    void clearLoggers();
    
    // Get all loggers
    const std::vector<std::shared_ptr<ILogger>>& getLoggers() const;
    
    // Shutdown all loggers
    void shutdown() noexcept override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Main Logger class
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
    
    // Configure with common presets
    static void setupDefault();
    static void setupMinimal();
    static void setupSilent();
    static void setupFile(const std::string& filepath, bool rotation = true);
    static void setupConsole(LogLevel level = LogLevel::Info, bool colors = true);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

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
    
    // Allow chaining
    LogStream& operator<<(std::ostream& (*manip)(std::ostream&)) {
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

// Utility functions
namespace LogUtils {
    // Format timestamp for log entries
    std::string formatTimestamp(const std::chrono::system_clock::time_point& timestamp);
    
    // Convert log level to string
    std::string levelToString(LogLevel level);
    
    // Convert log level to color code
    std::string levelToColor(LogLevel level);
    
    // Escape special characters for safe logging
    std::string escapeLogMessage(const std::string& message);
    
    // Check if a string contains control characters
    bool containsControlChars(const std::string& str);
    
    // Get current timestamp
    std::chrono::system_clock::time_point now();
}

// Global log macros (thread-safe)
#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)

// Stream-style logging macro
#define NLM_LOG_STREAM(level) nlm::LogStream(level, __FILE__, __LINE__, __func__)

// Convenience macros for common patterns
#define NLM_LOG_PERF(start, end, label) \
    do { \
        auto _start = std::chrono::high_resolution_clock::now(); \
        auto _end = std::chrono::high_resolution_clock::now(); \
        auto _duration = std::chrono::duration_cast<std::chrono::microseconds>(_end - _start).count(); \
        NLM_LOG_INFO(label " took " + std::to_string(_duration) + " microseconds"); \
    } while(0)

#define NLM_LOG_ERROR_FMT(fmt, ...) \
    do { \
        std::stringstream _ss; \
        _ss << "ERROR: " << fmt; \
        NLM_LOG_ERROR(_ss.str()); \
    } while(0)

// Logging level checking macros
#define NLM_LOG_IS_DEBUG() nlm::Logger::getGlobal().isEnabled(nlm::LogLevel::Debug)
#define NLM_LOG_IS_INFO() nlm::Logger::getGlobal().isEnabled(nlm::LogLevel::Info)
#define NLM_LOG_IS_WARNING() nlm::Logger::getGlobal().isEnabled(nlm::LogLevel::Warning)
#define NLM_LOG_IS_ERROR() nlm::Logger::getGlobal().isEnabled(nlm::LogLevel::Error)

} // namespace nlm