#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <unordered_map>
#include <mutex>
#include <queue>
#include <thread>
#include <atomic>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <nlohmann/json.hpp>

namespace nlm {

// ===============================================================
// ADVANCED LOG LEVELS
// ===============================================================

enum class AdvancedLogLevel {
    // Core levels
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4,
    
    // Advanced levels
    Trace = -1,
    Performance = 5,
    Memory = 6,
    Neural = 7,
    Integration = 8,
    Development = 9,
    Experiment = 10
};

// ===============================================================
// STRUCTURED LOG ENTRY WITH METADATA
// ===============================================================

struct StructuredLogEntry {
    AdvancedLogLevel level;
    std::string message;
    std::string category;
    std::string subcategory;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    
    // Metadata for advanced features
    std::unordered_map<std::string, std::string> metadata;
    std::unordered_map<std::string, double> metrics;
    std::unordered_map<std::string, std::vector<std::string>> tags;
    
    // Performance and profiling data
    double executionTime = 0.0;
    size_t memoryUsage = 0;
    int threadId = 0;
    
    // Event and system information
    std::string eventId;
    std::string sessionId;
    std::string correlationId;
    std::string source;
    
    // Thread and process information
    std::string threadName;
    std::string processName;
    
    // Performance metrics
    int systemLoad = 0;
    double cpuUsage = 0.0;
    double memoryUsagePercent = 0.0;
    
    // Integration system info
    std::string integrationStage;
    std::string componentName;
    std::string componentVersion;
    
    // Neural network metrics
    int neuronCount = 0;
    int synapseCount = 0;
    int spikeCount = 0;
    float firingRate = 0.0f;
    
    // Memory system metrics
    int workingMemorySize = 0;
    int episodicMemorySize = 0;
    int totalMemoryUsage = 0;
    
    // Development metrics
    std::string developmentalStage;
    float plasticityRate = 0.0f;
    
    // Experiment metrics
    double experimentReward = 0.0;
    int experimentSteps = 0;
    double experimentEfficiency = 0.0;
};

// ===============================================================
// ADVANCED LOGGER INTERFACE
// ===============================================================

class IAdvancedLogger {
public:
    virtual ~IAdvancedLogger() = default;
    
    // Core logging methods
    virtual void log(const StructuredLogEntry& entry) = 0;
    virtual void log(AdvancedLogLevel level, const std::string& message) = 0;
    virtual void log(AdvancedLogLevel level, const std::string& category, 
                   const std::string& message) = 0;
    virtual void log(AdvancedLogLevel level, const std::string& category,
                   const std::string& subcategory, const std::string& message) = 0;
    
    // Advanced logging with metadata
    virtual void logWithMetadata(AdvancedLogLevel level, const std::string& message,
                                const std::unordered_map<std::string, std::string>& metadata) = 0;
    
    virtual void logWithMetrics(AdvancedLogLevel level, const std::string& message,
                               const std::unordered_map<std::string, double>& metrics) = 0;
    
    // Category and level management
    virtual void setLevel(AdvancedLogLevel level) = 0;
    virtual AdvancedLogLevel getLevel() const = 0;
    virtual void addCategory(const std::string& category) = 0;
    virtual void removeCategory(const std::string& category) = 0;
    virtual bool isCategoryEnabled(const std::string& category) const = 0;
    
    // Output configuration
    virtual void setOutputFormat(const std::string& format) = 0;
    virtual void enableColors(bool enable) = 0;
    virtual bool getColorsEnabled() const = 0;
    
    // File output management
    virtual void setOutputFile(const std::string& filepath) = 0;
    virtual void setLogRotation(size_t maxFileSize, size_t maxFiles) = 0;
    virtual void enableAsynchronousLogging(bool enable) = 0;
    
    // Performance monitoring
    virtual void enablePerformanceLogging(bool enable) = 0;
    virtual bool isPerformanceLoggingEnabled() const = 0;
    virtual void setPerformanceSampleRate(double rate) = 0;
    
