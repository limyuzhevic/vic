#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <thread>
#include <mutex>
#include <shared_mutex>

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

// Async logger policy interface
class ILogPolicy {
public:
    virtual ~ILogPolicy() = default;
    virtual void log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
};

// Console logger implementation with thread safety
class ConsoleLogger : public ILogPolicy {
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
    
    // Get file size limit for rotation
    void setMaxFileSize(size_t maxBytes);
    size_t getMaxFileSize() const;
    
    // Force flush
    void flush();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
    std::string formatTimestamp(const std::chrono::system_clock::time_point& timestamp) const;
};

// File logger implementation with rotation
class FileLogger : public ILogPolicy {
public:
    FileLogger();
    explicit FileLogger(LogLevel level);
    ~FileLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    // Configure file output
    void setOutputFile(const std::string& filepath);
    void setMaxFileSize(size_t maxBytes);
    void setFileRotationCount(size_t rotationCount);
    
    void flush();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Buffering logger for performance
class BufferedLogger : public ILogPolicy {
public:
    BufferedLogger();
    explicit BufferedLogger(LogLevel level);
    ~BufferedLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    // Set buffer size and flush interval
    void setBufferSize(size_t bufferSize);
    void setFlushInterval(std::chrono::milliseconds interval);
    
    void flush();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Main Logger class with thread-safe multi-sink support
class Logger {
public:
    Logger();
    ~Logger();
    
    // Disable copying
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    // Add logger sink
    void addLogger(std::shared_ptr<ILogPolicy> logger);
    
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
    
    // Flush all loggers
    void flushAll();
    
    // Get statistics
    size_t getLoggerCount() const;
    std::string getStatistics() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Thread-safe access
    mutable std::shared_mutex mutex_;
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
    
    // Support for formatted output
    LogStream& operator<<(const char* str);
    LogStream& operator<<(const std::string& str);
    
private:
    LogLevel level_;
    std::ostringstream oss_;
    std::string file_;
    int line_;
    std::string function_;
};

// Convenience macro for file/line/function info
#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)

// Async logging macro for performance
#define NLM_LOG_ASYNC(level, msg) nlm::Logger::getGlobal().log(level, msg, __FILE__, __LINE__, __func__)

} // namespace nlm

#include "Logger.cpp"
