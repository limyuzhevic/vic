#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <unordered_map>
#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

namespace nlm {

// Enhanced Log severity levels
enum class LogLevel {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4
};

// Enhanced Log entry structure with additional metadata
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    
    // Additional metadata for advanced debugging
    std::thread::id threadId;
    uint64_t simulationStep;
    std::string agentId;
    std::string experimentId;
    
    // Performance tracking
    std::chrono::microseconds logDuration;
    
    // Structured data for advanced analysis
    std::unordered_map<std::string, std::string> structuredData;
};

// Log filter interface
class ILogFilter {
public:
    virtual ~ILogFilter() = default;
    virtual bool shouldLog(const LogEntry& entry) const = 0;
};

// Simple level-based filter
class LevelFilter : public ILogFilter {
public:
    LevelFilter(LogLevel minLevel) : minLevel_(minLevel) {}
    
    bool shouldLog(const LogEntry& entry) const override {
        return entry.level >= minLevel_;
    }
    
private:
    LogLevel minLevel_;
};

// Message pattern filter
class PatternFilter : public ILogFilter {
public:
    PatternFilter(const std::string& pattern) : pattern_(pattern) {}
    
    bool shouldLog(const LogEntry& entry) const override {
        return entry.message.find(pattern_) != std::string::npos;
    }
    
private:
    std::string pattern_;
};

// Structured data filter
class StructuredFilter : public ILogFilter {
public:
    StructuredFilter(const std::string& key, const std::string& value) 
        : key_(key), value_(value) {}
    
    bool shouldLog(const LogEntry& entry) const override {
        auto it = entry.structuredData.find(key_);
        return (it != entry.structuredData.end()) && (it->second == value_);
    }
    
private:
    std::string key_;
    std::string value_;
};

// Async logger implementation for high-performance logging
class AsyncLogger : public ILogger {
public:
    AsyncLogger(size_t queueSize = 10000);
    ~AsyncLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    // Flush all pending logs
    void flush();
    
    // Get queue statistics
    struct QueueStats {
        size_t size;
        size_t capacity;
        size_t dropped;
        double utilization;
    };
    
    QueueStats getQueueStats() const;
    
    // Configure thread pool
    void setWorkerThreadCount(size_t count);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Advanced Console logger with additional features
class AdvancedConsoleLogger : public ILogger {
public:
    AdvancedConsoleLogger();
    explicit AdvancedConsoleLogger(LogLevel level);
    ~AdvancedConsoleLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    // Enhanced output options
    void setUseColors(bool useColors);
    bool getUseColors() const;
    
    void setOutputFile(const std::string& filepath);
    void closeOutputFile();
    
    // Performance tracking
    void enablePerformanceTracking(bool enable);
    bool isPerformanceTrackingEnabled() const;
    
    // Log formatting options
    void setLogFormat(const std::string& format);
    void setTimestampFormat(const std::string& format);
    
    // File rotation
    void enableFileRotation(size_t maxSizeMB, int maxFiles);
    void rotateNow();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string formatTimestamp(const std::chrono::system_clock::time_point& timestamp) const;
    std::string formatLogEntry(const LogEntry& entry) const;
};

// Main Logger class with advanced features
class Logger {
public:
    Logger();
    ~Logger();
    
    // Disable copying
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    // Add logger sink
    void addLogger(std::shared_ptr<ILogger> logger);
    
    // Add filter
    void addFilter(std::shared_ptr<ILogFilter> filter);
    
    // Remove all loggers and filters
    void clearAll();
    
    // Set minimum level for all loggers
    void setLevel(LogLevel level);
    
    // Global logging functions
    void log(LogLevel level, const std::string& message,
             const std::string& file = "", int line = 0,
             const std::string& function = "");
    
    // Convenience methods with additional parameters
    void log(LogLevel level, const std::string& message, uint64_t step,
             const std::string& agent = "", const std::string& experiment = "",
             const std::string& file = "", int line = 0,
             const std::string& function = "");
    
