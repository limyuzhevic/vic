#pragma once

#include "Logger.hpp"
#include <unordered_map>
#include <vector>
#include <string>
#include <chrono>
#include <functional>

namespace nlm {

// Performance monitoring configuration
struct PerformanceLogConfig {
    bool enableTimingProfiling = true;
    bool enableThroughputMetrics = true;
    bool enableResourceMonitoring = true;
    bool enableMemoryProfiling = true;
    bool enableCPUMonitoring = true;
    bool enableNetworkMonitoring = false;
    bool enableDiskMonitoring = false;
    
    // Timing thresholds
    double slowOperationThresholdMs = 100.0;
    double verySlowOperationThresholdMs = 1000.0;
    
    // Resource thresholds
    double cpuThresholdPercent = 80.0;
    size_t memoryThresholdMB = 500;
    
    // Performance counters
    bool enableOperationCount = true;
    bool enableCycleCount = true;
    bool enableCacheHitRate = true;
    
    // Output format
    bool enableJSONOutput = true;
    bool enableCSVOutput = false;
    bool enableHumanReadableOutput = true;
};

// Performance metrics structure
struct PerformanceMetrics {
    std::string operationName;
    double executionTimeMs;
    size_t memoryUsed;
    double cpuUsagePercent;
    size_t cacheHits;
    size_t cacheMisses;
    std::chrono::system_clock::time_point timestamp;
    std::unordered_map<std::string, std::string> tags;
    
    // Derived metrics
    double getCacheHitRate() const {
        size_t total = cacheHits + cacheMisses;
        return total > 0 ? (static_cast<double>(cacheHits) / total) * 100.0 : 0.0;
    }
};

// Performance logger for comprehensive performance monitoring
class PerformanceLogger {
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
public:
    PerformanceLogger();
    ~PerformanceLogger();
    
    // Disable copying
    PerformanceLogger(const PerformanceLogger&) = delete;
    PerformanceLogger& operator=(const PerformanceLogger&) = delete;
    
    // Configuration
    void configure(const PerformanceLogConfig& config);
    PerformanceLogConfig getConfig() const;
    
    // Timing profiling
    class Timer {
    public:
        Timer(PerformanceLogger& logger, const std::string& operation,
              const std::unordered_map<std::string, std::string>& tags = {});
        ~Timer();
        
        Timer(const Timer&) = delete;
        Timer& operator=(const Timer&) = delete;
        
        void stop();
        double getDurationMs() const { return durationMs_; }
        const std::string& getOperationName() const { return operationName_; }
        
    private:
        PerformanceLogger* logger_;
        std::string operationName_;
        std::chrono::system_clock::time_point startTime_;
        double durationMs_ = 0.0;
        bool stopped_ = false;
        std::unordered_map<std::string, std::string> tags_;
    };
    
    // Start timing an operation
    Timer startTiming(const std::string& operation,
                     const std::unordered_map<std::string, std::string>& tags = {});
    
    // Log performance metrics directly
    void logMetrics(const PerformanceMetrics& metrics);
    
    // Throughput monitoring
    void logThroughput(const std::string& operation, size_t operationsCount,
                      double timeMs, size_t itemsPerSecond = 0);
    
    // Resource monitoring
    void logCPUUsage(double cpuUsagePercent, const std::string& context = "");
    void logMemoryUsage(size_t memoryMB, const std::string& context = "");
    void logDiskUsage(const std::string& mountPoint, double usedMB, double totalMB);
    void logNetworkUsage(double uploadMBps, double downloadMBps);
    
    // Performance profiling
    void logFunctionProfile(const std::string& functionName,
                           double executionTimeMs,
                           size_t memoryDelta = 0,
                           int callCount = 1,
                           const std::unordered_map<std::string, std::string>& metadata = {});
    
    void logThreadProfile(const std::string& threadName,
                         double cpuTimeMs, double wallTimeMs,
                         size_t stackSize = 0,
                         const std::unordered_map<std::string, std::string>& metadata = {});
    
    // Performance analysis
    void logPerformanceRegression(const std::string& operation,
                                 double baselineTimeMs,
                                 double currentTimeMs,
                                 double regressionPercent,
                                 const std::string& context = "");
    
    void logPerformanceImprovement(const std::string& operation,
                                  double baselineTimeMs,
                                  double currentTimeMs,
                                  double improvementPercent,
                                  const std::string& context = "");
    
    // Performance alerts
    void logPerformanceAlert(const std::string& alertType,
                            const std::string& message,
                            double severity = 1.0,
                            const std::unordered_map<std::string, std::string>& metadata = {});
    
    // Get performance statistics
    double getAverageOperationTime(const std::string& operation) const;
    size_t getOperationCount(const std::string& operation) const;
    size_t getTotalMemoryUsed() const;
    double getTotalCPUTime() const;
    
    // Export functionality
    void exportToCSV(const std::string& filepath) const;
    void exportToJSON(const std::string& filepath) const;
    std::string exportToString(const std::string& format = "json") const;
    
    // Get global performance logger instance
    static PerformanceLogger& getGlobal();
    
    // Set global performance logger
    static void setGlobal(std::shared_ptr<PerformanceLogger> logger);
};

// Convenience macros for performance logging
#define NLM_PERF_PROFILE(operation) \
    nlm::PerformanceLogger::Timer __perf_timer(nlm::PerformanceLogger::getGlobal(), operation)

#define NLM_PERF_LOG_THROUGHPUT(operation, count, timeMs) \
    nlm::PerformanceLogger::getGlobal().logThroughput(operation, count, timeMs)

#define NLM_PERF_LOG_CPU(cpuUsage) \
    nlm::PerformanceLogger::getGlobal().logCPUUsage(cpuUsage)

#define NLM_PERF_LOG_MEMORY(memoryMB) \
    nlm::PerformanceLogger::getGlobal().logMemoryUsage(memoryMB)

// Helper macro for timing blocks
#define NLM_TIME_BLOCK(operation) \
    nlm::PerformanceLogger::Timer __perf_timer(nlm::PerformanceLogger::getGlobal(), operation)

} // namespace nlm

#endif // PERFORMANCE_LOGGER_HPP