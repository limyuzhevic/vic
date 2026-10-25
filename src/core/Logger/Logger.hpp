#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <unordered_map>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <queue>
#include <atomic>

namespace nlm {

// Enhanced log severity levels with more granularity
enum class LogLevel {
    Off = -1,
    Critical = 0,
    Error = 1,
    Warning = 2,
    Info = 3,
    Debug = 4,
    Trace = 5,
    Perf = 6,  // Performance timing
    Memory = 7 // Memory usage
};

// Log entry structure with enhanced metadata
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    
    // Additional context
    std::string threadId;
    std::string processId;
    double threadCpuTime;
    size_t threadMemoryUsage;
    
    // Performance tracking
    std::chrono::high_resolution_clock::duration duration;
    bool hasDuration;
    
    // Structured logging support
    std::unordered_map<std::string, std::string> keyValues;
    
    // Error context
    std::string errorCode;
    std::string errorContext;
    
    // Filter criteria
    std::string category;
    std::vector<std::string> tags;
};

// Logging output format options
enum class LogFormat {
    Plain,
    JSON,
    XML,
    CSV,
    Enhanced
};

// Log filtering criteria
struct LogFilter {
    LogLevel minLevel = LogLevel::Off;
    LogLevel maxLevel = LogLevel::Trace;
    std::string filePattern;
    std::string functionPattern;
    std::string category;
    std::vector<std::string> includeTags;
    std::vector<std::string> excludeTags;
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point endTime;
    std::unordered_map<std::string, std::string> keyValueFilters;
};

// Statistics tracking for logging
struct LogStatistics {
    std::atomic<uint64_t> totalMessages{0};
    std::atomic<uint64_t> debugMessages{0};
    std::atomic<uint64_t> infoMessages{0};
    std::atomic<uint64_t> warningMessages{0};
    std::atomic<uint64_t> errorMessages{0};
    std::atomic<uint64_t> criticalMessages{0};
    std::atomic<uint64_t> traceMessages{0};
    std::atomic<uint64_t> perfMessages{0};
    std::atomic<uint64_t> memoryMessages{0};
    
    std::chrono::steady_clock::time_point firstMessageTime;
    std::chrono::steady_clock::time_point lastMessageTime;
    std::chrono::nanoseconds totalProcessingTime;
    size_t peakMemoryUsage;
    std::atomic<size_t> currentMemoryUsage{0};
};

// Log rotation configuration
struct LogRotationConfig {
    bool enabled = false;
    size_t maxFileSize = 10 * 1024 * 1024; // 10MB
    size_t maxFiles = 5;
    std::chrono::system_clock::duration rotationInterval = std::chrono::hours(24);
    std::string backupSuffix = ".bak";
    bool compressBackups = false;
};

// Logger interface
class ILogger {
public:
    virtual ~ILogger() = default;
    
    // Core logging methods
    virtual void log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
    
    // Enhanced logging methods
    virtual void logStructured(LogLevel level, const std::string& message,
                              const std::unordered_map<std::string, std::string>& keyValues,
                              const std::string& file = "", int line = 0,
                              const std::string& function = "");
    
    virtual void logPerformance(LogLevel level, const std::string& message,
                               const std::chrono::high_resolution_clock::duration& duration,
                               const std::string& file = "", int line = 0,
                               const std::string& function = "");
    
    virtual void logMemory(LogLevel level, const std::string& message,
                          size_t memoryUsage,
                          const std::string& file = "", int line = 0,
                          const std::string& function = "");
    
    // Configuration
    virtual void setFormat(LogFormat format) = 0;
    virtual void setFilter(const LogFilter& filter) = 0;
    virtual LogFilter getFilter() const = 0;
    
    // Output configuration
    virtual void setOutputFile(const std::string& filepath) = 0;
    virtual void setRotationConfig(const LogRotationConfig& config) = 0;
    
    // Statistics
    virtual LogStatistics getStatistics() const = 0;
    virtual void resetStatistics() = 0;
    
    // Asynchronous logging support
    virtual bool setAsyncMode(bool enabled, size_t queueSize = 10000, 
                             size_t workerThreads = 2) = 0;
    virtual bool isAsyncMode() const = 0;
};

// Enhanced Console logger implementation
class ConsoleLogger : public ILogger {
public:
    ConsoleLogger();
    explicit ConsoleLogger(LogLevel level);
    ~ConsoleLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    // Enhanced features
    void setFormat(LogFormat format) override;
    void setFilter(const LogFilter& filter) override;
    LogFilter getFilter() const override;
    
    void setOutputFile(const std::string& filepath) override;
    void setRotationConfig(const LogRotationConfig& config) override;
    
    LogStatistics getStatistics() const override;
    void resetStatistics() override;
    
    bool setAsyncMode(bool enabled, size_t queueSize, size_t workerThreads) override;
    bool isAsyncMode() const override;
    
    // Specialized logging methods
    void logStructured(LogLevel level, const std::string& message,
                      const std::unordered_map<std::string, std::string>& keyValues,
                      const std::string& file, int line, const std::string& function) override;
    
    void logPerformance(LogLevel level, const std::string& message,
                       const std::chrono::high_resolution_clock::duration& duration,
                       const std::string& file, int line, const std::string& function) override;
    
    void logMemory(LogLevel level, const std::string& message,
                  size_t memoryUsage, const std::string& file, int line,
                  const std::string& function) override;
    
    // Configuration
    void setUseColors(bool useColors);
    bool getUseColors() const;
    void closeOutputFile();
    
