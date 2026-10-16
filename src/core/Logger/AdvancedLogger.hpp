#pragma once

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <mutex>
#include <atomic>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <iostream>

namespace nlm {

// Advanced log severity levels
enum class LogLevel {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4,
    // Performance levels
    Trace = -1,           // Ultra-fine-grained timing/debug info
    Profile = -2,         // Performance profiling data
    Metric = -3,          // Performance metrics
    Benchmark = -4,       // Benchmark results
    // System levels
    System = 5,           // System-level events
    Config = 6,           // Configuration changes
    Plugin = 7,           // Plugin loading/unloading
    Network = 8,          // Network events
    Memory = 9,           // Memory management
    Thread = 10           // Thread-related events
};

// Advanced log entry structure
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    // Performance timing
    std::chrono::microseconds duration;
    // Context information
    std::string threadName;
    std::string threadId;
    std::unordered_map<std::string, std::string> metadata;
    // Performance metrics
    double cpuUsage;
    double memoryUsage;
    size_t allocatedMemory;
    
    LogEntry() : level(LogLevel::Info), line(0), duration(0), 
                 cpuUsage(0.0), memoryUsage(0.0), allocatedMemory(0) {}
    
    LogEntry(LogLevel lvl, const std::string& msg, 
             const std::string& file = "", int line = 0, 
             const std::string& function = "")
        : level(lvl), message(msg), file(file), line(line), 
          function(function), timestamp(std::chrono::system_clock::now()),
          duration(0), cpuUsage(0.0), memoryUsage(0.0), allocatedMemory(0) {}
};

// Performance metrics collector
class PerformanceMetrics {
public:
    struct Metrics {
        double cpuUsage = 0.0;
        double memoryUsage = 0.0;
        size_t allocatedMemory = 0;
        size_t peakMemory = 0;
        uint64_t totalSpikes = 0;
        double averageFiringRate = 0.0;
        uint64_t totalSynapses = 0;
        uint64_t totalNeurons = 0;
        std::chrono::microseconds lastStepTime = std::chrono::microseconds(0);
        uint64_t stepCount = 0;
        
        Metrics() = default;
    };
    
    void update(const Metrics& metrics) {
        std::lock_guard<std::mutex> lock(mutex_);
        currentMetrics_ = metrics;
        peakMemory_ = std::max(peakMemory_, metrics.allocatedMemory);
        metricsHistory_.push_back(metrics);
        if (metricsHistory_.size() > 1000) {
            metricsHistory_.pop_front();
        }
    }
    
    Metrics getCurrent() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return currentMetrics_;
    }
    
    Metrics getPeak() const {
        Metrics peak = currentMetrics_;
        peak.allocatedMemory = peakMemory_;
        return peak;
    }
    
    std::deque<Metrics> getHistory() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return metricsHistory_;
    }
    
    void reset() {
        std::lock_guard<std::mutex> lock(mutex_);
        currentMetrics_ = Metrics();
        peakMemory_ = 0;
    }

private:
    mutable std::mutex mutex_;
    Metrics currentMetrics_;
    size_t peakMemory_ = 0;
    std::deque<Metrics> metricsHistory_;
};

// Advanced logger interface with performance tracking
class IAdvancedLogger {
public:
    virtual ~IAdvancedLogger() = default;
    virtual void log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
    virtual void setPerformanceTracking(bool enable) = 0;
    virtual bool isPerformanceTrackingEnabled() const = 0;
    virtual PerformanceMetrics& getPerformanceMetrics() = 0;
    virtual void setMaxLogEntries(size_t maxEntries) = 0;
    virtual size_t getMaxLogEntries() const = 0;
    virtual void setLogFile(const std::string& filepath) = 0;
    virtual void enableStructuredLogging(bool enable) = 0;
    virtual bool isStructuredLoggingEnabled() const = 0;
};

// Performance-aware console logger
class AdvancedConsoleLogger : public IAdvancedLogger {
public:
    AdvancedConsoleLogger();
    explicit AdvancedConsoleLogger(LogLevel level);
    ~AdvancedConsoleLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    // Advanced configuration
    void setUseColors(bool useColors);
    bool getUseColors() const;
    
    // Output configuration
    void setOutputFile(const std::string& filepath);
    void closeOutputFile();
    
    // Performance tracking
    void setPerformanceTracking(bool enable) override;
    bool isPerformanceTrackingEnabled() const override;
    PerformanceMetrics& getPerformanceMetrics() override;
    
    // Advanced formatting
    void setMaxLogEntries(size_t maxEntries);
    size_t getMaxLogEntries() const;
    void enableStructuredLogging(bool enable) override;
    bool isStructuredLoggingEnabled() const override;
    
    // Performance filtering
    void setPerformanceThreshold(double threshold);
    void setMemoryThreshold(size_t threshold);
    
    // Time formatting
    std::string formatTimestamp(const std::chrono::system_clock::time_point& tp) const;
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
    
    // JSON formatting for structured logging
    std::string entryToJSON(const LogEntry& entry) const;
    
