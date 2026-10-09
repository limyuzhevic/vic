#pragma once

#include "../core/Types/Types.hpp"
#include "../performance/Performance.hpp"
#include <chrono>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <algorithm>
#include <numeric>
#include <functional>
#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <fstream>

namespace nlm {

/**
 * Performance Profiling System for NLM Brain
 * 
 * Comprehensive performance monitoring and profiling for the NLM brain simulation.
 * Provides unified interface for all performance monitoring components,
 * real-time and historical performance data access, and performance analysis.
 */

class PerformanceProfiler;

/**
 * Performance monitor base class
 * Provides common functionality for all performance monitors
 */
class PerformanceMonitor {
public:
    enum class MonitorType {
        SpikeProcessing,
        PlasticityComputation,
        MemoryUsage,
        SimulationTiming,
        CPUGPUUsage,
        IOPerformance,
        Custom
    };

    enum class MetricType {
        TimeBased,
        ResourceBased,
        CountBased,
        Custom
    };

    struct Metric {
        std::string name;
        MetricType type;
        double value;
        std::string unit;
        Timestamp timestamp;
        SimulationStep step;
        std::map<std::string, std::string> tags;
    };

    struct Statistics {
        double min = 0.0;
        double max = 0.0;
        double average = 0.0;
        double median = 0.0;
        double stdDeviation = 0.0;
        double p95 = 0.0;
        double p99 = 0.0;
        uint64_t count = 0;
        uint64_t samples = 0;
    };

    virtual ~PerformanceMonitor() = default;

    virtual MonitorType getType() const = 0;
    virtual const std::string& getName() const = 0;
    virtual void startMonitoring() = 0;
    virtual void stopMonitoring() = 0;
    virtual void reset() = 0;
    virtual bool isMonitoring() const = 0;

    // Metric management
    virtual void recordMetric(const Metric& metric) = 0;
    virtual const std::vector<Metric>& getMetrics() const = 0;
    virtual const Statistics& getStatistics() const = 0;

    // Performance analysis
    virtual void analyzeBottlenecks(std::vector<std::string>& bottlenecks) const = 0;
    virtual void generateRecommendations(std::vector<std::string>& recommendations) const = 0;

    // Profiling hooks
    virtual void onBeginOperation(const std::string& operation) = 0;
    virtual void onEndOperation(const std::string& operation, double duration) = 0;

    // Data export
    virtual bool exportData(const std::string& filepath) const = 0;
    virtual bool importData(const std::string& filepath) = 0;
};

/**
 * Spike processing performance monitor
 * Monitors spike generation, queuing, and processing performance
 */
class SpikeProcessingMonitor : public PerformanceMonitor {
public:
    struct SpikeMetrics {
        uint64_t spikesGenerated = 0;
        uint64_t spikesProcessed = 0;
        uint64_t spikesDelayed = 0;
        uint64_t spikesDelivered = 0;
        double totalSpikeProcessingTime = 0.0;
        double totalSpikeQueueTime = 0.0;
        double totalSpikeDeliveryTime = 0.0;
        size_t maxQueueSize = 0;
        double averageSpikeRate = 0.0;
        std::map<NeuronId, uint64_t> neuronSpikeCounts;
    };

    SpikeProcessingMonitor();
    ~SpikeProcessingMonitor() override;

    MonitorType getType() const override { return MonitorType::SpikeProcessing; }
    const std::string& getName() const override { static const std::string name = "SpikeProcessingMonitor"; return name; }

    void startMonitoring() override;
    void stopMonitoring() override;
    void reset() override;
    bool isMonitoring() const override;

    void recordMetric(const Metric& metric) override;
    const std::vector<Metric>& getMetrics() const override;
    const Statistics& getStatistics() const override;

    void analyzeBottlenecks(std::vector<std::string>& bottlenecks) const override;
    void generateRecommendations(std::vector<std::string>& recommendations) const override;

    void onBeginOperation(const std::string& operation) override;
    void onEndOperation(const std::string& operation, double duration) override;

    // Spike-specific methods
    void recordSpikeGenerated(NeuronId neuron, Timestamp timestamp, SimulationStep step);
    void recordSpikeProcessed(NeuronId neuron, Timestamp timestamp, SimulationStep step, double processingTime);
    void recordSpikeDelayed(const DelayedSpikeEvent& event, double delayTime);
    void recordSpikeDelivered(NeuronId destination, Timestamp timestamp, SimulationStep step, double deliveryTime);

