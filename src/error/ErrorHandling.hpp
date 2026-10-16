# Comprehensive Error Handling and Logging Infrastructure

This file implements comprehensive error handling and logging infrastructure for the NLM (Neural Learning Machine) brain simulation. It provides robust error management, detailed logging, performance monitoring, and system health tracking.

## Overview

The NLM brain simulation requires sophisticated error handling and logging for:

1. **Error Management**: Catching, reporting, and recovering from errors
2. **Logging**: Detailed logging of system events, performance metrics, and debugging information
3. **Performance Monitoring**: Real-time performance tracking and analysis
4. **Health Monitoring**: System health checks and anomaly detection
5. **Debugging Support**: Detailed error context and stack traces
6. **Configuration**: Flexible logging configuration and output management

## Design

### Error Handling Architecture

```
+------------------------------------------------------------+
|                  Error Handling System                   |
|                                                    |
|  +-------------------+                        |
|  |    Exception     |                        |
|  |   Handling       |                        |
|  |                  |                        |
|  +-------------------+                        |
|          |                                |
|  +-------------------+           +------------------+
|  |   Error         |           |   Error          |
|  |   Reporter      |           |   Recovery       |
|  |                  |           |   System         |
|  +-------------------+           +------------------+
|          |                                |
|  +-------------------+           +------------------+
|  |   Error         |           |   System         |
|  |   Context       |           |   Health         |
|  |   Manager       |           |   Monitoring      |
|  |                  |           |                  |
|  +-------------------+           +------------------+
```

### Logging Architecture

```
+------------------------------------------------------------+
|                   Logging Infrastructure                    |
|                                                    |
|  +-------------------+    +-------------------+    |
|  |    Log           |    |    Log           |    |   Log
|  |    Formatter     |    |    Output        |    |   Sink
|  |                  |    |    Manager       |    |   (File, Console,
|  +-------------------+    +-------------------+    |   Network, etc.)
|          |                                |
|  +-------------------+           +------------------+
|  |    Log           |           |   Log             |
|  |    Levels       |           |   Aggregation     |
|  |                  |           |                  |
|  +-------------------+           +------------------+
```

### Performance Monitoring Architecture

```
+------------------------------------------------------------+
|                Performance Monitoring                      |
|                                                    |
|  +-------------------+    +-------------------+    |
|  |    Metrics       |    |    Sampler       |    |   Analyzer
|  |    Collector      |    |                  |    |                  |
|  +-------------------+    +-------------------+    |
|          |                                |
|  +-------------------+           +------------------+
|  |    Profiler      |           |   Performance    |
|  |    System       |           |   Monitor        |
|  |                  |           |                  |
|  +-------------------+           +------------------+
```

## Implementation

### Error Handling Classes

#### ErrorLevel Enum

```c++
enum class ErrorLevel {
public:
    enum Type {
        DEBUG = 0,           // Detailed debugging information
        INFO = 1,            // General information
        WARNING = 2,         // Warning conditions that don't prevent operation
        ERROR = 3,           // Error conditions that prevent operation
        CRITICAL = 4,        // Critical errors that may cause system failure
        FATAL = 5            // Fatal errors that require immediate termination
    };
    
    static const char* toString(Type level);
    static Type fromString(const std::string& str);
};
```

#### ErrorContext Class

```c++
class ErrorContext {
public:
    ErrorContext() = default;
    
    ErrorContext(const std::string& module, 
                 const std::string& function,
                 const std::string& file,
                 int line,
                 const std::string& message);
    
    // Information getters
    const std::string& getModule() const { return module_; }
    const std::string& getFunction() const { return function_; }
    const std::string& getFile() const { return file_; }
    int getLine() const { return line_; }
    const std::string& getMessage() const { return message_; }
    ErrorLevel::Type getLevel() const { return level_; }
    
    // Timestamps and IDs
    Timestamp getTimestamp() const { return timestamp_; }
    const std::string& getErrorId() const { return errorId_; }
    
    // Stack trace
    const std::vector<std::string>& getStackTrace() const { return stackTrace_; }
    
    // Context data
    template<typename T>
    void addContextData(const std::string& key, const T& value) {
        contextData_[key] = value;
    }
    
    template<typename T>
    T getContextData(const std::string& key) const {
        auto it = contextData_.find(key);
        if (it != contextData_.end()) {
            return std::any_cast<T>(it->second);
        }
        throw std::runtime_error("Context data key not found: " + key);
    }
    
    // Cause chain
    void addCause(const std::shared_ptr<ErrorContext>& cause) {
        causes_.push_back(cause);
    }
    const std::vector<std::shared_ptr<ErrorContext>>& getCauses() const { return causes_; }
    
    // Exception integration
    std::exception_ptr toException() const;
    
    // Serialization
    std::string toString() const;
    void serialize(std::ostream& os) const;
    static ErrorContext deserialize(std::istream& is);
    
    // Comparison
    bool operator==(const ErrorContext& other) const;
    bool operator!=(const ErrorContext& other) const { return !(*this == other); }
    
private:
    std::string module_;
    std::string function_;
    std::string file_;
    int line_;
    std::string message_;
    ErrorLevel::Type level_;
    Timestamp timestamp_;
    std::string errorId_;
    std::vector<std::string> stackTrace_;
    std::map<std::string, std::any> contextData_;
    std::vector<std::shared_ptr<ErrorContext>> causes_;
};
```

