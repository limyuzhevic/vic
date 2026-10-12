#pragma once

// Forward declarations for backward compatibility
namespace nlm {
    class Logger;
    class ConsoleLogger;
}

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <unordered_map>
#include <atomic>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <shared_mutex>
#include <cstdint>

// Legacy ConsoleLogger for backward compatibility
class LegacyConsoleLogger;

namespace nlm {

// Advanced log severity levels with numeric values for filtering
enum class LogLevel {
    Off = -1,
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4,
    Trace = 5
};

// Enhanced log entry with structured data
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    
    // Structured logging support
    std::unordered_map<std::string, std::string> kvPairs;
    std::unordered_map<std::string, std::string> fields;
    
    // Correlation tracking
    std::string correlationId;
    std::string parentCorrelationId;
    
    // Performance tracking
    std::chrono::nanoseconds duration;
    uint64_t threadId;
    
    // Custom fields for metrics
    std::string eventType;
    std::unordered_map<std::string, double> metrics;
};

// ILogger interface with enhanced features
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
    
    // Sink-specific configuration
    virtual void configure(const std::string& config) = 0;
    virtual std::string getConfiguration() const = 0;
    
    // Filter support
    virtual void addFilter(const std::function<bool(const LogEntry&)>& filter) = 0;
    virtual void removeAllFilters() = 0;
    
    // Sink state and health
    virtual bool isHealthy() const = 0;
    virtual uint64_t getWriteCount() const = 0;
    virtual uint64_t getErrorCount() const = 0;
};

// Base sink interface for writing log entries
class ISink {
public:
    virtual ~ISink() = default;
    virtual bool write(const LogEntry& entry) = 0;
    virtual void flush() = 0;
    virtual bool isConnected() const = 0;
    virtual void reconnect() = 0;
    virtual uint64_t getWriteCount() const = 0;
    virtual uint64_t getErrorCount() const = 0;
};

// Abstract formatter for different output formats
class IFormatter {
public:
    virtual ~IFormatter() = default;
    virtual std::string format(const LogEntry& entry) = 0;
    virtual void configure(const std::string& formatString) = 0;
    virtual std::string getFormat() const = 0;
};

// Abstract filter for conditional logging
class IFilter {
public:
    virtual ~IFilter() = default;
    virtual bool evaluate(const LogEntry& entry) const = 0;
    virtual std::string getFilterName() const = 0;
};

// Async logger core with thread pool and queue
class AsyncLoggerCore {
public:
    AsyncLoggerCore(size_t numThreads = std::thread::hardware_concurrency(),
                   size_t maxQueueSize = 100000,
                   size_t batchSize = 100);
    ~AsyncLoggerCore();
    
    // Disable copying
    AsyncLoggerCore(const AsyncLoggerCore&) = delete;
    AsyncLoggerCore& operator=(const AsyncLoggerCore&) = delete;
    
    // Log entry processing
    void log(const LogEntry& entry);
    void flush();
    
    // Sink management
    void addSink(std::shared_ptr<ISink> sink);
    void removeSink(const std::string& sinkId);
    void clearSinks();
    
    // Filter management
    void addFilter(std::shared_ptr<IFilter> filter);
    void removeFilter(const std::string& filterId);
    void clearFilters();
    
    // Formatter management
    void setFormatter(std::shared_ptr<IFormatter> formatter);
    
    // Configuration
    void configure(const std::string& configJson);
    std::string getConfiguration() const;
    
    // Statistics
    uint64_t getTotalLogCount() const;
    uint64_t getDroppedLogCount() const;
    uint64_t getErrorCount() const;
    
    // Runtime operations
    void start();
    void stop();
    bool isRunning() const;
    
    // Health checks
    bool isHealthy() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Memory buffer sink for performance
class MemoryBufferSink : public ISink {
public:
    MemoryBufferSink(size_t maxBufferSize = 100000000); // 100MB default
    bool write(const LogEntry& entry) override;
    void flush() override;
    bool isConnected() const override;
    void reconnect() override;
    uint64_t getWriteCount() const override;
    uint64_t getErrorCount() const override;
    
    // Buffer access
    std::vector<LogEntry> getBuffer() const;
    size_t getBufferSize() const;
    
    // Buffer management
    void clearBuffer();
    void setMaxBufferSize(size_t maxSize);
    
    // Statistics
    uint64_t getRetainedCount() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// File sink with rotation and compression
class FileSink : public ISink {
public:
    FileSink(const std::string& filepath, size_t maxFileSize = 100 * 1024 * 1024, // 100MB
             int maxFiles = 10, bool compressOld = true);
    bool write(const LogEntry& entry) override;
    void flush() override;
    bool isConnected() const override;
    void reconnect() override;
    uint64_t getWriteCount() const override;
    uint64_t getErrorCount() const override;
    