    const SpikeMetrics& getSpikeMetrics() const { return spikeMetrics_; }

    bool exportData(const std::string& filepath) const override;
    bool importData(const std::string& filepath) override;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl_;
    SpikeMetrics spikeMetrics_;
    std::vector<Metric> metrics_;
    mutable Statistics statistics_;
    bool monitoring_ = false;
    std::mutex mutex_;
    std::chrono::steady_clock::time_point startTime_;
};

/**
 * Plasticity computation performance monitor
 * Monitors synaptic weight updates and plasticity computations
 */
class PlasticityMonitor : public PerformanceMonitor {
public:
    struct PlasticityMetrics {
        uint64_t totalPlasticityEvents = 0;
        uint64_t hebbianUpdates = 0;
        uint64_t stdpUpdates = 0;
        uint64_t structuralChanges = 0;
        double totalPlasticityComputationTime = 0.0;
        double totalWeightUpdateTime = 0.0;
        double totalEligibilityTraceTime = 0.0;
        size_t maxActivePlasticityEvents = 0;
        double averagePlasticityRate = 0.0;
        std::map<std::string, uint64_t> ruleEventCounts;
        std::map<std::string, double> ruleComputationTimes;
    };

    PlasticityMonitor();
    ~PlasticityMonitor() override;

    MonitorType getType() const override { return MonitorType::PlasticityComputation; }
    const std::string& getName() const override { static const std::string name = "PlasticityMonitor"; return name; }

    void startMonitoring() override;
    void stopMonitoring() override;
    void reset() override;
    bool isMonitoring() const override;

    void recordMetric(const Metric& metric) override;
    const std::vector<Metric>& getMetrics() const override;
    const Statistics& getStatistics() const override;

    void analyzeBottlenecks(std::vector<std::string>& bottlenecks) const override;
    void generateRecommendations(std::vector<std::string>& recommendations) const override;

    void onBeginOperation(const std::string& operation) override;
    void onEndOperation(const std::string& operation, double duration) override;

    // Plasticity-specific methods
    void recordPlasticityEvent(const std::string& ruleType, NeuronId pre, NeuronId post, 
                              double weightChange, double computationTime, Timestamp timestamp);
    void recordWeightUpdate(SynapseId synapse, double delta, double updateTime);
    void recordEligibilityTraceUpdate(SynapseId synapse, double traceValue, double computationTime);

    const PlasticityMetrics& getPlasticityMetrics() const { return plasticityMetrics_; }

    bool exportData(const std::string& filepath) const override;
    bool importData(const std::string& filepath) override;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl_;
    PlasticityMetrics plasticityMetrics_;
    std::vector<Metric> metrics_;
    mutable Statistics statistics_;
    bool monitoring_ = false;
    std::mutex mutex_;
    std::chrono::steady_clock::time_point startTime_;
};

/**
 * Memory usage performance monitor
 * Monitors memory allocation, deallocation, and usage patterns
 */
class MemoryMonitor : public PerformanceMonitor {
public:
    struct MemoryMetrics {
        uint64_t totalAllocations = 0;
        uint64_t totalDeallocations = 0;
        uint64_t peakAllocations = 0;
        size_t currentMemoryUsage = 0;  // bytes
        size_t peakMemoryUsage = 0;      // bytes
        size_t totalMemoryAllocated = 0; // bytes
        size_t totalMemoryFreed = 0;     // bytes
        double averageAllocationSize = 0.0;
        uint32_t activeAllocationCount = 0;
        std::map<std::string, size_t> poolAllocations;
        std::vector<size_t> memoryUsageHistory;
        std::vector<double> allocationRateHistory;
    };

    MemoryMonitor();
    ~MemoryMonitor() override;

    MonitorType getType() const override { return MonitorType::MemoryUsage; }
    const std::string& getName() const override { static const std::string name = "MemoryMonitor"; return name; }

    void startMonitoring() override;
    void stopMonitoring() override;
    void reset() override;
    bool isMonitoring() const override;

    void recordMetric(const Metric& metric) override;
    const std::vector<Metric>& getMetrics() const override;
    const Statistics& getStatistics() const override;

    void analyzeBottlenecks(std::vector<std::string>& bottlenecks) const override;
    void generateRecommendations(std::vector<std::string>& recommendations) const override;

    void onBeginOperation(const std::string& operation) override;
    void onEndOperation(const std::string& operation, double duration) override;