#### ErrorReporter Class

```c++
class ErrorReporter {
public:
    static ErrorReporter& getInstance();
    
    // Configuration
    void configure(const ErrorReporterConfig& config);
    ErrorReporterConfig getConfig() const { return config_; }
    
    // Error reporting
    std::string reportError(const ErrorContext& error);
    void reportErrorAsync(const ErrorContext& error);
    
    // Error retrieval
    std::vector<std::shared_ptr<ErrorContext>> getRecentErrors(size_t count = 100);
    std::vector<std::shared_ptr<ErrorContext>> getErrorsByLevel(ErrorLevel::Type level, size_t count = 100);
    std::vector<std::shared_ptr<ErrorContext>> getErrorsByModule(const std::string& module, size_t count = 100);
    
    // Error filtering
    std::vector<std::shared_ptr<ErrorContext>> filterErrors(std::function<bool(const ErrorContext&)> predicate);
    
    // Error analysis
    size_t getErrorCount() const { return errorCount_.load(); }
    float getErrorRate() const { return errorRate_.load(); }
    double getAverageErrorFrequency() const { return averageErrorFrequency_.load(); }
    
    // Error recovery
    bool attemptRecovery(const std::shared_ptr<ErrorContext>& error);
    std::vector<std::string> getRecoveryActions(const std::shared_ptr<ErrorContext>& error);
    
    // Statistics
    void updateStatistics(const ErrorContext& error);
    ErrorReporterStatistics getStatistics() const;
    
    // Export
    void exportErrors(const std::string& filename, const std::string& format = "json");
    void exportStatistics(const std::string& filename);
    
    // Cleanup
    void clearOldErrors();
    void reset();
    
private:
    ErrorReporterConfig config_;
    std::mutex mutex_;
    std::atomic<size_t> errorCount_;
    std::atomic<float> errorRate_;
    std::atomic<double> averageErrorFrequency_;
    
    std::queue<std::shared_ptr<ErrorContext>> errorQueue_;
    std::vector<std::shared_ptr<ErrorContext>> errorHistory_;
    
    std::map<std::string, std::atomic<size_t>> errorCountsByLevel_;
    std::map<std::string, std::atomic<size_t>> errorCountsByModule_;
    
    // Helper methods
    void processError(const ErrorContext& error);
    void deliverError(const ErrorContext& error);
    std::string formatErrorMessage(const ErrorContext& error) const;
    void updateErrorStatistics(const ErrorContext& error);
    
    // Thread-local storage
    thread_local static std::queue<std::shared_ptr<ErrorContext>> threadErrorQueue;
};
```

#### ErrorRecoverySystem Class