    // Configuration
    void setFilePath(const std::string& filepath);
    void setMaxFileSize(size_t maxSize);
    void setMaxFiles(int maxFiles);
    void setCompressOld(bool compress);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Network/UDP sink for distributed logging
class NetworkSink : public ISink {
public:
    NetworkSink(const std::string& host, uint16_t port, 
                const std::string& protocol = "udp",
                size_t bufferSize = 65536);
    bool write(const LogEntry& entry) override;
    void flush() override;
    bool isConnected() const override;
    void reconnect() override;
    uint64_t getWriteCount() const override;
    uint64_t getErrorCount() const override;
    
    // Configuration
    void setHost(const std::string& host);
    void setPort(uint16_t port);
    void setProtocol(const std::string& protocol);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Database sink for structured logging
class DatabaseSink : public ISink {
public:
    DatabaseSink(const std::string& connectionString, 
                 const std::string& tableName = "logs",
                 const std::string& schema = "nlm");
    bool write(const LogEntry& entry) override;
    void flush() override;
    bool isConnected() const override;
    void reconnect() override;
    uint64_t getWriteCount() const override;
    uint64_t getErrorCount() const override;
    
    // Configuration
    void setConnectionString(const std::string& connectionString);
    void setTableName(const std::string& tableName);
    void setSchema(const std::string& schema);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Enhanced formatter implementations
class JsonFormatter : public IFormatter {
public:
    JsonFormatter();
    std::string format(const LogEntry& entry) override;
    void configure(const std::string& formatString) override;
    std::string getFormat() const override;
    
    // Configuration options
    void setIncludeTimestamp(bool include);
    void setIncludeSourceLocation(bool include);
    void setIncludeCorrelationId(bool include);
    void setIncludeMetrics(bool include);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

class XmlFormatter : public IFormatter {
public:
    XmlFormatter();
    std::string format(const LogEntry& entry) override;
    void configure(const std::string& formatString) override;
    std::string getFormat() const override;
};

class CsvFormatter : public IFormatter {
public:
    CsvFormatter();
    std::string format(const LogEntry& entry) override;
    void configure(const std::string& formatString) override;
    std::string getFormat() const override;
};

// Enhanced filter implementations
class PatternFilter : public IFilter {
public:
    PatternFilter(const std::string& pattern, LogLevel minLevel = LogLevel::Debug);
    bool evaluate(const LogEntry& entry) const override;
    std::string getFilterName() const override;
    
    void setPattern(const std::string& pattern);
    void setMinLevel(LogLevel level);
    
private:
    std::string pattern_;
    LogLevel minLevel_;
};

class CorrelationIdFilter : public IFilter {
public:
    CorrelationIdFilter(const std::string& correlationId);
    bool evaluate(const LogEntry& entry) const override;
    std::string getFilterName() const override;
    
    void setCorrelationId(const std::string& correlationId);
    
private:
    std::string correlationId_;
};

class MetricsFilter : public IFilter {
public:
    MetricsFilter(const std::string& metricName, double minValue = 0.0);
    bool evaluate(const LogEntry& entry) const override;
    std::string getFilterName() const override;
    
    void setMetricName(const std::string& metricName);
    void setMinValue(double minValue);
    
private:
    std::string metricName_;
    double minValue_;
};

// Enhanced ConsoleLogger with multiple sinks
class ConsoleLogger : public ILogger {
public:
    ConsoleLogger();
    explicit ConsoleLogger(LogLevel level);
    ~ConsoleLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    // ILogger interface overrides
    void configure(const std::string& config) override;
    std::string getConfiguration() const override;
    void addFilter(const std::function<bool(const LogEntry&)>& filter) override;
    void removeAllFilters() override;
    bool isHealthy() const override;
    uint64_t getWriteCount() const override;
    uint64_t getErrorCount() const override;
    
    // Enable/disable colors
    void setUseColors(bool useColors);
    bool getUseColors() const;
    
    // Set output file (in addition to console)
    void setOutputFile(const std::string& filepath);
    void closeOutputFile();
    
    // Legacy compatibility
    void log(const LogEntry& entry) {
        logImpl(entry);
    }
    
private:
    void logImpl(const LogEntry& entry);
    
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
};

// Enhanced Logger class with async support and multiple sinks
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
    
    // Async logging methods
    void asyncLog(const LogEntry& entry);
    void asyncLog(LogLevel level, const std::string& message,
                 const std::string& file = "", int line = 0,
                 const std::string& function = "");
    
    // Global logging functions
    void log(LogLevel level, const std::string& message,
             const std::string& file = "", int line = 0,
             const std::string& function = "");
    
    // Structured logging with key-value pairs
    void log(LogLevel level, const std::string& message,
             const std::unordered_map<std::string, std::string>& kvPairs,
             const std::string& file = "", int line = 0,
             const std::string& function = "");
    