    // Filtering and routing
    virtual void addLogFilter(std::function<bool(const StructuredLogEntry&)> filter) = 0;
    virtual void clearLogFilters() = 0;
    
    // Statistics and metrics
    virtual size_t getLogCount() const = 0;
    virtual double getAverageLogTime() const = 0;
    virtual std::unordered_map<std::string, size_t> getCategoryStats() const = 0;
};

// ===============================================================
// CONCISE LOGGER (STRUCTURED)
// ===============================================================

class ConciseLogger : public IAdvancedLogger {
public:
    ConciseLogger();
    explicit ConciseLogger(AdvancedLogLevel level);
    ~ConciseLogger() override;
    
    void log(const StructuredLogEntry& entry) override;
    void log(AdvancedLogLevel level, const std::string& message) override;
    void log(AdvancedLogLevel level, const std::string& category, 
           const std::string& message) override;
    void log(AdvancedLogLevel level, const std::string& category,
           const std::string& subcategory, const std::string& message) override;
    
    void logWithMetadata(AdvancedLogLevel level, const std::string& message,
                        const std::unordered_map<std::string, std::string>& metadata) override;
    
    void logWithMetrics(AdvancedLogLevel level, const std::string& message,
                       const std::unordered_map<std::string, double>& metrics) override;
    
    // Category and level management
    void setLevel(AdvancedLogLevel level) override;
    AdvancedLogLevel getLevel() const override;
    void addCategory(const std::string& category) override;
    void removeCategory(const std::string& category) override;
    bool isCategoryEnabled(const std::string& category) const override;
    
    // Output configuration
    void setOutputFormat(const std::string& format) override;
    void enableColors(bool enable) override;
    bool getColorsEnabled() const override;
    
    // File output management
    void setOutputFile(const std::string& filepath) override;
    void setLogRotation(size_t maxFileSize, size_t maxFiles) override;
    void enableAsynchronousLogging(bool enable) override;
    
    // Performance monitoring
    void enablePerformanceLogging(bool enable) override;
    bool isPerformanceLoggingEnabled() const override;
    void setPerformanceSampleRate(double rate) override;
    
    // Filtering and routing
    void addLogFilter(std::function<bool(const StructuredLogEntry&)> filter) override;
    void clearLogFilters() override;
    
    // Statistics and metrics
    size_t getLogCount() const override;
    double getAverageLogTime() const override;
    std::unordered_map<std::string, size_t> getCategoryStats() const override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    void writeToFile(const StructuredLogEntry& entry);
    std::string formatEntry(const StructuredLogEntry& entry) const;
    std::string formatJsonEntry(const StructuredLogEntry& entry) const;
    std::string formatConciseEntry(const StructuredLogEntry& entry) const;
};

// ===============================================================
// ADVANCED LOGGER FACTORY AND UTILITIES
// ===============================================================

class LoggerFactory {
public:
    static std::shared_ptr<IAdvancedLogger> createConsoleLogger(AdvancedLogLevel level = AdvancedLogLevel::Info);
    static std::shared_ptr<IAdvancedLogger> createFileLogger(const std::string& filepath, AdvancedLogLevel level = AdvancedLogLevel::Info);
    static std::shared_ptr<IAdvancedLogger> createRotatingFileLogger(const std::string& filepath, 
                                                                   size_t maxFileSize = 10 * 1024 * 1024,
                                                                   size_t maxFiles = 5,
                                                                   AdvancedLogLevel level = AdvancedLogLevel::Info);
    static std::shared_ptr<IAdvancedLogger> createAsynchronousLogger(std::shared_ptr<IAdvancedLogger> backend, size_t queueSize = 10000);
    
    static std::string levelToString(AdvancedLogLevel level);
    static std::string levelToColor(AdvancedLogLevel level);
    static std::string timestampToString(const std::chrono::system_clock::time_point& timestamp);
    