```c++
class ErrorRecoverySystem {
public:
    ErrorRecoverySystem();
    
    // Error recovery
    bool recover(const std::shared_ptr<ErrorContext>& error);
    RecoveryResult attemptRecovery(const std::shared_ptr<ErrorContext>& error);
    
    // Recovery strategies
    bool recoverMemory(std::shared_ptr<ErrorContext> error);
    bool recoverNetwork(std::shared_ptr<ErrorContext> error);
    bool recoverComputation(std::shared_ptr<ErrorContext> error);
    bool recoverConfiguration(std::shared_ptr<ErrorContext> error);
    bool recoverExternal(std::shared_ptr<ErrorContext> error);
    
    // Recovery rules
    void addRecoveryRule(const RecoveryRule& rule);
    void removeRecoveryRule(const std::string& name);
    std::vector<RecoveryRule> getRecoveryRules() const { return recoveryRules_; }
    
    // Recovery statistics
    RecoveryStatistics getStatistics() const;
    
    // Configuration
    void configure(const RecoveryConfig& config);
    RecoveryConfig getConfig() const { return config_; }
    
    // Testing
    bool testRecovery(const std::shared_ptr<ErrorContext>& error);
    std::vector<RecoveryResult> testAllRecoveries(const std::shared_ptr<ErrorContext>& error);
    
private:
    RecoveryConfig config_;
    std::vector<RecoveryRule> recoveryRules_;
    
    // Recovery results
    std::mutex mutex_;
    std::map<std::string, RecoveryStatistics> recoveryStats_;
    
    // Helper methods
    RecoveryResult applyRecoveryRule(const RecoveryRule& rule, const std::shared_ptr<ErrorContext>& error);
    bool meetsRecoveryConditions(const RecoveryRule& rule, const std::shared_ptr<ErrorContext>& error);
    
    // Statistics update
    void updateRecoveryStatistics(const RecoveryResult& result);
    
    // Rule evaluation
    bool evaluateRecoveryConditions(const RecoveryCondition& condition);
};
```

#### Logging Infrastructure

```c++
struct LogEntry {
    Timestamp timestamp;
    ErrorLevel::Type level;
    std::string module;
    std::string message;
    std::map<std::string, std::any> contextData;
    std::string threadId;
    size_t threadIdHash;
};

class Logger {
public:
    static Logger& getInstance();
    
    // Configuration
    void configure(const LoggerConfig& config);
    LoggerConfig getConfig() const { return config_; }
    
    // Logging methods
    void log(ErrorLevel::Type level, const std::string& module,
             const std::string& message, const std::string& file = "",
             int line = 0, const std::map<std::string, std::any>& context = {});
    
    // Convenience methods
    void debug(const std::string& module, const std::string& message);
    void info(const std::string& module, const std::string& message);
    void warning(const std::string& module, const std::string& message);
    void error(const std::string& module, const std::string& message);
    void critical(const std::string& module, const std::string& message);
    void fatal(const std::string& module, const std::string& message);
    
    // Formatted logging
    template<typename... Args>
    void logFormat(ErrorLevel::Type level, const std::string& module,
                   const std::string& format, Args&&... args);
    
    // Log retrieval
    std::vector<LogEntry> getRecentLogs(size_t count = 100);
    std::vector<LogEntry> getLogsByLevel(ErrorLevel::Type level, size_t count = 100);
    std::vector<LogEntry> getLogsByModule(const std::string& module, size_t count = 100);
    
    // Log filtering
    std::vector<LogEntry> filterLogs(std::function<bool(const LogEntry&)> predicate);
    
    // Log output
    void setOutput(std::unique_ptr<LogOutput> output);
    std::unique_ptr<LogOutput> getOutput() const { return output_; }
    
    // Performance logging
    void logPerformance(const std::string& operation, double duration, size_t bytesProcessed = 0);
    void logMemoryUsage(size_t memoryUsage, size_t peakMemoryUsage);
    void logCPUUsage(double cpuUsage);
    
    // System health
    SystemHealth getSystemHealth() const;
    
    // Log statistics
    LogStatistics getStatistics() const;
    
    // Export
    void exportLogs(const std::string& filename, const std::string& format = "json");
    
    // Cleanup
    void clearOldLogs();
    void reset();
    
private:
    LoggerConfig config_;
    std::unique_ptr<LogOutput> output_;
    std::mutex mutex_;
    
    std::queue<LogEntry> logQueue_;
    std::vector<LogEntry> logHistory_;
    
    // Performance metrics
    std::atomic<size_t> totalLogEntries_;
    std::atomic<size_t> totalLogBytes_;
    std::atomic<double> averageLogLatency_;
    
    // Helper methods
    void processLog(const LogEntry& entry);
    void deliverLog(const LogEntry& entry);
    std::string formatLogMessage(const LogEntry& entry) const;
    
    // Thread-local storage
    thread_local static std::queue<LogEntry> threadLogQueue;
};
```

#### Performance Monitoring

