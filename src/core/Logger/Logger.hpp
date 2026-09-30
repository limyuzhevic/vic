#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <unordered_map>
#include <deque>
#include <mutex>
#include <algorithm>

namespace nlm {

// Enhanced Log severity levels
enum class LogLevel {
    Trace = -1,           // Detailed fine-grained information
    Debug = 0,            // General debugging information
    Info = 1,             // General information
    Notice = 2,           // Important events
    Warning = 3,          // Warnings
    Error = 4,            // Error conditions
    Critical = 5,         // Critical errors
    Alert = 6,            // Action must be taken immediately
    Emergency = 7         // System is unusable
};

// Log entry structure with extended information
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    std::string threadId;
    uint64_t threadHash;
    
    // Performance tracking
    double durationMs;
    std::string context;
    std::unordered_map<std::string, std::string> metadata;
};

// Performance metrics structure
struct PerformanceMetrics {
    double timestamp;
    std::string operation;
    double durationMs;
    std::string threadId;
    std::string file;
    int line;
    std::string function;
    std::unordered_map<std::string, std::string> metadata;
};

// Log statistics
struct LogStatistics {
    uint64_t totalLogs;
    uint64_t logsByLevel[8]; // One for each LogLevel
    double firstLogTime;
    double lastLogTime;
    uint64_t logsPerSecond;
    uint64_t memoryUsage;
    uint64_t performanceEvents;
};

// Performance tracker for detailed timing
class PerformanceTracker {
public:
    PerformanceTracker();
    ~PerformanceTracker();
    
    // Start timing an operation
    void start(const std::string& operation, const std::string& context = "");
    
    // Stop timing and record the measurement
    void stop(const std::string& operation, 
              const std::string& file = "", 
              int line = 0, 
              const std::string& function = "");
    
    // Get current timing duration (if active)
    double getCurrentDuration() const;
    
    // Clear all recorded metrics
    void clear();
    
    // Get all performance metrics
    const std::vector<PerformanceMetrics>& getMetrics() const;
    
    // Get performance metrics for specific operation
    std::vector<PerformanceMetrics> getMetricsByOperation(const std::string& operation) const;
    
    // Calculate average duration for operation
    double getAverageDuration(const std::string& operation) const;
    
    // Get performance statistics
    std::string getStatistics() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Advanced ILogger interface with performance tracking
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
    
    // Performance tracking support
    virtual void logPerformance(const PerformanceMetrics& metrics) = 0;
    virtual bool isPerformanceTrackingEnabled() const { return false; }
    
    // Context tracking
    virtual void setContext(const std::string& context) {}
    virtual std::string getContext() const { return ""; }
    
    // Filtering support
    virtual bool shouldLog(const LogEntry& entry) const { return true; }
    virtual void addLogFilter(std::function<bool(const LogEntry&)> filter) {}
};

// Enhanced Console logger with colors and performance tracking
class ConsoleLogger : public ILogger {
public:
    ConsoleLogger();
    explicit ConsoleLogger(LogLevel level);
    ~ConsoleLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    // Performance tracking support
    void logPerformance(const PerformanceMetrics& metrics) override;
    bool isPerformanceTrackingEnabled() const override { return true; }
    
    // Configuration options
    void setUseColors(bool useColors);
    bool getUseColors() const;
    
    void setOutputFile(const std::string& filepath);
    void closeOutputFile();
    
    // Performance logging options
    void setPerformanceLoggingEnabled(bool enabled);
    bool isPerformanceLoggingEnabled() const;
    
    // Color configuration
    void setColorScheme(const std::string& scheme);
    
    // Statistics
    std::string getPerformanceStatistics() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
    std::string formatPerformance(const PerformanceMetrics& metrics) const;
};

// Memory logger for capturing log entries in memory
class MemoryLogger : public ILogger {
public:
    MemoryLogger();
    explicit MemoryLogger(size_t maxEntries);
    ~MemoryLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    // Performance tracking support
    void logPerformance(const PerformanceMetrics& metrics) override;
    bool isPerformanceTrackingEnabled() const override { return true; }
    
    // Memory management
    void clear();
    size_t getEntryCount() const;
    
    // Access to log entries
    const std::vector<LogEntry>& getEntries() const;
    std::vector<LogEntry> getEntriesByLevel(LogLevel level) const;
    std::vector<LogEntry> getEntriesByContext(const std::string& context) const;
    std::vector<LogEntry> getEntriesByTimeRange(double startTime, double endTime) const;
    
    // Search functionality
    std::vector<LogEntry> search(const std::string& query) const;
    std::vector<LogEntry> searchByMessage(const std::string& message) const;
    std::vector<LogEntry> searchByFile(const std::string& filename) const;
    
    // Statistics
    std::string getStatistics() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Main enhanced Logger class
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
    
    // Performance tracking
    PerformanceTracker& getPerformanceTracker();
    
    // Global logging functions
    void log(LogLevel level, const std::string& message, 
             const std::string& file = "", int line = 0, 
             const std::string& function = "",
             const std::string& context = "");
    
    void logPerformance(const std::string& operation,
                       double durationMs,
                       const std::string& file = "",
                       int line = 0,
                       const std::string& function = "",
                       const std::string& context = "");
    
    // Convenience methods
    void trace(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const std::string& context = "");
    void debug(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const std::string& context = "");
    void info(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const std::string& context = "");
    void notice(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const std::string& context = "");
    void warning(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const std::string& context = "");
    void error(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const std::string& context = "");
    void critical(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const std::string& context = "");
    void alert(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const std::string& context = "");
    void emergency(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const std::string& context = "");
    
    // Check if level is enabled
    bool isEnabled(LogLevel level) const;
    
    // Get global logger instance
    static Logger& getGlobal();
    
    // Set global logger
    static void setGlobal(std::shared_ptr<Logger> logger);
    
    // Statistics and monitoring
    LogStatistics getLogStatistics() const;
    std::string getPerformanceStatistics() const;
    std::string getSystemStatistics() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Enhanced convenience macros with performance tracking
#define NLM_LOG_PERFORMANCE_START(op) nlm::PerformanceTracker::getGlobal().start(op, __func__)
#define NLM_LOG_PERFORMANCE_STOP(op) nlm::PerformanceTracker::getGlobal().stop(op, __FILE__, __LINE__, __func__)
#define NLM_LOG_PERFORMANCE(op) nlm::PerformanceTracker::getGlobal().stop(op, __FILE__, __LINE__, __func__)

#define NLM_LOG_TRACE(msg) nlm::Logger::getGlobal().trace(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_NOTICE(msg) nlm::Logger::getGlobal().notice(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ALERT(msg) nlm::Logger::getGlobal().alert(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_EMERGENCY(msg) nlm::Logger::getGlobal().emergency(msg, __FILE__, __LINE__, __func__)

// Stream-style logging helper with performance tracking
class LogStream {
public:
    LogStream(LogLevel level, const std::string& file, int line, const std::string& function, const std::string& context = "");
    ~LogStream();
    
    template<typename T>
    LogStream& operator<<(const T& value) {
        oss_ << value;
        return *this;
    }
    
    // Performance timing
    LogStream& withTiming(double durationMs);
    
private:
    LogLevel level_;
    std::ostringstream oss_;
    std::string file_;
    int line_;
    std::string function_;
    std::string context_;
    bool timingEnabled_;
    double durationMs_;
};

#define NLM_LOG_STREAM(level) nlm::LogStream(level, __FILE__, __LINE__, __func__)

// Global performance tracker accessor
PerformanceTracker& getGlobalPerformanceTracker();

} // namespace nlm