    static void setupDevelopmentLogger(const std::string& logFile = "nlm_development.log");
    static void setupProductionLogger(const std::string& logFile = "nlm_production.log");
    static void setupPerformanceLogger(const std::string& logFile = "nlm_performance.log");
    static void setupIntegrationLogger(const std::string& logFile = "nlm_integration.log");
};

// ===============================================================
// ADVANCED GLOBAL LOGGER MANAGER
// ===============================================================

class AdvancedLoggerManager {
public:
    static AdvancedLoggerManager& getInstance();
    
    void initialize(std::shared_ptr<IAdvancedLogger> logger);
    std::shared_ptr<IAdvancedLogger> getLogger();
    
    // Convenience logging methods
    static void log(AdvancedLogLevel level, const std::string& message);
    static void log(AdvancedLogLevel level, const std::string& category, const std::string& message);
    static void log(AdvancedLogLevel level, const std::string& category, const std::string& subcategory, const std::string& message);
    
    static void debug(const std::string& message);
    static void info(const std::string& message);
    static void warning(const std::string& message);
    static void error(const std::string& message);
    static void critical(const std::string& message);
    
    static void debug(const std::string& category, const std::string& message);
    static void info(const std::string& category, const std::string& message);
    static void warning(const std::string& category, const std::string& message);
    static void error(const std::string& category, const std::string& message);
    static void critical(const std::string& category, const std::string& message);
    
    // Performance logging
    static void logPerformance(const std::string& operation, double duration, 
                              const std::unordered_map<std::string, double>& metrics = {});
    
    static void logMemoryUsage(size_t memoryBytes);
    static void logNeuralActivity(int neuronCount, int spikeCount, float firingRate);
    static void logIntegrationEvent(const std::string& stage, const std::string& component);
    
    // Configuration
    static void setLogLevel(AdvancedLogLevel level);
    static void enablePerformanceMonitoring(bool enable);
    static void setLogCategoryFilter(const std::vector<std::string>& categories);
    
    // Statistics
    static size_t getTotalLogCount();
    static std::unordered_map<std::string, size_t> getLogStatistics();
    
    // File management
    static void setLogDirectory(const std::string& directory);
    static void rotateLogs();
    
private:
    AdvancedLoggerManager();
    ~AdvancedLoggerManager();
    
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    AdvancedLoggerManager(const AdvancedLoggerManager&) = delete;
    AdvancedLoggerManager& operator=(const AdvancedLoggerManager&) = delete;
};

// ===============================================================
// CONVENIENCE MACROS AND STREAMS
// ===============================================================

// Thread-local storage for category information
thread_local extern std::string currentLogCategory;
thread_local extern std::string currentLogSubcategory;

// Advanced logging macros with category support
#define NLM_LOG_TRACE(msg) nlm::AdvancedLoggerManager::log(nlm::AdvancedLogLevel::Trace, msg)
#define NLM_LOG_PERFORMANCE(msg) nlm::AdvancedLoggerManager::log(nlm::AdvancedLogLevel::Performance, msg)
#define NLM_LOG_MEMORY(msg) nlm::AdvancedLoggerManager::log(nlm::AdvancedLogLevel::Memory, msg)
#define NLM_LOG_NEURAL(msg) nlm::AdvancedLoggerManager::log(nlm::AdvancedLogLevel::Neural, msg)
#define NLM_LOG_INTEGRATION(msg) nlm::AdvancedLoggerManager::log(nlm::AdvancedLogLevel::Integration, msg)
#define NLM_LOG_DEVELOPMENT(msg) nlm::AdvancedLoggerManager::log(nlm::AdvancedLogLevel::Development, msg)
#define NLM_LOG_EXPERIMENT(msg) nlm::AdvancedLoggerManager::log(nlm::AdvancedLogLevel::Experiment, msg)

#define NLM_LOG_CATEGORY_TRACE(category, msg) nlm::AdvancedLoggerManager::log(nlm::AdvancedLogLevel::Trace, category, msg)
#define NLM_LOG_CATEGORY_PERFORMANCE(category, msg) nlm::AdvancedLoggerManager::log(nlm::AdvancedLogLevel::Performance, category, msg)
#define NLM_LOG_CATEGORY_MEMORY(category, msg) nlm::AdvancedLoggerManager::log(nlm::AdvancedLogLevel::Memory, category, msg)

#define NLM_LOG_CATEGORY_SUB_TRACE(category, subcategory, msg) nlm::AdvancedLoggerManager::log(nlm::AdvancedLogLevel::Trace, category, subcategory, msg)

// Stream-style advanced logging
class AdvancedLogStream {
public:
    AdvancedLogStream(AdvancedLogLevel level, const std::string& category = "",
                     const std::string& subcategory = "", const std::string& file = "",
                     int line = 0, const std::string& function = "");
    ~AdvancedLogStream();
    
