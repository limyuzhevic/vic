#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <map>
#include <unordered_map>
#include <shared_mutex>
#include <deque>
#include <atomic>
#include <thread>
#include <condition_variable>
#include <optional>
#include <variant>
#include <nlohmann/json.hpp>

namespace nlm {

// Log severity levels with weights for hierarchical filtering
enum class LogLevel {
    None = -1,         // No logging
    Debug = 0,         // 1 - 10
    Trace = 1,         // 2
    Verbose = 2,       // 3 - 5 (more verbose than trace)
    Info = 3,          // 10 - 50
    Notice = 4,        // 20
    Warning = 5,       // 50 - 100
    Error = 6,         // 100 - 500
    Critical = 7,      // 500 - 1000
    Alert = 8,         // 1000 - 5000
    Emergency = 9,     // 5000 - 10000
    Fatal = 10,        // 10000+
};

inline constexpr int getLogLevelWeight(LogLevel level) {
    switch (level) {
        case LogLevel::None: return -1;
        case LogLevel::Debug: return 5;
        case LogLevel::Trace: return 2;
        case LogLevel::Verbose: return 4;
        case LogLevel::Info: return 30;
        case LogLevel::Notice: return 20;
        case LogLevel::Warning: return 75;
        case LogLevel::Error: return 200;
        case LogLevel::Critical: return 500;
        case LogLevel::Alert: return 2000;
        case LogLevel::Emergency: return 7500;
        case LogLevel::Fatal: return 10000;
        default: return 0;
    }
}

inline constexpr LogLevel minLevel(LogLevel a, LogLevel b) {
    return static_cast<LogLevel>(std::min(static_cast<int>(a), static_cast<int>(b)));
}

inline constexpr LogLevel maxLevel(LogLevel a, LogLevel b) {
    return static_cast<LogLevel>(std::max(static_cast<int>(a), static_cast<int>(b)));
}

// Timestamp formats
enum class TimestampFormat {
    ISO8601,
    ISO8601Milli,
    ISO8601Nano,
    HumanReadable,
    HumanReadableMS,
    HumanReadablePrecise,
    Epoch,
    EpochMS,
    EpochNS,
    Custom
};

// Structured log entry with key-value pairs
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    
    // Structured data for advanced logging
    std::map<std::string, std::string> fields;
    std::map<std::string, std::variant<std::string, int, double, bool>> structuredData;
    
    // Performance tracking
    std::chrono::system_clock::time_point creationTime;
    uint64_t threadId;
    uint64_t sequenceNumber;
    
    // Source categorization
    std::string module;
    std::string category;
    std::string tags;
    
    // Message metadata
    std::string loggerName;
    std::string contextId;
    
    LogEntry() : level(LogLevel::Info), line(0), 
                 threadId(std::hash<std::thread::id>{}(std::this_thread::get_id())) {
        timestamp = std::chrono::system_clock::now();
        creationTime = timestamp;
        sequenceNumber = 0;
    }
};

// Log filter criteria for advanced filtering
struct LogFilter {
    std::optional<LogLevel> minLevel;
    std::optional<LogLevel> maxLevel;
    std::vector<std::string> modules;
    std::vector<std::string> categories;
    std::vector<std::string> tags;
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point endTime;
    std::string messagePattern;
    std::map<std::string, std::string> fieldFilters;
    bool negate = false;
};

// Log configuration
struct LogConfig {
    std::string name;
    LogLevel level;
    std::string filePath;
    uint64_t maxFileSize;
    std::string timeBasedRotation;
    bool useColors = true;
    TimestampFormat timestampFormat = TimestampFormat::ISO8601;
    std::string timestampFormatString;
    std::string customTimestampFormat;
    bool enableStructuredLogging = false;
    std::string structuredFormat; // "json", "csv", "xml", "keyvalue"
    uint32_t maxLogBufferSize = 10000;
    uint32_t flushInterval = 1000; // milliseconds
    bool enableAsyncLogging = false;
    uint32_t asyncQueueSize = 10000;
    uint32_t maxFlushThreads = 2;
    bool enableRotation = true;
    bool enableRetention = true;
    uint32_t maxLogFiles = 10;
};

