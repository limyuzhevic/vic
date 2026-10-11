#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <unordered_map>
#include <mutex>
#include <thread>
#include <optional>
#include <variant>
#include <nlohmann/json.hpp>

namespace nlm {

// Enhanced log severity levels with fine-grained hierarchy
enum class LogLevel {
    // DEBUG level family
    Debug = 0,
    Debug2 = 1,
    Debug3 = 2,
    Debug4 = 3,
    Trace = 4,
    
    // Standard levels
    Info = 10,
    
    // WARNING level family
    Warning = 20,
    Notice = 21,
    
    // ERROR level family
    Error = 30,
    Alert = 31,
    
    // Critical levels
    Critical = 40,
    Emergency = 41,
    Fatal = 42
};

// Log level comparison for hierarchy
inline bool operator<(const LogLevel& a, const LogLevel& b) {
    return static_cast<int>(a) < static_cast<int>(b);
}

inline bool operator<=(const LogLevel& a, const LogLevel& b) {
    return static_cast<int>(a) <= static_cast<int>(b);
}

inline bool operator>(const LogLevel& a, const LogLevel& b) {
    return static_cast<int>(a) > static_cast<int>(b);
}

inline bool operator>=(const LogLevel& a, const LogLevel& b) {
    return static_cast<int>(a) >= static_cast<int>(b);
}

// Enhanced Log entry structure with structured logging support
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    
    // Structured logging fields
    std::string component;           // Component name (e.g., "brain", "sensor", "motor")
    std::string threadId;            // Thread identifier
    std::string correlationId;       // For grouping related log messages
    std::unordered_map<std::string, std::string> structuredFields;  // Key-value pairs
    
    // Performance metrics
    double executionTimeMs = 0.0;    // Time since previous log entry
    size_t memoryUsageBytes = 0;    // Memory usage at time of logging
    
    // Enhanced source location
    std::string sourceFile;
    std::string sourceFunction;
    int sourceLine = 0;
    
    // For lazy formatting
    mutable bool formatted = false;
    mutable std::string formattedMessage;
};

// Log buffer strategy enumeration
enum class BufferStrategy {
    None,
    Simple,
    Adaptive,
    Circular
};

// Log sink interface for multiple output targets
class ILogSink {
public:
    virtual ~ILogSink() = default;
    virtual void write(const LogEntry& entry) = 0;
    virtual void flush() = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
    virtual std::string getName() const = 0;
};

// File sink with rotation and compression support
class FileLogSink : public ILogSink {
public:
    FileLogSink(const std::string& filepath, size_t maxFileSize = 10 * 1024 * 1024, // 10MB
                bool compress = true, const std::string& rotationPattern = "yyyy-MM-dd");
    ~FileLogSink() override;
    
    void write(const LogEntry& entry) override;
    void flush() override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    std::string getName() const override;
    
    // Force rotation
    void rotate();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Async logger with thread-safe queue
class AsyncLogger {
public:
    AsyncLogger(size_t maxQueueSize = 100000, size_t workerThreads = 2);
    ~AsyncLogger();
    
    void log(const LogEntry& entry);
    void shutdown();
    void addSink(std::shared_ptr<ILogSink> sink);
    void setLevel(LogLevel level);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Logger configuration
struct LoggerConfig {
    // Global settings
    LogLevel globalLevel = LogLevel::Info;
    bool useStructuredLogging = true;
    bool useColors = true;
    bool enableAsyncLogging = true;
    
    // Per-component settings
    std::unordered_map<std::string, LogLevel> componentLevels;
    
    // Performance settings
    size_t maxLogEntrySize = 4096;
    size_t maxQueueSize = 100000;
    size_t workerThreads = 2;
    
    // File settings
    std::string logFilePath = "nlm.log";
    size_t maxFileSize = 10 * 1024 * 1024; // 10MB
    bool compressLogs = true;
    
    // Formatting
    std::string timestampFormat = "%Y-%m-%d %H:%M:%S.%ms";
    std::string structuredFormat = "json";
    
    // Filtering
    std::vector<std::string> excludePatterns;
    std::unordered_map<std::string, std::vector<std::string>> componentMessageFilters;
};

// Main Logger class with enhanced capabilities
class Logger {
public:
    Logger(const LoggerConfig& config = LoggerConfig());
    ~Logger();
    
    // Disable copying
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    // Add logger sink
    void addSink(std::shared_ptr<ILogSink> sink);
    
    // Remove all loggers
    void clearSinks();
    
    // Set minimum level for all sinks
    void setLevel(LogLevel level);
    
    // Set per-component level
    void setComponentLevel(const std::string& component, LogLevel level);
    LogLevel getComponentLevel(const std::string& component) const;
    