```c++
class PerformanceMonitor {
public:
    static PerformanceMonitor& getInstance();
    
    // Configuration
    void configure(const PerformanceMonitorConfig& config);
    PerformanceMonitorConfig getConfig() const { return config_; }
    
    // Metric collection
    void startMeasurement(const std::string& name);
    void stopMeasurement(const std::string& name);
    void recordMetric(const std::string& name, double value, const std::string& unit = "");
    
    // Benchmarks
    BenchmarkResult runBenchmark(const std::string& name,
                                 std::function<void()> benchmarkFunction,
                                 size_t iterations = 1);
    
    // Performance profiling
    ProfileResult profileFunction(const std::string& name, std::function<void()> function);
    
    // System metrics
    SystemMetrics getSystemMetrics() const;
    MemoryMetrics getMemoryMetrics() const;
    ProcessMetrics getProcessMetrics() const;
    
    // Performance analysis
    PerformanceReport analyzePerformance(const std::string& operation = "");
    std::vector<PerformanceAlert> getPerformanceAlerts() const;
    
    // Configuration
    void setSamplingInterval(double interval) { samplingInterval_ = interval; }
    void setRetentionPeriod(double period) { retentionPeriod_ = period; }
    void enableRealTimeMonitoring(bool enable) { realTimeMonitoring_ = enable; }
    
    // Statistics
    PerformanceStatistics getStatistics() const;
    
    // Export
    void exportMetrics(const std::string& filename, const std::string& format = "json");
    void exportProfiles(const std::string& directory);
    
    // Cleanup
    void clearOldMetrics();
    void reset();
    
private:
    PerformanceMonitorConfig config_;
    std::mutex mutex_;
    
    // Metrics storage
    std::map<std::string, std::vector<MetricSample>> metrics_;
    std::map<std::string, BenchmarkResult> benchmarks_;
    std::map<std::string, ProfileResult> profiles_;
    
    // Performance monitoring
    std::chrono::steady_clock::time_point lastSampleTime_;
    std::atomic<bool> monitoringActive_;
    
    // System monitoring
    std::thread monitoringThread_;
    std::atomic<bool> monitoringThreadRunning_;
    
    // Alerting
    std::vector<PerformanceAlert> performanceAlerts_;
    
    // Helper methods
    void samplingThread();
    void updateSystemMetrics();
    MetricSample sampleMetric(const std::string& name);
    void analyzePerformanceMetrics();
    bool checkPerformanceThresholds();
    
    // Profile helper
    ProfileResult profileInternal(const std::string& name, std::function<void()> function);
};
```

### Configuration Classes

#### ErrorReporterConfig

```c++
struct ErrorReporterConfig {
    // Output configuration
    bool enableConsoleOutput = true;
    bool enableFileOutput = true;
    std::string logFilePath = "nlm_errors.log";
    std::string errorFormat = "detailed";  // "simple", "detailed", "json"
    
    // Error filtering
    bool logDebugErrors = false;
    bool logInfoErrors = true;
    bool logWarningErrors = true;
    bool logErrorErrors = true;
    bool logCriticalErrors = true;
    bool logFatalErrors = true;
    
    // Error recovery
    bool enableAutomaticRecovery = false;
    size_t maxRecoveryAttempts = 3;
    double recoveryTimeout = 10.0;  // seconds
    
    // Error statistics
    bool collectStatistics = true;
    size_t errorHistorySize = 10000;
    double statisticsUpdateInterval = 1.0;  // seconds
    
    // Error export
    bool exportOnError = false;
    std::string exportPath = "error_exports/";
    std::string exportFormat = "json";
    
    // Thread safety
    bool threadSafe = true;
    size_t maxConcurrentErrors = 100;
    
    // Performance
    size_t errorQueueSize = 10000;
    double processingTimeout = 5.0;  // seconds
    
    // Customization
    std::map<std::string, std::any> customSettings;
};
```

#### LoggerConfig

```c++
struct LoggerConfig {
    // Output configuration
    bool enableConsoleOutput = true;
    bool enableFileOutput = true;
    std::string logFilePath = "nlm.log";
    std::string logFormat = "detailed";  // "simple", "detailed", "json", "xml"
    
    // Log levels
    bool logDebug = false;
    bool logInfo = true;
    bool logWarning = true;
    bool logError = true;
    bool logCritical = true;
    bool logFatal = true;
    
    // Log filtering
    std::vector<std::string> includeModules;
    std::vector<std::string> excludeModules;
    std::map<std::string, ErrorLevel::Type> moduleLevels;
    
    // Performance logging
    bool logPerformance = true;
    bool logMemoryUsage = true;
    bool logCPUUsage = true;
    double performanceLogInterval = 60.0;  // seconds
    
    // System health
    bool logSystemHealth = true;
    SystemHealthCheckLevel healthCheckLevel = SystemHealthCheckLevel::STANDARD;
    
    // Output formatting
    std::string timestampFormat = "iso";  // "iso", "local", "epoch"
    bool includeThreadInfo = true;
    bool includeLocationInfo = true;
    
    // Log rotation
    bool enableLogRotation = true;
    size_t maxLogFileSize = 10 * 1024 * 1024;  // 10MB
    size_t maxLogFiles = 5;
    std::string rotationPattern = "nlm_%Y%m%d.log";
    
    // Performance
    size_t logQueueSize = 10000;
    bool asyncLogging = true;
    double flushInterval = 1.0;  // seconds
    
    // Customization
    std::map<std::string, std::any> customSettings;
};
```

