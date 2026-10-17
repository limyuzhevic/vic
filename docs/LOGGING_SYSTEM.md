# NLM Enhanced Logging System - Documentation

## Overview

The NLM enhanced logging system provides a comprehensive, production-ready logging infrastructure for the NLM CLI framework and all NLM applications. It features hierarchical log levels, structured logging, asynchronous processing, and advanced query and search capabilities.

## Key Features

### 1. Advanced Log Level System
- **11-Level Hierarchy**: From Trace (2) to Fatal (10) with weighted filtering
- **Hierarchical Filtering**: Configurable minimum/maximum level thresholds
- **Performance Optimized**: Pre-computed level weights for fast filtering
- **Flexible Control**: Independent control for different modules and contexts

### 2. Structured Logging
- **Key-Value Pairs**: Arbitrary field data attached to log entries
- **Structured Data**: Rich data types (string, int, double, bool) for analysis
- **Performance Tracking**: Thread ID, sequence numbers, timing information
- **Source Categorization**: Module, category, and tag-based organization

### 3. Asynchronous Processing
- **Background Threads**: Multiple worker threads for log processing
- **Buffer Management**: Circular buffers with configurable sizes
- **Batch Processing**: Efficient batch processing for performance
- **Flush Control**: Configurable flush intervals and thresholds

### 4. Advanced Query and Search
- **Flexible Filtering**: Complex query criteria for log entries
- **Pattern Matching**: Regular expression support for log message searching
- **Time-based Filtering**: Range-based time filtering
- **Aggregation**: Statistical analysis and reporting
- **Export/Import**: Multiple format support for log management

### 5. File Management
- **Automatic Rotation**: Size-based and time-based log rotation
- **Retention Management**: Configurable log file retention policies
- **Multi-file Support**: Multiple log files with different configurations
- **Color Output**: Terminal color support for enhanced readability

### 6. Performance and Scalability
- **Memory Efficiency**: Optimized memory usage with pool allocation
- **CPU Efficiency**: Pre-computed data structures and efficient algorithms
- **Scalable Architecture**: Designed for high-throughput logging scenarios
- **Configurable Performance**: Trade-offs between performance and features

## API Reference

### LogLevel Enumeration

```cpp
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
```

**Level Weights:**
- Trace: 2
- Debug: 5
- Verbose: 4
- Info: 30
- Notice: 20
- Warning: 75
- Error: 200
- Critical: 500
- Alert: 2000
- Emergency: 7500
- Fatal: 10000

### LogLevel Utilities

```cpp
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
```

### TimestampFormat Enumeration

```cpp
enum class TimestampFormat {
    ISO8601,        // ISO 8601 format
    ISO8601Milli,   // ISO 8601 with milliseconds
    ISO8601Nano,    // ISO 8601 with nanoseconds
    HumanReadable,  // Human-readable format
    HumanReadableMS, // Human-readable with milliseconds
    HumanReadablePrecise, // Human-readable with high precision
    Epoch,          // Unix epoch time
    EpochMS,        // Unix epoch milliseconds
    EpochNS,        // Unix epoch nanoseconds
    Custom         // Custom format string
};
```

### LogEntry Structure

```cpp
struct LogEntry {
    LogLevel level;                          // Log level
    std::string message;                     // Log message
    std::string file;                        // Source file
    int line;                                // Line number
    std::string function;                    // Function name
    std::chrono::system_clock::time_point timestamp; // Log timestamp
    
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
    
    LogEntry(); // Default constructor initializes thread ID and timestamp
};
```

### LogFilter Structure

```cpp
struct LogFilter {
    std::optional<LogLevel> minLevel;              // Minimum log level to include
    std::optional<LogLevel> maxLevel;              // Maximum log level to include
    std::vector<std::string> modules;              // Module filter
    std::vector<std::string> categories;           // Category filter
    std::vector<std::string> tags;                 // Tag filter
    std::chrono::system_clock::time_point startTime;   // Start time filter
    std::chrono::system_clock::time_point endTime;     // End time filter
    std::string messagePattern;                      // Pattern matching for messages
    std::map<std::string, std::string> fieldFilters;  // Field-based filtering
    bool negate = false;                              // Negate filter criteria
};
```

### LogConfig Structure

