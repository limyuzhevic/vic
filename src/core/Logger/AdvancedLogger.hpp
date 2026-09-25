#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <thread>
#include <atomic>
#include <mutex>
#include <unordered_map>
#include <map>
#include <condition_variable>
#include <queue>
#include <fstream>
#include <iomanip>
#include <cstdint>
#include <filesystem>
#include <algorithm>
#include <numeric>

namespace nlm {

// Multi-level logging levels (extended from original)
enum class LogLevel {
    Trace = 0,    // 0: Most detailed, lowest severity
    Debug = 1,    // 1: Debug information
    Info = 2,     // 2: General information
    Warning = 3,  // 3: Warning conditions
    Error = 4,    // 4: Error conditions
    Critical = 5, // 5: Critical errors
    Fatal = 6     // 6: Fatal errors, system may not recover
};

// Log entry with structured data
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string category;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    std::map<std::string, std::string> context;
    std::map<std::string, double> metrics;
    uint64_t threadId;
    std::string threadName;
    std::string processId;
    
    LogEntry()
        : level(LogLevel::Info), line(0), threadId(std::hash<std::thread::id>{}(std::this_thread::get_id())),
          threadName(""), processId(std::to_string(getpid())) {}
    
    std::string getFormattedTimestamp() const {
        auto time = std::chrono::system_clock::to_time_t(timestamp);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S.%F");
        return ss.str();
    }
};

// Performance metrics structure
struct PerformanceMetrics {
    std::string operation;
    double duration; // in seconds
    double cpuTime; // in seconds
    uint64_t memoryUsed; // in bytes
    uint64_t peakMemoryUsed; // in bytes
    double memoryPercent;
    std::map<std::string, double> customMetrics;
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point endTime;
    
    PerformanceMetrics() : duration(0.0), cpuTime(0.0), memoryUsed(0), peakMemoryUsed(0), memoryPercent(0.0) {
        startTime = std::chrono::system_clock::now();
    }
};

// Log filtering criteria
struct LogFilter {
    LogLevel minLevel;
    std::vector<std::string> categories;
    std::map<std::string, std::string> contextFilters;
    std::chrono::system_clock::time_point since;
    bool includePerformanceLogs;
    
    LogFilter()
        : minLevel(LogLevel::Trace), since(std::chrono::system_clock::time_point::min()),
          includePerformanceLogs(true) {}
};

// Log output interface
class ILogOutput {
public:
    virtual ~ILogOutput() = default;
    virtual void write(const LogEntry& entry) = 0;
    virtual void flush() = 0;
    virtual void setFormat(const std::string& format) = 0;
    virtual std::string getFormat() const = 0;
};

// Console log output with color support
class ConsoleLogOutput : public ILogOutput {
public:
    ConsoleLogOutput();
    explicit ConsoleLogOutput(LogLevel level);
    ~ConsoleLogOutput() override;
    
    void write(const LogEntry& entry) override;
    void flush() override;
    void setFormat(const std::string& format) override;
    std::string getFormat() const override;
    
    void setUseColors(bool useColors);
    bool getUseColors() const;
    void setLevel(LogLevel level);
    LogLevel getLevel() const;
    void setOutputFile(const std::string& filepath);
    void closeOutputFile();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string formatEntry(const LogEntry& entry) const;
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
};

// File log output with rotation support
class FileLogOutput : public ILogOutput {
public:
    FileLogOutput(const std::string& filepath, size_t maxFileSize = 10 * 1024 * 1024, int maxFiles = 5);
    ~FileLogOutput() override;
    
    void write(const LogEntry& entry) override;
    void flush() override;
    void setFormat(const std::string& format) override;
    std::string getFormat() const override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Structured log output for JSON
class JSONLogOutput : public ILogOutput {
public:
    JSONLogOutput(const std::string& filepath);
    ~JSONLogOutput() override;
    
    void write(const LogEntry& entry) override;
    void flush() override;
    void setFormat(const std::string& format) override;
    std::string getFormat() const override;
    
private:
    std::ofstream fileStream;
    std::mutex mutex;
    std::string format;
    
    std::string entryToJSON(const LogEntry& entry) const;
};

// Asynchronous log output
class AsyncLogOutput : public ILogOutput {
public:
    AsyncLogOutput(size_t queueSize = 10000);
    ~AsyncLogOutput() override;
    
