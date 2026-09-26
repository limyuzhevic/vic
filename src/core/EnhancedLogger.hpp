// Enhanced NLM Logging System with Structured Output and Performance Integration

#ifndef NLM_LOGGING_H
#define NLM_LOGGING_H

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <unordered_map>
#include <mutex>
#include <iomanip>
#include <fstream>
#include <filesystem>
#include <atomic>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace nlm {

// Enhanced log severity levels
enum class LogLevel {
    Trace = 0,
    Debug = 1,
    Info = 2,
    Warning = 3,
    Error = 4,
    Critical = 5,
    Off = 6
};

// Structured log entry with metadata
struct StructuredLogEntry {
    std::chrono::system_clock::time_point timestamp;
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::string threadName;
    uint64_t threadId;
    std::unordered_map<std::string, std::string> metadata;
    double processingTimeMs;
    bool performanceMetric;
};

// Enhanced logger interface with structured logging support
class IStructuredLogger {
public:
    virtual ~IStructuredLogger() = default;
    virtual void logStructured(const StructuredLogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
    virtual void setFormat(const std::string& format) = 0;
    virtual std::string getFormat() const = 0;
};

// Enhanced console logger with structured output support
class StructuredConsoleLogger : public IStructuredLogger {
public:
    StructuredConsoleLogger();
    explicit StructuredConsoleLogger(LogLevel level);
    ~StructuredConsoleLogger() override;
    
    void logStructured(const StructuredLogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    void setFormat(const std::string& format) override;
    std::string getFormat() const override;
    
    // Enhanced color support
    void setColorEnabled(bool enabled);
    bool isColorEnabled() const;
    
    // Structured logging format presets
    void setPreset(StructuredConsoleLogger::Preset preset);
    std::string getPresetName() const;
    
    // Performance metrics output
    void enablePerformanceOutput(bool enabled);
    bool isPerformanceOutputEnabled() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string formatStructuredEntry(const StructuredLogEntry& entry) const;
    std::string levelToColorCode(LogLevel level) const;
};

// Main enhanced Logger class with performance integration
class EnhancedLogger {
public:
    EnhancedLogger();
    ~EnhancedLogger();
    
    // Disable copying
    EnhancedLogger(const EnhancedLogger&) = delete;
    EnhancedLogger& operator=(const EnhancedLogger&) = delete;
    
    // Add structured logger sink
    void addStructuredLogger(std::shared_ptr<IStructuredLogger> logger);
    
    // Remove all loggers
    void clearLoggers();
    
    // Set minimum level for all loggers
    void setLevel(LogLevel level);
    
    // Enhanced logging functions with structured support
    void logStructured(LogLevel level, const std::string& message,
                      const std::string& file = "", int line = 0,
                      const std::string& function = "");
    
    // Performance measurement logging
    void logWithTiming(LogLevel level, const std::string& operation,
                      const std::function<void()>& operationFunc,
                      const std::string& file = "", int line = 0,
                      const std::string& function = "");
    
    // Structured metadata logging
    template<typename... Args>
    void logWithMetadata(LogLevel level, const std::string& message,
                         const std::string& file = "", int line = 0,
                         const std::string& function = "",
                         const std::unordered_map<std::string, std::string>& metadata = {});
    
    // Convenience methods with performance tracking
    void debugWithTiming(const std::string& operation,
                        const std::function<void()>& operationFunc,
                        const std::string& file = "", int line = 0,
                        const std::string& function = "");
    
    void infoWithTiming(const std::string& operation,
                       const std::function<void()>& operationFunc,
                       const std::string& file = "", int line = 0,
                       const std::string& function = "");
    
    // Global logging functions
    void log(LogLevel level, const std::string& message,
             const std::string& file = "", int line = 0,
             const std::string& function = "");
    