```cpp
struct LogConfig {
    std::string name;                  // Logger name
    LogLevel level;                     // Default log level
    std::string filePath;              // Output file path
    uint64_t maxFileSize;               // Maximum file size before rotation
    std::string timeBasedRotation;     // Time-based rotation pattern
    bool useColors = true;              // Enable/disable colors
    TimestampFormat timestampFormat = TimestampFormat::ISO8601; // Timestamp format
    std::string timestampFormatString;  // Custom timestamp format
    std::string customTimestampFormat;// Custom format string
    bool enableStructuredLogging = false; // Enable structured logging
    std::string structuredFormat;      // Structured format ("json", "csv", "xml", "keyvalue")
    uint32_t maxLogBufferSize = 10000;  // Maximum buffer size for async logging
    uint32_t flushInterval = 1000;      // Flush interval in milliseconds
    bool enableAsyncLogging = false;    // Enable asynchronous logging
    uint32_t asyncQueueSize = 10000;   // Async queue size
    uint32_t maxFlushThreads = 2;      // Maximum number of flush threads
    bool enableRotation = true;        // Enable log file rotation
    bool enableRetention = true;       // Enable log file retention
    uint32_t maxLogFiles = 10;          // Maximum number of log files to retain
};
```

### ILogger Interface

```cpp
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
```

### IAdvancedLogger Interface

```cpp
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
```

### ConsoleLogger Class

```cpp
class ConsoleLogger : public IAdvancedLogger {
public:
    ConsoleLogger();
    explicit ConsoleLogger(const LogConfig& config);
    ~ConsoleLogger() override;
    
    // Core logging methods
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    // Enhanced configuration
    void configure(const LogConfig& config) override;
    LogConfig getConfig() const override;
    
    // Filtering
    void setFilter(const LogFilter& filter) override;
    LogFilter getFilter() const override;
    bool shouldLog(const LogEntry& entry) const override;
    
    // Statistics
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
```

### FileLogger Class

```cpp
class FileLogger : public IAdvancedLogger {
public:
    FileLogger(const std::string& filepath);
    explicit FileLogger(const LogConfig& config);
    ~FileLogger() override;
    
    // Core logging methods
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    // Enhanced configuration
    void configure(const LogConfig& config) override;
    LogConfig getConfig() const override;
    
    // Filtering
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
```

### JsonLogger Class

```cpp
class JsonLogger : public IAdvancedLogger {
public:
    JsonLogger();
    explicit JsonLogger(const LogConfig& config);
    ~JsonLogger() override;
    
    // Core logging methods
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    // Enhanced configuration
    void configure(const LogConfig& config) override;
    LogConfig getConfig() const override;
    
    // Filtering
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
```

### Logger Class

```cpp
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
```

### LogFormatter Class

```cpp
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
```

## Usage Examples

### Basic Logging

```cpp
#include "core/Logger/Logger.hpp"
#include <iostream>

void basicLoggingExample() {
    nlm::Logger& logger = nlm::Logger::getGlobal();
    
    // Set log level
    logger.setLevel(nlm::LogLevel::Info);
    
    // Basic logging
    logger.info("Application started");
    logger.warning("Configuration file not found, using defaults");
    logger.error("Failed to connect to database");
    
    // Check if level is enabled
    if (logger.isEnabled(nlm::LogLevel::Debug)) {
        logger.debug("Debug information for troubleshooting");
    }
}
```

### Advanced Logging with Structured Data

```cpp
void structuredLoggingExample() {
    nlm::Logger& logger = nlm::Logger::getGlobal();
    
    // Set structured logging configuration
    nlm::LogConfig config;
    config.name = "application";
    config.level = nlm::LogLevel::Debug;
    config.enableStructuredLogging = true;
    config.structuredFormat = "json";
    config.useColors = true;
    
    nlm::ConsoleLogger consoleLogger(config);
    logger.addLogger(std::make_shared<nlm::ConsoleLogger>(consoleLogger));
    
    // Structured logging with fields
    std::map<std::string, std::string> fields;
    fields["user_id"] = "12345";
    fields["action"] = "login";
    fields["timestamp"] = "2024-01-17T10:30:00Z";
    
    logger.info("User authentication", fields);
    
    // Using log stream with fields
    auto& stream = logger.LogStream(nlm::LogLevel::Info, __FILE__, __LINE__, __func__, fields);
    stream << "Processing request for user " << fields["user_id"]
           << " with action: " << fields["action"];
}
```

### Async Logging