// Logger interface with enhanced functionality
class ILogger {
public:
    virtual ~ILogger() = default;
    
    // Core logging methods
    virtual void log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
    
    // Enhanced configuration
    virtual void configure(const LogConfig& config) = 0;
    virtual LogConfig getConfig() const = 0;
    
    // Filtering
    virtual void setFilter(const LogFilter& filter) = 0;
    virtual LogFilter getFilter() const = 0;
    virtual bool shouldLog(const LogEntry& entry) const = 0;
    
    // Statistics
    virtual uint64_t getLogCount() const = 0;
    virtual std::chrono::system_clock::time_point getLastLogTime() const = 0;
    virtual double getAverageLogRate() const = 0;
    
    // Performance monitoring
    virtual void resetStats() = 0;
    
    // Sink management
    virtual void flush() = 0;
    virtual bool isBufferFull() const = 0;
};

// Advanced logger with query and search capabilities
class IAdvancedLogger : public ILogger {
public:
    virtual ~IAdvancedLogger() = default;
    
    // Query and search functionality
    virtual std::vector<LogEntry> query(const LogFilter& filter) const = 0;
    virtual std::vector<LogEntry> search(const std::string& pattern, 
                                        const LogFilter& filter = LogFilter()) const = 0;
    virtual void exportLogs(const std::string& filepath, 
                          const LogFilter& filter = LogFilter(),
                          const std::string& format = "json") = 0;
    virtual void importLogs(const std::string& filepath) = 0;
    
    // Aggregation and analysis
    virtual nlohmann::json aggregate(const LogFilter& filter, 
                                    const std::string& interval = "hour") const = 0;
    virtual std::map<LogLevel, uint64_t> getLevelCounts(const LogFilter& filter) const = 0;
    virtual std::map<std::string, uint64_t> getSourceCounts(const LogFilter& filter) const = 0;
    virtual std::map<std::string, uint64_t> getModuleCounts(const LogFilter& filter) const = 0;
    
    // Log management
    virtual void clear() = 0;
    virtual uint64_t getTotalLogCount() const = 0;
    virtual void compact() = 0;
    virtual void rotate() = 0;
};

// Performance-optimized buffer for async logging
class LogBuffer {
public:
    LogBuffer(size_t capacity) : capacity_(capacity), size_(0), 
                                  head_(0), tail_(0), closed_(false) {}
    
    bool push(const LogEntry& entry);
    bool pop(LogEntry& entry);
    void close();
    bool isFull() const { return size_ == capacity_; }
    bool isEmpty() const { return size_ == 0; }
    bool isClosed() const { return closed_; }
    size_t size() const { return size_; }
    size_t capacity() const { return capacity_; }
    
private:
    std::deque<LogEntry> buffer_;
    size_t capacity_;
    size_t size_;
    size_t head_;
    size_t tail_;
    bool closed_;
    mutable std::mutex mutex_;
    std::condition_variable notEmpty_;
    std::condition_variable notFull_;
};

// Async logging processor
class AsyncLogProcessor {
public:
    AsyncLogProcessor(std::shared_ptr<IAdvancedLogger> logger, 
                     size_t queueSize = 10000,
                     size_t numThreads = 2);
    ~AsyncLogProcessor();
    
    void start();
    void stop();
    void push(const LogEntry& entry);
    void waitForFlush();
    
    void setFlushInterval(uint32_t interval) { flushInterval_ = interval; }
    void setMaxBatchSize(size_t size) { maxBatchSize_ = size; }
    
private:
    void workerThread();
    void flushBatch();
    
