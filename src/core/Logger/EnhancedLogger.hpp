// Enhanced Logger System for NLM
// Provides production-ready logging with multiple sinks, structured logging,
// category filtering, async processing, and rich metadata

#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <mutex>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <queue>
#include <fstream>
#include <iomanip>
#include <filesystem>
#include <thread>

namespace nlm {

// Enhanced log levels with more granularity
enum class LogLevel {
    Off = -1,
    Critical = 0,
    Error = 1,
    Warning = 2,
    Info = 3,
    Debug = 4,
    Trace = 5,
    Verbose = 6
};

// Log categories for system organization
enum class LogCategory {
    System,
    Brain,
    Motor,
    Sensory,
    Neuromodulation,
    Cognition,
    Development,
    Environment,
    Performance,
    Debug,
    All
};

// Enhanced log entry with rich metadata
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    
    // Rich metadata for advanced logging
    pid_t processId;
    std::thread::id threadId;
    std::string module;
    std::string category;
    
    // Structured fields for key-value logging
    std::unordered_map<std::string, std::string> fields;
    
    // Exception information
    std::string exceptionType;
    std::string exceptionMessage;
    std::string stackTrace;
    
    // Performance metrics
    std::chrono::nanoseconds processingTime;
    std::chrono::nanoseconds queueWaitTime;
    
    // Additional context
    std::string sessionId;
    std::string userId;
    std::unordered_map<std::string, std::string> context;
    
    LogEntry()
        : level(LogLevel::Info), line(0), processId(getpid()),
          processingTime(0), queueWaitTime(0) {
        timestamp = std::chrono::system_clock::now();
        threadId = std::this_thread::get_id();
    }
};

// Metrics for log statistics and monitoring
struct LogMetrics {
    std::atomic<uint64_t> totalLogs{0};
    std::atomic<uint64_t> logsByLevel[static_cast<size_t>(LogLevel::Verbose) + 1]{0};
    std::atomic<uint64_t> logsByCategory[static_cast<size_t>(LogCategory::All) + 1]{0};
    std::atomic<double> averageProcessingTime{0.0};
    std::atomic<double> averageQueueWaitTime{0.0};
    std::atomic<uint64_t> droppedLogs{0};
    std::chrono::system_clock::time_point startTime;
    
    LogMetrics() {
        startTime = std::chrono::system_clock::now();
    }
};

// Logger interface for all logger types
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
    virtual void setCategoryFilter(LogCategory category, bool enabled) = 0;
    virtual bool isCategoryEnabled(LogCategory category) const = 0;
    virtual LogMetrics getMetrics() const = 0;
    virtual void flush() = 0;
};

// Thread pool for async logging operations
class AsyncLoggerThreadPool {
public:
    AsyncLoggerThreadPool(size_t numThreads);
    ~AsyncLoggerThreadPool();
    
    void submit(std::function<void()> task);
    
private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex queueMutex;
    std::condition_variable condition;
    std::atomic<bool> running{true};
};

// Logger factory for creating different logger types
class LoggerFactory {
public:
    static std::unique_ptr<ILogger> createConsoleLogger(LogLevel level = LogLevel::Info, bool useColors = true);
    static std::unique_ptr<ILogger> createFileLogger(LogLevel level = LogLevel::Info,
                                                     const std::string& filepath = "",
                                                     size_t maxFileSize = 10 * 1024 * 1024,
                                                     int maxFiles = 5,
                                                     bool jsonFormat = false);
    static std::unique_ptr<ILogger> createQueueLogger(LogLevel level = LogLevel::Info,
                                                      size_t queueSize = 10000,
                                                      size_t workerThreads = 2,
                                                      std::chrono::milliseconds flushInterval = std::chrono::milliseconds(100));
    static std::unique_ptr<ILogger> createMetricsLogger(LogLevel level = LogLevel::Info,
                                                        const std::string& outputFile = "metrics.log");
    static std::unique_ptr<ILogger> createDebugLogger(LogLevel level = LogLevel::Info,
                                                      const std::string& outputFile = "debug.log");
    static std::unique_ptr<ILogger> createStructuredLogger(LogLevel level = LogLevel::Info,
                                                           const std::string& outputFile = "structured.log");
};

// Configuration for enhanced logger system
struct LoggerConfig {
    LogLevel level = LogLevel::Info;
    std::string defaultFilepath = "nlm.log";
    bool enableColors = true;
    bool jsonFormat = false;
    size_t maxFileSize = 10 * 1024 * 1024;
    int maxLogFiles = 5;
    std::unordered_map<LogCategory, bool> categoryFilters;
    std::unordered_map<std::string, std::string> defaultFields;
    bool enableAsyncLogging = true;
    size_t asyncQueueSize = 10000;
    size_t asyncWorkerThreads = 2;
    bool enableMetrics = true;
    bool enableDebug = false;
    
    LoggerConfig() {
        // Enable all categories by default
        for (int i = 0; i <= static_cast<int>(LogCategory::All); ++i) {
            categoryFilters[static_cast<LogCategory>(i)] = true;
        }
    }
};