    template<typename T>
    AdvancedLogStream& operator<<(const T& value) {
        oss_ << value;
        return *this;
    }
    
    // Metadata and metrics
    AdvancedLogStream& withMetadata(const std::string& key, const std::string& value);
    AdvancedLogStream& withMetric(const std::string& key, double value);
    AdvancedLogStream& withTag(const std::string& tag);
    
    // Performance metrics
    AdvancedLogStream& withExecutionTime(double time);
    AdvancedLogStream& withMemoryUsage(size_t bytes);
    AdvancedLogStream& withSystemLoad(int load);
    
    // Neural system metrics
    AdvancedLogStream& withNeuralStats(int neuronCount, int synapseCount, int spikeCount, float firingRate);
    
    // Memory system metrics
    AdvancedLogStream& withMemoryStats(int workingMemSize, int episodicMemSize, int totalMemUsage);
    
    // Development metrics
    AdvancedLogStream& withDevelopmentStage(const std::string& stage);
    AdvancedLogStream& withPlasticityRate(float rate);
    
    // Integration system metrics
    AdvancedLogStream& withIntegrationStage(const std::string& stage);
    AdvancedLogStream& withComponentInfo(const std::string& name, const std::string& version);

private:
    AdvancedLogLevel level_;
    std::string category_;
    std::string subcategory_;
    std::string file_;
    int line_;
    std::string function_;
    std::ostringstream oss_;
    
    std::unordered_map<std::string, std::string> metadata_;
    std::unordered_map<std::string, double> metrics_;
    std::vector<std::string> tags_;
    
    double executionTime_ = 0.0;
    size_t memoryUsage_ = 0;
    int systemLoad_ = 0;
    
    int neuronCount_ = 0;
    int synapseCount_ = 0;
    int spikeCount_ = 0;
    float firingRate_ = 0.0f;
    
    int workingMemorySize_ = 0;
    int episodicMemorySize_ = 0;
    int totalMemoryUsage_ = 0;
    
    std::string developmentalStage_;
    float plasticityRate_ = 0.0f;
    
    std::string integrationStage_;
    std::string componentName_;
    std::string componentVersion_;
};

// Macro for advanced stream logging
#define NLM_LOG_STREAM_ADVANCED(level, category, subcategory) \
    nlm::AdvancedLogStream(level, category, subcategory, __FILE__, __LINE__, __func__)

// ===============================================================
// SPECIALIZED LOGGERS FOR DIFFERENT SYSTEMS
// ===============================================================

class PerformanceLogger : public IAdvancedLogger {
public:
    PerformanceLogger();
    ~PerformanceLogger() override;
    
    void log(const StructuredLogEntry& entry) override;
    void log(AdvancedLogLevel level, const std::string& message) override;
    void log(AdvancedLogLevel level, const std::string& category, const std::string& message) override;
    void log(AdvancedLogLevel level, const std::string& category, 
           const std::string& subcategory, const std::string& message) override;
    
    void logWithMetadata(AdvancedLogLevel level, const std::string& message,
                        const std::unordered_map<std::string, std::string>& metadata) override;
    