    // Memory-specific methods
    void recordAllocation(size_t size, const std::string& pool = "default");
    void recordDeallocation(size_t size, const std::string& pool = "default");
    void recordMemoryUsage(size_t usage, Timestamp timestamp, SimulationStep step);

    const MemoryMetrics& getMemoryMetrics() const { return memoryMetrics_; }

    bool exportData(const std::string& filepath) const override;
    bool importData(const std::string& filepath) override;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl_;
    MemoryMetrics memoryMetrics_;
    std::vector<Metric> metrics_;
    mutable Statistics statistics_;
    bool monitoring_ = false;
    std::mutex mutex_;
    std::chrono::steady_clock::time_point startTime_;
};

/**
 * Simulation timing performance monitor
 * Monitors simulation step duration, frame timing, and real-time performance
 */
class SimulationMonitor : public PerformanceMonitor {
public:
    struct SimulationMetrics {
        uint64_t totalSteps = 0;
        double totalSimulationTime = 0.0;    // wall-clock time
        double totalComputationTime = 0.0;   // CPU computation time
        double simulationWallClockTime = 0.0; // real-time duration
        double maxStepTime = 0.0;
        double minStepTime = std::numeric_limits<double>::max();
        double averageStepTime = 0.0;
        double stdDeviationStepTime = 0.0;
        double targetFPS = 0.0;              // frames per second target
        double actualFPS = 0.0;              // actual frames per second
        double timeWarpFactor = 1.0;         // simulation time vs real time
        SimulationStep stepsBehind = 0;
        Timestamp currentSimulationTime = 0.0;
        Timestamp targetSimulationTime = 0.0;
        std::vector<double> stepTimeHistory;
        std::vector<double> realTimeHistory;
    };

    SimulationMonitor();
    ~SimulationMonitor() override;

    MonitorType getType() const override { return MonitorType::SimulationTiming; }
    const std::string& getName() const override { static const std::string name = "SimulationMonitor"; return name; }

    void startMonitoring() override;
    void stopMonitoring() override;
    void reset() override;
    bool isMonitoring() const override;

    void recordMetric(const Metric& metric) override;
    const std::vector<Metric>& getMetrics() const override;
    const Statistics& getStatistics() const override;

    void analyzeBottlenecks(std::vector<std::string>& bottlenecks) const override;
    void generateRecommendations(std::vector<std::string>& recommendations) const override;

    void onBeginOperation(const std::string& operation) override;
    void onEndOperation(const std::string& operation, double duration) override;

    // Simulation-specific methods
    void recordStep(SimulationStep step, Timestamp time, double wallClockTime, double computationTime);
    void recordRealTimeUpdate(double realTimeElapsed, double simulationTimeProgress);

    const SimulationMetrics& getSimulationMetrics() const { return simulationMetrics_; }

    bool exportData(const std::string& filepath) const override;
    bool importData(const std::string& filepath) override;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl_;
    SimulationMetrics simulationMetrics_;
    std::vector<Metric> metrics_;
    mutable Statistics statistics_;
    bool monitoring_ = false;
    std::mutex mutex_;
    std::chrono::steady_clock::time_point startTime_;
    std::chrono::steady_clock::time_point realTimeStart_;
};

/**
 * I/O performance monitor
 * Monitors file I/O, network I/O, and checkpoint operations
 */
class IOMonitor : public PerformanceMonitor {
public:
    struct IOMetrics {
        uint64_t readOperations = 0;
        uint64_t writeOperations = 0;
        uint64_t seekOperations = 0;
        uint64_t otherOperations = 0;
        uint64_t checkpointSaves = 0;
        uint64_t checkpointLoads = 0;
        double totalReadTime = 0.0;
        double totalWriteTime = 0.0;
        double totalSeekTime = 0.0;
        uint64_t totalBytesRead = 0;
        uint64_t totalBytesWritten = 0;
        double averageIOBandwidth = 0.0;
        double maxIOThroughput = 0.0;
        std::map<std::string, uint64_t> fileOperationCounts;
        std::vector<std::pair<std::string, uint64_t>> topFiles;
    };

    IOMonitor();
    ~IOMonitor() override;

    MonitorType getType() const override { return MonitorType::IOPerformance; }
    const std::string& getName() const override { static const std::string name = "IOMonitor"; return name; }

    void startMonitoring() override;
    void stopMonitoring() override;
    void reset() override;
    bool isMonitoring() const override;

    void recordMetric(const Metric& metric) override;
    const std::vector<Metric>& getMetrics() const override;
    const Statistics& getStatistics() const override;

