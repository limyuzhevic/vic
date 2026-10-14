#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <mutex>
#include <unordered_map>
#include <atomic>
#include <condition_variable>
#include <thread>
#include <queue>
#include <map>
#include <iomanip>
#include <ctime>
#include <algorithm>

namespace nlm {

// Enhanced LogLevel with additional categories
enum class LogLevel {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4,
    // Enhanced levels for specific categories
    NeuralDebug = 5,      // Detailed neural simulation debug
    PlasticityDebug = 6,  // Plasticity rule details
    MemoryDebug = 7,      // Memory system debug
    Performance = 8,      // Performance metrics
    System = 9,           // System-level events
    UserAction = 10,      // User interactions
    Validation = 11,      // Validation and testing
    Security = 12,        // Security events
    Network = 13,         // Network events (if applicable)
    Config = 14,          // Configuration changes
    Experiment = 15       // Experiment-specific logging
};

// Enhanced LogEntry with additional metadata
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    
    // Enhanced metadata for structured logging
    std::string component;           // Log component (e.g., "brain", "plasticity", "memory")
    std::string correlationId;       // For tracing related events
    std::string userId;              // User identifier (if applicable)
    std::string sessionId;           // Session identifier
    std::unordered_map<std::string, std::string> context;  // Arbitrary key-value context
    
    // Error-specific information
    std::string errorCode;
    std::string errorCategory;
    std::string stackTrace;
    
    // Performance metrics
    double durationMs;               // For timing events
    uint64_t memoryUsage;            // For memory tracking
    
    LogEntry() : level(LogLevel::Info), line(0), durationMs(0.0), memoryUsage(0) {}
    
    // Helper constructor for basic logging
    LogEntry(LogLevel lvl, const std::string& msg, 
             const std::string& file = "", int lineNum = 0, 
             const std::string& func = "")
        : level(lvl), message(msg), file(file), line(lineNum), 
          function(func), timestamp(std::chrono::system_clock::now()),
          durationMs(0.0), memoryUsage(0) {}
};

// Enhanced ILogger interface with additional methods
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
    
    // Enhanced features
    virtual void setFilter(std::function<bool(const LogEntry&)> filter) = 0;
    virtual void setFormatter(std::function<std::string(const LogEntry&)> formatter) = 0;
    virtual void setErrorHandler(std::function<void(const LogEntry&)> errorHandler) = 0;
    virtual void flush() = 0;
    virtual bool isAsync() const { return false; }
    
    // Component-specific filtering
    virtual void setComponentFilter(const std::string& component, bool enabled) = 0;
    virtual bool isComponentEnabled(const std::string& component) const = 0;
};

// Async logger for high-throughput scenarios
class AsyncLogger : public ILogger {
public:
    AsyncLogger(size_t maxQueueSize = 10000, size_t workerThreads = 2);
    ~AsyncLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    void setFilter(std::function<bool(const LogEntry&)> filter) override;
    void setFormatter(std::function<std::string(const LogEntry&)> formatter) override;
    void setErrorHandler(std::function<void(const LogEntry&)> errorHandler) override;
    void flush() override;
    bool isAsync() const override { return true; }
    
    void setComponentFilter(const std::string& component, bool enabled) override;
    bool isComponentEnabled(const std::string& component) const override;
    
    void setLogFile(const std::string& filepath, size_t maxFileSize = 10485760, // 10MB
                    size_t maxFiles = 5, bool rotateDaily = true);
    void setFormat(const std::string& format); // Custom format string
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Console logger with enhanced features
class EnhancedConsoleLogger : public ILogger {
public:
    EnhancedConsoleLogger();
    explicit EnhancedConsoleLogger(LogLevel level);
    ~EnhancedConsoleLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    void setFilter(std::function<bool(const LogEntry&)> filter) override;
    void setFormatter(std::function<std::string(const LogEntry&)> formatter) override;
    void setErrorHandler(std::function<void(const LogEntry&)> errorHandler) override;
    void flush() override;
    
    void setComponentFilter(const std::string& component, bool enabled) override;
    bool isComponentEnabled(const std::string& component) const override;
    
    void setUseColors(bool useColors);
    bool getUseColors() const;
    void setOutputFile(const std::string& filepath);
    void closeOutputFile();
    
    // Enhanced formatting options
    void enableStructuredLogging(bool enable);
    void setTimestampFormat(const std::string& format);
    void setIncludeLocation(bool include);
    void setIncludeFunction(bool include);
    void setIncludeThreadId(bool include);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string formatEntry(const LogEntry& entry) const;
    std::string formatTimestamp(const std::chrono::system_clock::time_point& tp) const;
};

// Main enhanced Logger class
class EnhancedLogger {
public:
    EnhancedLogger();
    ~EnhancedLogger();
    