    // Convenience methods
    void trace(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void debug(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void info(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void warning(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void error(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void critical(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    // Check if level is enabled
    bool isEnabled(LogLevel level) const;
    
    // Get global logger instance
    static EnhancedLogger& getGlobal();
    
    // Set global logger
    static void setGlobal(std::shared_ptr<EnhancedLogger> logger);
    
    // Performance monitoring integration
    void enablePerformanceMonitoring(bool enabled);
    bool isPerformanceMonitoringEnabled() const;
    
    // Log statistics and metrics
    std::string getStatistics() const;
    
    // Flush all loggers
    void flush();
    
    // Set log file output
    void setLogFile(const std::string& filepath, bool append = false);
    
    // Get log file path
    std::string getLogFile() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Enhanced macros for structured logging
#define NLM_LOG_TRACE(msg) nlm::EnhancedLogger::getGlobal().trace(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_DEBUG(msg) nlm::EnhancedLogger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO(msg) nlm::EnhancedLogger::getGlobal().info(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING(msg) nlm::EnhancedLogger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR(msg) nlm::EnhancedLogger::getGlobal().error(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_CRITICAL(msg) nlm::EnhancedLogger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)

// Structured logging macros
#define NLM_LOG_STRUCTURED(level, msg, ...) nlm::EnhancedLogger::getGlobal().logWithMetadata(level, msg, __FILE__, __LINE__, __func__, {{__VA_ARGS__}})

// Performance timing macros
#define NLM_PERFORMANCE_SCOPE(operation) \
    NLM::PerformanceTimer operation##_timer(__FILE__, __LINE__, __func__, #operation)

// Log stream with structured support
class StructuredLogStream {
public:
    StructuredLogStream(LogLevel level, const std::string& file, int line, const std::string& function);
    ~StructuredLogStream();
    
    template<typename T>
    StructuredLogStream& operator<<(const T& value) {
        oss_ << value;
        return *this;
    }
    
    // Add structured metadata
    StructuredLogStream& withMetadata(const std::string& key, const std::string& value);
    
private:
    LogLevel level_;
    std::ostringstream oss_;
    std::string file_;
    int line_;
    std::string function_;
    std::unordered_map<std::string, std::string> metadata_;
};

// Performance timer class for measuring operation durations
class PerformanceTimer {
public:
    PerformanceTimer(const std::string& operation, const std::string& file = "", int line = 0, const std::string& function = "");
    ~PerformanceTimer();
    
    double getDurationMs() const;
    
private:
    std::string operation_;
    std::chrono::high_resolution_clock::time_point startTime_;
    std::string file_;
    int line_;
    std::string function_;
};

// Log level enumeration helper
class LogLevelHelper {
public:
    static std::string toString(LogLevel level);
    static LogLevel fromString(const std::string& levelStr);
    static std::string levelColorCode(LogLevel level);
    static std::vector<std::string> getAllLevelNames();
    static bool isLevelEnabled(LogLevel level, LogLevel threshold);
};

// Logging configuration structure
struct LoggingConfig {
    LogLevel level;
    bool enableColors;
    bool enablePerformanceMetrics;
    std::string logFile;
    bool appendToFile;
    std::string format;
    std::string structuredFormat;
    bool enableStructuredLogging;
    size_t maxFileSize;
    size_t maxFiles;
    std::unordered_map<std::string, std::string> defaultMetadata;
    
    LoggingConfig() : level(LogLevel::Info), enableColors(true),
                     enablePerformanceMetrics(true), appendToFile(false),
                     enableStructuredLogging(true), maxFileSize(10 * 1024 * 1024),
                     maxFiles(5) {}
};

// Global logging configuration
void setLoggingConfig(const LoggingConfig& config);
LoggingConfig getLoggingConfig();
void resetLoggingConfig();

// Log file rotation utilities
class LogFileRotator {
public:
    static bool rotateIfNeeded(const std::string& filepath, size_t maxFileSize, size_t maxFiles);
    static std::vector<std::string> getLogFiles(const std::string& directory);
    static bool cleanupOldLogs(const std::string& directory, size_t maxFiles);
};

// Log statistics collector
class LogStatistics {
public:
    struct Statistics {
        uint64_t totalLogs;
        uint64_t logsByLevel[6];  // Trace, Debug, Info, Warning, Error, Critical
        double averageLogIntervalMs;
        std::string mostFrequentOperation;
        uint64_t mostFrequentOperationCount;
        double averageProcessingTimeMs;
        double peakProcessingTimeMs;
    };
    
    static void recordLog(const StructuredLogEntry& entry);
    static Statistics getStatistics();
    static void resetStatistics();
    static std::string generateStatisticsReport();
    
private:
    static Statistics stats_;
    static std::mutex mutex_;
    static std::chrono::system_clock::time_point lastLogTime_;
};

// Convenience logging functions
void logSystemInfo();
void logPerformanceMetrics();
void logConfiguration();
void logStartupInfo(const std::string& version, const std::string& buildInfo);

} // namespace nlm

#endif // NLM_LOGGING_H