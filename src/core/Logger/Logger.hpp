#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <mutex>
#include <map>
#include <ctime>
#include <filesystem>

using namespace std::chrono;

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
    
    // Additional metadata for enhanced logging
    std::string threadId;
    std::string processId;
    std::map<std::string, std::string> metadata;
};

// Log rotation configuration
struct LogRotationConfig {
    size_t maxFileSize;  // Maximum file size before rotation (bytes)
    size_t maxFiles;     // Maximum number of rotated files to keep
    bool compress;       // Compress rotated files
    std::string rotationTime; // Time of day to rotate (e.g., "00:00")
};

// Logger interface
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
    virtual void flush() = 0;
    
    // Get current log file size
    virtual size_t getLogFileSize() const = 0;
    
    // Check if rotation is needed
    virtual bool needsRotation() const = 0;
};

// Enhanced Console Logger with file support and rotation
class ConsoleLogger : public ILogger {
public:
    ConsoleLogger();
    explicit ConsoleLogger(LogLevel level);
    ~ConsoleLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    void flush() override;
    
    // Enable/disable colors
    void setUseColors(bool useColors);
    bool getUseColors() const;
    
    // Configure log rotation
    void configureRotation(const LogRotationConfig& config);
    
    // Set output file (in addition to console)
    void setOutputFile(const std::string& filepath);
    void closeOutputFile();
    
    // Log management
    void clear();
    void setMaxLogSize(size_t maxSize);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
    std::string formatTimestamp(const system_clock::time_point& timestamp) const;
};

// Async logger for performance
class AsyncLogger : public ILogger {
public:
    AsyncLogger(size_t queueSize = 10000);
    ~AsyncLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    void flush() override;
    
    // Configure async logging
    void setWorkerThreads(size_t threads);
    void setQueueSize(size_t size);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// File logger with rotation support
class FileLogger : public ILogger {
public:
    FileLogger();
    explicit FileLogger(const std::string& filepath);
    ~FileLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    void flush() override;
    
    // File configuration
    void setOutputFile(const std::string& filepath);
    void setFileMode(std::ios_base::openmode mode);
    void configureRotation(const LogRotationConfig& config);
    
    // Log management
    void clear();
    void setMaxLogSize(size_t maxSize);
    
    // Get log statistics
    size_t getLogFileSize() const override;
    bool needsRotation() const override;
    std::string getCurrentLogFile() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Main Logger class with multiple sinks
class Logger {
public:
    Logger();
    ~Logger();
    
    // Disable copying
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    // Add logger sink
    void addLogger(std::shared_ptr<ILogger> logger);
    
    // Remove specific logger
    void removeLogger(const std::shared_ptr<ILogger>& logger);
    
    // Remove all loggers
    void clearLoggers();
    
    // Set minimum level for all loggers
    void setLevel(LogLevel level);
    
    // Set level for specific logger type
    void setConsoleLevel(LogLevel level);
    void setFileLevel(LogLevel level);
    void setAsyncLevel(LogLevel level);
    
    // Global logging functions
    void log(LogLevel level, const std::string& message, 
             const std::string& file = "", int line = 0, 
             const std::string& function = "", const std::map<std::string, std::string>& metadata = {});
    
    // Convenience methods
    void debug(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const std::map<std::string, std::string>& metadata = {});
    void info(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const std::map<std::string, std::string>& metadata = {});
    void warning(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const std::map<std::string, std::string>& metadata = {});
    void error(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const std::map<std::string, std::string>& metadata = {});
    void critical(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const std::map<std::string, std::string>& metadata = {});
    
    // Stream-style logging
    LogStream debugStream(const std::string& file = "", int line = 0, const std::string& function = "");
    LogStream infoStream(const std::string& file = "", int line = 0, const std::string& function = "");
    LogStream warningStream(const std::string& file = "", int line = 0, const std::string& function = "");
    LogStream errorStream(const std::string& file = "", int line = 0, const std::string& function = "");
    LogStream criticalStream(const std::string& file = "", int line = 0, const std::string& function = "");
    
    // Check if level is enabled
    bool isEnabled(LogLevel level) const;
    
    // Flush all loggers
    void flushAll();
    
    // Get global logger instance
    static Logger& getGlobal();
    
    // Set global logger
    static void setGlobal(std::shared_ptr<Logger> logger);
    
    // Logger configuration
    void configureDefaultLogging(const std::string& logFile = "nlm.log");
    void enableAsyncLogging(size_t queueSize = 10000);
    
    // Get logging statistics
    size_t getTotalLogCount() const;
    std::string getLogStatistics() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Enhanced logging macros with thread safety
#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debugStream(__FILE__, __LINE__, __func__) << msg
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().infoStream(__FILE__, __LINE__, __func__) << msg
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warningStream(__FILE__, __LINE__, __func__) << msg
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().errorStream(__FILE__, __LINE__, __func__) << msg
#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().criticalStream(__FILE__, __LINE__, __func__) << msg

// Contextual logging macros
#define NLM_LOG_PERF(operation, duration_ms) \
    nlm::Logger::getGlobal().infoStream(__FILE__, __LINE__, __func__) \
    << "PERF: " << operation << " took " << duration_ms << "ms"

#define NLM_LOG_MEMORY_ALLOC(size_bytes) \
    nlm::Logger::getGlobal().debugStream(__FILE__, __LINE__, __func__) \
    << "MEM_ALLOC: " << size_bytes << " bytes"

#define NLM_LOG_ERROR_WITH_CONTEXT(msg, context) \
    nlm::Logger::getGlobal().errorStream(__FILE__, __LINE__, __func__) \
    << "ERROR: " << msg << " [Context: " << context << "]"

// LogStream implementation (moved from Logger.hpp for better encapsulation)
class LogStream {
public:
    LogStream(LogLevel level, const std::string& file, int line, const std::string& function, Logger* logger = nullptr);
    ~LogStream();
    
    template<typename T>
    LogStream& operator<<(const T& value) {
        oss_ << value;
        return *this;
    }
    
    // Add metadata
    LogStream& withMetadata(const std::string& key, const std::string& value);
    
private:
    LogLevel level_;
    std::ostringstream oss_;
    std::string file_;
    int line_;
    std::string function_;
    Logger* logger_;
    std::map<std::string, std::string> metadata_;
};

// Helper functions
namespace log_helpers {
    std::string formatDuration(std::chrono::milliseconds::rep ms);
    std::string formatBytes(size_t bytes);
    std::string getThreadId();
    std::string getProcessId();
    std::string generateLogFilename(const std::string& baseName);
}

} // namespace nlm
