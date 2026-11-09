# Advanced NLM Logging and Debugging System
# Comprehensive logging, monitoring, and debugging tools for NLM

#ifndef ADVANCED_LOGGING_HPP
#define ADVANCED_LOGGING_HPP

#include "core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <chrono>
#include <map>
#include <vector>
#include <functional>
#include <thread>
#include <atomic>
#include <mutex>
#include <iomanip>
#include <filesystem>
#include <sstream>

namespace nlm {

enum class LogLevel {
    Trace,
    Debug,
    Info,
    Warning,
    Error,
    Fatal
};

enum class PerformanceMetric {
    SimulationSteps,
    SpikeCount,
    FiringRate,
    MemoryUsage,
    CPUUsage,
    CheckpointTime,
    NetworkLatency,
    PlasticityEvents,
    MemoryOperations
};

struct PerformanceSnapshot {
    std::chrono::steady_clock::time_point timestamp;
    uint64_t simulationStep;
    float simulationTime;
    std::map<PerformanceMetric, double> metrics;
    std::string brainState;
};

struct EventLog {
    std::chrono::steady_clock::time_point timestamp;
    LogLevel level;
    std::string source;
    std::string message;
    std::map<std::string, std::string> metadata;
    std::vector<std::string> stackTrace;
};

class AdvancedLogger {
private:
    struct Impl {
        std::mutex logMutex;
        std::mutex performanceMutex;
        std::mutex eventMutex;
        
        // File logging
        std::ofstream logFile;
        std::string logFilePath;
        bool fileLoggingEnabled;
        
        // Console logging
        bool consoleLoggingEnabled;
        LogLevel consoleLogLevel;
        bool coloredOutput;
        
        // Performance monitoring
        std::vector<PerformanceSnapshot> performanceHistory;
        std::vector<EventLog> eventLog;
        std::atomic<uint64_t> totalSpikes;
        std::atomic<uint64_t> totalSteps;
        
        // Callbacks
        std::vector<std::function<void(const EventLog&)>> eventCallbacks;
        std::function<void(const PerformanceSnapshot&)> performanceCallback;
        
        // Filtering
        std::map<std::string, LogLevel> sourceLogLevels;
        std::vector<std::string> includedSources;
        std::vector<std::string> excludedSources;
        
        // Performance thresholds
        std::map<PerformanceMetric, std::pair<double, double>> performanceThresholds;
        
        Impl() 
            : fileLoggingEnabled(false)
            , consoleLoggingEnabled(true)
            , consoleLogLevel(LogLevel::Info)
            , coloredOutput(true)
            , totalSpikes(0)
            , totalSteps(0) {
            
            // Initialize performance thresholds
            performanceThresholds[PerformanceMetric::SimulationSteps] = {1000.0, 10000.0};
            performanceThresholds[PerformanceMetric::SpikeCount] = {100.0, 100000.0};
            performanceThresholds[PerformanceMetric::FiringRate] = {0.1, 100.0};
            performanceThresholds[PerformanceMetric::MemoryUsage] = {10.0, 1000.0};
            performanceThresholds[PerformanceMetric::CPUUsage] = {1.0, 50.0};
            }
    };
    
    std::unique_ptr<Impl> pImpl;

public:
    AdvancedLogger();
    ~AdvancedLogger();
    
    // Configuration
    void enableFileLogging(const std::string& filepath, bool append = false);
    void disableFileLogging();
    void setConsoleLogging(bool enabled);
    void setConsoleLogLevel(LogLevel level);
    void enableColoredOutput(bool enabled);
    
    // Source filtering
    void setSourceLogLevel(const std::string& source, LogLevel level);
    void includeSource(const std::string& source);
    void excludeSource(const std::string& source);
    void setPerformanceThreshold(PerformanceMetric metric, double min, double max);
    
    // Event logging
    void logEvent(LogLevel level, const std::string& source, const std::string& message,
                  const std::map<std::string, std::string>& metadata = {});
    void logTrace(const std::string& source, const std::string& message,
                  const std::map<std::string, std::string>& metadata = {});
    void logDebug(const std::string& source, const std::string& message,
                  const std::map<std::string, std::string>& metadata = {});
    void logInfo(const std::string& source, const std::string& message,
                 const std::map<std::string, std::string>& metadata = {});
    void logWarning(const std::string& source, const std::string& message,
                    const std::map<std::string, std::string>& metadata = {});
    void logError(const std::string& source, const std::string& message,
                  const std::map<std::string, std::string>& metadata = {});
    void logFatal(const std::string& source, const std::string& message,
                  const std::map<std::string, std::string>& metadata = {});
    
    // Performance monitoring
    void recordPerformanceMetric(PerformanceMetric metric, double value,
                                 const std::string& context = "");
    void updatePerformance(const std::map<PerformanceMetric, double>& metrics,
                          uint64_t step, float simulationTime);
    void recordSpike();
    void recordStep();
    
    // Callbacks
    void addEventCallback(const std::function<void(const EventLog&)>& callback);
    void setPerformanceCallback(const std::function<void(const PerformanceSnapshot&)>& callback);
    
    // Query methods
    std::vector<EventLog> getEvents(const std::string& source = "",
                                    LogLevel minLevel = LogLevel::Trace,
                                    const std::chrono::steady_clock::time_point& since = {});
    std::vector<PerformanceSnapshot> getPerformanceHistory(const std::chrono::steady_clock::time_point& since = {});
    
    // Analysis and reporting
    std::string generatePerformanceReport() const;
    std::string generateEventReport(const std::string& source = "") const;
    std::map<std::string, double> analyzePerformanceTrends() const;
    
    // Export and import
    bool exportLogs(const std::string& filepath);
    bool importLogs(const std::string& filepath);
    
    // Status
    void printStatus() const;
    bool isHealthy() const;
    
    // Clean up
    void clearHistory(size_t maxEvents = 10000, size_t maxSnapshots = 1000);
    void flush();
};

// Global instance
extern AdvancedLogger g_advancedLogger;

// Convenience macros
#define LOG_TRACE(source, message, ...) nlm::g_advancedLogger.logTrace(source, message, ##__VA_ARGS__)
#define LOG_DEBUG(source, message, ...) nlm::g_advancedLogger.logDebug(source, message, ##__VA_ARGS__)
#define LOG_INFO(source, message, ...) nlm::g_advancedLogger.logInfo(source, message, ##__VA_ARGS__)
#define LOG_WARNING(source, message, ...) nlm::g_advancedLogger.logWarning(source, message, ##__VA_ARGS__)
#define LOG_ERROR(source, message, ...) nlm::g_advancedLogger.logError(source, message, ##__VA_ARGS__)
#define LOG_FATAL(source, message, ...) nlm::g_advancedLogger.logFatal(source, message, ##__VA_ARGS__)

} // namespace nlm

#endif // ADVANCED_LOGGING_HPP