    void logWithMetrics(AdvancedLogLevel level, const std::string& message,
                       const std::unordered_map<std::string, double>& metrics) override;
    
    void setLevel(AdvancedLogLevel level) override;
    AdvancedLogLevel getLevel() const override;
    void addCategory(const std::string& category) override;
    void removeCategory(const std::string& category) override;
    bool isCategoryEnabled(const std::string& category) const override;
    
    void setOutputFormat(const std::string& format) override;
    void enableColors(bool enable) override;
    bool getColorsEnabled() const override;
    
    void setOutputFile(const std::string& filepath) override;
    void setLogRotation(size_t maxFileSize, size_t maxFiles) override;
    void enableAsynchronousLogging(bool enable) override;
    
    void enablePerformanceLogging(bool enable) override;
    bool isPerformanceLoggingEnabled() const override;
    void setPerformanceSampleRate(double rate) override;
    
    void addLogFilter(std::function<bool(const StructuredLogEntry&)> filter) override;
    void clearLogFilters() override;
    
    size_t getLogCount() const override;
    double getAverageLogTime() const override;
    std::unordered_map<std::string, size_t> getCategoryStats() const override;
    
    void profileOperation(const std::string& operation, double duration, 
                         const std::unordered_map<std::string, double>& metrics = {});
    
    void logNeuralPerformance(const std::string& metric, double value, 
                             const std::string& details = "");
    
    void logMemoryPerformance(size_t memoryBytes, double efficiency);
    
    void logIntegrationPerformance(const std::string& stage, double successRate, double efficiency);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

class IntegrationLogger : public IAdvancedLogger {
public:
    IntegrationLogger();
    ~IntegrationLogger() override;
    
    void log(const StructuredLogEntry& entry) override;
    void log(AdvancedLogLevel level, const std::string& message) override;
    void log(AdvancedLogLevel level, const std::string& category, const std::string& message) override;
    void log(AdvancedLogLevel level, const std::string& category,
           const std::string& subcategory, const std::string& message) override;
    
    void logWithMetadata(AdvancedLogLevel level, const std::string& message,
                        const std::unordered_map<std::string, std::string>& metadata) override;
    
    void logWithMetrics(AdvancedLogLevel level, const std::string& message,
                       const std::unordered_map<std::string, double>& metrics) override;
    
    void setLevel(AdvancedLogLevel level) override;
    AdvancedLogLevel getLevel() const override;
    void addCategory(const std::string& category) override;
    void removeCategory(const std::string& category) override;
    bool isCategoryEnabled(const std::string& category) const override;
    
    void setOutputFormat(const std::string& format) override;
    void enableColors(bool enable) override;
    bool getColorsEnabled() const override;
    
    void setOutputFile(const std::string& filepath) override;
    void setLogRotation(size_t maxFileSize, size_t maxFiles) override;
    void enableAsynchronousLogging(bool enable) override;
    
    void enablePerformanceLogging(bool enable) override;
    bool isPerformanceLoggingEnabled() const override;
    void setPerformanceSampleRate(double rate) override;
    
    void addLogFilter(std::function<bool(const StructuredLogEntry&)> filter) override;
    void clearLogFilters() override;
    
    size_t getLogCount() const override;
    double getAverageLogTime() const override;
    std::unordered_map<std::string, size_t> getCategoryStats() const override;
    
    void logSystemIntegration(const std::string& system, bool success, 
                             const std::string& details = "");
    
    void logMemorySystemIntegration(const std::string& system, bool connected, 
                                   const std::string& details = "");
    
    void logNeuromodulationIntegration(const std::string& modulator, double level);
    
    void logPredictionSystemIntegration(const std::string& system, bool enabled);
    
    void logDevelopmentSystemIntegration(const std::string& stage, float rate);
    
    void logCheckpointingStatus(bool works, const std::string& details = "");
    
    void logReplayStatus(bool enabled, const std::string& details = "");

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm

// Include implementation header
#include "AdvancedLogger.ipp"
