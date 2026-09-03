#pragma once

/**
 * ScalingBenchmark - Automated neural scale experiments
 * 
 * Provides systematic benchmarking across different neural scales.
 * Measures performance, memory, and capability scaling.
 * 
 * Scale progression:
 * 10,000 neurons
 *       ↓
 * 100,000
 *       ↓
 * 1,000,000
 *       ↓
 * 10,000,000
 *       ↓
 * 100,000,000
 *       ↓
 * larger experimental scales
 * 
 * Features:
 * - Automated scale progression
 * - Performance metrics collection
 * - Memory profiling
 * - Learning curve tracking
 * - Multi-seed experiments
 * - Hardware utilization tracking
 * - Report generation
 */

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <chrono>
#include <optional>
#include <filesystem>

namespace nlm {

/**
 * Scale levels for benchmarking
 */
struct ScaleLevel {
    std::string name;
    size_t neurons;
    size_t expectedSynapses;  // Based on connectivity probability
    float connectionProbability;
    std::string hardwareTarget;  // "single_core", "multi_core", "gpu"
    std::string notes;
    
    static std::vector<ScaleLevel> getStandardLevels();
};

/**
 * Performance metrics for a benchmark run
 */
struct BenchmarkMetrics {
    // Simulation performance
    double neuronsPerSecond;
    double synapsesPerSecond;
    double spikesPerSecond;
    double synapticEventsPerSecond;
    double realTimeFactor;  // simulated_time / wall_clock_time
    
    // Step timing
    double avgStepTimeMs;
    double minStepTimeMs;
    double maxStepTimeMs;
    double stepTimeStdDev;
    
    // Memory usage
    size_t neuronMemoryMB;
    size_t synapseMemoryMB;
    size_t eventQueueMemoryMB;
    size_t totalMemoryMB;
    size_t peakMemoryMB;
    
    // Neural activity
    size_t activeNeurons;  // Avg active per step
    size_t firingNeurons;  // Avg firing per step
    double avgFiringRateHz;
    double spikeDensity;  // spikes / (neurons * steps)
    
    // Synaptic activity
    double synapticWeightMean;
    double synapticWeightStdDev;
    double excitatoryRatio;
    double connectionDensity;
    
    // Hardware utilization (if available)
    double cpuUtilizationPercent;
    double gpuUtilizationPercent;
    size_t threadCount;
    
    // Experiment results (if applicable)
    float taskPerformance;
    float learningAccuracy;
    float memoryRetention;
    
    BenchmarkMetrics();
    
    std::string toCSV() const;
    std::string toJSON() const;
};

/**
 * Single benchmark run result
 */
struct BenchmarkRun {
    std::string name;
    ScaleLevel scale;
    uint64_t randomSeed;
    uint64_t stepsRun;
    double wallClockTime;
    double simulatedTime;
    
    BenchmarkMetrics metrics;
    
    // Hardware info
    std::string cpuModel;
    std::string gpuModel;
    size_t cpuCores;
    size_t ramGB;
    size_t vramGB;
    
    // Configuration
    std::unordered_map<std::string, std::string> config;
    
    // Timestamps
    time_t startTime;
    time_t endTime;
    
    BenchmarkRun();
    
    std::string toJSON() const;
};

/**
 * Scaling analysis result
 */
struct ScalingAnalysis {
    std::vector<BenchmarkRun> runs;
    
    // Scaling efficiency (how well does it scale)
    double scalingEfficiency;  // 1.0 = perfect linear scaling
    double parallelEfficiency;  // For multi-threaded runs
    
    // Bottleneck analysis
    std::string identifiedBottleneck;  // "memory", "compute", "communication", "unknown"
    double bottleneckSeverity;  // 0.0 - 1.0
    
    // Recommendations
    std::vector<std::string> recommendations;
    
    // Projections
    double projectedPerformance10x;
    double projectedPerformance100x;
    