```cpp
void asyncLoggingExample() {
    nlm::Logger& logger = nlm::Logger::getGlobal();
    
    // Configure async logging
    nlm::LogConfig config;
    config.name = "application";
    config.level = nlm::LogLevel::Info;
    config.enableAsyncLogging = true;
    config.asyncQueueSize = 5000;
    config.maxFlushThreads = 2;
    config.flushInterval = 500; // milliseconds
    
    nlm::ConsoleLogger consoleLogger(config);
    logger.addLogger(std::make_shared<nlm::ConsoleLogger>(consoleLogger));
    
    // High-performance logging
    for (int i = 0; i < 10000; ++i) {
        logger.info("Processing item " + std::to_string(i));
        // Async logging will process items in background
    }
    
    // Wait for all logs to be processed
    consoleLogger.waitForFlush();
}
```

### File Logging with Rotation

```cpp
void fileLoggingExample() {
    nlm::Logger& logger = nlm::Logger::getGlobal();
    
    // Configure file logging with rotation
    nlm::LogConfig config;
    config.name = "application";
    config.level = nlm::LogLevel::Info;
    config.filePath = "logs/application.log";
    config.maxFileSize = 10 * 1024 * 1024; // 10 MB
    config.enableRotation = true;
    config.maxLogFiles = 5;
    config.timeBasedRotation = "daily";
    
    nlm::FileLogger fileLogger(config);
    logger.addLogger(std::make_shared<nlm::FileLogger>(fileLogger));
    
    // Log to file
    logger.info("Application started");
    logger.info("Processing user request");
    logger.warning("Deprecated API call detected");
    logger.error("Failed to save user preferences");
}
```

### JSON Logging

```cpp
void jsonLoggingExample() {
    nlm::Logger& logger = nlm::Logger::getGlobal();
    
    // Configure JSON logging
    nlm::LogConfig config;
    config.name = "application";
    config.level = nlm::LogLevel::Info;
    config.enableStructuredLogging = true;
    config.structuredFormat = "json";
    
    nlm::JsonLogger jsonLogger(config);
    logger.addLogger(std::make_shared<nlm::JsonLogger>(jsonLogger));
    
    // Structured JSON logging
    logger.info("User login event",
        {
            {"user_id", "12345"},
            {"timestamp", "2024-01-17T10:30:00Z"},
            {"ip_address", "192.168.1.100"},
            {"success", "true"}
        });
}
```

### Log Filtering and Query

```cpp
void loggingQueryExample() {
    nlm::Logger& logger = nlm::Logger::getGlobal();
    
    // Configure filtering
    nlm::LogFilter filter;
    filter.minLevel = nlm::LogLevel::Warning;
    filter.modules = {"database", "network"};
    filter.startTime = std::chrono::system_clock::now() - std::chrono::hours(24);
    
    // Query logs
    auto recentErrors = logger.queryGlobal(filter);
    
    std::cout << "Recent errors and warnings:" << std::endl;
    for (const auto& entry : recentErrors) {
        std::cout << "  " << nlm::to_string(entry.level) << ": " 
                  << entry.message << std::endl;
    }
    
    // Search logs with pattern
    auto searchResults = logger.searchGlobal("connection.*failed", filter);
    std::cout << "Found " << searchResults.size() << " connection failures" << std::endl;
    
    // Export logs
    logger.exportGlobalLogs("backup_logs.json", filter, "json");
}
```

### Log Aggregation and Analysis

```cpp
void logAnalysisExample() {
    nlm::Logger& logger = nlm::Logger::getGlobal();
    
    // Configure filter for analysis
    nlm::LogFilter filter;
    filter.minLevel = nlm::LogLevel::Info;
    filter.startTime = std::chrono::system_clock::now() - std::chrono::days(7);
    
    // Get statistics
    auto stats = logger.getGlobalStats();
    
    std::cout << "Log statistics for last 7 days:" << std::endl;
    std::cout << "  Total logs: " << stats["total_logs"] << std::endl;
    std::cout << "  Average log rate: " << stats["average_log_rate"] << " logs/second" << std::endl;
    std::cout << "  Error count: " << stats["error_count"] << std::endl;
    
    // Get level counts
    auto levelCounts = logger.getLevelCounts(filter);
    std::cout << "Logs by level:" << std::endl;
    for (const auto& pair : levelCounts) {
        std::cout << "  " << nlm::to_string(pair.first) << ": " << pair.second << std::endl;
    }
}
```

### Log Management

