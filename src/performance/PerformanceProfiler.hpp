// Comprehensive Performance Profiler and Monitoring System for NLM
// Provides detailed performance measurement, profiling, and analysis

#pragma once

#include "experiments/Metrics.hpp"
#include "experiments/ScalingBenchmark.hpp"
#include <chrono>
#include <thread>
#include <vector>
#include <string>
#include <map>
#include <unordered_map>
#include <functional>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <memory>
#include <cmath>
#include <algorithm>

namespace nlm {

// Performance measurement types
enum class MetricType {
    COUNTER,
    GAUGE,
    TIMER,
    PERCENTILE
};

enum class ProfilingLevel {
    BASIC,
    DETAILED,
    DEEP
};

enum class AlertLevel {
    INFO,
    WARNING,
    ERROR,
    CRITICAL
};

struct PerformanceThreshold {
    std::string name;
    MetricType metricType;
    AlertLevel alertLevel;
    double warningThreshold;
    double errorThreshold;
    double criticalThreshold;
    std::string description;
};

struct PerformanceSample {
    std::chrono::high_resolution_clock::time_point timestamp;
    std::unordered_map<std::string, double> metrics;
    std::unordered_map<std::string, double> deltas;
};

class PerformanceProfiler {
public:
    PerformanceProfiler();
    ~PerformanceProfiler();
    
    // Start/stop profiling
    void start();
    void stop();
    bool isRunning() const { return running_.load(); }
    
    // Core profiling operations
    void recordMetric(const std::string& name, double value);
    void recordTimer(const std::string& name, double seconds);
    void incrementCounter(const std::string& name, double delta = 1.0);
    void setGauge(const std::string& name, double value);
    
    // Memory profiling
    void recordMemoryUsage();
    void recordMemoryAllocation(const std::string& component, size_t bytes);
    void recordMemoryDeallocation(const std::string& component, size_t bytes);
    
    // CPU profiling
    void recordCPUUsage(double usagePercent);
    void recordThreadUsage(const std::string& threadName, double usagePercent);
    
    // Simulation time profiling
    void recordStepTime(double stepTimeSeconds);
    void recordSimulationTime(double realTime, double simulatedTime);
    void recordSpeedFactor(double factor);
    
    // Component profiling
    void startComponentProfile(const std::string& componentName);
    void stopComponentProfile(const std::string& componentName);
    
    // Batch operations for simulation steps
    void startStepProfiling();
    void endStepProfiling();
    
    // Sample management
    std::vector<PerformanceSample> getSamples(const std::string& component = "") const;
    PerformanceSample getCurrentSample() const;
    
    // Metrics aggregation
    double getMetricMean(const std::string& name) const;
    double getMetricStdDev(const std::string& name) const;
    double getMetricMin(const std::string& name) const;
    double getMetricMax(const std::string& name) const;
    std::vector<double> getMetricValues(const std::string& name) const;
    
    // Performance analysis
    std::string analyzePerformanceBottlenecks() const;
    std::string generatePerformanceReport() const;
    
    // Alerting and thresholding
    void addThreshold(const PerformanceThreshold& threshold);
    bool checkThresholds() const;
    std::vector<std::string> getActiveAlerts() const;
    
    // File operations
    bool saveToCSV(const std::string& filename) const;
    bool saveToJSON(const std::string& filename) const;
    bool loadFromFile(const std::string& filename);
    
    // Feature impact analysis
    void startFeatureProfiling(const std::string& featureName);
    void endFeatureProfiling(const std::string& featureName);
    std::string compareFeaturePerformance(const std::string& featureName1, const std::string& featureName2) const;
    
    // Regression detection
    bool detectRegression(const std::string& baselineFile, double regressionThreshold) const;
    std::string generateRegressionReport(const std::string& baselineFile) const;
    
    // Benchmarking utilities
    BenchmarkMetrics convertToBenchmarkMetrics() const;
    
    // Real-time monitoring
    void startRealTimeMonitoring(double sampleIntervalSeconds = 1.0);
    void stopRealTimeMonitoring();
    BenchmarkMetrics getCurrentRealTimeMetrics() const;
    
    // Performance comparison
    std::string compareProfiles(const PerformanceProfiler& otherProfiler) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl_;
    
    void updateThresholds();
    double getCurrentMetric(const std::string& name) const;
};

class CPUMonitor {
public:
    CPUMonitor();
    ~CPUMonitor();
    
    // Start/stop monitoring
    void start();
    void stop();
    
    // Get current CPU metrics
    struct CPUMetrics {
        double totalUsagePercent;
        double systemIdle;
        double userTime;
        double systemTime;
        std::vector<double> coreUsages;
        std::chrono::high_resolution_clock::time_point timestamp;
    };
    
    CPUMetrics getCurrentMetrics() const;
    
    // Get historical data
    std::vector<CPUMetrics> getHistory() const;
    
    // CPU events
    void onCPUThrottling(bool isThrottled);
    void onFrequencyChange(int newFrequencyMHz);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl_;
};

class MemoryMonitor {
public:
    MemoryMonitor();
    ~MemoryMonitor();
    
    // Start/stop monitoring
    void start();
    void stop();
    
    // Get current memory metrics
    struct MemoryMetrics {
        size_t totalBytes;
        size_t usedBytes;
        size_t availableBytes;
        size_t residentSetSize;
        size_t virtualMemorySize;
        std::unordered_map<std::string, size_t> allocationBreakdown;
        std::chrono::high_resolution_clock::time_point timestamp;
    };
    
    MemoryMetrics getCurrentMetrics() const;
    
    // Get historical data
    std::vector<MemoryMetrics> getHistory() const;
    