    ScalingAnalysis();
};

/**
 * Benchmark configuration
 */
struct BenchmarkConfig {
    std::string name;
    std::vector<ScaleLevel> scales;
    std::vector<uint64_t> seeds;
    uint64_t stepsPerScale;
    uint64_t warmupSteps;
    bool runUntilStable;  // Run until metrics stabilize
    double stabilityThreshold;
    size_t maxStabilityChecks;
    
    // Optimization flags
    bool enableMultithreading;
    bool enableSIMD;
    bool enableSparseConnectivity;
    bool enableEventDriven;
    size_t threadCount;
    
    // Output
    std::string outputDir;
    bool generatePlots;
    bool generateReport;
    
    BenchmarkConfig();
};

/**
 * Scaling benchmark runner
 */
class ScalingBenchmark {
public:
    ScalingBenchmark();
    ~ScalingBenchmark();
    
    /**
     * Run a complete scaling benchmark
     * @param config Benchmark configuration
     * @param createBrain Function to create brain for a given scale
     * @param runSimulation Function to run simulation: bool(brain, steps)
     * @param collectMetrics Function to collect metrics: BenchmarkMetrics()
     * @return Analysis results
     */
    ScalingAnalysis run(
        const BenchmarkConfig& config,
        std::function<std::shared_ptr<class Brain>(const ScaleLevel& scale)> createBrain,
        std::function<bool(class Brain* brain, uint64_t steps)> runSimulation,
        std::function<BenchmarkMetrics(class Brain* brain)> collectMetrics
    );
    
    /**
     * Run a single scale benchmark
     */
    BenchmarkRun runSingleScale(
        const ScaleLevel& scale,
        uint64_t seed,
        const BenchmarkConfig& config,
        std::function<std::shared_ptr<class Brain>(const ScaleLevel& scale)> createBrain,
        std::function<bool(class Brain* brain, uint64_t steps)> runSimulation,
        std::function<BenchmarkMetrics(class Brain* brain)> collectMetrics
    );
    
    /**
     * Analyze scaling results
     */
    ScalingAnalysis analyze(
        const std::vector<BenchmarkRun>& runs,
        const BenchmarkConfig& config
    );
    
    /**
     * Generate report
     */
    std::string generateReport(const ScalingAnalysis& analysis);
    
    /**
     * Save results to directory
     */
    bool saveResults(const std::string& dir, const std::vector<BenchmarkRun>& runs);
    
    /**
     * Load results from directory
     */
    std::vector<BenchmarkRun> loadResults(const std::string& dir);

private:
    std::string getOutputPath(const std::string& dir, const std::string& filename);
    std::string getHardwareInfo();
};

/**
 * Real-time performance monitor
 */
class PerformanceMonitor {
public:
    PerformanceMonitor();
    ~PerformanceMonitor();
    
    /**
     * Start monitoring
     */
    void start();
    
    /**
     * Stop monitoring
     */
    void stop();
    
    /**
     * Record a simulation step
     */
    void recordStep(double stepTimeMs, size_t spikes, size_t activeNeurons);
    
    /**
     * Get current metrics (averaged over window)
     */
    BenchmarkMetrics getCurrentMetrics() const;
    
    /**
     * Get metrics snapshot
     */
    BenchmarkMetrics getSnapshot() const;
    
    /**
     * Check if performance is stable
     */
    bool isStable(size_t windowSize = 100, double threshold = 0.1) const;
    
    /**
     * Reset counters
     */
    void reset();
    
    /**
     * Get step times
     */
    const std::vector<double>& getStepTimes() const { return stepTimes_; }
    
    /**
     * Get spike counts
     */
    const std::vector<size_t>& getSpikeCounts() const { return spikeCounts_; }

private:
    bool running_;
    std::chrono::high_resolution_clock::time_point startTime_;
    
    std::vector<double> stepTimes_;
    std::vector<size_t> spikeCounts_;
    std::vector<size_t> activeNeuronCounts_;
    
    // Running averages
    double avgStepTimeMs_;
    double avgSpikesPerStep_;
    double avgActiveNeurons_;
    