    // Disable copying
    EnhancedLogger(const EnhancedLogger&) = delete;
    EnhancedLogger& operator=(const EnhancedLogger&) = delete;
    
    // Logger management
    void addLogger(std::shared_ptr<ILogger> logger);
    void removeLogger(std::shared_ptr<ILogger> logger);
    void clearLoggers();
    
    // Configuration
    void setLevel(LogLevel level);
    void setComponentLevel(const std::string& component, LogLevel level);
    void setGlobalContext(const std::string& key, const std::string& value);
    void setContext(const std::string& key, const std::string& value);
    std::string getContext(const std::string& key) const;
    
    // Logging methods with enhanced signatures
    void log(LogLevel level, const std::string& message, 
             const std::string& file = "", int line = 0, 
             const std::string& function = "",
             const std::string& component = "",
             const std::unordered_map<std::string, std::string>& context = {});
    
    void logWithDuration(LogLevel level, const std::string& message,
                        double durationMs,
                        const std::string& file = "", int line = 0,
                        const std::string& function = "",
                        const std::string& component = "");
    
    void logError(const std::string& errorCode, const std::string& errorMessage,
                  const std::string& errorCategory = "",
                  const std::string& stackTrace = "",
                  const std::string& component = "");
    
    // Convenience methods
    void debug(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const std::string& component = "");
    void info(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const std::string& component = "");
    void warning(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const std::string& component = "");
    void error(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const std::string& component = "");
    void critical(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const std::string& component = "");
    
    // Enhanced logging for specific categories
    void neuralDebug(const std::string& message, const std::string& component = "brain");
    void plasticityDebug(const std::string& message, const std::string& component = "plasticity");
    void memoryDebug(const std::string& message, const std::string& component = "memory");
    void performance(const std::string& message, double durationMs, const std::string& component = "performance");
    void system(const std::string& message, const std::string& component = "system");
    void userAction(const std::string& message, const std::string& userId = "", const std::string& component = "user");
    void validation(const std::string& message, const std::string& component = "validation");
    void security(const std::string& message, const std::string& component = "security");
    void config(const std::string& message, const std::string& component = "config");
    void experiment(const std::string& message, const std::string& experimentId = "", const std::string& component = "experiment");
    
    // Check if level is enabled
    bool isEnabled(LogLevel level) const;
    bool isEnabledForComponent(const std::string& component, LogLevel level) const;
    
    // Global logger instance
    static EnhancedLogger& getGlobal();
    static void setGlobal(std::shared_ptr<EnhancedLogger> logger);
    
    // Thread-local context management
    void setThreadLocalContext(const std::string& key, const std::string& value);
    std::string getThreadLocalContext(const std::string& key) const;
    
    // Performance monitoring
    void startTiming(const std::string& operationId);
    void endTiming(const std::string& operationId, const std::string& category = "");
    
    // Log rotation and file management
    void setLogRotation(size_t maxFileSize, size_t maxFiles, bool rotateDaily = true);
    
    // Sanitization
    void setSanitizePatterns(const std::vector<std::string>& patterns);
    void setRedactFields(const std::vector<std::string>& fields);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Convenience macros for enhanced logging
#define NLM_LOG_NEURAL_DEBUG(msg) nlm::EnhancedLogger::getGlobal().neuralDebug(msg, __func__)
#define NLM_LOG_PLASTICITY_DEBUG(msg) nlm::EnhancedLogger::getGlobal().plasticityDebug(msg, __func__)
#define NLM_LOG_MEMORY_DEBUG(msg) nlm::EnhancedLogger::getGlobal().memoryDebug(msg, __func__)
#define NLM_LOG_PERFORMANCE(msg, duration) nlm::EnhancedLogger::getGlobal().performance(msg, duration, __func__)
#define NLM_LOG_ERROR_CODE(code, msg, category) nlm::EnhancedLogger::getGlobal().logError(code, msg, category, "", "brain")
#define NLM_LOG_WITH_CONTEXT(level, msg, context) do { \
    nlm::LogEntry entry; \
    entry.level = level; \
    entry.message = msg; \
    entry.context = context; \
    nlm::EnhancedLogger::getGlobal().log(entry.level, entry.message, __FILE__, __LINE__, __func__); \
} while(0)

} // namespace nlm