// Logger utility functions
class LoggerUtils {
public:
    static std::string levelToString(LogLevel level);
    static std::string categoryToString(LogCategory category);
    static LogLevel stringToLevel(const std::string& str);
    static LogCategory stringToCategory(const std::string& str);
    static std::string getCurrentProcessId();
    static std::string getCurrentThreadId();
    static std::string getCurrentModule();
    static std::string generateSessionId();
    static std::string getStackTrace();
    static std::string getCurrentTimeString(const std::string& format = "%Y-%m-%d %H:%M:%S");
    
    // Key-value pair utilities
    static std::string fieldsToString(const std::unordered_map<std::string, std::string>& fields);
    static void addField(LogEntry& entry, const std::string& key, const std::string& value);
    static std::string fieldToString(const std::string& key, const std::string& value);
    static std::string toLower(const std::string& str);
    static std::string trim(const std::string& str);
    static std::string escapeJSON(const std::string& str);
};

// Log rotation and management utilities
class LogRotationManager {
public:
    static bool shouldRotate(const std::string& filepath, size_t maxFileSize);
    static void rotateLogFile(const std::string& filepath, int maxFiles);
    static std::string getRotatedFilepath(const std::string& filepath, int index);
    static void cleanupOldLogs(const std::string& pattern, int keepCount);
};

// JSON formatter for structured logging
class JSONFormatter {
public:
    static std::string format(const LogEntry& entry);
    static std::string format(const LogEntry& entry, const std::unordered_map<std::string, std::string>& extraFields);
    
private:
    static std::string escapeJSON(const std::string& str);
    static std::string levelToString(LogLevel level);
    static std::string categoryToString(LogCategory category);
    static std::string timestampToString(const std::chrono::system_clock::time_point& timestamp);
};

// Enhanced Logger with all production features
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
    
    // Set category filters for all loggers
    void setCategoryFilter(LogCategory category, bool enabled);
    void setAllCategories(bool enabled);
    
    // Global logging functions
    void log(LogLevel level, const std::string& message,
             const std::string& file = "", int line = 0,
             const std::string& function = "",
             const std::string& category = "");
    
    // Enhanced convenience methods
    void log(const LogEntry& entry);
    void logWithFields(LogLevel level, const std::string& message,
                       const std::unordered_map<std::string, std::string>& fields,
                       const std::string& file = "", int line = 0,
                       const std::string& function = "",
                       const std::string& category = "");
    
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
    
    // Special logging methods
    void logException(const std::string& message, const std::exception& ex,
                      const std::string& file = "", int line = 0,
                      const std::string& function = "",
                      const std::string& category = "");
    
    void logPerformance(const std::string& operation, const std::chrono::nanoseconds& duration,
                        const std::string& file = "", int line = 0,
                        const std::string& function = "",
                        const std::string& category = "");
    
    // Check if level is enabled
    bool isEnabled(LogLevel level) const;
    
    // Get global logger instance
    static Logger& getGlobal();
    
    // Set global logger
    static void setGlobal(std::shared_ptr<Logger> logger);
    
    // Get log statistics
    LogMetrics getMetrics() const;
    
    // Flush all loggers
    void flush();
    
    // Clear log statistics
    void resetMetrics();
    
    // Configuration management
    void loadConfig(const std::string& filepath);
    void saveConfig(const std::string& filepath) const;
    
    // Factory methods for easy creation of different logger types
    static std::unique_ptr<Logger> createEnhancedLogger();
    static std::unique_ptr<Logger> createProductionLogger(const LoggerConfig& config = LoggerConfig());
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Stream-style logging helper with enhanced features
class LogStream {
public:
    LogStream(LogLevel level, const std::string& file, int line, const std::string& function,
              const std::string& category = "");
    ~LogStream();
    
    template<typename T>
    LogStream& operator<<(const T& value) {
        oss_ << value;
        return *this;
    }
    
    // Add structured field
    LogStream& operator<<(const std::pair<std::string, std::string>& field);
    
    // Add key-value pair
    LogStream& field(const std::string& key, const std::string& value);
    
private:
    LogLevel level_;
    std::ostringstream oss_;
    std::string file_;
    int line_;
    std::string function_;
    std::string category_;
    std::unordered_map<std::string, std::string> fields_;
};

// Convenience macros
#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)

#define NLM_LOG_STREAM(level) nlm::LogStream(level, __FILE__, __LINE__, __func__)

// Enhanced logging macros with category support
#define NLM_LOG_DEBUG_CAT(msg, cat) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__, cat)
#define NLM_LOG_INFO_CAT(msg, cat) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__, cat)
#define NLM_LOG_WARNING_CAT(msg, cat) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__, cat)
#define NLM_LOG_ERROR_CAT(msg, cat) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__, cat)
#define NLM_LOG_CRITICAL_CAT(msg, cat) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__, cat)

// Performance logging macros
#define NLM_LOG_PERFORMANCE(op, duration) \
    nlm::Logger::getGlobal().logPerformance(op, duration, __FILE__, __LINE__, __func__)

// Exception logging macros
#define NLM_LOG_EXCEPTION(msg) \
    nlm::Logger::getGlobal().logException(msg, std::current_exception(), __FILE__, __LINE__, __func__)

} // namespace nlm