    std::shared_ptr<IAdvancedLogger> logger_;
    std::vector<std::thread> workers_;
    LogBuffer buffer_;
    std::atomic<bool> running_;
    std::atomic<bool> flushing_;
    uint32_t flushInterval_;
    size_t maxBatchSize_;
    std::mutex mutex_;
    std::condition_variable cv_;
    uint64_t batchCount_;
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

// Logger interface
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
};

// Console logger implementation
class ConsoleLogger : public ILogger {
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
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
};

// Format string parser and formatter
class LogFormatter {
public:
    LogFormatter(const std::string& format = "%timestamp %level[%module] %message");
    std::string format(const LogEntry& entry) const;
    
    void setTimestampFormat(TimestampFormat format) { timestampFormat_ = format; }
    void setTimestampFormat(const std::string& format) { timestampFormatString_ = format; }
    void setIncludeFileInfo(bool include) { includeFileInfo_ = include; }
    void setIncludeFunctionInfo(bool include) { includeFunctionInfo_ = include; }
    void setIncludeThreadId(bool include) { includeThreadId_ = include; }
    void setIncludeSequence(bool include) { includeSequence_ = include; }
    
private:
    std::string timestampToString(const std::chrono::system_clock::time_point& tp) const;
    std::string formatToString(const std::variant<std::string, int, double, bool>& value) const;
    
    std::string format_;
    TimestampFormat timestampFormat_;
    std::string timestampFormatString_;
    bool includeFileInfo_;
    bool includeFunctionInfo_;
    bool includeThreadId_;
    bool includeSequence_;
};

// Console logger with enhanced features
class ConsoleLogger : public IAdvancedLogger {
public:
    ConsoleLogger();
    explicit ConsoleLogger(const LogConfig& config);
    ~ConsoleLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    void configure(const LogConfig& config) override;
    LogConfig getConfig() const override;
    
    void setFilter(const LogFilter& filter) override;
    LogFilter getFilter() const override;
    bool shouldLog(const LogEntry& entry) const override;
    
    uint64_t getLogCount() const override;
    std::chrono::system_clock::time_point getLastLogTime() const override;
    double getAverageLogRate() const override;
    
    void resetStats() override;
    void flush() override;
    bool isBufferFull() const override;
    
    // Advanced query methods
    std::vector<LogEntry> query(const LogFilter& filter) const override;
    std::vector<LogEntry> search(const std::string& pattern, 
                                const LogFilter& filter = LogFilter()) const override;
    void exportLogs(const std::string& filepath, 
                   const LogFilter& filter = LogFilter(),
                   const std::string& format = "json") override;
    void importLogs(const std::string& filepath) override;
    
    // Aggregation and analysis
    nlohmann::json aggregate(const LogFilter& filter, 
                            const std::string& interval = "hour") const override;
    std::map<LogLevel, uint64_t> getLevelCounts(const LogFilter& filter) const override;
    std::map<std::string, uint64_t> getSourceCounts(const LogFilter& filter) const override;
    std::map<std::string, uint64_t> getModuleCounts(const LogFilter& filter) const override;
    
    // Log management
    void clear() override;
    uint64_t getTotalLogCount() const override;
    void compact() override;
    void rotate() override;
    
    // Colors and formatting
    void setUseColors(bool useColors);
    bool getUseColors() const;
    void setFormatter(const LogFormatter& formatter);
    void setOutputFormat(const std::string& format);
    
    // File output with rotation
    void setOutputFile(const std::string& filepath);
    void closeOutputFile();
    void enableFileRotation(uint64_t maxSize, const std::string& timePattern = "daily");
    
    // Async logging support
    void enableAsyncLogging(size_t queueSize = 10000, size_t numThreads = 2);
    void setFlushInterval(uint32_t interval);
    void waitForFlush();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
    void rotateFileIfNeeded();
    std::string getRotatedFilename(const std::string& basePath, uint64_t timestamp) const;
};

// File logger with rotation support
class FileLogger : public IAdvancedLogger {
public:
    FileLogger(const std::string& filepath);
    explicit FileLogger(const LogConfig& config);
    ~FileLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    void configure(const LogConfig& config) override;
    LogConfig getConfig() const override;
    