#### PerformanceMonitorConfig

```c++
struct PerformanceMonitorConfig {
    // Sampling configuration
    double samplingInterval = 0.1;  // seconds
    bool enableRealTimeMonitoring = true;
    size_t metricHistorySize = 100000;
    
    // Benchmark configuration
    bool runBenchmarks = true;
    size_t benchmarkIterationCount = 100;
    double benchmarkTimeout = 60.0;  // seconds
    
    // Profiling configuration
    bool enableProfiling = true;
    size_t profileSampleCount = 1000;
    double profileTimeout = 30.0;  // seconds
    
    // System monitoring
    bool monitorSystemResources = true;
    double systemSampleInterval = 1.0;  // seconds
    bool monitorMemoryUsage = true;
    bool monitorCPUUsage = true;
    bool monitorNetworkUsage = true;
    bool monitorDiskUsage = true;
    
    // Alerting configuration
    bool enablePerformanceAlerts = true;
    double alertThresholdHigh = 0.9;  // 90%
    double alertThresholdLow = 0.1;   // 10%
    PerformanceAlertSeverity alertSeverity = PerformanceAlertSeverity::MEDIUM;
    
    // Export configuration
    bool exportMetrics = true;
    std::string metricsExportPath = "performance_metrics/";
    std::string exportFormat = "csv";  // "csv", "json", "parquet"
    
    // Performance
    bool enableMultiThreading = true;
    size_t concurrentSamplers = std::thread::hardware_concurrency();
    double maxProcessingTime = 10.0;  // seconds
    
    // Customization
    std::map<std::string, std::any> customSettings;
};
```

### Output Classes

#### LogOutput Interface

```c++
class LogOutput {
public:
    virtual ~LogOutput() = default;
    
    virtual void write(const LogEntry& entry) = 0;
    virtual void flush() = 0;
    virtual bool isAvailable() const = 0;
    virtual void configure(const std::map<std::string, std::any>& config) = 0;
    
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
};
```

#### ConsoleLogOutput

```c++
class ConsoleLogOutput : public LogOutput {
public:
    ConsoleLogOutput();
    
    void write(const LogEntry& entry) override;
    void flush() override;
    bool isAvailable() const override { return true; }
    void configure(const std::map<std::string, std::any>& config) override;
    
    std::string getName() const override { return "console"; }
    std::string getDescription() const override { return "Console output"; }
    
    // Configuration
    void setColorOutput(bool enable) { colorOutput_ = enable; }
    bool isColorOutput() const { return colorOutput_; }
    
    void setTimestampFormat(const std::string& format) { timestampFormat_ = format; }
    std::string getTimestampFormat() const { return timestampFormat_; }
    
private:
    bool colorOutput_ = true;
    std::string timestampFormat_ = "iso";
    std::mutex mutex_;
};
```

#### FileLogOutput

```c++
class FileLogOutput : public LogOutput {
public:
    FileLogOutput();
    
    void write(const LogEntry& entry) override;
    void flush() override;
    bool isAvailable() const override;
    void configure(const std::map<std::string, std::any>>& config) override;
    
    std::string getName() const override { return "file"; }
    std::string getDescription() const override { return "File output"; }
    
    // Configuration
    void setFilePath(const std::string& path) { filePath_ = path; }
    std::string getFilePath() const { return filePath_; }
    
    void setLogRotation(bool enable) { enableRotation_ = enable; }
    bool isLogRotationEnabled() const { return enableRotation_; }
    
    void setMaxFileSize(size_t size) { maxFileSize_ = size; }
    size_t getMaxFileSize() const { return maxFileSize_; }
    
    void setMaxFiles(size_t count) { maxFiles_ = count; }
    size_t getMaxFiles() const { return maxFiles_; }
    
private:
    std::string filePath_;
    std::ofstream logFile_;
    bool enableRotation_ = true;
    size_t maxFileSize_ = 10 * 1024 * 1024;  // 10MB
    size_t maxFiles_ = 5;
    std::mutex fileMutex_;
};
```

