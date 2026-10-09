// Comprehensive Logging and Debugging Infrastructure
// Enhanced logging system for NLM Python API with thread-safe operations,
// multiple output formats, and advanced analysis capabilities

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
#include <fstream>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include <map>
#include <list>
#include <atomic>
#include <condition_variable>
#include <queue>

namespace nlm {

// Enhanced Log Level enum with additional levels
enum class LogLevel {
    Trace = 0,        // 5: Fine-grained events
    Debug = 1,        // 4: Detailed information
    Info = 2,         // 3: General information
    Warning = 3,      // 2: Warnings
    Error = 4,        // 1: Error conditions
    Critical = 5,     // 0: Critical errors
    Off = 6           // 6: No logging
};

// Log entry with enhanced metadata
struct EnhancedLogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    uint64_t threadId;
    std::string threadName;
    std::unordered_map<std::string, std::string> metadata;
    std::unordered_map<std::string, double> metrics;
    std::string category;           // Log category for filtering
    uint64_t simulationStep;        // Simulation step context
    uint64_t agentId;               // Agent ID context
    uint64_t brainRegionId;         // Region ID context
    
    // Performance timing
    std::chrono::duration<double> processingTime;
    bool isPerformanceEvent;
    
    EnhancedLogEntry() : level(LogLevel::Info), line(0), threadId(0), 
                         simulationStep(0), agentId(0), brainRegionId(0),
                         processingTime(0.0), isPerformanceEvent(false) {}
};

// Log Category enumeration
enum class LogCategory {
    System,           // System initialization and shutdown
    Neural,           // Neural computation details
    Simulation,       // Simulation progress
    Performance,      // Performance metrics and profiling
    Memory,           // Memory usage and debugging
    Configuration,    // Configuration changes
    Validation,       // Validation and error checking
    World,            // World simulation
    Agent,            // Agent behavior
    Development,      // Development and debugging
    Custom            // User-defined categories
};

// Log Format enumeration
enum class LogFormat {
    PlainText,        // Human-readable plain text
    JSON,             // JSON format for machine processing
    CSV,              // Comma-separated values
    XML,              // XML format
    HTML              // HTML formatted logs
};

// Log Aggregation Strategy
enum class AggregationStrategy {
    None,             // No aggregation
    Periodic,         // Aggregate periodically
    SizeBased,        // Aggregate based on size
    TimeBased,        // Aggregate based on time
    EventBased        // Aggregate based on event count
};

// Advanced Logger Sink Interface
class ILoggerSink {
public:
    virtual ~ILoggerSink() = default;
    virtual void write(const EnhancedLogEntry& entry) = 0;
    virtual void flush() = 0;
    virtual void close() = 0;
    virtual bool isConnected() const = 0;
};

// Console Logger Sink with enhanced features
class ConsoleLoggerSink : public ILoggerSink {
public:
    ConsoleLoggerSink();
    explicit ConsoleLoggerSink(LogLevel minLevel);
    ~ConsoleLoggerSink() override;
    
    void write(const EnhancedLogEntry& entry) override;
    void flush() override;
    void close() override;
    bool isConnected() const override;
    
    // Configuration options
    void setMinLevel(LogLevel level);
    void setUseColors(bool useColors);
    void setUseTimestamps(bool useTimestamps);
    void setTimestampFormat(const std::string& format);
    void setThreadInfoEnabled(bool enabled);
    void setPerformanceHighlighting(bool enabled);
    void setEnableCategoryPrefix(bool enabled);
    
    // Output formatting
    void setLogFormat(LogFormat format);
    void setEnableJSONPrettify(bool enabled);
    
    // Performance-specific formatting
    void setIncludeProcessingTime(bool include);
    void setIncludeMetrics(bool include);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string formatTimestamp(const std::chrono::system_clock::time_point& timestamp) const;
    std::string formatEntry(const EnhancedLogEntry& entry) const;
    std::string formatJSON(const EnhancedLogEntry& entry) const;
};

// File Logger Sink with rotation and indexing
class FileLoggerSink : public ILoggerSink {
public:
    FileLoggerSink(const std::string& basePath, const std::string& filenamePrefix = "nlm");
    ~FileLoggerSink() override;
    
    void write(const EnhancedLogEntry& entry) override;
    void flush() override;
    void close() override;
    bool isConnected() const override;
    
    // File management options
    void setFileRotationEnabled(bool enabled);
    void setMaxFileSize(size_t maxSize);
    void setMaxBackupFiles(size_t maxBackups);
    void setRotationInterval(const std::chrono::system_clock::time_point& interval);
    void setCompressionEnabled(bool enabled);
    
    // Indexing options
    void setIndexingEnabled(bool enabled);
    void setEnableSearchIndex(bool enabled);
    
    // Performance options
    void setBufferSize(size_t bufferSize);
    void setWriteAsync(bool async);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Remote Logger Sink for distributed logging
class RemoteLoggerSink : public ILoggerSink {
public:
    RemoteLoggerSink(const std::string& endpoint, const std::string& protocol = "udp");
    ~RemoteLoggerSink() override;
    
