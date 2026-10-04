#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <map>
#include <set>
#include <mutex>
#include <atomic>

namespace nlm {

// Enhanced log severity levels with more granular control
enum class LogLevel {
    Off = -1,              // No logging at all
    Critical = 0,          // Severe errors that may lead to crashes
    Error = 1,             // Errors that prevent certain functionality
    Warning = 2,           // Warnings about potential issues
    Info = 3,              // General information about system operation
    Debug = 4,             // Detailed debugging information
    Trace = 5,             // Very detailed tracing information
    Performance = 6,       // Performance-related metrics
    Memory = 7,            // Memory usage tracking
    Network = 8,           // Network-related events
    Security = 9,          // Security-related events
    Development = 10,      // Development-specific logging
    Config = 11,           // Configuration changes
    Neural = 12,           // Neural network specific events
    Plasticity = 13,       // Plasticity-related events
    MemoryManagement = 14, // Memory management events
    Threading = 15,        // Threading/synchronization events
    FileIO = 16,           // File input/output events
    Database = 17,         // Database operations
    HTTP = 18,             // HTTP/web requests
    UI = 19,               // User interface events
    All = 100              // Special value to enable all levels
};

// Log entry with enhanced metadata
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    
    // Enhanced metadata for better filtering and analysis
    std::string threadName;
    uint64_t threadId;
    int processId;
    std::string module;
    std::string category;
    std::map<std::string, std::string> metadata;
    
    // Performance timing
    double elapsedTime;  // Time taken for the operation (if applicable)
    uint64_t memoryDelta; // Memory change in bytes (if applicable)
    
    LogEntry()
        : level(LogLevel::Info)
        , line(0)
        , threadId(0)
        , processId(0)
        , elapsedTime(0.0)
        , memoryDelta(0) {}
        
    LogEntry(LogLevel lvl, const std::string& msg, const std::string& file = "", int line = 0, 
             const std::string& function = "")
        : level(lvl)
        , message(msg)
        , file(file)
        , line(line)
        , function(function)
        , timestamp(std::chrono::system_clock::now())
        , threadId(0)
        , processId(0)
        , elapsedTime(0.0)
        , memoryDelta(0) {}
};

// Logger interface with enhanced capabilities
class ILogger {
public:
    virtual ~ILogger() = default;
    
    // Basic logging interface
    virtual void log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
    
    // Enhanced capabilities
    virtual void setFormat(const std::string& format) = 0;
    virtual std::string getFormat() const = 0;
    virtual void setFilter(std::function<bool(const LogEntry&)> filter) = 0;
    virtual void addCategory(const std::string& category) = 0;
    virtual void removeCategory(const std::string& category) = 0;
    virtual std::set<std::string> getCategories() const = 0;
    virtual void setMetadata(const std::string& key, const std::string& value) = 0;
    virtual std::string getMetadata(const std::string& key) const = 0;
    virtual void clearMetadata() = 0;
    
    // Output control
    virtual void enableColors(bool enable) = 0;
    virtual bool getUseColors() const = 0;
    virtual void setMaxMessageSize(size_t maxSize) = 0;
    virtual size_t getMaxMessageSize() const = 0;
    virtual void setMaxFileSize(size_t maxSize) = 0;
    virtual size_t getMaxFileSize() const = 0;
    
    // File output management
    virtual void setOutputFile(const std::string& filepath) = 0;
    virtual void closeOutputFile() = 0;
    virtual bool isFileOutputOpen() const = 0;
    virtual std::string getOutputFile() const = 0;
    
    // Performance and analysis
    virtual void enablePerformanceTracking(bool enable) = 0;
    virtual bool isPerformanceTrackingEnabled() const = 0;
    virtual void enableMemoryTracking(bool enable) = 0;
    virtual bool isMemoryTrackingEnabled() const = 0;
    virtual void enableThreadingInfo(bool enable) = 0;
    virtual bool isThreadingInfoEnabled() const = 0;
    
    // Buffering and batching
    virtual void setBufferSize(size_t bufferSize) = 0;
    virtual size_t getBufferSize() const = 0;
    virtual void flush() = 0;
    virtual bool isBuffered() const = 0;
    