### Statistics and Analytics

#### ErrorReporterStatistics

```c++
struct ErrorReporterStatistics {
    // Error counts
    size_t totalErrors = 0;
    size_t debugErrors = 0;
    size_t infoErrors = 0;
    size_t warningErrors = 0;
    size_t errorErrors = 0;
    size_t criticalErrors = 0;
    size_t fatalErrors = 0;
    
    // Error rates
    double errorRate = 0.0;
    double debugErrorRate = 0.0;
    double infoErrorRate = 0.0;
    double warningErrorRate = 0.0;
    double errorErrorRate = 0.0;
    double criticalErrorRate = 0.0;
    double fatalErrorRate = 0.0;
    
    // Error frequency
    double averageErrorFrequency = 0.0;
    double averageDebugFrequency = 0.0;
    double averageInfoFrequency = 0.0;
    double averageWarningFrequency = 0.0;
    double averageErrorFrequency = 0.0;
    double averageCriticalFrequency = 0.0;
    double averageFatalFrequency = 0.0;
    
    // Recovery statistics
    size_t successfulRecoveries = 0;
    size_t failedRecoveries = 0;
    double recoverySuccessRate = 0.0;
    
    // Time statistics
    double averageErrorProcessingTime = 0.0;
    double averageRecoveryTime = 0.0;
    
    // Module statistics
    std::map<std::string, size_t> errorsByModule;
    std::map<std::string, double> errorRatesByModule;
    
    // Recovery actions
    std::map<std::string, size_t> recoveryActionsByType;
    
    // Export
    void reset();
    std::string toString() const;
    void serialize(std::ostream& os) const;
    static ErrorReporterStatistics deserialize(std::istream& is);
};
```

#### LogStatistics

```c++
struct LogStatistics {
    // Log counts
    size_t totalLogEntries = 0;
    size_t debugLogs = 0;
    size_t infoLogs = 0;
    size_t warningLogs = 0;
    size_t errorLogs = 0;
    size_t criticalLogs = 0;
    size_t fatalLogs = 0;
    
    // Log rates
    double logRate = 0.0;
    double debugLogRate = 0.0;
    double infoLogRate = 0.0;
    double warningLogRate = 0.0;
    double errorLogRate = 0.0;
    double criticalLogRate = 0.0;
    double fatalLogRate = 0.0;
    
    // Performance metrics
    double averageLogLatency = 0.0;
    double averageLogThroughput = 0.0;
    size_t totalLogBytes = 0;
    
    // Module statistics
    std::map<std::string, size_t> logCountsByModule;
    std::map<std::string, double> logRatesByModule;
    
    // Output statistics
    std::map<std::string, size_t> logCountsByOutput;
    std::map<std::string, double> logRatesByOutput;
    
    // Export
    void reset();
    std::string toString() const;
    void serialize(std::ostream& os) const;
    static LogStatistics deserialize(std::istream& is);
};
```

#### PerformanceStatistics

```c++
struct PerformanceStatistics {
    // Metric counts
    size_t totalMetrics = 0;
    size_t totalBenchmarks = 0;
    size_t totalProfiles = 0;
    
    // Performance metrics
    double averageMetricLatency = 0.0;
    double averageBenchmarkTime = 0.0;
    double averageProfileTime = 0.0;
    
    // Resource usage
    double averageCPUUsage = 0.0;
    double peakCPUUsage = 0.0;
    size_t peakMemoryUsage = 0;
    double averageMemoryUsage = 0.0;
    
    // System metrics
    size_t systemMetricsCollected = 0;
    double systemMonitoringDuration = 0.0;
    
    // Alert statistics
    size_t performanceAlerts = 0;
    size_t criticalAlerts = 0;
    size_t resolvedAlerts = 0;
    
    // Export
    void reset();
    std::string toString() const;
    void serialize(std::ostream& os) const;
    static PerformanceStatistics deserialize(std::istream& is);
};
```

### Integration

#### Integration with NLM Brain

