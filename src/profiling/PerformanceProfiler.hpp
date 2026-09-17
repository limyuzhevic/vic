#pragma once

/**
 * Enhanced PerformanceProfiler - Production-ready performance monitoring and profiling
 * 
 * Tracks simulation performance, resource utilization, and neural activity metrics
 * with configurable modes, bottleneck detection, and regression analysis.
 * 
 * Features:
 * - Non-invasive CPU/memory usage tracking (sampling, not intrusive)
 * - Neural activity monitoring and 2D heatmaps
 * - Simulation timeline profiling
 * - Resource allocation metrics with historical data storage
 * - Bottleneck identification and optimization suggestions
 * - Performance regression detection against baseline
 * - Real-time monitoring capabilities with configurable sampling
 * - Export functionality for analysis and reporting
 */

#include <chrono>
#include <cstdint>
#include <memory>
#include <mutex>
#include <string>
#include <vector>
#include <unordered_map>
#include <optional>
#include <thread>
#include <atomic>
#include <algorithm>
#include <numeric>
#include <deque>
#include <span>
#include <fstream>
#include <sstream>
#include <map>

namespace nlm {

// Forward declarations
class Brain;

// Progress reporting for long-running operations
struct ProgressReport {
    double progress;           // 0.0 to 1.0
    std::string message;       // Human-readable progress description
    int64_t currentStep;       // Current simulation step
    int64_t totalSteps;        // Total steps for current operation
    double estimatedTimeRemaining; // Estimated seconds remaining
    double progressPerSecond;  // Progress rate (steps/sec)
    
    ProgressReport(double p = 0.0, const std::string& msg = "",
                   int64_t current = 0, int64_t total = 0,
                   double eta = 0.0, double rate = 0.0)
        : progress(p), message(msg), currentStep(current), totalSteps(total),
          estimatedTimeRemaining(eta), progressPerSecond(rate) {}
};

// Performance metrics snapshot
struct PerformanceMetrics {
    // Resource usage
    double cpuUsage;                    // CPU usage percentage (0-100)
    uint64_t memoryUsed;                // Bytes of memory used
    uint64_t memoryPeak;               // Peak memory usage bytes
    double memoryUsagePercent;          // Memory usage percentage
    
    // Simulation metrics
    int64_t simulationStep;            // Current simulation step
    double simulationTime;              // Current simulation time (seconds)
    double timePerStep;                // Average time per simulation step
    double timePerStepStdDev;           // Standard deviation of time per step
    
    // Neural activity metrics
    uint64_t totalSpikes;              // Total spike count
    uint64_t activeNeurons;            // Number of active neurons
    uint64_t firingNeurons;            // Number of neurons firing this step
    double averageFiringRate;          // Average firing rate (Hz)
    double averageFiringRateStdDev;    // Standard deviation of firing rate
    
    // System metrics
    int64_t pendingSpikeCount;         // Number of pending spike events
    int64_t systemUptime;              // System uptime in seconds
    uint32_t threadCount;              // Active thread count
    
    // Memory system metrics
    uint64_t workingMemoryTraces;      // Active working memory traces
    uint64_t episodicMemorySize;       // Episodic memory storage size
    
    // Performance counters
    int64_t checkpointSaves;           // Number of checkpoint saves
    int64_t checkpointLoads;           // Number of checkpoint loads
    double totalCpuTime;               // Total CPU time accumulated
    double idleCpuTime;                // Total idle CPU time
    
    PerformanceMetrics()
        : cpuUsage(0.0), memoryUsed(0), memoryPeak(0), memoryUsagePercent(0.0),
          simulationStep(0), simulationTime(0.0), timePerStep(0.0),
          timePerStepStdDev(0.0), totalSpikes(0), activeNeurons(0), firingNeurons(0),
          averageFiringRate(0.0), averageFiringRateStdDev(0.0), pendingSpikeCount(0),
          systemUptime(0), threadCount(0), workingMemoryTraces(0), episodicMemorySize(0),
          checkpointSaves(0), checkpointLoads(0), totalCpuTime(0.0), idleCpuTime(0.0) {}
};

// Neural activity heatmaps
struct NeuralActivityHeatmap {
    int64_t step;                      // Step when heatmap was captured
    double timestamp;                   // Timestamp when captured
    std::vector<float> activityData;    // 2D flattened activity data (region x neuron)
    int regionCount;                    // Number of regions
    int neuronsPerRegion;              // Neurons per region (assumed uniform)
    double maxActivity;                // Maximum activity value
    double minActivity;                // Minimum activity value
    std::vector<float> normalizedData;   // Normalized activity data (0.0 to 1.0)
    double averageActivity;            // Average activity across all neurons
    
    NeuralActivityHeatmap(int regionCount = 0, int neuronsPerRegion = 0)
        : step(0), timestamp(0.0), regionCount(regionCount), 
          neuronsPerRegion(neuronsPerRegion), maxActivity(0.0), minActivity(0.0),
          averageActivity(0.0) {}
};

// Resource allocation metrics
struct ResourceMetrics {
    uint64_t timestamp;               // Measurement timestamp
    uint64_t cpuUserTime;             // CPU time in user mode (ns)
    uint64_t cpuSystemTime;           // CPU time in system mode (ns)
    uint64_t cpuTotalTime;            // Total CPU time (ns)
    uint64_t memoryAllocated;         // Memory allocated (bytes)
    uint64_t memoryFreed;            // Memory freed (bytes)
    size_t fileDescriptors;           // Open file descriptors
    uint64_t contextSwitches;         // Number of context switches
    