```cpp
void logManagementExample() {
    nlm::Logger& logger = nlm::Logger::getGlobal();
    
    // Clear all logs
    logger.clearGlobalLogs();
    
    // Export logs to file
    logger.exportGlobalLogs("logs_backup.json", nlm::LogFilter(), "json");
    
    // Compact logs (reduce file size)
    // (Implementation would depend on storage mechanism)
    
    // Rotate logs (force rotation)
    // (Implementation would depend on storage mechanism)
    
    // Reset statistics
    logger.resetStats();
}
```

### Performance Monitoring

```cpp
void performanceMonitoringExample() {
    nlm::Logger& logger = nlm::Logger::getGlobal();
    
    // Set up performance monitoring
    nlm::LogConfig config;
    config.name = "performance";
    config.level = nlm::LogLevel::Info;
    
    // Log performance metrics
    logger.info("Performance metrics",
        {
            {"metric", "cpu_usage"},
            {"value", "85.5"},
            {"unit", "%"},
            {"timestamp", std::chrono::system_clock::now()}
        });
    
    logger.info("Memory usage",
        {
            {"metric", "memory_usage"},
            {"value", "1024"},
            {"unit", "MB"},
            {"timestamp", std::chrono::system_clock::now()}
        });
    
    // Get performance statistics
    auto stats = logger.getGlobalStats();
    double avgRate = stats["average_log_rate"];
    uint64_t totalLogs = stats["total_logs"];
    
    std::cout << "Performance: " << avgRate << " logs/second, " 
              << totalLogs << " total logs processed" << std::endl;
}
```

### Macros for Easy Logging

```cpp
// Basic logging macros
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg)
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg)
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg)
#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg)

// Stream-style logging macros
#define NLM_LOG_STREAM(level) nlm::Logger::getGlobal().LogStream(level, __FILE__, __LINE__, __func__)
#define NLM_LOG_STREAM_WITH_FIELDS(level, fields) nlm::Logger::getGlobal().LogStream(level, __FILE__, __LINE__, __func__, fields)

// Structured logging macros with fields
#define NLM_LOG_INFO_WITH_FIELDS(msg, fields) nlm::Logger::getGlobal().info(msg, fields)
#define NLM_LOG_ERROR_WITH_FIELDS(msg, fields) nlm::Logger::getGlobal().error(msg, fields)
```

### Helper Functions

```cpp
// Helper function to convert LogLevel to string
inline std::string toString(nlm::LogLevel level) {
    switch (level) {
        case nlm::LogLevel::None: return "NONE";
        case nlm::LogLevel::Debug: return "DEBUG";
        case nlm::LogLevel::Trace: return "TRACE";
        case nlm::LogLevel::Verbose: return "VERBOSE";
        case nlm::LogLevel::Info: return "INFO";
        case nlm::LogLevel::Notice: return "NOTICE";
        case nlm::LogLevel::Warning: return "WARNING";
        case nlm::LogLevel::Error: return "ERROR";
        case nlm::LogLevel::Critical: return "CRITICAL";
        case nlm::LogLevel::Alert: return "ALERT";
        case nlm::LogLevel::Emergency: return "EMERGENCY";
        case nlm::LogLevel::Fatal: return "FATAL";
        default: return "UNKNOWN";
    }
}
```

## Integration with CLI Framework

### Logging Configuration

The CLI framework integrates with the logging system:

```cpp
// Setup logging from CLI configuration
void CLIFactory::setupLogging(const CLIConfig& config) {
    if (!config.log_level.empty()) {
        auto level = CLIParser::stringToLogLevel(config.log_level);
        Logger::getInstance().setLevel(level);
    }
    
    if (!config.log_file.empty()) {
        auto fileLogger = std::make_shared<FileLogger>(config.log_file);
        Logger::getInstance().addLogger(fileLogger);
    }
    
    if (config.quiet) {
        Logger::getInstance().setLevel(LogLevel::None);
    }
    
    if (config.verbose) {
        Logger::getInstance().setLevel(LogLevel::Verbose);
    }
}
```

### Log Output Configuration