    void write(const EnhancedLogEntry& entry) override;
    void flush() override;
    void close() override;
    bool isConnected() const override;
    
    // Configuration
    void setReconnectEnabled(bool enabled);
    void setMaxReconnectAttempts(size_t attempts);
    void setBatchSize(size_t size);
    void setSendInterval(double interval);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Log Aggregator for batch processing
class LogAggregator {
public:
    LogAggregator();
    ~LogAggregator();
    
    // Event processing
    void processEntry(const EnhancedLogEntry& entry);
    
    // Configuration
    void setAggregationStrategy(AggregationStrategy strategy);
    void setAggregationWindow(double windowSeconds);
    void setMaxBatchSize(size_t size);
    void setEnableMetricsAggregation(bool enabled);
    
    // Aggregation management
    void flushAggregates();
    void clearAggregates();
    
    // Query interface
    std::vector<EnhancedLogEntry> getAggregatedEntries() const;
    size_t getEntryCount() const;
    double getProcessingRate() const;
    
    // Callbacks
    void setAggregationCompleteCallback(std::function<void(const std::vector<EnhancedLogEntry>&)> callback);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Log Filter for selective logging
class LogFilter {
public:
    LogFilter();
    ~LogFilter();
    
    // Filtering rules
    void addLevelFilter(LogLevel minLevel, LogLevel maxLevel);
    void addCategoryFilter(LogCategory category);
    void addKeywordFilter(const std::string& keyword);
    void addMetadataFilter(const std::string& key, const std::string& value);
    
    // Performance filtering
    void addPerformanceThreshold(double minProcessingTime, double maxProcessingTime);
    void addMetricThreshold(const std::string& metricName, double minValue, double maxValue);
    
    // Simulation and context filtering
    void addSimulationStepFilter(uint64_t minStep, uint64_t maxStep);
    void addAgentFilter(uint64_t agentId);
    void addRegionFilter(uint64_t regionId);
    
    // Time filtering
    void addTimeFilter(const std::chrono::system_clock::time_point& start, 
                      const std::chrono::system_clock::time_point& end);
    
    // Filtering management
    void clearFilters();
    
    // Filtering logic
    bool shouldLog(const EnhancedLogEntry& entry) const;
    
    // Configuration
    void setCaseInsensitive(bool caseInsensitive);
    void setRegexMatching(bool enabled);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Advanced Logger class with comprehensive features
class AdvancedLogger {
public:
    AdvancedLogger();
    ~AdvancedLogger();
    
    // Disable copying
    AdvancedLogger(const AdvancedLogger&) = delete;
    AdvancedLogger& operator=(const AdvancedLogger&) = delete;
    
    // Sink management
    void addSink(std::shared_ptr<ILoggerSink> sink);
    void removeSink(const ILoggerSink* sink);
    void clearSinks();
    
    // Global filtering
    void setGlobalFilter(std::shared_ptr<LogFilter> filter);
    void removeGlobalFilter();
    
    // Global aggregation
    void setGlobalAggregator(std::shared_ptr<LogAggregator> aggregator);
    void removeGlobalAggregator();
    
    // Configuration
    void setMinLevel(LogLevel level);
    void setDefaultCategory(LogCategory category);
    void setEnableThreadId(bool enabled);
    void setEnableThreadName(bool enabled);
    void setEnablePerformanceLogging(bool enabled);
    void setEnableMetadataLogging(bool enabled);
    
    // Log methods
    void log(LogLevel level, const std::string& message, 
             const std::string& file = "", int line = 0, 
             const std::string& function = "", 
             LogCategory category = LogCategory::Custom,
             const std::unordered_map<std::string, std::string>& metadata = {});
    
    // Convenience methods
    void trace(const std::string& message, const std::string& file = "", int line = 0, 
              const std::string& function = "", LogCategory category = LogCategory::Custom);
    void debug(const std::string& message, const std::string& file = "", int line = 0,
              const std::string& function = "", LogCategory category = LogCategory::Custom);
    void info(const std::string& message, const std::string& file = "", int line = 0,
             const std::string& function = "", LogCategory category = LogCategory::Custom);
    void warning(const std::string& message, const std::string& file = "", int line = 0,
                 const std::string& function = "", LogCategory category = LogCategory::Custom);
    void error(const std::string& message, const std::string& file = "", int line = 0,
              const std::string& function = "", LogCategory category = LogCategory::Custom);
    void critical(const std::string& message, const std::string& file = "", int line = 0,
                  const std::string& function = "", LogCategory category = LogCategory::Custom);
    
    // Performance logging
    void logPerformance(const std::string& operation, 
                       std::chrono::duration<double> processingTime,
                       const std::unordered_map<std::string, double>& metrics = {});
    