    ResourceMetrics() : timestamp(0), cpuUserTime(0), cpuSystemTime(0),
                       cpuTotalTime(0), memoryAllocated(0), memoryFreed(0),
                       fileDescriptors(0), contextSwitches(0) {}
};

// Time series data for metrics
struct TimeSeriesPoint {
    uint64_t timestamp;               // Unix timestamp (ns)
    PerformanceMetrics metrics;       // Metrics at this point
    
    TimeSeriesPoint(uint64_t ts = 0) : timestamp(ts) {}
};

// Bottleneck analysis result
struct BottleneckAnalysis {
    std::string type;                 // Type of bottleneck
    double severity;                  // Severity score (0.0 to 1.0)
    std::string location;             // Where bottleneck occurs
    std::string recommendation;       // Recommended fix
    
    BottleneckAnalysis(const std::string& t = "", double s = 0.0,
                       const std::string& loc = "", const std::string& rec = "")
        : type(t), severity(s), location(loc), recommendation(rec) {}
};

// Performance regression detection
struct PerformanceRegression {
    std::string metric;                // Metric name (e.g., "time_per_step", "cpu_usage")
    double baselineValue;              // Baseline value from reference run
    double currentValue;               // Current measured value
    double regressionPercent;          // Percentage change (positive = regression)
    std::string severity;              // "critical", "warning", "info"
    std::string recommendation;         // Recommended action
    
    PerformanceRegression(const std::string& metric = "", double baseline = 0.0,
                         double current = 0.0, double regression = 0.0,
                         const std::string& severity = "", const std::string& rec = "")
        : metric(metric), baselineValue(baseline), currentValue(current),
          regressionPercent(regression), severity(severity), recommendation(rec) {}
};

// Profiling modes for different use cases
enum class ProfilingMode {
    Realtime,        // Real-time monitoring with minimal overhead
    Recording,       // Full recording of metrics for analysis
    Benchmarking,    // Detailed performance benchmarking
    Production,      // Production monitoring with error detection
    Research         // Research-grade detailed profiling
};

class PerformanceProfiler {
public:
    PerformanceProfiler();
    ~PerformanceProfiler();
    
    // Configuration
    bool initialize();
    void shutdown();
    bool isInitialized() const;
    
    // Configuration methods
    void setProfilingMode(ProfilingMode mode);
    void setSamplingInterval(double intervalSeconds);
    void setMaxHistorySize(size_t size);
    void setEnableResourceMonitoring(bool enable);
    void setEnableNeuralActivityMonitoring(bool enable);
    void setEnableBottleneckDetection(bool enable);
    void setEnableRegressionDetection(bool enable);
    void setBaselineMetrics(const std::string& baselineFile);
    void setOutputDirectory(const std::string& outputDir);
    
    // Start/stop profiling
    void startProfiling();
    void stopProfiling();
    bool isProfiling() const;
    
    // Main profiling loop
    void update();
    
    // Metrics access
    PerformanceMetrics getCurrentMetrics() const;
    std::vector<PerformanceMetrics> getRecentMetrics(size_t count = 100) const;
    std::vector<TimeSeriesPoint> getTimeSeries(size_t count = 100) const;
    
    // Heatmap management
    void captureActivityHeatmap();
    std::vector<NeuralActivityHeatmap> getActivityHeatmaps(size_t count = 10) const;
    
    // Resource metrics
    ResourceMetrics getCurrentResourceMetrics() const;
    
    // Progress reporting
    void setProgress(double progress, const std::string& message = "",
                     int64_t currentStep = 0, int64_t totalSteps = 0);
    ProgressReport getCurrentProgress() const;
    
    // Event-driven metrics updates
    void onSpikeGenerated();
    void onNeuronActivated(NeuronId neuronId);
    void onCheckpointSaved(const std::string& filepath, uint64_t bytesWritten);
    void onCheckpointLoaded(const std::string& filepath);
    
    // Analysis methods
    BottleneckAnalysis analyzeBottlenecks() const;
    double calculatePerformanceScore() const;
    std::vector<PerformanceRegression> analyzePerformanceRegression() const;
    
    // Utilities
    double getUptime() const;
    void reset();
    
    // Progress callback for long-running operations
    using ProgressCallback = std::function<void(const ProgressReport&)>;
    void setProgressCallback(ProgressCallback callback);
    
    // Export functionality
    bool exportMetrics(const std::string& filename) const;
    bool exportHeatmaps(const std::string& filename) const;
    bool exportTimeSeries(const std::string& filename) const;
    bool exportAnalysisReport(const std::string& filename) const;
    
    // Data management
    bool saveSession(const std::string& sessionName);
    bool loadSession(const std::string& sessionName);
    std::vector<std::string> listSessions() const;
    bool deleteSession(const std::string& sessionName);
    
    // Real-time monitoring
    void enableRealTimeMonitoring(bool enable);
    bool isRealTimeMonitoringEnabled() const;
    
    // Performance benchmarking
    void startBenchmark(const std::string& benchmarkName);
    void endBenchmark();
    bool isBenchmarking() const;
    std::string getCurrentBenchmarkName() const;
    
    // Heatmap analysis
    std::vector<float> analyzeHeatmapRegion(int regionId) const;
    double calculateHeatmapActivityScore(int regionId) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Prevent copying
    PerformanceProfiler(const PerformanceProfiler&) = delete;
    PerformanceProfiler& operator=(const PerformanceProfiler&) = delete;
};

} // namespace nlm
