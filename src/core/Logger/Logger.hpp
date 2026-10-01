#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <map>
#include <unordered_map>
#include <mutex>
#include <iostream>
#include <iomanip>
#include <atomic>

namespace nlm {

// Enhanced log severity levels with more granular control
enum class LogLevel {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4,
    Perf = 5,        // Performance metrics
    Memory = 6,      // Memory usage tracking
    Neural = 7,      // Neural system specific
    Cognition = 8,   // Cognitive processes
    Reward = 9,      // Reward/modulation signals
    Custom = 99      // User-defined level
};

// Log entry structure with additional metadata
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    
    // Additional metadata for advanced logging
    std::string category;
    std::map<std::string, std::string> metadata;
    uint64_t threadId;
    double relativeTime;  // Time since first log entry
};

// Performance metrics tracking
struct PerformanceMetrics {
    std::chrono::system_clock::time_point startTime;
    std::atomic<uint64_t> totalSteps{0};
    std::atomic<uint64_t> totalSpikes{0};
    std::atomic<double> avgFiringRate{0.0};
    std::atomic<size_t> memoryUsage{0};
    std::atomic<size_t> neuronCount{0};
    std::atomic<size_t> synapseCount{0};
    std::chrono::system_clock::time_point lastUpdate;
    
    void reset() {
        startTime = std::chrono::system_clock::now();
        totalSteps = 0;
        totalSpikes = 0;
        avgFiringRate = 0.0;
        memoryUsage = 0;
        neuronCount = 0;
        synapseCount = 0;
        lastUpdate = startTime;
    }
    
    double getElapsedSeconds() const {
        auto now = std::chrono::system_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime);
        return elapsed.count() / 1000.0;
    }
};

// Advanced logger interface with extended capabilities
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
    
    // Enhanced logging with context
    virtual void logWithContext(LogLevel level, const std::string& message,
                               const std::string& category = "",
                               const std::map<std::string, std::string>& metadata = {});
    
    // Performance logging
    virtual void logPerformance(const std::string& metric, double value,
                               const std::string& unit = "");
    
    // Memory logging
    virtual void logMemoryUsage(size_t used, size_t total, const std::string& context = "");
    
    // Neural activity logging
    virtual void logNeuralActivity(const std::string& event, int neuronId, double value);
    
    // Enable/disable specific log categories
    virtual void enableCategory(const std::string& category);
    virtual void disableCategory(const std::string& category);
    virtual bool isCategoryEnabled(const std::string& category) const;
    
    // Set custom formatter
    virtual void setFormatter(std::function<std::string(const LogEntry&)> formatter);
};

// Enhanced Console logger with advanced features
class ConsoleLogger : public ILogger {
public:
    ConsoleLogger();
    explicit ConsoleLogger(LogLevel level);
    ~ConsoleLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    // Enhanced console features
    void setUseColors(bool useColors);
    bool getUseColors() const;
    
    // Set output file (in addition to console)
    void setOutputFile(const std::string& filepath);
    void closeOutputFile();
    
    // Advanced formatting options
    void setTimestampFormat(const std::string& format);
    void setEnablePerformanceMetrics(bool enable);
    void setEnableMemoryTracking(bool enable);
    
    // Real-time monitoring
    void enableRealTimeMonitoring(bool enable);
    void setUpdateInterval(double seconds);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
    std::string formatTimestamp(const LogEntry& entry) const;
    std::string formatEntry(const LogEntry& entry) const;
    void updatePerformanceMetrics();
};

// Main enhanced Logger class with comprehensive features
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
    
    // Global logging functions with enhanced features
    void log(LogLevel level, const std::string& message,
             const std::string& file = "", int line = 0,
             const std::string& function = "", const std::string& category = "",
             const std::map<std::string, std::string>& metadata = {});
    
    // Convenience methods with category support
    void debug(const std::string& message, const std::string& file = "", int line = 0,
               const std::string& function = "", const std::string& category = "");
    void info(const std::string& message, const std::string& file = "", int line = 0,
              const std::string& function = "", const std::string& category = "");
    void warning(const std::string& message, const std::string& file = "", int line = 0,
                 const std::string& function = "", const std::string& category = "");
    void error(const std::string& message, const std::string& file = "", int line = 0,
               const std::string& function = "", const std::string& category = "");
    void critical(const std::string& message, const std::string& file = "", int line = 0,
                  const std::string& function = "", const std::string& category = "");
    
    // Performance and system metrics logging
    void logPerformanceMetric(const std::string& name, double value, const std::string& unit = "",
                             const std::string& context = "");
    void logMemoryUsage(size_t used, size_t total, const std::string& context = "");
    void logNeuralEvent(const std::string& event, int neuronId, double value,
                       const std::string& context = "");
    
    // Advanced logging helpers
    void logWithContext(LogLevel level, const std::string& message,
                        const std::string& category,
                        const std::map<std::string, std::string>& metadata = {});
    
    // Check if level is enabled
    bool isEnabled(LogLevel level) const;
    
    // Category management
    void enableCategory(const std::string& category);
    void disableCategory(const std::string& category);
    bool isCategoryEnabled(const std::string& category) const;
    
    // Performance metrics access
    const PerformanceMetrics& getPerformanceMetrics() const;
    void resetPerformanceMetrics();
    
    // Get global logger instance
    static Logger& getGlobal();
    
    // Set global logger
    static void setGlobal(std::shared_ptr<Logger> logger);
    
    // Export functionality for analysis
    std::vector<LogEntry> getRecentLogs(size_t count = 100) const;
    void exportLogs(const std::string& filepath, const std::vector<LogEntry>& logs) const;
    
    // Configuration
    void configure(const std::string& configFile);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Enhanced convenience macros
#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)

// Category-specific macros
#define NLM_LOG_NEURAL(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__, "NEURAL")
#define NLM_LOG_COGNITION(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__, "COGNITION")
#define NLM_LOG_REWARD(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__, "REWARD")
#define NLM_LOG_PERF(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__, "PERFORMANCE")

// Stream-style logging helper with enhanced capabilities
class LogStream {
public:
    LogStream(LogLevel level, const std::string& file, int line, const std::string& function);
    ~LogStream();
    
    template<typename T>
    LogStream& operator<<(const T& value) {
        oss_ << value;
        return *this;
    }
    
    // Enhanced context methods
    LogStream& withCategory(const std::string& category);
    LogStream& withMetadata(const std::string& key, const std::string& value);
    
private:
    LogLevel level_;
    std::ostringstream oss_;
    std::string file_;
    int line_;
    std::string function_;
    std::string category_;
    std::map<std::string, std::string> metadata_;
};

#define NLM_LOG_STREAM(level) nlm::LogStream(level, __FILE__, __LINE__, __func__)

// Performance monitoring macros
#define NLM_PERF_START(name) auto name##_start = std::chrono::high_resolution_clock::now()
#define NLM_PERF_END(name) do { \
    auto name##_end = std::chrono::high_resolution_clock::now(); \
    auto name##_duration = std::chrono::duration_cast<std::chrono::microseconds>(name##_end - name##_start); \
    nlm::Logger::getGlobal().logPerformanceMetric(#name, name##_duration.count(), "microseconds"); \
} while(0)

// Memory tracking macros
#define NLM_MEMORY_TRACK(context) \
    nlm::Logger::getGlobal().logMemoryUsage(getMemoryUsage(), getTotalMemory(), context)

} // namespace nlm