    void analyzeBottlenecks(std::vector<std::string>& bottlenecks) const override;
    void generateRecommendations(std::vector<std::string>& recommendations) const override;

    void onBeginOperation(const std::string& operation) override;
    void onEndOperation(const std::string& operation, double duration) override;

    // I/O-specific methods
    void recordRead(const std::string& filepath, size_t bytes, double duration);
    void recordWrite(const std::string& filepath, size_t bytes, double duration);
    void recordCheckpoint(const std::string& filepath, bool isSave, double duration, uint64_t bytes);

    const IOMetrics& getIOMetrics() const { return ioMetrics_; }

    bool exportData(const std::string& filepath) const override;
    bool importData(const std::string& filepath) override;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl_;
    IOMetrics ioMetrics_;
    std::vector<Metric> metrics_;
    mutable Statistics statistics_;
    bool monitoring_ = false;
    std::mutex mutex_;
    std::chrono::steady_clock::time_point startTime_;
};

/**
 * Core PerformanceProfiler class
 * Main interface for performance profiling and monitoring
 */
class PerformanceProfiler {
public:
    // Profiling modes
    enum class Mode {
        Passive,      // Read-only monitoring
        Active,       // Real-time monitoring with control
        Deep,         // Detailed profiling with sampling
        Continuous,   // Continuous performance tracking
        Batch        // Batch performance analysis
    };

    // Alert levels
    enum class AlertLevel {
        Info,
        Warning,
        Error,
        Critical
    };

    struct PerformanceAlert {
        std::string id;
        std::string message;
        AlertLevel level;
        MonitorType source;
        Timestamp timestamp;
        SimulationStep step;
        std::map<std::string, std::string> metadata;
    };

    struct PerformanceReport {
        Timestamp timestamp;
        SimulationStep step;
        std::map<MonitorType, PerformanceMonitor::Statistics> monitorStatistics;
        std::vector<PerformanceAlert> alerts;
        std::vector<std::string> recommendations;
        double totalCpuUsage;
        double totalMemoryUsage;
        double totalSimulationTime;
        std::map<std::string, double> customMetrics;
    };

    PerformanceProfiler();
    ~PerformanceProfiler();

    // Disable copying, enable moving
    PerformanceProfiler(const PerformanceProfiler&) = delete;
    PerformanceProfiler& operator=(const PerformanceProfiler&) = delete;
    PerformanceProfiler(PerformanceProfiler&&) noexcept;
    PerformanceProfiler& operator=(PerformanceProfiler&&) noexcept;

    // Configuration
    void configure(Mode mode = Mode::Active);
    void setSamplingInterval(double interval); // seconds
    void setAlertThreshold(const std::string& metric, double threshold, AlertLevel level);
    void enableMonitor(MonitorType type, bool enabled = true);
    void setMonitorProvider(MonitorType type, std::function<std::unique_ptr<PerformanceMonitor>()> provider);

    // Monitoring control
    void startMonitoring();
    void stopMonitoring();
    void pauseMonitoring();
    void resumeMonitoring();
    bool isMonitoring() const;

    // Data recording
    void recordTimeMetric(const std::string& name, double value, const std::string& unit = "seconds",
                         const std::map<std::string, std::string>& tags = {});
    void recordResourceMetric(const std::string& name, double value, const std::string& unit = "bytes",
                            const std::map<std::string, std::string>& tags = {});
    void recordCountMetric(const std::string& name, uint64_t value, const std::string& unit = "count",
                         const std::map<std::string, std::string>& tags = {});

    // Integration with NLM systems
    void integrateWithSpikeSystem(const class SpikeSystem& spikeSystem);
    void integrateWithPlasticitySystem(const class PlasticityRule& plasticityRule);
    void integrateWithMemorySystem(const class Memory& memory);
    void integrateWithSimulationClock(const class SimulationClock& clock);
    void integrateWithCheckpointSystem(const class CheckpointManager& checkpointManager);

    // Profiling hooks
    void startProfile(const std::string& operation);
    void endProfile(const std::string& operation);

    // Performance queries
    PerformanceMonitor::Metric getLatestMetric(const std::string& name) const;
    PerformanceMonitor::Statistics getMetricStatistics(const std::string& name) const;
    const std::vector<PerformanceMonitor::Metric>& getAllMetrics() const;

