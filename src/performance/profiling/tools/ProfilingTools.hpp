#pragma once

#include "../PerformanceProfiler.hpp"
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <cmath>

namespace nlm {
namespace profiling {

/**
 * Timeline Profiler Tool
 * Provides detailed timeline-based profiling for NLM brain simulations
 */
class TimelineProfiler {
public:
    struct TimelineEvent {
        std::string label;
        std::string category;
        double startTime;
        double duration;
        double cumulativeTime;
        uint64_t callCount;
        std::string threadId;
        std::map<std::string, std::string> metadata;
    };

    TimelineProfiler() = default;
    ~TimelineProfiler() = default;

    // Start/stop profiling
    void startProfiling();
    void stopProfiling();
    bool isProfiling() const;

    // Record timeline events
    void recordEvent(const std::string& label, const std::string& category,
                    double startTime, double duration, uint64_t callCount = 1);
    void recordStackTrace(const std::string& label, const std::string& category,
                         double startTime, double duration);

    // Get timeline data
    const std::vector<TimelineEvent>& getEvents() const { return events_; }
    std::vector<TimelineEvent> getEventsByCategory(const std::string& category) const;
    std::vector<TimelineEvent> getEventsByLabel(const std::string& label) const;

    // Analysis
    double getTotalTime() const;
    double getAverageEventDuration(const std::string& label) const;
    std::vector<std::string> findLongestRunningEvents(size_t limit = 10) const;
    std::vector<std::string> findMostFrequentEvents(size_t limit = 10) const;

    // Export
    bool exportToJSON(const std::string& filepath) const;
    bool exportToCSV(const std::string& filepath) const;
    bool exportToTimelineHTML(const std::string& filepath) const;

    // Integration with PerformanceProfiler
    void integrateWithProfiler(PerformanceProfiler& profiler);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl_;
    std::vector<TimelineEvent> events_;
    bool profiling_ = false;
    double totalProfilingTime_ = 0.0;
};

/**
 * Call Stack Profiler Tool
 * Captures and analyzes call stack information for performance debugging
 */
class CallStackProfiler {
public:
    struct CallStackSample {
        std::vector<std::string> stackFrames;
        double timestamp;
        uint64_t threadId;
        std::string operation;
        std::map<std::string, std::string> metadata;
    };

    CallStackProfiler() = default;
    ~CallStackProfiler() = default;

    // Control
    void startProfiling();
    void stopProfiling();
    bool isProfiling() const;

    // Stack capture
    void captureStack(const std::string& operation = "");
    void setMaxStackDepth(size_t depth);
    void setSamplingInterval(double interval); // seconds

    // Analysis
    std::vector<CallStackSample> getMostCommonStacks(size_t limit = 10) const;
    std::vector<std::string> getTopCallers(size_t limit = 10) const;
    double calculateTimeInFunction(const std::string& functionName) const;
    std::vector<std::string> findCallStackPatterns(const std::string& pattern) const;

    // Exporting
    bool exportToProfileFile(const std::string& filepath) const;
    bool exportToCallGraph(const std::string& filepath) const;

    // Integration
    void integrateWithProfiler(PerformanceProfiler& profiler);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl_;
    std::vector<CallStackSample> stackSamples_;
    bool profiling_ = false;
    size_t maxStackDepth_ = 50;
    double samplingInterval_ = 0.1; // 100ms default
};

/**
 * Memory Allocation Profiler Tool
 * Tracks memory allocation patterns and identifies leaks
 */
class MemoryAllocationProfiler {
public:
    struct MemoryAllocation {
        void* address;
        size_t size;
        std::string allocationSite;
        std::string allocationFunction;
        int allocationLine;
        double timestamp;
        uint64_t threadId;
        std::string allocationType;
        std::map<std::string, std::string> metadata;
    };

    struct MemoryDeallocation {
        void* address;
        std::string deallocationSite;
        std::string deallocationFunction;
        int deallocationLine;
        double timestamp;
        uint64_t threadId;
        std::map<std::string, std::string> metadata;
    };

