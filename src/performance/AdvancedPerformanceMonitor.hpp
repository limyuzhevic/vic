// Advanced Performance Monitor
// This class provides advanced performance monitoring capabilities

#pragma once

#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <algorithm>
#include <unordered_map>

namespace nlm {
namespace performance {

class AdvancedPerformanceMonitor {
public:
    AdvancedPerformanceMonitor();
    ~AdvancedPerformanceMonitor();
    
    // Performance monitoring control
    bool startMonitoring(const std::string& profile = "default");
    void stopMonitoring();
    void pauseMonitoring();
    void resumeMonitoring();
    
    // Performance threshold management
    bool setThreshold(const std::string& name, double value, const std::string& type = "absolute");
    bool setThresholds(const std::map<std::string, double>& thresholds);
    std::map<std::string, double> getThresholds() const;
    
    // Performance alerting
    bool addAlert(const std::string& name, double threshold, const std::string& severity = "WARNING");
    void checkAlerts();
    std::vector<std::string> getActiveAlerts() const;
    
    // Performance analysis
    struct PerformanceSnapshot {
        std::chrono::nanoseconds timestamp;
        std::string operation;
        std::chrono::nanoseconds duration;
        std::string context;
        std::string tag;
        size_t threadId;
        uint64_t memoryUsage;
        uint64_t cpuUsage;
        
        PerformanceSnapshot() : timestamp(0), duration(0), threadId(0), memoryUsage(0), cpuUsage(0) {}
    };
    
    struct PerformanceStats {
        double avgDuration;
        double medianDuration;
        double stddevDuration;
        double minDuration;
        double maxDuration;
        double p95Duration;
        double p99Duration;
        double throughput;
        uint64_t totalOperations;
        
        PerformanceStats() : avgDuration(0), medianDuration(0), stddevDuration(0), 
                           minDuration(0), maxDuration(0), p95Duration(0), p99Duration(0),
                           throughput(0), totalOperations(0) {}
    };
    
    void recordOperation(const std::string& operation, const std::string& context = "", const std::string& tag = "");
    void recordOperation(const std::string& operation, std::chrono::nanoseconds duration, 
                        const std::string& context = "", const std::string& tag = "");
    
    PerformanceStats calculateStatistics(const std::vector<PerformanceSnapshot>& snapshots) const;
    std::vector<PerformanceSnapshot> getSnapshots(const std::string& operation = "", const std::string& context = "") const;
    
    // Historical performance analysis
    bool loadHistoricalData(const std::string& filepath);
    bool saveHistoricalData(const std::string& filepath);
    bool detectRegressions(const std::string& baselineFile, const std::string& currentFile);
    bool identifyTrends();
    
    // Performance benchmarking
    bool runBenchmark(const std::string& benchmarkName, const std::string& workload, int iterations = 100);
    bool createBenchmarkProfile(const std::string& profileName, const std::string& workload);
    
    // Real-time monitoring
    bool enableRealTimeMonitoring();
    bool disableRealTimeMonitoring();
    bool isMonitoringActive() const;
    
    // System performance monitoring
    struct SystemPerformanceStats {
        uint64_t totalMemoryUsage;
        uint64_t peakMemoryUsage;
        double totalCPUUsage;
        double avgCPUUsage;
        double maxCPUUsage;
        double avgOperationDuration;
        double maxOperationDuration;
        double minOperationDuration;
        size_t totalOperations;
        
        SystemPerformanceStats() : totalMemoryUsage(0), peakMemoryUsage(0), totalCPUUsage(0),
                                 avgCPUUsage(0), maxCPUUsage(0), avgOperationDuration(0),
                                 maxOperationDuration(0), minOperationDuration(0), totalOperations(0) {}
    };
    
    SystemPerformanceStats getSystemStats() const;
    
    // Performance reports
    std::string generateReport(const std::string& format = "text") const;
    bool exportReport(const std::string& filepath, const std::string& format = "json");
    bool generateComparisonReport(const std::string& baselineFile, const std::string& currentFile);
    
    // Memory and CPU profiling
    bool enableMemoryProfiling();
    bool disableMemoryProfiling();
    bool enableCPUProfiling();
    bool disableCPUProfiling();
    
    // Bottleneck detection
    bool detectBottlenecks();
    std::vector<std::string> getBottleneckReport() const;
    
    // Monitoring status
    bool isMemoryProfilingEnabled() const { return memoryProfilingEnabled_; }
    bool isCPUProfilingEnabled() const { return cpuProfilingEnabled_; }
    bool isRealTimeMonitoringEnabled() const { return realTimeMonitoringEnabled_; }
    
private:
    bool monitoringActive_;
    bool memoryProfilingEnabled_;
    bool cpuProfilingEnabled_;
    bool realTimeMonitoringEnabled_;
    std::chrono::nanoseconds monitoringInterval_;
    
    mutable std::mutex mutex_;
    
    std::vector<PerformanceSnapshot> snapshots_;
    std::map<std::string, double> thresholds_;
    std::vector<std::pair<std::string, std::pair<double, std::string>>> alerts_;
    
    // Thread-local storage for per-thread metrics
    struct ThreadLocalMetrics {
        std::chrono::nanoseconds totalDuration;
        size_t operationCount;
        uint64_t memoryUsage;
        uint64_t cpuUsage;
        
        ThreadLocalMetrics() : totalDuration(0), operationCount(0), memoryUsage(0), cpuUsage(0) {}
    };
    
    std::map<size_t, ThreadLocalMetrics> threadMetrics_;
    
    // Performance calculation helpers
    static double calculateMean(const std::vector<double>& values);
    static double calculateMedian(const std::vector<double>& values);
    static double calculateStddev(const std::vector<double>& values, double mean);
    static double calculatePercentile(const std::vector<double>& values, double percentile);
    
    // Snapshot management
    void addSnapshot(const PerformanceSnapshot& snapshot);
    std::vector<PerformanceSnapshot> filterSnapshots(const std::string& operation, const std::string& context) const;
    
    // Threshold checking
    void checkThresholds();
    
    // Report generation
    std::string generateTextReport() const;
    std::string generateJSONReport() const;
    std::string generateCSVReport() const;
};

} // namespace performance
} // namespace nlm