    // File rotation
    virtual void setFileRotation(size_t maxFiles, size_t maxSizePerFile) = 0;
    virtual void rotateFile() = 0;
    virtual size_t getFileCount() const = 0;
    
    // Asynchronous logging
    virtual void enableAsyncLogging(bool enable) = 0;
    virtual bool isAsyncLoggingEnabled() const = 0;
    virtual void setAsyncQueueSize(size_t queueSize) = 0;
    virtual size_t getAsyncQueueSize() const = 0;
};

// Enhanced Console logger implementation
class ConsoleLogger : public ILogger {
public:
    ConsoleLogger();
    explicit ConsoleLogger(LogLevel level);
    ~ConsoleLogger() override;
    
    // Basic interface
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    // Enhanced interface
    void setFormat(const std::string& format) override;
    std::string getFormat() const override;
    void setFilter(std::function<bool(const LogEntry&)> filter) override;
    void addCategory(const std::string& category) override;
    void removeCategory(const std::string& category) override;
    std::set<std::string> getCategories() const override;
    void setMetadata(const std::string& key, const std::string& value) override;
    std::string getMetadata(const std::string& key) const override;
    void clearMetadata() override;
    
    // Output control
    void enableColors(bool enable) override;
    bool getUseColors() const override;
    void setMaxMessageSize(size_t maxSize) override;
    size_t getMaxMessageSize() const override;
    void setMaxFileSize(size_t maxSize) override;
    size_t getMaxFileSize() const override;
    
    // File output management
    void setOutputFile(const std::string& filepath) override;
    void closeOutputFile() override;
    bool isFileOutputOpen() const override;
    std::string getOutputFile() const override;
    
    // Performance and analysis
    void enablePerformanceTracking(bool enable) override;
    bool isPerformanceTrackingEnabled() const override;
    void enableMemoryTracking(bool enable) override;
    bool isMemoryTrackingEnabled() const override;
    void enableThreadingInfo(bool enable) override;
    bool isThreadingInfoEnabled() const override;
    
    // Buffering and batching
    void setBufferSize(size_t bufferSize) override;
    size_t getBufferSize() const override;
    void flush() override;
    bool isBuffered() const override;
    
    // File rotation
    void setFileRotation(size_t maxFiles, size_t maxSizePerFile) override;
    void rotateFile() override;
    size_t getFileCount() const override;
    
    // Asynchronous logging
    void enableAsyncLogging(bool enable) override;
    bool isAsyncLoggingEnabled() const override;
    void setAsyncQueueSize(size_t queueSize) override;
    size_t getAsyncQueueSize() const override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string formatMessage(const LogEntry& entry) const;
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
    bool shouldLog(const LogEntry& entry) const;
    std::string getThreadInfo() const;
    std::string getTimestamp(const LogEntry& entry) const;
    std::string escapeHtml(const std::string& text) const;
};

// Enhanced Main Logger class
class Logger {
public:
    Logger();
    ~Logger();
    
    // Disable copying
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    // Basic interface
    void addLogger(std::shared_ptr<ILogger> logger);
    void clearLoggers();
    void setLevel(LogLevel level);
    
    // Enhanced logging
    void log(LogLevel level, const std::string& message, 
             const std::string& file = "", int line = 0, 
             const std::string& function = "", const std::string& module = "",
             const std::string& category = "");
    
