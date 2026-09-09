#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <atomic>
#include <mutex>
#include <unordered_map>
#include <map>
#include <deque>
#include <thread>
#include <condition_variable>
#include <cstdint>
#include <shared_mutex>
#include <format>
#include <iomanip>

namespace nlm {

// Enhanced log severity levels
enum class LogLevel {
    Trace = 0,
    Debug = 1,
    Info = 2,
    Warning = 3,
    Error = 4,
    Critical = 5
};

// Log categories for better organization
class LogCategory {
public:
    static const std::string Neural;
    static const std::string Memory;
    static const std::string Sensory;
    static const std::string Motor;
    static const std::string Cognition;
    static const std::string Plasticity;
    static const std::string Development;
    static const std::string Neuromodulation;
    static const std::string Experiment;
    static const std::string Performance;
    static const std::string Config;
    static const std::string System;
    static const std::string All;
};

// Log entry with structured data
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    uint64_t threadId;
    std::string category;
    
    // Structured key-value pairs
    std::vector<std::pair<std::string, std::string>> kvPairs;
    
    // Log context
    std::string neuronId;
    std::string simulationStep;
    std::string regionId;
    
    // Performance tracking
    std::chrono::high_resolution_clock::time_point entryTime;
    size_t messageSize;
};

// Memory pool for log entries to reduce allocations
class LogEntryPool {
public:
    struct Entry {
        LogEntry entry;
        Entry* next;
    };
    
    LogEntryPool(size_t initialSize = 1024);
    ~LogEntryPool();
    
    LogEntry* acquire();
    void release(LogEntry* entry);
    void clear();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Logger statistics
struct LoggerStats {
    std::atomic<uint64_t> messagesLogged{0};
    std::atomic<uint64_t> messagesFiltered{0};
    std::atomic<uint64_t> entriesAllocated{0};
    std::atomic<uint64_t> entriesReused{0};
    std::atomic<double> totalProcessingTime{0.0};
    std::unordered_map<std::string, uint64_t> messagesByCategory;
    std::unordered_map<LogLevel, uint64_t> messagesByLevel;
    std::atomic<uint64_t> currentMemoryUsage{0};
};

// Abstract logger sink interface
class ILogger {
public:
    virtual ~ILogger() = default;
    
    // Log an entry
    virtual void log(const LogEntry& entry) = 0;
    
    // Set minimum log level
    virtual void setLevel(LogLevel level) = 0;
    
    // Get current log level
    virtual LogLevel getLevel() const = 0;
    
    // Check if a log level is enabled
    virtual bool isLevelEnabled(LogLevel level) const = 0;
    
    // Set category filter
    virtual void setCategoryFilter(const std::string& category, bool enabled) = 0;
    
    // Check if category is enabled
    virtual bool isCategoryEnabled(const std::string& category) const = 0;
    
    // Flush any buffered logs
    virtual void flush() = 0;
    
    // Get logger-specific statistics
    virtual LoggerStats getStats() const = 0;
    
    // Set formatting function
    virtual void setFormatter(std::function<std::string(const LogEntry&)> formatter) = 0;
};

// Async logger sink interface for performance
class IAsyncLogger {
public:
    virtual ~IAsyncLogger() = default;
    
    // Log an entry asynchronously
    virtual void logAsync(const LogEntry& entry) = 0;
    
    // Set worker thread count
    virtual void setWorkerThreadCount(size_t count) = 0;
    
    // Wait for all queued logs to complete
    virtual void waitForCompletion() = 0;
};

// Main Logger class with thread-safe operations
class Logger {
public:
    Logger();
    ~Logger();
    
    // Disable copying
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    // Add logger sink
    void addLogger(std::shared_ptr<ILogger> logger);
    
    // Add async logger sink
    void addAsyncLogger(std::shared_ptr<IAsyncLogger> asyncLogger);
    
    // Remove all loggers
    void clearLoggers();
    
    // Set minimum level for all loggers
    void setLevel(LogLevel level);
    
    // Set category filter globally
    void setCategoryFilter(const std::string& category, bool enabled);
    
    // Log with full metadata (core logging function)
    void log(LogLevel level, const std::string& message,
             const std::string& file = "", int line = 0,
             const std::string& function = "",
             const std::string& category = LogCategory::All,
             const std::vector<std::pair<std::string, std::string>>& kvPairs = {});
    
    // Convenience methods with category
    void log(LogLevel level, const std::string& message,
             const std::string& category,
             const std::vector<std::pair<std::string, std::string>>& kvPairs = {});
    