    MemoryAllocationProfiler() = default;
    ~MemoryAllocationProfiler() = default;

    // Control
    void startProfiling();
    void stopProfiling();
    bool isProfiling() const;

    // Memory tracking
    void trackAllocation(void* address, size_t size, const std::string& site,
                        const std::string& function, int line);
    void trackDeallocation(void* address, const std::string& site,
                         const std::string& function, int line);

    // Analysis
    size_t getCurrentMemoryUsage() const;
    size_t getTotalMemoryAllocated() const;
    size_t getMemoryLeakSize() const;
    std::vector<MemoryAllocation> findLeaks() const;
    std::vector<std::string> getLargestAllocations(size_t limit = 10) const;
    double calculateAllocationFrequency(const std::string& function) const;

    // Exporting
    bool exportAllocationReport(const std::string& filepath) const;
    bool exportLeakReport(const std::string& filepath) const;

    // Integration
    void integrateWithProfiler(PerformanceProfiler& profiler);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl_;
    std::vector<MemoryAllocation> allocations_;
    std::vector<MemoryDeallocation> deallocations_;
    bool profiling_ = false;
    size_t currentMemoryUsage_ = 0;
};

/**
 * Performance Benchmarking Tool
 * Provides utilities for performance testing and benchmarking
 */
class PerformanceBenchmarker {
public:
    struct BenchmarkResult {
        std::string testName;
        double duration;
        bool success;
        double memoryUsage;
        double cpuUsage;
        std::map<std::string, double> customMetrics;
        Timestamp timestamp;
        SimulationStep step;
        std::string errorMessage;
    };

    PerformanceBenchmarker() = default;
    ~PerformanceBenchmarker() = default;

    // Benchmarking
    void runBenchmark(const std::string& name, std::function<void()> testFunction);
    void runBenchmark(const std::string& name, std::function<void()> testFunction,
                     const std::map<std::string, std::string>& tags);
    BenchmarkResult getLastResult(const std::string& name) const;
    std::vector<BenchmarkResult> getAllResults(const std::string& name) const;

    // Performance testing utilities
    template<typename Func>
    double measureExecutionTime(Func&& func);

    template<typename Func>
    double measureMemoryUsage(Func&& func);

    template<typename Func>
    double measureCpuUsage(Func&& func);

    // Analysis
    double getAverageDuration(const std::string& name) const;
    double getMinDuration(const std::string& name) const;
    double getMaxDuration(const std::string& name) const;
    double getStandardDeviation(const std::string& name) const;
    std::vector<std::string> findPerformanceRegressions(const std::string& baselineName,
                                                         const std::string& currentName,
                                                         double threshold = 0.1) const;

    // Exporting
    bool exportBenchmarkReport(const std::string& filepath) const;
    bool exportPerformanceTrends(const std::string& filepath) const;

    // Integration
    void integrateWithProfiler(PerformanceProfiler& profiler);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl_;
    std::vector<BenchmarkResult> benchmarkResults_;
};

/**
 * Performance Alert System
 * Provides real-time performance alerting and notification system
 */
class PerformanceAlertSystem {
public:
    enum class AlertType {
        PerformanceRegression,
        ResourceExhaustion,
        BottleneckDetected,
        MemoryLeak,
        HighCPUUsage,
        LowFPS,
        Custom
    };

    struct Alert {
        std::string id;
        AlertType type;
        std::string message;
        double severity;
        Timestamp timestamp;
        SimulationStep step;
        std::map<std::string, double> metrics;
        std::map<std::string, std::string> metadata;
        bool isActive;
    };

    PerformanceAlertSystem() = default;
    ~PerformanceAlertSystem() = default;

    // Alert configuration
    void addAlertRule(const std::string& name, AlertType type, double threshold,
                     const std::string& message, const std::string& severity = "warning");
    void removeAlertRule(const std::string& name);
    void clearAlertRules();