```c++
// Include error handling and logging headers
#include "error/ErrorReporter.hpp"
#include "error/ErrorRecoverySystem.hpp"
#include "logging/Logger.hpp"
#include "performance/PerformanceMonitor.hpp"

// Initialize error handling and logging
void initializeErrorHandlingAndLogging(const ErrorReporterConfig& errorConfig,
                                        const LoggerConfig& loggerConfig,
                                        const PerformanceMonitorConfig& perfConfig);

void shutdownErrorHandlingAndLogging();

// Error reporting convenience functions
void reportError(const std::string& module, const std::string& message,
                ErrorLevel::Type level = ErrorLevel::Type::ERROR,
                const std::map<std::string, std::any>& context = {});

void reportException(const std::exception& e, const std::string& module = "");

// Logging convenience functions
void logDebug(const std::string& module, const std::string& message);
void logInfo(const std::string& module, const std::string& message);
void logWarning(const std::string& module, const std::string& message);
void logError(const std::string& module, const std::string& message);
void logCritical(const std::string& module, const std::string& message);
void logFatal(const std::string& module, const std::string& message);

// Performance monitoring
void startPerformanceMeasurement(const std::string& name);
void stopPerformanceMeasurement(const std::string& name);
void recordPerformanceMetric(const std::string& name, double value, const std::string& unit = "");

// System health
SystemHealth getSystemHealth();
```

#### Error Handling in Brain Class

```c++
class Brain {
public:
    // ... existing methods ...
    
    // Error handling
    void setErrorHandler(std::function<void(const ErrorContext&)> handler);
    void setErrorRecovery(std::function<bool(const ErrorContext&)> recovery);
    
    // Error reporting
    void reportError(const std::string& module, const std::string& message,
                    ErrorLevel::Type level = ErrorLevel::Type::ERROR,
                    const std::map<std::string, std::any>& context = {});
    
    // Performance monitoring
    void startPerformanceMeasurement(const std::string& name);
    void stopPerformanceMeasurement(const std::string& name);
    
    // Logging
    void logDebug(const std::string& module, const std::string& message);
    void logInfo(const std::string& module, const std::string& message);
    
private:
    std::unique_ptr<ErrorReporter> errorReporter_;
    std::unique_ptr<ErrorRecoverySystem> recoverySystem_;
    std::unique_ptr<Logger> logger_;
    std::unique_ptr<PerformanceMonitor> performanceMonitor_;
    
    std::function<void(const ErrorContext&)> errorHandler_;
    std::function<bool(const ErrorContext&)> errorRecovery_;
};
```

## Usage Examples

### Basic Error Handling

```c++
#include "error/ErrorReporter.hpp"

void someFunction() {
    try {
        // Risky operation
        riskyOperation();
    } catch (const std::exception& e) {
        // Report error with context
        ErrorContext error("Module", "Function", "file.cpp", 123, "Operation failed");
        error.addContextData("operation", "riskyOperation");
        error.addContextData("attempt", 3);
        
        ErrorReporter::getInstance().reportError(error);
        
        // Try recovery
        if (ErrorReporter::getInstance().attemptRecovery(error)) {
            // Recovery successful
            logInfo("Module", "Operation recovered successfully");
        } else {
            // Recovery failed
            logError("Module", "Operation recovery failed");
        }
    }
}
```

### Performance Monitoring

```c++
#include "performance/PerformanceMonitor.hpp"

void expensiveOperation() {
    PerformanceMonitor::getInstance().startMeasurement("expensiveOperation");
    
    // Do expensive work
    for (int i = 0; i < 1000000; ++i) {
        // ... work ...
    }
    
    PerformanceMonitor::getInstance().stopMeasurement("expensiveOperation");
    
    // Record performance metrics
    PerformanceMonitor::getInstance().recordMetric("memory_usage", getMemoryUsage());
    PerformanceMonitor::getInstance().recordMetric("cpu_usage", getCPUUsage());
}

void runBenchmarks() {
    auto result = PerformanceMonitor::getInstance().runBenchmark(
        "matrixMultiplication",
        []() {
            // Benchmark matrix multiplication
            Matrix A = generateMatrix(1000, 1000);
            Matrix B = generateMatrix(1000, 1000);
            Matrix C = A * B;
        },
        10  // 10 iterations
    );
    
    std::cout << "Benchmark completed in " << result.elapsedTime << " seconds"
              << " (throughput: " << result.itemsPerSecond << " items/sec)" << std::endl;
}
```

### Comprehensive Logging

