#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <unordered_map>
#include <mutex>
#include <thread>
#include <atomic>
#include <queue>
#include <condition_variable>

namespace nlm {

// Log severity levels
enum class LogLevel {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4
};

// Performance metrics for logging
struct PerformanceMetrics {
    std::chrono::duration<double> executionTime;
    size_t memoryUsage;
    int cpuUsage;
    int threadCount;
    uint64_t totalBytesProcessed;
    float averageFiringRate;
    float energyConsumption;
    float noveltyLevel;
    float curiosityLevel;
    float dopamineLevel;
    
    PerformanceMetrics()
        : executionTime(0.0)
        , memoryUsage(0)
        , cpuUsage(0)
        , threadCount(0)
        , totalBytesProcessed(0)
        , averageFiringRate(0.0f)
        , energyConsumption(0.0f)
        , noveltyLevel(0.0f)
        , curiosityLevel(0.0f)
        , dopamineLevel(0.0f) {}
};

// Log entry structure with performance metrics
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    PerformanceMetrics performanceMetrics;
    std::unordered_map<std::string, std::string> structuredData;
    
    LogEntry()
        : level(LogLevel::Info)
        , line(0)
        , timestamp(std::chrono::system_clock::now()) {}
};

// Logger interface
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
    
    // Enable/disable structured logging
    virtual void setStructuredLogging(bool enabled) = 0;
    virtual bool getStructuredLogging() const = 0;
    
    // Enable/disable performance metrics
    virtual void setPerformanceMetrics(bool enabled) = 0;
    virtual bool getPerformanceMetrics() const = 0;
    
    // Enable/disable async logging
    virtual void setAsyncLogging(bool enabled) = 0;
    virtual bool getAsyncLogging() const = 0;
    
    // Set log file rotation
    virtual void setLogRotation(bool enabled) = 0;
    virtual bool getLogRotation() const = 0;
    virtual void setMaxFileSize(size_t maxSize) = 0;
    virtual size_t getMaxFileSize() const = 0;
    
    // Set rate limiting
    virtual void setRateLimit(LogLevel level, int messagesPerSecond) = 0;
    virtual int getRateLimit(LogLevel level) const = 0;
};

// Performance monitoring helper
class PerformanceMonitor {
public:
    PerformanceMonitor() : startTime(std::chrono::high_resolution_clock::now()) {}
    
    // Start timing a function/block
    void startTiming(const std::string& name) {
        timingPoints[name] = std::chrono::high_resolution_clock::now();
    }
    
    // End timing and return duration
    double endTiming(const std::string& name) {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(endTime - timingPoints[name]);
        timingPoints.erase(name);
        return duration.count();
    }
    
    // Get current memory usage (platform-specific)
    size_t getMemoryUsage() const;
    
    // Get current CPU usage (platform-specific)
    float getCPUUsage() const;
    
    // Get thread count
    int getThreadCount() const;
    
    // Update performance metrics
    void updatePerformanceMetrics(PerformanceMetrics& metrics) const;
    
private:
    std::unordered_map<std::string, std::chrono::high_resolution_clock::time_point> timingPoints;
    std::chrono::high_resolution_clock::time_point startTime;
};

// Async logger implementation
class AsyncLogger : public ILogger {
public:
    AsyncLogger(size_t queueSize = 10000);
    ~AsyncLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    void setStructuredLogging(bool enabled) override;
    bool getStructuredLogging() const override;
    
    void setPerformanceMetrics(bool enabled) override;
    bool getPerformanceMetrics() const override;
    
    void setAsyncLogging(bool enabled) override;
    bool getAsyncLogging() const override;
    
    void setLogRotation(bool enabled) override;
    bool getLogRotation() const override;
    
    void setMaxFileSize(size_t maxSize) override;
    size_t getMaxFileSize() const override;
    
    void setRateLimit(LogLevel level, int messagesPerSecond) override;
    int getRateLimit(LogLevel level) const override;
    