    // Performance analysis
    void analyzeBottlenecks(std::vector<std::string>& bottlenecks) const;
    void analyzeTrends(std::map<std::string, std::vector<double>>& trends) const;
    void generateOptimizationRecommendations(std::vector<std::string>& recommendations) const;

    // Reporting
    PerformanceReport generateReport() const;
    bool exportReport(const std::string& filepath, const std::string& format = "json") const;
    bool importReport(const std::string& filepath);

    // Alert management
    void addAlert(const PerformanceAlert& alert);
    void clearAlerts();
    const std::vector<PerformanceAlert>& getAlerts() const;
    void processAlerts();

    // Performance profiling tools
    void startTimelineProfiling(const std::string& label);
    void endTimelineProfiling(const std::string& label);
    void startCallStackProfiling();
    void endCallStackProfiling();
    void startMemoryAllocationProfiling();
    void endMemoryAllocationProfiling();

    // Performance testing utilities
    void runPerformanceBenchmark(const std::string& testName, std::function<void()> testFunction);
    void recordPerformanceTestResult(const std::string& testName, double duration, bool success);

    // Performance visualization data
    struct VisualizationData {
        std::vector<Timestamp> timestamps;
        std::map<std::string, std::vector<double>> metrics;
        std::map<std::string, std::vector<std::string>> events;
    };

    VisualizationData getVisualizationData() const;

    // Performance snapshot
    struct PerformanceSnapshot {
        Timestamp timestamp;
        SimulationStep step;
        std::map<MonitorType, PerformanceMonitor::Statistics> monitorStats;
        double cpuUsage;
        double memoryUsage;
        uint64_t activeMonitors;
    };

    PerformanceSnapshot takeSnapshot() const;

    // Performance comparison
    struct PerformanceComparison {
        std::string baselineVersion;
        std::string currentVersion;
        std::map<std::string, double> improvementFactors;
        std::map<std::string, std::string> regressions;
        double overallPerformanceScore;
    };

    PerformanceComparison compareWithBaseline(const PerformanceSnapshot& baseline) const;

    // Configuration
    void loadConfiguration(const std::string& filepath);
    void saveConfiguration(const std::string& filepath) const;
    const std::string& getConfiguration() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl_;
};

/**
 * Performance data structures and utilities
 */
namespace perf {

// Performance metrics collection
class MetricsCollector {
public:
    struct MetricPoint {
        Timestamp timestamp;
        SimulationStep step;
        std::string name;
        double value;
        std::string unit;
        std::map<std::string, std::string> tags;
    };

    void addMetric(const MetricPoint& point);
    std::vector<MetricPoint> getMetrics(const std::string& name, Timestamp startTime, Timestamp endTime) const;
    double calculateAverage(const std::string& name, Timestamp startTime, Timestamp endTime) const;
    double calculateMax(const std::string& name, Timestamp startTime, Timestamp endTime) const;
    double calculateMin(const std::string& name, Timestamp startTime, Timestamp endTime) const;
    size_t getMetricCount(const std::string& name) const;

private:
    std::vector<MetricPoint> metrics_;
    mutable std::mutex mutex_;
};

// Performance statistics calculator
class StatisticsCalculator {
public:
    struct TimeWindow {
        Timestamp start;
        Timestamp end;
    };

    static double calculateMean(const std::vector<double>& values);
    static double calculateStandardDeviation(const std::vector<double>& values, double mean);
    static double calculateMedian(std::vector<double> values);
    static double calculatePercentile(const std::vector<double>& values, double percentile);
    static double calculateMovingAverage(const std::vector<double>& values, size_t windowSize);
    static std::pair<double, double> calculateConfidenceInterval(const std::vector<double>& values, double confidence = 0.95);
    static bool detectAnomalies(const std::vector<double>& values, double threshold = 2.0);
};

// Performance trend analyzer
class TrendAnalyzer {
public:
    struct Trend {
        std::string metricName;
        double slope;
        double rSquared;
        bool isSignificant;
        enum class Direction { Increasing, Decreasing, Stable } direction;
    };

    std::vector<Trend> analyzeTrends(const std::map<std::string, std::vector<double>>& timeSeriesData) const;
    double forecastNextValue(const std::string& metricName, const std::vector<double>& history, size_t periods = 1) const;
    std::vector<double> calculateMovingAverage(const std::vector<double>& data, size_t windowSize) const;
    std::vector<double> calculateExponentialMovingAverage(const std::vector<double>& data, double alpha) const;
};

} // namespace perf

} // namespace nlm