    // Memory allocation tracking
    void trackAllocation(size_t bytes, const std::string& source = "unknown");
    void trackDeallocation(size_t bytes, const std::string& source = "unknown");
    
    // Memory fragmentation
    double getFragmentationRatio() const;
    size_t getLargestAllocation() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl_;
};

class SimulationTimeMonitor {
public:
    SimulationTimeMonitor();
    ~SimulationTimeMonitor();
    
    // Start/stop monitoring
    void start(double simulationTimePerStep = 0.001);
    void stop();
    
    // Simulation state tracking
    struct SimulationMetrics {
        double realTimeElapsed;
        double simulatedTimeElapsed;
        double simulatedTimePerStep;
        double realTimePerStep;
        double speedFactor;
        uint64_t stepsCompleted;
        uint64_t neurons;
        uint64_t synapses;
        double spikesPerSecond;
        double averageStepTime;
        double stepTimeStdDev;
        double stepTimeMin;
        double stepTimeMax;
        std::chrono::high_resolution_clock::time_point startTime;
    };
    
    SimulationMetrics getCurrentMetrics() const;
    
    // Real-time factor analysis
    double getRealTimeFactor() const;
    bool isSimulatingInRealTime() const;
    
    // Time-based events
    void onSimulationStart();
    void onSimulationStop();
    void onStepComplete(double stepTimeSeconds);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl_;
};

class PerformanceVisualizer {
public:
    PerformanceVisualizer();
    ~PerformanceVisualizer();
    
    // Generate various types of visualizations
    std::string generateTimeSeriesPlot(const PerformanceProfiler& profiler, const std::string& metricName) const;
    std::string generateComparisonChart(const std::vector<PerformanceProfiler>& profilers, const std::vector<std::string>& metricNames) const;
    std::string generateBottleneckAnalysis(const PerformanceProfiler& profiler) const;
    std::string generateMemoryHeatmap(const PerformanceProfiler& profiler) const;
    
    // Generate reports
    std::string generateHTMLReport(const PerformanceProfiler& profiler) const;
    std::string generateTextReport(const PerformanceProfiler& profiler) const;
    std::string generateJSONReport(const PerformanceProfiler& profiler) const;
    
    // Save visualizations
    bool saveChart(const std::string& filename, const std::string& chartType, const std::string& data) const;
    bool saveReport(const std::string& filename, const std::string& reportType, const std::string& content) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl_;
};

class PerformanceRegressionDetector {
public:
    PerformanceRegressionDetector();
    ~PerformanceRegressionDetector();
    
    // Load baseline performance data
    bool loadBaseline(const std::string& filename);
    
    // Compare current performance against baseline
    struct RegressionResult {
        bool hasRegression;
        std::vector<std::string> regressions;
        std::vector<std::string> improvements;
        double overallRegressionScore;
        std::unordered_map<std::string, double> metricChanges;
    };
    
    RegressionResult compareWithBaseline(const PerformanceProfiler& currentProfiler) const;
    
    // Statistical significance testing
    bool isStatisticallySignificant(double currentValue, double baselineValue, double baselineStdDev) const;
    
    // Generate regression reports
    std::string generateRegressionReport(const RegressionResult& result) const;
    std::string generateTrendAnalysis(const std::vector<double>& metricValues) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl_;
};

// Global performance monitoring interface
class GlobalPerformanceMonitor {
public:
    static GlobalPerformanceMonitor& getInstance();
    
    // Get the global profiler
    PerformanceProfiler& getProfiler();
    
    // Get individual monitors
    CPUMonitor& getCPUMonitor();
    MemoryMonitor& getMemoryMonitor();
    SimulationTimeMonitor& getSimulationTimeMonitor();
    
    // Control global monitoring
    void startAllMonitors();
    void stopAllMonitors();
    bool areAllMonitorsRunning() const;
    
    // Global performance aggregation
    BenchmarkMetrics getGlobalBenchmarkMetrics() const;
    std::string getPerformanceSummary() const;
    
private:
    GlobalPerformanceMonitor();
    ~GlobalPerformanceMonitor();
    
    struct Impl;
    std::unique_ptr<Impl> pImpl_;
};

// Convenience functions
inline void PERF_START() {
    GlobalPerformanceMonitor::getInstance().getProfiler().start();
}

inline void PERF_STOP() {
    GlobalPerformanceMonitor::getInstance().getProfiler().stop();
}

inline void PERF_RECORD(const std::string& name, double value) {
    GlobalPerformanceMonitor::getInstance().getProfiler().recordMetric(name, value);
}

inline void PERF_TIMER(const std::string& name, std::chrono::high_resolution_clock::time_point start) {
    auto end = std::chrono::high_resolution_clock::now();
    double duration = std::chrono::duration<double>(end - start).count();
    GlobalPerformanceMonitor::getInstance().getProfiler().recordTimer(name, duration);
}

inline std::chrono::high_resolution_clock::time_point PERF_TIMER_START(const std::string& name = "default") {
    GlobalPerformanceMonitor::getInstance().getProfiler().startComponentProfile(name);
    return std::chrono::high_resolution_clock::now();
}

inline void PERF_TIMER_END(const std::string& name = "default", const std::chrono::high_resolution_clock::time_point& start = std::chrono::high_resolution_clock::now()) {
    GlobalPerformanceMonitor::getInstance().getProfiler().stopComponentProfile(name);
}

inline void PERF_MEMORY_TRACK(size_t bytes, const std::string& source = "unknown") {
    GlobalPerformanceMonitor::getInstance().getMemoryMonitor().trackAllocation(bytes, source);
}

} // namespace nlm