    // Formatting helpers
    std::string formatEntry(const LogEntry& entry) const;
    std::string formatEntryJSON(const LogEntry& entry) const;
    std::string formatEntryXML(const LogEntry& entry) const;
    std::string formatEntryEnhanced(const LogEntry& entry) const;
    
    // Filter matching
    bool matchesFilter(const LogEntry& entry) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    void processAsyncQueue();
    void rotateLogFile();
    std::string getCurrentTimestamp() const;
};

// Main Logger class with enhanced features
class Logger {
public:
    Logger();
    ~Logger();
    
    // Disable copying
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    // Logger management
    void addLogger(std::shared_ptr<ILogger> logger);
    void removeLogger(std::shared_ptr<ILogger> logger);
    void clearLoggers();
    
    // Global logging functions
    void log(LogLevel level, const std::string& message,
             const std::string& file = "", int line = 0,
             const std::string& function = "", const std::string& category = "");
    
    // Enhanced logging methods
    void logStructured(LogLevel level, const std::string& message,
                      const std::unordered_map<std::string, std::string>& keyValues,
                      const std::string& file = "", int line = 0,
                      const std::string& function = "", const std::string& category = "");
    
    void logPerformance(const std::string& message,
                       const std::chrono::high_resolution_clock::duration& duration,
                       const std::string& file = "", int line = 0,
                       const std::string& function = "", const std::string& category = "");
    
    void logMemory(const std::string& message,
                  size_t memoryUsage,
                  const std::string& file = "", int line = 0,
                  const std::string& function = "", const std::string& category = "");
    
    // Convenience methods
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
    
    // Performance timing helpers
    class Timer {
    public:
        Timer(const std::string& operation, LogLevel level = LogLevel::Perf,
              const std::string& category = "performance");
        ~Timer();
        std::chrono::high_resolution_clock::duration elapsed() const;
        
    private:
        std::string operation_;
        LogLevel level_;
        std::string category_;
        std::chrono::high_resolution_clock::time_point start_;
    };
    
    // Memory tracking helpers
    class MemoryTracker {
    public:
        MemoryTracker(const std::string& operation, LogLevel level = LogLevel::Memory,
                     const std::string& category = "memory");
        ~MemoryTracker();
        size_t getCurrentMemoryUsage() const;
        
    private:
        std::string operation_;
        LogLevel level_;
        std::string category_;
    };
    
    // Configuration
    void setGlobalLevel(LogLevel level);
    void setGlobalFormat(LogFormat format);
    void setGlobalFilter(const LogFilter& filter);
    
    // Statistics
    LogStatistics getGlobalStatistics() const;
    void resetGlobalStatistics();
    
    // Global logger management
    static Logger& getGlobal();
    static void setGlobal(std::shared_ptr<Logger> logger);
    
    // Error context management
    void setErrorContext(const std::string& key, const std::string& value);
    std::string getErrorContext(const std::string& key) const;
    void clearErrorContext();
    
    // Thread-local logging
    static void setThreadName(const std::string& name);
    static std::string getThreadName();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
}; // namespace nlm

// Enhanced logging macros with performance timing
#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)

// Enhanced macros with context
#define NLM_LOG_PERF(msg) nlm::Logger::Timer __perfTimer__(msg)
#define NLM_LOG_MEM(msg) nlm::Logger::MemoryTracker __memTracker__(msg)

// Structured logging macro
#define NLM_LOG_STRUCT(level, msg, ...) do { \
    std::unordered_map<std::string, std::string> _keyValues = {__VA_ARGS__}; \
    nlm::Logger::getGlobal().logStructured(level, msg, _keyValues, __FILE__, __LINE__, __func__); \
} while(0)

// Performance timing macro
#define NLM_TIMED_BLOCK(name) nlm::Logger::Timer _timer_##name(#name, nlm::LogLevel::Perf, "block")

// Memory tracking macro
#define NLM_MEMORY_TRACK(block_name, mem_op) nlm::Logger::MemoryTracker _memTracker_##mem_op(mem_op, nlm::LogLevel::Memory, block_name)

// Async logging macro
#define NLM_LOG_ASYNC(level, msg) do { \
    nlm::Logger::getGlobal().log(level, msg, __FILE__, __LINE__, __func__); \
} while(0)

// Error with context macro
#define NLM_LOG_ERROR_WITH_CONTEXT(error_msg, error_code) do { \
    nlm::Logger::getGlobal().setErrorContext("error_code", error_code); \
    nlm::Logger::getGlobal().error(error_msg, __FILE__, __LINE__, __func__); \
    nlm::Logger::getGlobal().clearErrorContext(); \
} while(0)

// Performance checkpoint macro
#define NLM_PERF_CHECKPOINT(checkpoint_name) do { \
    static auto _checkpoint_time = std::chrono::high_resolution_clock::now(); \
    auto _now = std::chrono::high_resolution_clock::now(); \
    nlm::Logger::getGlobal().logPerformance("Checkpoint " #checkpoint_name " took " \
                                           + std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(_now - _checkpoint_time).count()) + " μs", \
                                           _now - _checkpoint_time, __FILE__, __LINE__, __func__, "perf_checkpoint"); \
    _checkpoint_time = _now; \
} while(0)

// Memory usage macro
#define NLM_LOG_MEMORY_USAGE(mem_label) do { \
    nlm::Logger::MemoryTracker _memTracker(mem_label, nlm::LogLevel::Memory, "memory_usage"); \
} while(0)