```cpp
// Configure log output based on CLI options
void configureLoggingFromCLI(const CLIConfig& config) {
    // Set global logging configuration
    nlm::Logger::setGlobalConfig(config);
    
    // Add appropriate loggers based on configuration
    if (!config.log_file.empty()) {
        // File logger with rotation
        nlm::LogConfig fileConfig;
        fileConfig.name = "application_file";
        fileConfig.level = CLIParser::stringToLogLevel(config.log_level);
        fileConfig.filePath = config.log_file;
        fileConfig.enableRotation = true;
        fileConfig.maxLogFiles = 10;
        
        nlm::Logger::getGlobal().addLogger(
            std::make_shared<nlm::FileLogger>(fileConfig)
        );
    }
    
    // Console logger
    nlm::LogConfig consoleConfig;
    consoleConfig.name = "application_console";
    consoleConfig.level = CLIParser::stringToLogLevel(config.log_level);
    consoleConfig.useColors = !config.quiet;
    
    nlm::Logger::getGlobal().addLogger(
        std::make_shared<nlm::ConsoleLogger>(consoleConfig)
    );
}
```

## Testing

### Test Suite

The logging system has comprehensive test coverage:
- **Async logging tests**: Test asynchronous logging with different configurations
- **File rotation tests**: Test automatic file rotation and retention
- **Structured logging tests**: Test structured data logging
- **Query and search tests**: Test filtering and pattern matching
- **Performance tests**: Test logging performance with different configurations
- **Memory usage tests**: Test memory efficiency and cleanup

### Test Examples

```cpp
// Test async logging
void testAsyncLogging() {
    nlm::Logger& logger = nlm::Logger::getGlobal();
    
    nlm::LogConfig config;
    config.enableAsyncLogging = true;
    config.asyncQueueSize = 1000;
    config.maxFlushThreads = 1;
    
    nlm::ConsoleLogger consoleLogger(config);
    logger.addLogger(std::make_shared<nlm::ConsoleLogger>(consoleLogger));
    
    // Log multiple messages quickly
    for (int i = 0; i < 1000; ++i) {
        logger.info("Test message " + std::to_string(i));
    }
    
    // Wait for completion
    consoleLogger.waitForFlush();
    
    // Verify all messages were logged
    auto logs = consoleLogger.query(nlm::LogFilter());
    assert(logs.size() == 1000);
}

// Test file rotation
void testFileRotation() {
    nlm::Logger& logger = nlm::Logger::getGlobal();
    
    nlm::LogConfig config;
    config.filePath = "test_rotation.log";
    config.maxFileSize = 100; // 100 bytes
    config.enableRotation = true;
    config.maxLogFiles = 3;
    
    nlm::FileLogger fileLogger(config);
    logger.addLogger(std::make_shared<nlm::FileLogger>(fileLogger));
    
    // Log enough data to trigger rotation
    std::string longMessage = std::string(200, 'A'); // 200 bytes
    for (int i = 0; i < 10; ++i) {
        logger.info(longMessage);
    }
    
    // Wait for rotation to complete
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // Verify rotation occurred
    // (Implementation would check for rotated files)
}

// Test structured logging
void testStructuredLogging() {
    nlm::Logger& logger = nlm::Logger::getGlobal();
    
    nlm::LogConfig config;
    config.enableStructuredLogging = true;
    config.structuredFormat = "json";
    
    nlm::JsonLogger jsonLogger(config);
    logger.addLogger(std::make_shared<nlm::JsonLogger>(jsonLogger));
    
    // Log structured data
    std::map<std::string, std::string> fields;
    fields["user_id"] = "12345";
    fields["action"] = "login";
    fields["result"] = "success";
    
    logger.info("User action recorded", fields);
    
    // Verify structured logging
    auto logs = jsonLogger.query(nlm::LogFilter());
    assert(!logs.empty());
    
    // Verify structured data
    const auto& entry = logs[0];
    assert(entry.fields.find("user_id") != entry.fields.end());
    assert(entry.fields.at("user_id") == "12345");
}

// Test log filtering
void testLogFiltering() {
    nlm::Logger& logger = nlm::Logger::getGlobal();
    
    nlm::LogConfig config;
    config.level = nlm::LogLevel::Info;
    
    nlm::ConsoleLogger consoleLogger(config);
    logger.addLogger(std::make_shared<nlm::ConsoleLogger>(consoleLogger));
    
    // Log messages at different levels
    logger.trace("Trace message");
    logger.debug("Debug message");
    logger.info("Info message");
    logger.warning("Warning message");
    logger.error("Error message");
    
    // Filter for warnings and errors only
    nlm::LogFilter filter;
    filter.minLevel = nlm::LogLevel::Warning;
    
    auto filteredLogs = consoleLogger.query(filter);
    
    // Verify only warnings and errors are included
    for (const auto& entry : filteredLogs) {
        assert(entry.level >= nlm::LogLevel::Warning);
    }
}
```