    // Convenience methods with full file info
    void logWithLocation(LogLevel level, const std::string& message,
                         const std::string& file, int line,
                         const std::string& function,
                         const std::string& category = LogCategory::All,
                         const std::vector<std::pair<std::string, std::string>>& kvPairs = {});
    
    // Convenience methods (backward compatibility)
    void log(LogLevel level, const std::string& message,
             const std::string& file = "", int line = 0,
             const std::string& function = "");
    
    // Structured logging convenience methods
    void trace(const std::string& message, const std::string& category = LogCategory::All,
               const std::vector<std::pair<std::string, std::string>>& kvPairs = {});
    void traceWithLocation(const std::string& message, const std::string& file, int line,
                           const std::string& function = "",
                           const std::string& category = LogCategory::All,
                           const std::vector<std::pair<std::string, std::string>>& kvPairs = {});
    
    void debug(const std::string& message, const std::string& category = LogCategory::All,
               const std::vector<std::pair<std::string, std::string>>& kvPairs = {});
    void debugWithLocation(const std::string& message, const std::string& file, int line,
                           const std::string& function = "",
                           const std::string& category = LogCategory::All,
                           const std::vector<std::pair<std::string, std::string>>& kvPairs = {});
    
    void info(const std::string& message, const std::string& category = LogCategory::All,
              const std::vector<std::pair<std::string, std::string>>& kvPairs = {});
    void infoWithLocation(const std::string& message, const std::string& file, int line,
                          const std::string& function = "",
                          const std::string& category = LogCategory::All,
                          const std::vector<std::pair<std::string, std::string>>& kvPairs = {});
    
    void warning(const std::string& message, const std::string& category = LogCategory::All,
                 const std::vector<std::pair<std::string, std::string>>& kvPairs = {});
    void warningWithLocation(const std::string& message, const std::string& file, int line,
                             const std::string& function = "",
                             const std::string& category = LogCategory::All,
                             const std::vector<std::pair<std::string, std::string>>& kvPairs = {});
    
    void error(const std::string& message, const std::string& category = LogCategory::All,
               const std::vector<std::pair<std::string, std::string>>& kvPairs = {});
    void errorWithLocation(const std::string& message, const std::string& file, int line,
                           const std::string& function = "",
                           const std::string& category = LogCategory::All,
                           const std::vector<std::pair<std::string, std::string>>& kvPairs = {});
    
    void critical(const std::string& message, const std::string& category = LogCategory::All,
                  const std::vector<std::pair<std::string, std::string>>& kvPairs = {});
    void criticalWithLocation(const std::string& message, const std::string& file, int line,
                              const std::string& function = "",
                              const std::string& category = LogCategory::All,
                              const std::vector<std::pair<std::string, std::string>>& kvPairs = {});
    
    // Check if level is enabled
    bool isEnabled(LogLevel level) const;
    
    // Check if category is enabled
    bool isCategoryEnabled(const std::string& category) const;
    
    // Get global logger instance
    static Logger& getGlobal();
    
    // Set global logger
    static void setGlobal(std::shared_ptr<Logger> logger);
    
    // Get global statistics
    static LoggerStats getGlobalStats();
    
    // Flush all loggers
    static void flushAll();
    
    // Wait for async completion
    static void waitForAsyncCompletion();
    
    // Memory pool management
    static LogEntryPool& getEntryPool();
    static void setEntryPoolSize(size_t maxEntries);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Stream-style logging helper for structured logging
class LogStream {
public:
    LogStream(LogLevel level, const std::string& category,
              const std::string& file, int line, const std::string& function);
    ~LogStream();
    
    // Key-value pair operators
    LogStream& operator<<(const std::pair<std::string, std::string>& kv);
    
    template<typename T>
    LogStream& operator<<(const T& value) {
        oss_ << value;
        return *this;
    }
    