    // Legacy compatibility (maintaining old API)
    
    // Legacy logging method for backward compatibility
    void log(const LogEntry& entry);
    
    // Legacy convenience methods
    void debug(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void info(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void warning(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void error(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void critical(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    // Legacy global logger methods
    static Logger& getGlobal();
    static void setGlobal(std::shared_ptr<Logger> logger);
    
    // Legacy compatibility with old ConsoleLogger interface
    void addLogger(std::shared_ptr<ILogger> logger);
    void clearLoggers();
    void setLevel(LogLevel level);
    bool isEnabled(LogLevel level) const;
    
    // Performance metrics
    uint64_t getLogCount() const;
    uint64_t getErrorCount() const;
    uint64_t getDroppedCount() const;
    
    // Configuration management
    void configure(const std::string& configJson);
    std::string getConfiguration() const;
    void reloadConfiguration();
    
    // Runtime operations
    void startAsync();
    void stopAsync();
    bool isAsyncRunning() const;
    
    // Health and monitoring
    bool isHealthy() const;
    std::string getHealthStatus() const;
    
    // Correlation ID tracking
    std::string createCorrelationId();
    void setCorrelationId(const std::string& correlationId);
    std::string getCorrelationId() const;
    
    // Legacy compatibility (using existing ConsoleLogger)
    void log(const LogEntry& entry);
    
    // Legacy compatibility with old ILogger interface
    void addLogger(std::shared_ptr<ILogger> logger);
    void clearLoggers();
    void setLevel(LogLevel level);
    bool isEnabled(LogLevel level) const;
    
    // Legacy convenience methods for backward compatibility
    void debug(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void info(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void warning(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void error(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void critical(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    // Legacy global logger methods
    static Logger& getGlobal();
    static void setGlobal(std::shared_ptr<Logger> logger);
    
    // Legacy API methods
    uint64_t getLogCount() const;
    uint64_t getErrorCount() const;
    uint64_t getDroppedCount() const;
    
    // Configuration management
    void configure(const std::string& configJson);
    std::string getConfiguration() const;
    void reloadConfiguration();
    
    // Runtime operations
    void startAsync();
    void stopAsync();
    bool isAsyncRunning() const;
    
    // Health and monitoring
    bool isHealthy() const;
    std::string getHealthStatus() const;
    
    // Correlation ID tracking
    std::string createCorrelationId();
    void setCorrelationId(const std::string& correlationId);
    std::string getCorrelationId() const;
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Convenience macro for file/line/function info
#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg)
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg)
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg)
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg)
#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical(msg)

// Structured logging macros
#define NLM_LOG_DEBUG_KV(msg, ...) nlm::Logger::getGlobal().debug(msg, nlm::parseKeyValue(#__VA_ARGS__))
#define NLM_LOG_INFO_KV(msg, ...) nlm::Logger::getGlobal().info(msg, nlm::parseKeyValue(#__VA_ARGS__))
#define NLM_LOG_WARNING_KV(msg, ...) nlm::Logger::getGlobal().warning(msg, nlm::parseKeyValue(#__VA_ARGS__))
#define NLM_LOG_ERROR_KV(msg, ...) nlm::Logger::getGlobal().error(msg, nlm::parseKeyValue(#__VA_ARGS__))
#define NLM_LOG_CRITICAL_KV(msg, ...) nlm::Logger::getGlobal().critical(msg, nlm::parseKeyValue(#__VA_ARGS__))

// Stream-style logging helper with structured support
class LogStream {
public:
    LogStream(LogLevel level, const std::string& file, int line, const std::string& function);
    ~LogStream();
    
    template<typename T>
    LogStream& operator<<(const T& value) {
        oss_ << value;
        return *this;
    }
    
    // Add key-value pair to structured log
    LogStream& kv(const std::string& key, const std::string& value);
    LogStream& kv(const std::string& key, int value);
    LogStream& kv(const std::string& key, double value);
    
private:
    LogLevel level_;
    std::ostringstream oss_;
    std::string file_;
    int line_;
    std::string function_;
    std::unordered_map<std::string, std::string> kvPairs_;
    
    // For streaming context
    std::string message_;
};

#define NLM_LOG_STREAM(level) nlm::LogStream(level, __FILE__, __LINE__, __func__)

// Configuration utilities
namespace config {
    std::string loadFromFile(const std::string& filepath);
    std::string mergeConfigurations(const std::string& base, const std::string& override);
    std::string validateConfiguration(const std::string& configJson);
}

// Helper functions
namespace utils {
    std::string generateCorrelationId();
    std::string levelToString(LogLevel level);
    LogLevel stringToLevel(const std::string& levelStr);
    std::unordered_map<std::string, std::string> parseKeyValue(const std::string& kvString);
    uint64_t getThreadId();
}

} // namespace nlm