    void write(const LogEntry& entry) override;
    void flush() override;
    void setFormat(const std::string& format) override;
    std::string getFormat() const override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Performance metrics collector
class IPerformanceMetricsCollector {
public:
    virtual ~IPerformanceMetricsCollector() = default;
    virtual void recordMetric(const std::string& name, double value, const std::map<std::string, std::string>& tags = {}) = 0;
    virtual void recordTiming(const std::string& operation, double duration, const std::map<std::string, std::string>& tags = {}) = 0;
    virtual void recordMemory(uint64_t memoryUsed, uint64_t peakMemoryUsed) = 0;
    virtual std::vector<PerformanceMetrics> getMetrics(const std::string& operation, const std::chrono::system_clock::time_point& since) = 0;
};

// System performance metrics collector
class SystemPerformanceMetricsCollector : public IPerformanceMetricsCollector {
public:
    SystemPerformanceMetricsCollector();
    ~SystemPerformanceMetricsCollector() override;
    
    void recordMetric(const std::string& name, double value, const std::map<std::string, std::string>& tags = {}) override;
    void recordTiming(const std::string& operation, double duration, const std::map<std::string, std::string>& tags = {}) override;
    void recordMemory(uint64_t memoryUsed, uint64_t peakMemoryUsed) override;
    std::vector<PerformanceMetrics> getMetrics(const std::string& operation, const std::chrono::system_clock::time_point& since) override;
    
    void startMeasurement(const std::string& operation);
    void stopMeasurement(const std::string& operation);
    void updateMemoryStats();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Log formatter interface
class ILogFormatter {
public:
    virtual ~ILogFormatter() = default;
    virtual std::string format(const LogEntry& entry) = 0;
    virtual std::string format(const PerformanceMetrics& metrics) = 0;
    virtual std::string getFormatName() const = 0;
};

// JSON formatter
class JSONLogFormatter : public ILogFormatter {
public:
    JSONLogFormatter();
    ~JSONLogFormatter() override;
    
    std::string format(const LogEntry& entry) override;
    std::string format(const PerformanceMetrics& metrics) override;
    std::string getFormatName() const override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// CSV formatter
class CSVLogFormatter : public ILogFormatter {
public:
    CSVLogFormatter();
    ~CSVLogFormatter() override;
    
    std::string format(const LogEntry& entry) override;
    std::string format(const PerformanceMetrics& metrics) override;
    std::string getFormatName() const override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// HTML formatter
class HTMLFormatter : public ILogFormatter {
public:
    HTMLFormatter();
    ~HTMLFormatter() override;
    
    std::string format(const LogEntry& entry) override;
    std::string format(const PerformanceMetrics& metrics) override;
    std::string getFormatName() const override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Advanced Logger class with comprehensive features
class AdvancedLogger {
public:
    AdvancedLogger();
    ~AdvancedLogger();
    
    // Disable copying, enable moving
    AdvancedLogger(const AdvancedLogger&) = delete;
    AdvancedLogger& operator=(const AdvancedLogger&) = delete;
    AdvancedLogger(AdvancedLogger&&) noexcept;
    AdvancedLogger& operator=(AdvancedLogger&&) noexcept;
    
    // Initialize logger with configuration
    void initialize(const std::string& name = "nlm", LogLevel level = LogLevel::Info,
                   const std::string& outputPath = "", const std::string& format = "console");
    
    // Core logging methods
    void log(LogLevel level, const std::string& message, const std::string& category = "");
    void logStructured(LogLevel level, const std::string& category, 
                     const std::map<std::string, std::string>& context,
                     const std::string& message, const std::map<std::string, double>& metrics = {});
    void logPerformance(LogLevel level, const std::string& operation, const PerformanceMetrics& metrics);
    
    // Convenience methods
    void trace(const std::string& message, const std::string& category = "");
    void debug(const std::string& message, const std::string& category = "");
    void info(const std::string& message, const std::string& category = "");
    void warning(const std::string& message, const std::string& category = "");
    void error(const std::string& message, const std::string& category = "");
    void critical(const std::string& message, const std::string& category = "");
    void fatal(const std::string& message, const std::string& category = "");
    
    // Performance monitoring methods
    void startProfiling(const std::string& operation);
    void stopProfiling(const std::string& operation);
    void recordMetric(const std::string& name, double value, const std::map<std::string, std::string>& tags = {});
    void recordTiming(const std::string& operation, double duration, const std::map<std::string, std::string>& tags = {});
    void recordMemory(uint64_t memoryUsed, uint64_t peakMemoryUsed);
    
    // Log management
    void addLogOutput(std::shared_ptr<ILogOutput> output);
    void removeLogOutput(std::shared_ptr<ILogOutput> output);
    void clearLogOutputs();
    void setLevel(LogLevel level);
    LogLevel getLevel() const;
    void setFilter(const LogFilter& filter);
    LogFilter getFilter() const;
    
    // Configuration
    void setLogFormat(const std::string& format);
    void setMaxFileSize(size_t maxSize);
    void setMaxFiles(int maxFiles);
    void enableColors(bool enable);
    void enableAsyncLogging(bool enable);
    void enablePerformanceLogging(bool enable);
    