    void setFilter(const LogFilter& filter) override;
    LogFilter getFilter() const override;
    bool shouldLog(const LogEntry& entry) const override;
    
    // Implement IAdvancedLogger methods
    uint64_t getLogCount() const override;
    std::chrono::system_clock::time_point getLastLogTime() const override;
    double getAverageLogRate() const override;
    void resetStats() override;
    void flush() override;
    bool isBufferFull() const override;
    std::vector<LogEntry> query(const LogFilter& filter) const override;
    std::vector<LogEntry> search(const std::string& pattern, 
                                const LogFilter& filter = LogFilter()) const override;
    void exportLogs(const std::string& filepath, 
                   const LogFilter& filter = LogFilter(),
                   const std::string& format = "json") override;
    void importLogs(const std::string& filepath) override;
    nlohmann::json aggregate(const LogFilter& filter, 
                            const std::string& interval = "hour") const override;
    std::map<LogLevel, uint64_t> getLevelCounts(const LogFilter& filter) const override;
    std::map<std::string, uint64_t> getSourceCounts(const LogFilter& filter) const override;
    std::map<std::string, uint64_t> getModuleCounts(const LogFilter& filter) const override;
    void clear() override;
    uint64_t getTotalLogCount() const override;
    void compact() override;
    void rotate() override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// JSON logger for structured logging
class JsonLogger : public IAdvancedLogger {
public:
    JsonLogger();
    explicit JsonLogger(const LogConfig& config);
    ~JsonLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    void configure(const LogConfig& config) override;
    LogConfig getConfig() const override;
    
    void setFilter(const LogFilter& filter) override;
    LogFilter getFilter() const override;
    bool shouldLog(const LogEntry& entry) const override;
    
    // Implement IAdvancedLogger methods (simplified for JSON format)
    uint64_t getLogCount() const override;
    std::chrono::system_clock::time_point getLastLogTime() const override;
    double getAverageLogRate() const override;
    void resetStats() override;
    void flush() override;
    bool isBufferFull() const override;
    std::vector<LogEntry> query(const LogFilter& filter) const override;
    std::vector<LogEntry> search(const std::string& pattern, 
                                const LogFilter& filter = LogFilter()) const override;
    void exportLogs(const std::string& filepath, 
                   const LogFilter& filter = LogFilter(),
                   const std::string& format = "json") override;
    void importLogs(const std::string& filepath) override;
    nlohmann::json aggregate(const LogFilter& filter, 
                            const std::string& interval = "hour") const override;
    std::map<LogLevel, uint64_t> getLevelCounts(const LogFilter& filter) const override;
    std::map<std::string, uint64_t> getSourceCounts(const LogFilter& filter) const override;
    std::map<std::string, uint64_t> getModuleCounts(const LogFilter& filter) const override;
    void clear() override;
    uint64_t getTotalLogCount() const override;
    void compact() override;
    void rotate() override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Main Logger class with enhanced functionality
class Logger {
public:
    Logger();
    ~Logger();
    
    // Disable copying
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    // Logger sink management
    void addLogger(std::shared_ptr<IAdvancedLogger> logger);
    void addLogger(std::shared_ptr<ILogger> logger); // Backward compatibility
    
    // Remove all loggers
    void clearLoggers();
    
    // Set minimum level for all loggers
    void setLevel(LogLevel level);
    
    // Global logging functions
    void log(LogLevel level, const std::string& message, 
             const std::string& file = "", int line = 0, 
             const std::string& function = "", 
             const std::map<std::string, std::string>& fields = {});
    
