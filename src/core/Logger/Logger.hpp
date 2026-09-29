#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>

namespace nlm {

// Log severity levels
enum class LogLevel {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4
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
    
    // Performance timing utilities
    class Timer {
    public:
        Timer(const std::string& operationName, LogLevel level = LogLevel::Info);
        ~Timer();
        template<typename T>
        Timer& operator<<(const T& value) {
            message_ << value;
            return *this;
        }
    private:
        std::string operationName_;
        std::ostringstream message_;
        LogLevel level_;
        std::chrono::steady_clock::time_point startTime_;
    };
    
    // Error categorization and tracking
    enum class ErrorCategory {
        Memory,
        Network,
        FileSystem,
        Configuration,
        Neural,
        World,
        Agent,
        Performance,
        Unknown
    };
    
    // Performance monitoring capabilities
    class PerformanceMonitor {
    public:
        void startMeasurement(const std::string& name);
        void endMeasurement(const std::string& name);
        void logPerformanceReport();
        void reset();
    private:
        struct Measurement {
            std::string name;
            std::chrono::duration<double> duration;
            std::chrono::system_clock::time_point startTime;
        };
        std::vector<Measurement> measurements_;
    };
    
    // Thread-safe counters and statistics
    class Statistics {
    public:
        struct Counter {
            size_t value = 0;
            std::mutex mutex;
        };
        
        void increment(const std::string& name);
        size_t get(const std::string& name) const;
        void reset();
        std::string report() const;
        
    private:
        std::unordered_map<std::string, Counter> counters_;
    };
    
    // Configuration logging
    void logConfiguration(const std::string& configFile = "");
    
    // Structured logging support
    struct LogEntryBuilder {
        LogEntryBuilder& setLevel(LogLevel level);
        LogEntryBuilder& setMessage(const std::string& message);
        LogEntryBuilder& addField(const std::string& key, const std::string& value);
        LogEntryBuilder& addField(const std::string& key, int value);
        LogEntryBuilder& addField(const std::string& key, double value);
        LogEntryBuilder& addField(const std::string& key, bool value);
        LogEntryBuilder& setFile(const std::string& file);
        LogEntryBuilder& setLine(int line);
        LogEntryBuilder& setFunction(const std::string& function);
        LogEntryBuilder& setTimestamp(std::chrono::system_clock::time_point timestamp);
        LogEntry build() const;
        
    private:
        LogEntry entry_;
        std::map<std::string, std::string> fields_;
    };
    
    // Performance monitoring integration
    void integratePerformanceMonitor(PerformanceMonitor& monitor);
    
    // Statistics integration
    void integrateStatistics(Statistics& stats);
    
    // Error tracking and analysis
    struct ErrorTracker {
        struct ErrorInfo {
            std::string message;
            ErrorCategory category;
            std::chrono::system_clock::time_point timestamp;
            std::string stackTrace;
        };
        
        void logError(const std::string& message, ErrorCategory category = ErrorCategory::Unknown);
        std::vector<ErrorInfo> getRecentErrors(size_t count = 100) const;
        void clearErrors();
        
    private:
        std::vector<ErrorInfo> errors_;
        std::mutex mutex_;
    };
    
    // Performance profiling
    void startProfile(const std::string& name);
    void endProfile(const std::string& name);
    void logProfileReport();
    
    // Real-time monitoring
    void setRealTimeEnabled(bool enabled);
    bool isRealTimeEnabled() const;
    void logRealTimeMetrics();
    
    // System resource monitoring
    struct ResourceUsage {
        double cpuUsage;        // Percentage (0-100)
        double memoryUsage;     // Percentage (0-100)
        size_t activeThreads;   // Number of active threads
        size_t memoryBytes;     // Memory usage in bytes
        double diskUsage;       // Disk usage percentage (0-100)
    };
    
    ResourceUsage getResourceUsage() const;
    
    // Log level filtering improvements
    void setCategoryEnabled(ErrorCategory category, bool enabled);
    bool isCategoryEnabled(ErrorCategory category) const;
    
    // Advanced logging methods
    template<typename... Args>
    void logf(LogLevel level, const std::string& format, Args&&... args);
    
    void logStructured(LogLevel level, const std::map<std::string, std::string>& data);
    
    // Batch logging for performance
    void batchLog(const std::vector<LogEntry>& entries);
    
    // Thread pool for concurrent logging
    void setThreadPoolSize(size_t size);
    
    // Log level threshold configuration
    struct ThresholdConfig {
        LogLevel infoThreshold = LogLevel::Info;
        LogLevel warningThreshold = LogLevel::Warning;
        LogLevel errorThreshold = LogLevel::Error;
        LogLevel criticalThreshold = LogLevel::Critical;
    };
    
    void setThresholdConfig(const ThresholdConfig& config);
    ThresholdConfig getThresholdConfig() const;

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