    // Global logging functions with enhanced parameters
    void log(LogLevel level, const std::string& message, 
             const std::string& file = "", int line = 0, 
             const std::string& function = "", 
             const std::string& component = "",
             const std::string& correlationId = "",
             const std::unordered_map<std::string, std::string>& structuredFields = {});
    
    // Convenience methods with correlation ID support
    void debug(const std::string& message, const std::string& file = "", int line = 0, 
               const std::string& function = "", const std::string& component = "",
               const std::string& correlationId = "");
    void info(const std::string& message, const std::string& file = "", int line = 0,
              const std::string& function = "", const std::string& component = "",
              const std::string& correlationId = "");
    void warning(const std::string& message, const std::string& file = "", int line = 0,
                 const std::string& function = "", const std::string& component = "",
                 const std::string& correlationId = "");
    void error(const std::string& message, const std::string& file = "", int line = 0,
               const std::string& function = "", const std::string& component = "",
               const std::string& correlationId = "");
    void critical(const std::string& message, const std::string& file = "", int line = 0,
                  const std::string& function = "", const std::string& component = "",
                  const std::string& correlationId = "");
    
    // Check if level is enabled for a component
    bool isEnabled(LogLevel level, const std::string& component = "") const;
    
    // Get global logger instance
    static Logger& getGlobal();
    
    // Set global logger
    static void setGlobal(std::shared_ptr<Logger> logger);
    
    // Configuration access
    const LoggerConfig& getConfig() const { return config_; }
    void updateConfig(const LoggerConfig& config);
    
    // Performance monitoring
    struct PerformanceStats {
        uint64_t totalLogs = 0;
        uint64_t droppedLogs = 0;
        uint64_t filteredLogs = 0;
        double avgLogTimeMs = 0.0;
        size_t currentMemoryUsage = 0;
    };
    
    PerformanceStats getPerformanceStats() const;
    
    // Start/stop performance metrics collection
    void startPerformanceMonitoring();
    void stopPerformanceMonitoring();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    LoggerConfig config_;
};

// Convenience macro for file/line/function info
#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)

// Enhanced stream-style logging with correlation IDs and structured data
class LogStream {
public:
    LogStream(LogLevel level, const std::string& file, int line, const std::string& function,
              const std::string& component = "", const std::string& correlationId = "");
    ~LogStream();
    
    template<typename T>
    LogStream& operator<<(const T& value) {
        oss_ << value;
        return *this;
    }
    
    // Add structured field
    template<typename T>
    LogStream& addField(const std::string& key, const T& value) {
        structuredFields_[key] = nlm::toString(value);
        return *this;
    }
    
    // Set correlation ID
    LogStream& correlationId(const std::string& id) {
        correlationId_ = id;
        return *this;
    }
    
    // Set component
    LogStream& component(const std::string& comp) {
        component_ = comp;
        return *this;
    }
    
private:
    LogLevel level_;
    std::ostringstream oss_;
    std::string file_;
    int line_;
    std::string function_;
    std::string component_;
    std::string correlationId_;
    std::unordered_map<std::string, std::string> structuredFields_;
};

// Helper function for converting various types to string
template<typename T>
std::string toString(const T& value);

// Macro for enhanced logging with correlation IDs and structured data
#define NLM_LOG_STREAM(level) nlm::LogStream(level, __FILE__, __LINE__, __func__)

// Backward compatibility macros
#define NLM_LOG_DEBUG_COMPACT(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO_COMPACT(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__)

// Structured logging helper functions
namespace log_helpers {
    std::string generateCorrelationId();
    std::string getThreadId();
    std::string formatTimestamp(const std::chrono::system_clock::time_point& timestamp,
                                const std::string& format = "%Y-%m-%d %H:%M:%S.%ms");
    nlohmann::json entryToJSON(const LogEntry& entry);
    std::string entryToString(const LogEntry& entry, bool useStructured = false);
}

} // namespace nlm

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

// Main Logger class
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
    
    // Global logging functions
    void log(LogLevel level, const std::string& message, 
             const std::string& file = "", int line = 0, 
             const std::string& function = "");
    
    // Convenience methods
    void debug(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void info(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void warning(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void error(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void critical(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    // Check if level is enabled
    bool isEnabled(LogLevel level) const;
    
    // Get global logger instance
    static Logger& getGlobal();
    
    // Set global logger
    static void setGlobal(std::shared_ptr<Logger> logger);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Convenience macro for file/line/function info
#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)

// Stream-style logging helper
class LogStream {
public:
    LogStream(LogLevel level, const std::string& file, int line, const std::string& function);
    ~LogStream();
    
    template<typename T>
    LogStream& operator<<(const T& value) {
        oss_ << value;
        return *this;
    }
    
private:
    LogLevel level_;
    std::ostringstream oss_;
    std::string file_;
    int line_;
    std::string function_;
};

#define NLM_LOG_STREAM(level) nlm::LogStream(level, __FILE__, __LINE__, __func__)

} // namespace nlm