    // Convenience methods with enhanced signatures
    void debug(const std::string& message, const std::string& file = "", int line = 0,
                const std::string& function = "", const std::map<std::string, std::string>& fields = {});
    void info(const std::string& message, const std::string& file = "", int line = 0,
              const std::string& function = "", const std::map<std::string, std::string>& fields = {});
    void warning(const std::string& message, const std::string& file = "", int line = 0,
                 const std::string& function = "", const std::map<std::string, std::string>& fields = {});
    void error(const std::string& message, const std::string& file = "", int line = 0,
               const std::string& function = "", const std::map<std::string, std::string>& fields = {});
    void critical(const std::string& message, const std::string& file = "", int line = 0,
                  const std::string& function = "", const std::map<std::string, std::string>& fields = {});
    
    // Stream-style logging helper
    class LogStream {
    public:
        LogStream(LogLevel level, const std::string& file, int line, 
                  const std::string& function,
                  const std::map<std::string, std::string>& fields = {});
        ~LogStream();
        
        template<typename T>
        LogStream& operator<<(const T& value) {
            oss_ << value;
            return *this;
        }
        
        // Allow setting fields during stream operation
        LogStream& setField(const std::string& key, const std::string& value);
        
    private:
        LogLevel level_;
        std::ostringstream oss_;
        std::string file_;
        int line_;
        std::string function_;
        std::map<std::string, std::string> fields_;
    };
    
    #define NLM_LOG_STREAM(level) nlm::Logger::getGlobal().LogStream(level, __FILE__, __LINE__, __func__)
    #define NLM_LOG_STREAM_WITH_FIELDS(level, fields) nlm::Logger::getGlobal().LogStream(level, __FILE__, __LINE__, __func__, fields)
    
    // Check if level is enabled
    bool isEnabled(LogLevel level) const;
    
    // Global logger instance
    static Logger& getGlobal();
    
    // Set global logger
    static void setGlobal(std::shared_ptr<Logger> logger);
    
    // Global configuration
    static void setGlobalConfig(const LogConfig& config);
    static LogConfig getGlobalConfig();
    
    // Query and search across all loggers
    static std::vector<LogEntry> queryGlobal(const LogFilter& filter);
    static std::vector<LogEntry> searchGlobal(const std::string& pattern, 
                                             const LogFilter& filter = LogFilter());
    static void exportGlobalLogs(const std::string& filepath, 
                                const LogFilter& filter = LogFilter(),
                                const std::string& format = "json");
    
    // Global statistics and management
    static uint64_t getTotalLogCount();
    static void clearGlobalLogs();
    static nlohmann::json getGlobalStats();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Convenience macros for advanced logging
#define NLM_LOG_VERBOSE(msg, ...) nlm::Logger::getGlobal().verbose(msg, __FILE__, __LINE__, __func__, {__VA_ARGS__})
#define NLM_LOG_TRACE(msg, ...) nlm::Logger::getGlobal().trace(msg, __FILE__, __LINE__, __func__, {__VA_ARGS__})
#define NLM_LOG_DEBUG(msg, ...) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__, {__VA_ARGS__})
#define NLM_LOG_INFO(msg, ...) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__, {__VA_ARGS__})
#define NLM_LOG_NOTICE(msg, ...) nlm::Logger::getGlobal().notice(msg, __FILE__, __LINE__, __func__, {__VA_ARGS__})
#define NLM_LOG_WARNING(msg, ...) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__, {__VA_ARGS__})
#define NLM_LOG_ERROR(msg, ...) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__, {__VA_ARGS__})
#define NLM_LOG_CRITICAL(msg, ...) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__, {__VA_ARGS__})
#define NLM_LOG_ALERT(msg, ...) nlm::Logger::getGlobal().alert(msg, __FILE__, __LINE__, __func__, {__VA_ARGS__})
#define NLM_LOG_EMERGENCY(msg, ...) nlm::Logger::getGlobal().emergency(msg, __FILE__, __LINE__, __func__, {__VA_ARGS__})
#define NLM_LOG_FATAL(msg, ...) nlm::Logger::getGlobal().fatal(msg, __FILE__, __LINE__, __func__, {__VA_ARGS__})

// Structured logging macros
#define NLM_LOG_FIELD(name, value) std::make_pair(name, std::to_string(value))

} // namespace nlm