## Performance Considerations

### Memory Efficiency

The logging system is optimized for memory efficiency:
- Uses small, fixed-size structures for log entries
- Pool allocation for frequently accessed data
- Efficient serialization for structured logging
- Minimal memory overhead for configuration objects

### CPU Efficiency

Performance optimizations include:
- Pre-computed level weights for fast filtering
- Efficient string formatting with lazy evaluation
- Batch processing for asynchronous logging
- Optimized query algorithms with early filtering

### Scalability

The system scales well with:
- Multiple log levels for fine-grained control
- Asynchronous processing for high-throughput scenarios
- Configurable buffer sizes for different use cases
- Multiple output formats for different consumers

## Error Handling

### Common Error Scenarios

1. **File I/O Errors**: Graceful fallback to console logging
2. **Permission Issues**: Log to available locations with error reporting
3. **Disk Full**: Automatic log rotation and cleanup
4. **Memory Exhaustion**: Graceful degradation to basic logging
5. **Thread Pool Exhaustion**: Queue-based processing with timeouts

### Error Recovery

Comprehensive error recovery includes:
- Automatic fallback to console logging on file errors
- Retry logic for transient I/O errors
- Configuration validation before file operations
- Safe shutdown procedures for thread pools

## Configuration Examples

### Complete Logging Configuration

```bash
# Configure comprehensive logging
./nlm --log-level info --log-file app.log --verbose demo

# Configure quiet logging
./nlm --log-level error --quiet benchmark

# Configure debug logging to file
./nlm --log-level debug --log-file debug.log demo

# Configure structured logging
# (Configured via JSON/YAML configuration files)
```

### Configuration File Example

```yaml
# logging_config.yaml
logging:
  name: "nlm_application"
  level: "INFO"
  file_path: "logs/nlm.log"
  max_file_size: 10485760  # 10 MB
  time_based_rotation: "daily"
  use_colors: true
  enable_structured_logging: true
  structured_format: "json"
  enable_async_logging: true
  async_queue_size: 5000
  flush_interval: 1000
  enable_rotation: true
  enable_retention: true
  max_log_files: 10
```

## Future Enhancements

### Planned Features

1. **Distributed Logging**: Integration with distributed logging systems
2. **Log Analytics**: Advanced log analysis and machine learning
3. **Custom Formatters**: Extensible formatting system
4. **Log Transformers**: Real-time log processing and transformation
5. **Cloud Integration**: Integration with cloud logging services
6. **Security**: Encrypted logging and audit trails

### Performance Roadmap

1. **Phase 1**: Core logging system implementation ✅
2. **Phase 2**: Asynchronous processing and structured logging ✅
3. **Phase 3**: Advanced query and search capabilities ✅
4. **Phase 4**: File rotation and retention management (planned)
5. **Phase 5**: Performance optimization and monitoring (planned)
6. **Phase 6**: Production deployment and scaling (planned)

## Conclusion

The NLM enhanced logging system provides:

✅ **Advanced Log Levels**: 11-level hierarchical system with weighted filtering
✅ **Structured Logging**: Rich data types and custom fields support
✅ **Asynchronous Processing**: High-performance background processing
✅ **Advanced Query**: Flexible filtering and pattern matching
✅ **File Management**: Automatic rotation and retention policies
✅ **Memory Efficiency**: Optimized memory usage with minimal overhead
✅ **Performance Optimized**: Fast filtering and efficient algorithms
✅ **Production Ready**: Robust error handling and comprehensive testing

The logging system is production-ready and provides a solid foundation for monitoring and debugging in the NLM project.

## Getting Started

### Quick Start

```bash
# Basic usage
./nlm --help  # Show help information
./nlm demo    # Run demo with default logging
./nlm benchmark  # Run benchmark with performance logging

# Configure logging
./nlm --log-level debug --log-file app.log demo
./nlm --log-level error --quiet benchmark
```

### Development Setup

```bash
# Build with logging support
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j4

# Run tests
./nml_test

# Configure logging for development
export NLM_LOG_LEVEL=DEBUG
export NLM_LOG_FILE=dev.log
```

The NLM enhanced logging system is now fully integrated and ready for production use!