    // Counters
    uint64_t totalSteps_;
    uint64_t totalSpikes_;
};

/**
 * Memory profiler
 */
class MemoryProfiler {
public:
    MemoryProfiler();
    ~MemoryProfiler();
    
    /**
     * Record current memory usage
     */
    void record();
    
    /**
     * Get peak memory usage
     */
    size_t getPeakMemoryMB() const { return peakMemoryMB_; }
    
    /**
     * Get current memory usage
     */
    size_t getCurrentMemoryMB() const;
    
    /**
     * Get memory breakdown
     */
    struct MemoryBreakdown {
        size_t neuronsMB;
        size_t synapsesMB;
        size_t eventsMB;
        size_t otherMB;
    };
    
    MemoryBreakdown getBreakdown() const;
    
    /**
     * Reset peak tracking
     */
    void resetPeak() { peakMemoryMB_ = 0; }

private:
    size_t peakMemoryMB_;
    std::vector<size_t> samples_;
};

// Inline implementations

inline BenchmarkMetrics::BenchmarkMetrics()
    : neuronsPerSecond(0.0)
    , synapsesPerSecond(0.0)
    , spikesPerSecond(0.0)
    , synapticEventsPerSecond(0.0)
    , realTimeFactor(0.0)
    , avgStepTimeMs(0.0)
    , minStepTimeMs(0.0)
    , maxStepTimeMs(0.0)
    , stepTimeStdDev(0.0)
    , neuronMemoryMB(0)
    , synapseMemoryMB(0)
    , eventQueueMemoryMB(0)
    , totalMemoryMB(0)
    , peakMemoryMB(0)
    , activeNeurons(0)
    , firingNeurons(0)
    , avgFiringRateHz(0.0)
    , spikeDensity(0.0)
    , synapticWeightMean(0.0)
    , synapticWeightStdDev(0.0)
    , excitatoryRatio(0.5)
    , connectionDensity(0.0)
    , cpuUtilizationPercent(0.0)
    , gpuUtilizationPercent(0.0)
    , threadCount(1)
    , taskPerformance(0.0)
    , learningAccuracy(0.0)
    , memoryRetention(0.0) {}

inline std::string BenchmarkMetrics::toCSV() const {
    return std::to_string(neuronsPerSecond) + "," +
           std::to_string(synapsesPerSecond) + "," +
           std::to_string(spikesPerSecond) + "," +
           std::to_string(realTimeFactor) + "," +
           std::to_string(avgStepTimeMs) + "," +
           std::to_string(totalMemoryMB) + "," +
           std::to_string(peakMemoryMB) + "," +
           std::to_string(activeNeurons) + "," +
           std::to_string(firingNeurons) + "," +
           std::to_string(avgFiringRateHz) + "," +
           std::to_string(synapticWeightMean) + "," +
           std::to_string(excitatoryRatio) + "," +
           std::to_string(cpuUtilizationPercent) + "," +
           std::to_string(threadCount);
}

inline BenchmarkRun::BenchmarkRun()
    : randomSeed(0)
    , stepsRun(0)
    , wallClockTime(0.0)
    , simulatedTime(0.0)
    , cpuCores(0)
    , ramGB(0)
    , vramGB(0)
    , startTime(0)
    , endTime(0) {}

inline ScalingAnalysis::ScalingAnalysis()
    : scalingEfficiency(0.0)
    , parallelEfficiency(0.0)
    , bottleneckSeverity(0.0) {}

inline BenchmarkConfig::BenchmarkConfig()
    : stepsPerScale(10000)
    , warmupSteps(1000)
    , runUntilStable(false)
    , stabilityThreshold(0.05)
    , maxStabilityChecks(100)
    , enableMultithreading(true)
    , enableSIMD(true)
    , enableSparseConnectivity(true)
    , enableEventDriven(true)
    , threadCount(0)  // 0 = auto-detect
    , generatePlots(true)
    , generateReport(true) {}

} // namespace nlm