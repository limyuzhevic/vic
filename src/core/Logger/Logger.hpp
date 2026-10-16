#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <unordered_map>
#include <map>
#include <thread>
#include <atomic>
#include <regex>
#include <iomanip>
#include <sstream>

namespace nlm {

// Log severity levels
enum class LogLevel {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4
};

// Log entry structure with enhanced metadata
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    std::string threadId;
    uint64_t operationId;
    double performanceDuration;
    std::string context;
    std::map<std::string, std::string> metadata;
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
    
    void setUseColors(bool useColors);
    bool getUseColors() const;
    void setOutputFile(const std::string& filepath);
    void closeOutputFile();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Helper functions
std::string levelToString(LogLevel level);
std::string levelToColor(LogLevel level);

// Performance timing information
struct PerformanceInfo {
    std::string operation;
    double duration;
    std::string category;
    uint64_t memoryUsage;
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point endTime;
};

// Neural activity statistics
struct NeuralStats {
    size_t totalSpikes;
    size_t activeNeurons;
    size_t firingNeurons;
    float averageFiringRate;
    float excitationInhibitionRatio;
    std::map<std::string, double> firingRates;
    std::vector<std::pair<double, size_t>> spikeHistogram;
};

// Event callback for log events
class LogEventCallback {
public:
    virtual ~LogEventCallback() = default;
    virtual void onLogEntryAdded(const LogEntry& entry) = 0;
    virtual void onLevelChanged(LogLevel oldLevel, LogLevel newLevel) = 0;
    virtual void onFileRotation(const std::string& oldFile, const std::string& newFile) = 0;
};

// Log rotation configuration
struct LogRotationConfig {
    size_t maxFileSizeMB;
    size_t maxFiles;
    bool compressOldFiles;
    std::string filePattern;
};

// Advanced logger sink interface
class IAdvancedLogger : public ILogger {
public:
    virtual ~IAdvancedLogger() = default;
    virtual void logPerformance(const PerformanceInfo& perf) = 0;
    virtual void logNeuralStats(const NeuralStats& stats) = 0;
    virtual void logEvent(const std::string& eventType, const std::string& data) = 0;
    virtual void addEventCallback(std::shared_ptr<LogEventCallback> callback) = 0;
    virtual void setLogRotation(const LogRotationConfig& config) = 0;
};

// File-based logger with rotation
class FileLogger : public IAdvancedLogger {
public:
    FileLogger();
    explicit FileLogger(const std::string& filepath, LogLevel level = LogLevel::Info);
    ~FileLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    // IAdvancedLogger implementation
    void logPerformance(const PerformanceInfo& perf) override;
    void logNeuralStats(const NeuralStats& stats) override;
    void logEvent(const std::string& eventType, const std::string& data) override;
    void addEventCallback(std::shared_ptr<LogEventCallback> callback) override;
    void setLogRotation(const LogRotationConfig& config) override;
    
    // File management
    void close();
    bool isOpen() const;
    std::string getCurrentFilePath() const;
    
    // Event-driven logging hooks
    void setLogHook(std::function<bool(const LogEntry&)> hook);
    void setNeuralStatsHook(std::function<bool(const NeuralStats&)> hook);
    void setPerformanceHook(std::function<bool(const PerformanceInfo&)> hook);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    void rotateIfNeeded();
    std::string getRotatedFilePath(int index) const;
};

// Statistics collection and analysis
class StatisticsCollector {
public:
    StatisticsCollector();
    ~StatisticsCollector();
    
    // Neural activity statistics
    void recordSpike(size_t neuronId, double time);
    void recordFiringRate(float rate);
    void recordExcitationInhibitionRatio(float ratio);
    void updateNeuralPopulationStats(const std::string& population, float rate);
    
    // Performance metrics
    void recordOperation(const std::string& operation, double duration, uint64_t memoryUsage);
    void recordSimulationStep(double stepDuration, size_t stepNumber);
    void recordMemoryUsage(uint64_t memoryBytes);
    
    // System-level metrics
    void recordTemperature(double temp);  // Placeholder for future thermal metrics
    void recordNetworkLatency(double latency);
    void recordDataThroughput(double throughput);
    
    // Get current statistics
    NeuralStats getNeuralStats() const;
    std::vector<PerformanceInfo> getPerformanceStats(size_t maxCount = 100) const;
    std::map<std::string, std::vector<double>> getMetricHistory() const;
    
    // Statistical analysis
    double calculateMovingAverage(const std::string& metric, size_t windowSize) const;
    double calculateStandardDeviation(const std::string& metric) const;
    std::string generateStatisticalReport() const;
    
    // Export and import
    bool exportToFile(const std::string& filepath) const;
    bool importFromFile(const std::string& filepath);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Real-time monitoring system
class RealTimeMonitor {
public:
    RealTimeMonitor();
    ~RealTimeMonitor();
    
    // Progress indicators
    void startProgressIndicator(const std::string& label, size_t totalSteps);
    void updateProgress(size_t currentStep, const std::string& details = "");
    void completeProgress(const std::string& finalMessage = "");
    
    // Performance profiling
    void startProfile(const std::string& operation, const std::string& category = "general");
    void endProfile(const std::string& operation);
    std::map<std::string, double> getProfileSummary() const;
    