```c++
#include "logging/Logger.hpp"

void neuralNetworkTraining() {
    logInfo("Training", "Starting neural network training");
    
    for (int epoch = 0; epoch < maxEpochs; ++epoch) {
        logDebug("Training", "Processing epoch " + std::to_string(epoch));
        
        // Train on batch
        auto result = trainBatch(currentBatch);
        
        logInfo("Training", "Epoch " + std::to_string(epoch) +
               " completed - loss: " + std::to_string(result.loss) +
               ", accuracy: " + std::to_string(result.accuracy));
        
        // Log performance metrics
        PerformanceMonitor::getInstance().recordMetric(
            "training_loss", result.loss, "loss");
        PerformanceMonitor::getInstance().recordMetric(
            "training_accuracy", result.accuracy, "accuracy");
        
        // Check for convergence
        if (result.converged) {
            logWarning("Training", "Training converged early at epoch " + std::to_string(epoch));
            break;
        }
    }
    
    logInfo("Training", "Training completed");
}
```

### Error Recovery

```c++
#include "error/ErrorReporter.hpp"
#include "error/ErrorRecoverySystem.hpp"

void criticalOperation() {
    try {
        // Risky operation that might fail
        performCriticalOperation();
    } catch (const std::exception& e) {
        // Create error context
        ErrorContext error("Critical", "criticalOperation", 
                          "critical.cpp", 42, "Operation failed");
        error.addContextData("operation", "performCriticalOperation");
        error.addContextData("attempt", 1);
        
        // Attempt recovery
        ErrorRecoverySystem::getInstance().recover(error);
        
        // If recovery fails, handle gracefully
        if (error.getLevel() == ErrorLevel::Type::FATAL) {
            logFatal("Critical", "Fatal error, system shutdown required");
            std::exit(1);
        } else {
            logError("Critical", "Error recovered, continuing operation");
        }
    }
}
```

## Testing and Validation

### Error Handling Tests

```bash
# Test error reporting
./test-error-handling --test-type error-reporting

# Test error recovery
./test-error-handling --test-type error-recovery

# Test logging
./test-error-handling --test-type logging

# Test performance monitoring
./test-error-handling --test-type performance-monitoring

# Run all error handling tests
./test-error-handling --all
```

### Performance Monitoring Tests

```bash
# Test metrics collection
./test-performance --test-type metrics-collection

# Test benchmarking
./test-performance --test-type benchmarking

# Test profiling
./test-performance --test-type profiling

# Test system monitoring
./test-performance --test-type system-monitoring

# Run all performance tests
./test-performance --all
```

## Future Enhancements

1. **AI-Powered Error Detection**: Machine learning-based anomaly detection
2. **Predictive Error Handling**: Proactive error prediction and prevention
3. **Adaptive Logging**: Dynamic adjustment of logging levels based on system state
4. **Real-Time Analytics**: Real-time performance analysis and visualization
5. **Distributed Logging**: Network-based logging for distributed systems
6. **Compliance Logging**: Logging for regulatory compliance

## Conclusion

The comprehensive error handling and logging infrastructure provides:

1. **Robust Error Management**: Catching, reporting, and recovery from errors
2. **Detailed Logging**: Comprehensive logging of system events and performance metrics
3. **Performance Monitoring**: Real-time performance tracking and analysis
4. **System Health**: Health checks and anomaly detection
5. **Debugging Support**: Detailed error context and stack traces
6. **Flexible Configuration**: Configurable logging and error handling behavior

This infrastructure is essential for production-grade NLM systems, providing the reliability, observability, and debugging capabilities needed for complex neural simulations in real-world applications.

---

This comprehensive error handling and logging infrastructure provides the foundation for robust, observable, and maintainable NLM brain simulation systems. It enables developers to:

- **Detect and diagnose errors** quickly and accurately
- **Monitor system performance** and identify bottlenecks
- **Track system health** and proactively address issues
- **Debug complex problems** with detailed contextual information
- **Optimize system performance** based on real-time metrics
- **Ensure reliability** and fault tolerance in production environments

The integration with the NLM brain class ensures that errors and performance metrics are automatically captured throughout the neural simulation, providing a complete view of system behavior and performance.

This infrastructure is critical for:

- **Production deployment**: Reliable operation in real-world environments
- **Performance optimization**: Identifying and resolving performance bottlenecks
- **Debugging**: Quickly diagnosing and fixing complex issues
- **Monitoring**: Tracking system health and performance over time
- **Compliance**: Meeting regulatory and quality requirements
- **Maintenance**: Planning and executing system maintenance

The comprehensive error handling and logging system transforms the NLM from a research prototype into a production-ready brain simulation system with the robustness, observability, and maintainability needed for real-world applications.