    // Convenience methods
    void debug(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void info(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void warning(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void error(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void critical(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    // Advanced logging with context
    void debugWithContext(const std::string& message, uint64_t step, const std::string& agent = "", const std::string& experiment = "");
    void infoWithContext(const std::string& message, uint64_t step, const std::string& agent = "", const std::string& experiment = "");
    void warningWithContext(const std::string& message, uint64_t step, const std::string& agent = "", const std::string& experiment = "");
    void errorWithContext(const std::string& message, uint64_t step, const std::string& agent = "", const std::string& experiment = "");
    void criticalWithContext(const std::string& message, uint64_t step, const std::string& agent = "", const std::string& experiment = "");
    
    // Check if level is enabled
    bool isEnabled(LogLevel level) const;
    
    // Get log statistics
    struct LogStats {
        uint64_t totalLogs;
        uint64_t debugLogs;
        uint64_t infoLogs;
        uint64_t warningLogs;
        uint64_t errorLogs;
        uint64_t criticalLogs;
        std::chrono::microseconds averageLogDuration;
        double logsPerSecond;
    };
    
    LogStats getStats() const;
    
    // Get global logger instance
    static Logger& getGlobal();
    
    // Set global logger
    static void setGlobal(std::shared_ptr<Logger> logger);
    
    // Configure performance
    void enableAsyncLogging(bool enable);
    bool isAsyncLoggingEnabled() const;
    
    // Memory management
    void setLogBufferSize(size_t size);
    void optimizeForThroughput();
    void optimizeForLatency();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Enhanced convenience macros for advanced logging
#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)

// Enhanced stream-style logging helper with context
class AdvancedLogStream {
public:
    AdvancedLogStream(LogLevel level, const std::string& file, int line, const std::string& function,
                     uint64_t step = 0, const std::string& agent = "", const std::string& experiment = "");
    ~AdvancedLogStream();
    
    template<typename T>
    AdvancedLogStream& operator<<(const T& value) {
        oss_ << value;
        return *this;
    }
    
    // Add structured data
    AdvancedLogStream& withAgent(const std::string& agent) {
        agent_ = agent;
        return *this;
    }
    
    AdvancedLogStream& withExperiment(const std::string& experiment) {
        experiment_ = experiment;
        return *this;
    }
    
    AdvancedLogStream& withStep(uint64_t step) {
        step_ = step;
        return *this;
    }
    
    AdvancedLogStream& withData(const std::string& key, const std::string& value) {
        data_[key] = value;
        return *this;
    }
    
private:
    LogLevel level_;
    std::ostringstream oss_;
    std::string file_;
    int line_;
    std::string function_;
    uint64_t step_;
    std::string agent_;
    std::string experiment_;
    std::unordered_map<std::string, std::string> data_;
};

#define NLM_LOG_STREAM(level) nlm::AdvancedLogStream(level, __FILE__, __LINE__, __func__)

// Performance monitoring utilities
class PerformanceMonitor {
public:
    PerformanceMonitor();
    ~PerformanceMonitor();
    
    // Start monitoring for a specific operation
    void startOperation(const std::string& operationName);
    
    // End monitoring and log performance
    void endOperation(const std::string& operationName);
    
    // Get performance statistics
    struct OperationStats {
        std::string name;
        std::chrono::microseconds duration;
        size_t callCount;
        double averageDuration;
        double totalDuration;
    };
    
    std::vector<OperationStats> getStats() const;
    
    // Clear all statistics
    void clearStats();
    
    // Enable/disable monitoring
    void enableMonitoring(bool enable);
    bool isMonitoringEnabled() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Global performance monitor instance
PerformanceMonitor& getPerformanceMonitor();

// Convenient macros for performance tracking
#define NLM_PERF_START(op) nlm::getPerformanceMonitor().startOperation(op)
#define NLM_PERF_END(op) nlm::getPerformanceMonitor().endOperation(op)

} // namespace nlm