    // Alert evaluation
    void evaluateAlerts(const PerformanceProfiler::PerformanceSnapshot& snapshot);
    void addAlert(const Alert& alert);
    void acknowledgeAlert(const std::string& alertId);
    void resolveAlert(const std::string& alertId);

    // Alert management
    const std::vector<Alert>& getActiveAlerts() const;
    const std::vector<Alert>& getAllAlerts() const;
    size_t getActiveAlertCount() const;

    // Notification
    void setNotificationCallback(std::function<void(const Alert&)>
                                callback);
    void setAlertLevel(AlertType type, const std::string& level);
    std::string getAlertLevel(AlertType type) const;

    // Exporting
    bool exportAlertReport(const std::string& filepath) const;

    // Integration
    void integrateWithProfiler(PerformanceProfiler& profiler);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl_;
    std::vector<Alert> alerts_;
};

/**
 * Performance Visualization Tool
 * Provides performance data visualization and reporting
 */
class PerformanceVisualizer {
public:
    struct ChartData {
        std::string chartType; // "line", "bar", "heatmap", "scatter"
        std::string title;
        std::vector<Timestamp> timestamps;
        std::map<std::string, std::vector<double>> datasets;
        std::map<std::string, std::string> options;
    };

    PerformanceVisualizer() = default;
    ~PerformanceVisualizer() = default;

    // Chart creation
    ChartData createTimeSeriesChart(const std::string& title,
                                   const std::vector<Timestamp>& timestamps,
                                   const std::map<std::string, std::vector<double>>& datasets,
                                   const std::map<std::string, std::string>& options = {});

    ChartData createBarChart(const std::string& title,
                           const std::map<std::string, double>& data,
                           const std::map<std::string, std::string>& options = {});

    ChartData createHeatmapChart(const std::string& title,
                                const std::vector<std::vector<double>>& data,
                                const std::map<std::string, std::string>& options = {});

    // Exporting
    bool exportChart(const ChartData& chart, const std::string& filepath) const;
    bool exportDashboard(const std::string& filepath) const;
    bool exportHTMLReport(const std::string& filepath) const;

    // Analysis visualization
    bool generatePerformanceTrendsReport(const std::string& filepath,
                                         const PerformanceProfiler::PerformanceSnapshot& snapshot) const;
    bool generateBottleneckReport(const std::string& filepath,
                                  const std::vector<std::string>& bottlenecks) const;
    bool generateOptimizationReport(const std::string& filepath,
                                   const std::vector<std::string>& recommendations) const;

    // Integration
    void integrateWithProfiler(PerformanceProfiler& profiler);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl_;
};

/**
 * Performance Optimizer
 * Provides automated performance optimization recommendations
 */
class PerformanceOptimizer {
public:
    struct OptimizationRecommendation {
        std::string id;
        std::string category; // "memory", "cpu", "i/o", "algorithm"
        std::string description;
        double expectedImprovement;
        double implementationComplexity; // 0.0 to 1.0
        double riskLevel; // 0.0 to 1.0
        std::map<std::string, std::string> details;
        std::vector<std::string> implementationSteps;
    };

    PerformanceOptimizer() = default;
    ~PerformanceOptimizer() = default;

    // Optimization analysis
    std::vector<OptimizationRecommendation> analyzeBottlenecks(
        const PerformanceProfiler::PerformanceSnapshot& snapshot) const;

    std::vector<OptimizationRecommendation> analyzeResourceUsage(
        const PerformanceProfiler::PerformanceSnapshot& snapshot) const;

    std::vector<OptimizationRecommendation> analyzeScalability(
        const PerformanceProfiler::PerformanceSnapshot& baselineSnapshot,
        const PerformanceProfiler::PerformanceSnapshot& currentSnapshot) const;

    // Integration
    void integrateWithProfiler(PerformanceProfiler& profiler);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl_;
};

} // namespace profiling
} // namespace nlm