    // State information
    bool isEnabled(LogLevel level) const;
    size_t getLogEntryCount() const;
    std::chrono::system_clock::time_point getLastLogTime() const;
    
    // Performance metrics access
    PerformanceMetrics getPerformanceMetrics(const std::string& operation) const;
    std::vector<PerformanceMetrics> getAllPerformanceMetrics() const;
    
    // Static instance management
    static AdvancedLogger& getInstance();
    static void setGlobal(std::shared_ptr<AdvancedLogger> logger);
    
    // Configuration file support
    bool loadConfiguration(const std::string& filepath);
    bool saveConfiguration() const;
    
    // Brain integration helpers
    void recordBrainMetrics(const class Brain& brain);
    void recordSimulationMetrics(const std::string& category, uint64_t step,
                               uint64_t spikeCount, float firingRate);
    void recordMemoryPoolMetrics(const std::string& poolName, size_t currentSize,
                                size_t maxSize, size_t totalAllocated);
    void recordEventMetrics(size_t queueDepth, double avgProcessingTime);
    void recordNeuromodulatorMetrics(const std::string& type, double level, double effect);
    
    // Alert system
    void addAlert(const std::string& name, LogLevel level, const std::string& condition,
                  const std::string& message, double threshold = 0.0);
    void clearAlerts();
    std::vector<std::string> getActiveAlerts() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    void addToHistory(const LogEntry& entry);
    void checkAlerts(const LogEntry& entry);
    bool passesFilter(const LogEntry& entry) const;
    PerformanceMetrics createPerformanceMetric(const std::string& operation) const;
};

// Macro for convenience logging with file/line/function info
#define NLM_LOG_TRACE(msg) nlm::AdvancedLogger::getInstance().trace(msg, "#custom")
#define NLM_LOG_DEBUG(msg) nlm::AdvancedLogger::getInstance().debug(msg, "#custom")
#define NLM_LOG_INFO(msg) nlm::AdvancedLogger::getInstance().info(msg, "#custom")
#define NLM_LOG_WARNING(msg) nlm::AdvancedLogger::getInstance().warning(msg, "#custom")
#define NLM_LOG_ERROR(msg) nlm::AdvancedLogger::getInstance().error(msg, "#custom")
#define NLM_LOG_CRITICAL(msg) nlm::AdvancedLogger::getInstance().critical(msg, "#custom")
#define NLM_LOG_FATAL(msg) nlm::AdvancedLogger::getInstance().fatal(msg, "#custom")

// Stream-style logging helper
class LogStream {
public:
    LogStream(LogLevel level, const std::string& category, const std::string& file, 
              int line, const std::string& function);
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
    
private:
    LogLevel level_;
    std::string category_;
    std::string file_;
    int line_;
    std::string function_;
    std::ostringstream oss_;
    
    std::string getCurrentThreadName();
};

#define NLM_LOG_STREAM(category, level) nlm::LogStream(level, category, __FILE__, __LINE__, __func__)

// Configuration structures
struct LoggingConfig {
    std::string name;
    LogLevel level;
    std::string outputPath;
    std::string format;
    bool useColors;
    bool asyncLogging;
    size_t maxFileSize;
    int maxFiles;
    bool enablePerformanceLogging;
    std::map<std::string, std::string> customSettings;
    
    LoggingConfig()
        : level(LogLevel::Info), outputPath(""), format("console"), useColors(true),
          asyncLogging(true), maxFileSize(10 * 1024 * 1024), maxFiles(5),
          enablePerformanceLogging(true) {}
};

struct PerformanceConfig {
    bool enableMemoryTracking;
    bool enableCPUTracking;
    bool enableNetworkTracking;
    double memoryThreshold;
    double cpuThreshold;
    double networkThreshold;
    std::chrono::seconds performanceBaselineInterval;
    
    PerformanceConfig()
        : enableMemoryTracking(true), enableCPUTracking(true), enableNetworkTracking(false),
          memoryThreshold(0.8), cpuThreshold(0.9), networkThreshold(0.7),
          performanceBaselineInterval(std::chrono::seconds(60)) {}
};

struct MonitoringConfig {
    std::string dashboardUrl;
    std::string dashboardApiKey;
    bool enableWebDashboard;
    bool enableRealTimeGraphs;
    std::chrono::seconds alertCheckInterval;
    std::chrono::seconds metricsCollectionInterval;
    
    MonitoringConfig()
        : enableWebDashboard(false), enableRealTimeGraphs(true),
          alertCheckInterval(std::chrono::seconds(5)),
          metricsCollectionInterval(std::chrono::seconds(1)) {}
};

// Configuration structure combining all settings
struct LoggingSystemConfig {
    LoggingConfig logging;
    PerformanceConfig performance;
    MonitoringConfig monitoring;
    bool validate() const {
        return !logging.name.empty();
    }
};

} // namespace nlm