    // Convenience methods
    void debug(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void info(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void warning(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void error(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void critical(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void trace(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    // Performance logging
    void logPerformance(const std::string& operation, double elapsedTime, 
                       const std::string& file = "", int line = 0, 
                       const std::string& function = "");
    
    // Memory logging
    void logMemory(const std::string& operation, uint64_t memoryDelta,
                   const std::string& file = "", int line = 0,
                   const std::string& function = "");
    
    // Development logging
    void logDevelopment(const std::string& message, const std::string& file = "", int line = 0,
                       const std::string& function = "", const std::string& module = "");
    
    // Configuration logging
    void logConfig(const std::string& key, const std::string& value,
                   const std::string& file = "", int line = 0,
                   const std::string& function = "");
    
    // Check if level is enabled
    bool isEnabled(LogLevel level) const;
    
    // Get global logger instance
    static Logger& getGlobal();
    
    // Set global logger
    static void setGlobal(std::shared_ptr<Logger> logger);
    
    // Get configuration
    std::set<LogLevel> getLogLevels() const;
    std::set<std::string> getCategories() const;
    std::map<std::string, std::string> getMetadata() const;
    std::string getConfigurationSummary() const;
    
    // File management
    void rotateLogs();
    void cleanupOldLogs();
    
    // Export and import
    bool exportConfiguration(const std::string& filepath) const;
    bool importConfiguration(const std::string& filepath);
    
    // Statistics
    uint64_t getLogCount(LogLevel level) const;
    uint64_t getTotalLogCount() const;
    double getAverageLogSize() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    void initializeDefaultLoggers();
    void updateStatistics(const LogEntry& entry);
    bool shouldLogByCategory(const LogEntry& entry) const;
    bool shouldLogByMetadata(const LogEntry& entry) const;
    std::string formatStatistics() const;
};

// Convenience macros for enhanced logging
#define NLM_LOG_OFF(msg) nlm::Logger::getGlobal().log(nlm::LogLevel::Off, msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().log(nlm::LogLevel::Critical, msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().log(nlm::LogLevel::Error, msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().log(nlm::LogLevel::Warning, msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().log(nlm::LogLevel::Info, msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().log(nlm::LogLevel::Debug, msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_TRACE(msg) nlm::Logger::getGlobal().log(nlm::LogLevel::Trace, msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_PERFORMANCE(op, elapsed) nlm::Logger::getGlobal().logPerformance(op, elapsed, __FILE__, __LINE__, __func__)
#define NLM_LOG_MEMORY(op, delta) nlm::Logger::getGlobal().logMemory(op, delta, __FILE__, __LINE__, __func__)
#define NLM_LOG_DEVELOPMENT(msg) nlm::Logger::getGlobal().logDevelopment(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_CONFIG(key, value) nlm::Logger::getGlobal().logConfig(key, value, __FILE__, __LINE__, __func__)

// Enhanced stream-style logging helper
class LogStream {
public:
    LogStream(LogLevel level, const std::string& file, int line, const std::string& function,
              const std::string& module = "", const std::string& category = "");
    ~LogStream();
    
    template<typename T>
    LogStream& operator<<(const T& value) {
        oss_ << value;
        return *this;
    }
    
    LogStream& operator<<(const std::string& value) {
        oss_ << value;
        return *this;
    }
    
    LogStream& operator<<(const char* value) {
        oss_ << value;
        return *this;
    }
    
    LogStream& operator<<(int value) {
        oss_ << value;
        return *this;
    }
    
    LogStream& operator<<(double value) {
        oss_ << value;
        return *this;
    }
    
    LogStream& operator<<(bool value) {
        oss_ << (value ? "true" : "false");
        return *this;
    }
    
    // Set additional metadata
    LogStream& withModule(const std::string& module) {
        module_ = module;
        return *this;
    }
    
    LogStream& withCategory(const std::string& category) {
        category_ = category;
        return *this;
    }
    
    LogStream& withMetadata(const std::string& key, const std::string& value) {
        metadata_[key] = value;
        return *this;
    }
    
    LogStream& withElapsedTime(double elapsed) {
        elapsedTime_ = elapsed;
        return *this;
    }
    
    LogStream& withMemoryDelta(uint64_t delta) {
        memoryDelta_ = delta;
        return *this;
    }

private:
    LogLevel level_;
    std::ostringstream oss_;
    std::string file_;
    int line_;
    std::string function_;
    std::string module_;
    std::string category_;
    std::map<std::string, std::string> metadata_;
    double elapsedTime_;
    uint64_t memoryDelta_;
    std::chrono::system_clock::time_point startTime_;
};

#define NLM_LOG_STREAM(level) nlm::LogStream(level, __FILE__, __LINE__, __func__)

} // namespace nlm
