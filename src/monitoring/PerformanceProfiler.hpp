#pragma once

/**
 * PerformanceProfiler - Real-time performance monitoring and profiling
 * 
 * Tracks simulation performance, resource utilization, and neural activity metrics
 * for production monitoring and analysis.
 * 
 * Features:
 * - CPU and memory usage tracking
 * - Simulation timeline profiling
 * - Neural activity heatmaps
 * - Resource allocation metrics
 * - Bottleneck identification
 * - Progress reporting for long-running operations
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

namespace nlm {

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
    
    // Neural activity metrics
    uint64_t totalSpikes;              // Total spike count
    uint64_t activeNeurons;            // Number of active neurons
    uint64_t firingNeurons;            // Number of neurons firing this step
    double averageFiringRate;          // Average firing rate (Hz)
    
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
    
    PerformanceMetrics()
        : cpuUsage(0.0), memoryUsed(0), memoryPeak(0), memoryUsagePercent(0.0),
          simulationStep(0), simulationTime(0.0), timePerStep(0.0),
          totalSpikes(0), activeNeurons(0), firingNeurons(0), averageFiringRate(0.0),
          pendingSpikeCount(0), systemUptime(0), threadCount(0),
          workingMemoryTraces(0), episodicMemorySize(0),
          checkpointSaves(0), checkpointLoads(0), totalCpuTime(0.0) {}
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
    
    NeuralActivityHeatmap(int regionCount = 0, int neuronsPerRegion = 0)
        : step(0), timestamp(0.0), regionCount(regionCount), 
          neuronsPerRegion(neuronsPerRegion), maxActivity(0.0), minActivity(0.0) {}
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
    
    ResourceMetrics() : timestamp(0), cpuUserTime(0), cpuSystemTime(0),
                       cpuTotalTime(0), memoryAllocated(0), memoryFreed(0),
                       fileDescriptors(0) {}
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

class PerformanceProfiler {
public:
    PerformanceProfiler();
    ~PerformanceProfiler();
    
    // Configuration
    bool initialize();
    void shutdown();
    bool isInitialized() const;
    
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
    
    // Configuration
    void setSamplingInterval(double intervalSeconds);
    void setMaxHistorySize(size_t size);
    void setEnableResourceMonitoring(bool enable);
    
    // Utilities
    double getUptime() const;
    void reset();
    
    // Progress callback for long-running operations
    using ProgressCallback = std::function<void(const ProgressReport&)>;
    void setProgressCallback(ProgressCallback callback);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Prevent copying
    PerformanceProfiler(const PerformanceProfiler&) = delete;
    PerformanceProfiler& operator=(const PerformanceProfiler&) = delete;
};

} // namespace nlm