    // Context setters
    LogStream& withThreadId(uint64_t threadId);
    LogStream& withNeuronId(const std::string& neuronId);
    LogStream& withStep(const std::string& step);
    LogStream& withRegionId(const std::string& regionId);
    
private:
    LogLevel level_;
    std::string category_;
    std::string file_;
    int line_;
    std::string function_;
    std::ostringstream oss_;
    uint64_t threadId_;
    std::string neuronId_;
    std::string step_;
    std::string regionId_;
};

// Async logging utility
namespace async {
    void startLoggingThreadPool(size_t threadCount = std::thread::hardware_concurrency());
    void stopLoggingThreadPool();
    bool isLoggingThreadPoolRunning();
}

// Configuration for logging system
struct LoggingConfig {
    LogLevel defaultLevel{LogLevel::Info};
    bool enableColors{true};
    bool enableFileLogging{false};
    std::string logFile{ "nlm.log" };
    size_t maxFileSize{ 10 * 1024 * 1024 }; // 10MB
    size_t maxFiles{ 5 };
    bool enableAsyncLogging{true};
    size_t asyncThreadCount{ std::thread::hardware_concurrency() };
    bool enablePerformanceStats{true};
    bool enableStructuredLogging{true};
    std::vector<std::string> enabledCategories;
    std::vector<std::string> disabledCategories;
};

// Initialize the global logging system
void initializeLogging(const LoggingConfig& config = LoggingConfig{});

// Shutdown the global logging system
void shutdownLogging();

// Helper function to add a category
void addLogCategory(const std::string& name, bool enabled = true);

// Helper function to enable/disable specific log categories
void enableLogCategory(const std::string& category, bool enabled = true);
void disableLogCategory(const std::string& category);

// Get list of all available log categories
std::vector<std::string> getLogCategories();

// Helper function to configure logging from file
bool configureLoggingFromFile(const std::string& configFile);

// Performance monitoring functions
void enablePerformanceMonitoring(bool enable);
bool isPerformanceMonitoringEnabled();

// Log statistics and diagnostics
LoggerStats getLoggingStatistics();
void logSystemInformation();
void logPerformanceStatistics();

// Specialized logging for neural events
void logNeuronCreated(const std::string& neuronId, const std::string& regionId);
void logSynapseCreated(const std::string& preNeuronId, const std::string& postNeuronId, float weight);
void logNeuronFired(const std::string& neuronId, const std::string& regionId, float voltage);
void logLearningEvent(const std::string& eventType, const std::string& category, float strength);

// Context-based logging for simulation steps
void logStepBegin(SimulationStep step);
void logStepEnd(SimulationStep step);
void logMemoryOperation(const std::string& operation, const std::string& category, size_t bytes);
void logCheckpointSaved(const std::string& filepath, size_t size, SimulationStep step);
void logCheckpointLoaded(const std::string& filepath, size_t size, SimulationStep step);

// Debug and development logging
void logDevelopmentStage(const std::string& stage, const std::string& description);
void logNeuromodulatorActivation(const std::string& neuromodulator, float level);
void logSensoryInput(const std::string& modality, const std::string& source);
void logActionGenerated(const std::string& actionType, const std::string& target);

// Performance measurement macros
#define NLM_PERF_MEASURE(name) nlm::PerformanceLogger perfLogger(name)

// Unified logging macros (backward compatible)
#define NLM_LOG_TRACE(msg) nlm::Logger::getGlobal().trace(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)

// Stream-style logging (backward compatible)
#define NLM_LOG_STREAM(level) nlm::LogStream(level, nlm::LogCategory::All, __FILE__, __LINE__, __func__)

// Structured logging macros
#define NLM_LOG_CAT_TRACE(category, msg) nlm::Logger::getGlobal().trace(msg, category)
#define NLM_LOG_CAT_DEBUG(category, msg) nlm::Logger::getGlobal().debug(msg, category)
#define NLM_LOG_CAT_INFO(category, msg) nlm::Logger::getGlobal().info(msg, category)
#define NLM_LOG_CAT_WARNING(category, msg) nlm::Logger::getGlobal().warning(msg, category)
#define NLM_LOG_CAT_ERROR(category, msg) nlm::Logger::getGlobal().error(msg, category)
#define NLM_LOG_CAT_CRITICAL(category, msg) nlm::Logger::getGlobal().critical(msg, category)

// Context-based logging macros
#define NLM_LOG_CTX_TRACE(neuronId, step, msg) nlm::Logger::getGlobal().traceWithContext(msg, neuronId, step)
#define NLM_LOG_CTX_DEBUG(neuronId, step, msg) nlm::Logger::getGlobal().debugWithContext(msg, neuronId, step)

// Performance measurement logging
class PerformanceLogger {
public:
    PerformanceLogger(const std::string& operation, const std::string& category = LogCategory::Performance);
    ~PerformanceLogger();
    
    void setThreshold(double threshold);
    double getElapsedTime() const;
    bool exceededThreshold() const;
    
private:
    std::chrono::high_resolution_clock::time_point startTime_;
    std::string operation_;
    std::string category_;
    double threshold_;
};

#define NLM_PERF_LOG(operation) nlm::PerformanceLogger perfLogger(operation)
#define NLM_PERF_LOG_CAT(operation, category) nlm::PerformanceLogger perfLogger(operation, category)

} // namespace nlm

} // namespace nlm