    // Performance metrics formatting
    std::string metricsToString(const PerformanceMetrics::Metrics& metrics) const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Main advanced logger class
class AdvancedLogger {
public:
    AdvancedLogger();
    ~AdvancedLogger();
    
    // Disable copying
    AdvancedLogger(const AdvancedLogger&) = delete;
    AdvancedLogger& operator=(const AdvancedLogger&) = delete;
    
    // Add logger sink
    void addLogger(std::shared_ptr<IAdvancedLogger> logger);
    
    // Remove all loggers
    void clearLoggers();
    
    // Set minimum level for all loggers
    void setLevel(LogLevel level);
    
    // Global performance tracking
    void setPerformanceTracking(bool enable);
    bool isPerformanceTrackingEnabled() const;
    PerformanceMetrics& getGlobalPerformanceMetrics();
    
    // Global logging functions
    void log(LogLevel level, const std::string& message, 
             const std::string& file = "", int line = 0, 
             const std::string& function = "");
    
    // Performance-aware logging functions
    void logPerformance(LogLevel level, const std::string& message, 
                       const std::string& file = "", int line = 0, 
                       const std::string& function = "");
    
    // Convenience methods with enhanced context
    void debug(const std::string& message, const std::string& file = "", 
               int line = 0, const std::string& function = "", 
               const std::unordered_map<std::string, std::string>& metadata = {});
    void info(const std::string& message, const std::string& file = "", 
              int line = 0, const std::string& function = "",
              const std::unordered_map<std::string, std::string>& metadata = {});
    void warning(const std::string& message, const std::string& file = "", 
                 int line = 0, const std::string& function = "",
                 const std::unordered_map<std::string, std::string>& metadata = {});
    void error(const std::string& message, const std::string& file = "", 
               int line = 0, const std::string& function = "",
               const std::unordered_map<std::string, std::string>& metadata = {});
    void critical(const std::string& message, const std::string& file = "", 
                  int line = 0, const std::string& function = "",
                  const std::unordered_map<std::string, std::string>& metadata = {});
    void system(const std::string& message, const std::string& file = "", 
                int line = 0, const std::string& function = "",
                const std::unordered_map<std::string, std::string>& metadata = {});
    void config(const std::string& message, const std::string& file = "", 
                int line = 0, const std::string& function = "",
                const std::unordered_map<std::string, std::string>& metadata = {});
    
    // Performance profiling functions
    void profile(const std::string& operation, 
                 std::chrono::microseconds duration,
                 const std::string& file = "", int line = 0,
                 const std::string& function = "");
    
    void metric(const std::string& name, double value,
                const std::string& unit = "",
                const std::string& file = "", int line = 0,
                const std::string& function = "");
    
    // Check if level is enabled
    bool isEnabled(LogLevel level) const;
    
    // Get global logger instance
    static AdvancedLogger& getGlobal();
    
    // Set global logger
    static void setGlobal(std::shared_ptr<AdvancedLogger> logger);
    
    // Streaming helper for complex logging
    class LogStream {
    public:
        LogStream(LogLevel level, const std::string& file, 
                  int line, const std::string& function,
                  std::shared_ptr<AdvancedLogger> logger);
        ~LogStream();
        
        template<typename T>
        LogStream& operator<<(const T& value) {
            oss_ << value;
            return *this;
        }
        
        // Add metadata
        LogStream& addMetadata(const std::string& key, const std::string& value);
        
    private:
        LogLevel level_;
        std::ostringstream oss_;
        std::string file_;
        int line_;
        std::string function_;
        std::shared_ptr<AdvancedLogger> logger_;
    };
    
    // Stream-style logging macro
    LogStream& stream(LogLevel level, const std::string& file, 
                      int line, const std::string& function);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Global macros for advanced logging
#define NLM_LOG_TRACE(msg, ...) nlm::AdvancedLogger::getGlobal().debug(
    "[TRACE] " + std::string(msg), __FILE__, __LINE__, __func__)

#define NLM_LOG_PROFILE(op, duration, ...) nlm::AdvancedLogger::getGlobal().profile(
    op, std::chrono::microseconds(duration), __FILE__, __LINE__, __func__)

#define NLM_LOG_METRIC(name, value, unit, ...) nlm::AdvancedLogger::getGlobal().metric(
    name, value, unit, __FILE__, __LINE__, __func__)

#define NLM_LOG_STREAM_ADVANCED(level) nlm::AdvancedLogger::getGlobal().stream(
    level, __FILE__, __LINE__, __func__)

// Convenience macros for common operations
#define NLM_LOG_PERFORMANCE(msg, ...) nlm::AdvancedLogger::getGlobal().logPerformance(
    nlm::LogLevel::Metric, msg, __FILE__, __LINE__, __func__)

#define NLM_LOG_SYSTEM_EVENT(msg, ...) nlm::AdvancedLogger::getGlobal().system(
    msg, __FILE__, __LINE__, __func__)

} // namespace nlm