    // Force flush of log queue
    void flush();
    
    // Get queue statistics
    size_t getQueueSize() const;
    size_t getProcessedMessages() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Main Logger class with enhanced features
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
    
    // Set level for specific logger
    void setLevel(std::shared_ptr<ILogger> logger, LogLevel level);
    
    // Global logging functions with performance metrics
    void log(LogLevel level, const std::string& message, 
             const std::string& file = "", int line = 0, 
             const std::string& function = "", 
             const PerformanceMonitor& monitor = PerformanceMonitor());
    
    // Convenience methods with performance metrics
    void debug(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "",
               const PerformanceMonitor& monitor = PerformanceMonitor());
    void info(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "",
              const PerformanceMonitor& monitor = PerformanceMonitor());
    void warning(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "",
                 const PerformanceMonitor& monitor = PerformanceMonitor());
    void error(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "",
               const PerformanceMonitor& monitor = PerformanceMonitor());
    void critical(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "",
                  const PerformanceMonitor& monitor = PerformanceMonitor());
    
    // Enhanced stream-style logging helper
    class LogStream {
    public:
        LogStream(LogLevel level, const std::string& file, int line, const std::string& function);
        ~LogStream();
        
        template<typename T>
        LogStream& operator<<(const T& value) {
            oss_ << value;
            return *this;
        }
        
        // Performance monitor support
        LogStream& operator<<(const PerformanceMonitor& monitor) {
            monitor_ = &monitor;
            return *this;
        }
        
    private:
        LogLevel level_;
        std::ostringstream oss_;
        std::string file_;
        int line_;
        std::string function_;
        const PerformanceMonitor* monitor_;
    };
    
    // Performance metrics helpers
    static PerformanceMonitor createPerformanceMonitor();
    
    // Check if level is enabled
    bool isEnabled(LogLevel level) const;
    
    // Get global logger instance
    static Logger& getGlobal();
    
    // Set global logger
    static void setGlobal(std::shared_ptr<Logger> logger);
    
    // Configuration
    struct Config {
        bool enableStructuredLogging = false;
        bool enablePerformanceMetrics = false;
        bool enableAsyncLogging = true;
        bool enableLogRotation = false;
        size_t maxFileSize = 10 * 1024 * 1024;  // 10 MB
        LogLevel minLevel = LogLevel::Info;
        std::string outputFile;
        bool useColors = true;
        
        // Rate limiting per level
        std::unordered_map<LogLevel, int> rateLimits;
    };
    
    static void setConfig(const Config& config);
    static Config getConfig();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Enhanced logging macros with performance support
#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)

// Stream-style logging helper with performance support
#define NLM_PERFORMANCE_MONITOR(name) nlm::PerformanceMonitor name;
#define NLM_PERFORMANCE_START(name) name.startTiming(#name)
#define NLM_PERFORMANCE_END(name) name.endTiming(#name)

#define NLM_LOG_PERFORMANCE(level, msg, monitor) nlm::Logger::getGlobal().log(level, msg, __FILE__, __LINE__, __func__, monitor)

// Convenience macros for common patterns
#define NLM_LOG_PERFORMANCE_INFO(msg, monitor) NLM_LOG_PERFORMANCE(nlm::LogLevel::Info, msg, monitor)
#define NLM_LOG_PERFORMANCE_DEBUG(msg, monitor) NLM_LOG_PERFORMANCE(nlm::LogLevel::Debug, msg, monitor)

// Structured logging helper
class StructuredLogger {
public:
    static void logStructured(const std::string& level, const std::string& message,
                             const std::unordered_map<std::string, std::string>& data = {});
    
    static void logWithPerformance(const std::string& level, const std::string& message,
                                  const PerformanceMonitor& monitor,
                                  const std::unordered_map<std::string, std::string>& data = {});
    
    static std::string toJSON(const LogEntry& entry);
};

} // namespace nlm