    // Custom metrics
    void addCustomMetric(const std::string& name, double value, const std::string& unit = "");
    double getCustomMetric(const std::string& name) const;
    
    // Statistical analysis functions
    double calculateCorrelation(const std::string& metric1, const std::string& metric2) const;
    double calculateRegression(const std::string& dependent, const std::string& independent) const;
    std::string analyzeTrends(const std::vector<std::string>& metrics) const;
    
    // Visualization data extraction
    std::string extractTimeSeriesData(const std::string& metric, size_t points) const;
    std::string extractStatisticalData(const std::string& metric) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Log analysis tools
class LogAnalyzer {
public:
    LogAnalyzer();
    ~LogAnalyzer();
    
    // Log file parsing
    bool parseLogFile(const std::string& filepath);
    std::vector<LogEntry> getLogEntries() const;
    std::vector<PerformanceInfo> getPerformanceLogs() const;
    std::vector<NeuralStats> getNeuralStatsLogs() const;
    
    // Statistical analysis of log data
    double analyzeMetricFrequency(const std::string& pattern) const;
    std::map<std::string, double> analyzeLogLevelDistribution() const;
    std::vector<std::pair<std::string, double>> analyzeEventFrequency(const std::string& eventType) const;
    
    // Log pattern matching
    std::vector<LogEntry> searchLogs(const std::string& query, LogLevel minLevel = LogLevel::Debug) const;
    std::vector<LogEntry> filterLogs(const std::string& filterExpression, const std::map<std::string, std::string>& context) const;
    
    // Visualization data extraction
    std::string extractTimeSeriesData(const std::string& metric, const std::string& startTime, const std::string& endTime) const;
    std::string extractStatisticalSummary(const std::string& metric, const std::string& startTime, const std::string& endTime) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Enhanced main Logger class with statistics integration
class Logger {
public:
    Logger();
    ~Logger();
    
    // Disable copying
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    // Add logger sink
    void addLogger(std::shared_ptr<ILogger> logger);
    
    // Add advanced logger sink
    void addAdvancedLogger(std::shared_ptr<IAdvancedLogger> logger);
    
    // Remove all loggers
    void clearLoggers();
    
    // Set minimum level for all loggers
    void setLevel(LogLevel level);
    
    // Statistics integration
    void setStatisticsCollector(std::shared_ptr<StatisticsCollector> collector);
    void setRealTimeMonitor(std::shared_ptr<RealTimeMonitor> monitor);
    void setLogAnalyzer(std::shared_ptr<LogAnalyzer> analyzer);
    
    // Enhanced logging functions with statistics
    void log(LogLevel level, const std::string& message, 
             const std::string& file = "", int line = 0, 
             const std::string& function = "", const std::string& context = "");
    
    // Performance timing
    std::chrono::system_clock::time_point startTiming(const std::string& operation, const std::string& category = "general");
    void endTiming(const std::chrono::system_clock::time_point& startTime, const std::string& operation, uint64_t memoryUsage = 0);
    
    // Neural statistics logging
    void logNeuralStats(const NeuralStats& stats);
    
    // Event-driven logging
    void logEvent(const std::string& eventType, const std::string& data);
    
    // Convenience methods
    void debug(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const std::string& context = "");
    void info(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const std::string& context = "");
    void warning(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const std::string& context = "");
    void error(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const std::string& context = "");
    void critical(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const std::string& context = "");
    
    // Check if level is enabled
    bool isEnabled(LogLevel level) const;
    
    // Get global logger instance
    static Logger& getGlobal();
    
    // Set global logger
    static void setGlobal(std::shared_ptr<Logger> logger);
    
    // Statistics getters
    NeuralStats getNeuralStats() const;
    std::map<std::string, double> getSystemStats() const;
    
    // Export/import statistics
    bool exportStatistics(const std::string& filepath) const;
    bool importStatistics(const std::string& filepath);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Enhanced convenience macros for structured logging
#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)

// Performance timing macros
#define NLM_PERF_SCOPE(operation) nlm::PerformanceTimer timer(operation)
#define NLM_PERF_START(operation) auto operation##_start = std::chrono::high_resolution_clock::now()
#define NLM_PERF_END(operation, category) nlm::Logger::getGlobal().endTiming(operation##_start, operation, category)

// Stream-style logging with context
class LogStream {
public:
    LogStream(LogLevel level, const std::string& file, int line, const std::string& function);
    ~LogStream();
    
    template<typename T>
    LogStream& operator<<(const T& value) {
        oss_ << value;
        return *this;
    }
    
    // Context setting
    LogStream& withContext(const std::string& key, const std::string& value);
    LogStream& withPerformance(const std::string& operation, double duration);
    LogStream& withNeuralStats(const nlm::NeuralStats& stats);
    
private:
    LogLevel level_;
    std::ostringstream oss_;
    std::string file_;
    int line_;
    std::string function_;
    std::map<std::string, std::string> context_;
};

#define NLM_LOG_STREAM(level) nlm::LogStream(level, __FILE__, __LINE__, __func__)

// Performance timer helper class
class PerformanceTimer {
public:
    PerformanceTimer(const std::string& operation, const std::string& category = "general");
    ~PerformanceTimer();
    double getDuration() const;
    
private:
    std::string operation_;
    std::string category_;
    std::chrono::high_resolution_clock::time_point startTime_;
    uint64_t memoryUsage_;
};

} // namespace nlm