    // Neural computation logging
    void logNeuralEvent(const std::string& eventType, uint64_t neuronId,
                       uint64_t regionId, double value = 0.0,
                       const std::unordered_map<std::string, std::string>& metadata = {});
    
    // Simulation context logging
    void logSimulationStep(uint64_t step, double time,
                          const std::unordered_map<std::string, double>& metrics = {});
    
    // Memory logging
    void logMemoryEvent(const std::string& eventType, uint64_t bytes,
                       const std::string& context = "");
    
    // Health check logging
    void logHealthCheck(const std::string& checkName, bool passed,
                       const std::string& details = "");
    
    // Check if level is enabled
    bool isEnabled(LogLevel level) const;
    
    // Get global logger instance
    static AdvancedLogger& getGlobal();
    
    // Set global logger
    static void setGlobal(std::shared_ptr<AdvancedLogger> logger);
    
    // Log analysis utilities
    struct LogAnalysis {
        size_t totalEntries;
        size_t entriesByLevel[6];  // Trace, Debug, Info, Warning, Error, Critical
        std::unordered_map<std::string, size_t> entriesByCategory;
        std::unordered_map<std::string, size_t> entriesByFile;
        std::chrono::duration<double> totalProcessingTime;
        size_t performanceEvents;
        std::unordered_map<std::string, double> averageMetrics;
        std::chrono::system_clock::time_point earliestEntry;
        std::chrono::system_clock::time_point latestEntry;
    };
    
    LogAnalysis analyzeLogs(const std::vector<EnhancedLogEntry>& entries) const;
    
    // Log export utilities
    bool exportLogs(const std::string& outputPath, LogFormat format = LogFormat::JSON,
                   const std::shared_ptr<LogFilter>& filter = nullptr);
    
    // Log correlation utilities
    struct CorrelationResult {
        std::vector<uint64_t> correlatedEntries;
        std::string correlationType;
        double correlationStrength;
        std::unordered_map<std::string, double> metrics;
    };
    
    CorrelationResult correlateLogs(const std::vector<EnhancedLogEntry>& entries,
                                    const std::string& correlationType,
                                    const std::unordered_map<std::string, std::string>& options = {});
    
    // Log-based troubleshooting
    struct TroubleshootingResult {
        std::vector<std::string> identifiedIssues;
        std::vector<std::string> suggestedSolutions;
        std::unordered_map<std::string, std::vector<std::string>> issueCategories;
        std::chrono::duration<double> analysisTime;
    };
    
    TroubleshootingResult troubleshootFromLogs(const std::vector<EnhancedLogEntry>& entries,
                                               const std::string& problemDescription = "");
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Convenience macro for logging with advanced features
#define NLM_LOG_TRACE(msg, category) nlm::AdvancedLogger::getGlobal().trace(msg, __FILE__, __LINE__, __func__, category)
#define NLM_LOG_DEBUG(msg, category) nlm::AdvancedLogger::getGlobal().debug(msg, __FILE__, __LINE__, __func__, category)
#define NLM_LOG_INFO(msg, category) nlm::AdvancedLogger::getGlobal().info(msg, __FILE__, __LINE__, __func__, category)
#define NLM_LOG_WARNING(msg, category) nlm::AdvancedLogger::getGlobal().warning(msg, __FILE__, __LINE__, __func__, category)
#define NLM_LOG_ERROR(msg, category) nlm::AdvancedLogger::getGlobal().error(msg, __FILE__, __LINE__, __func__, category)
#define NLM_LOG_CRITICAL(msg, category) nlm::AdvancedLogger::getGlobal().critical(msg, __FILE__, __LINE__, __func__, category)

// Enhanced LogStream for formatted logging
class AdvancedLogStream {
public:
    AdvancedLogStream(LogLevel level, const std::string& file, int line,
                      const std::string& function, LogCategory category = LogCategory::Custom,
                      const std::unordered_map<std::string, std::string>& metadata = {});
    ~AdvancedLogStream();
    
    template<typename T>
    AdvancedLogStream& operator<<(const T& value) {
        oss_ << value;
        return *this;
    }
    
    // Add metadata
    AdvancedLogStream& withMetadata(const std::string& key, const std::string& value);
    AdvancedLogStream& withMetric(const std::string& key, double value);
    AdvancedLogStream& withAgentId(uint64_t agentId);
    AdvancedLogStream& withRegionId(uint64_t regionId);
    AdvancedLogStream& withSimulationStep(uint64_t step);
    
private:
    LogLevel level_;
    std::string file_;
    int line_;
    std::string function_;
    LogCategory category_;
    std::unordered_map<std::string, std::string> metadata_;
    std::unordered_map<std::string, double> metrics_;
    uint64_t agentId_;
    uint64_t regionId_;
    uint64_t simulationStep_;
    std::ostringstream oss_;
};

#define NLM_LOG_STREAM(level, category) nlm::AdvancedLogStream(level, __FILE__, __LINE__, __func__, category)

} // namespace nlm
