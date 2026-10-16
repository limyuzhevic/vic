// Performance Monitoring Framework
// Provides comprehensive performance monitoring and profiling for NLM
// Integrates with existing Phase 5 performance infrastructure

#pragma once

#include <chrono>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <atomic>
#include <thread>
#include <mutex>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <cstdint>

#include "performance/Performance.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "core/Logger/Logger.hpp"

namespace nlm {

/**
 * Performance Metric Types
 */
enum class PerformanceMetric {
    NEURONS_PER_SECOND,
    SPIKES_PER_SECOND,
    SYNAPSE_OPERATIONS_PER_SECOND,
    MEMORY_USAGE_BYTES,
    MEMORY_ALLOCATIONS,
    SIMD_EFFICIENCY,
    THREAD_UTILIZATION,
    CACHE_HIT_RATE,
    FLUSH_REDUCTION,
    NUMERICAL_STABILITY,
    CPU_USAGE,
    SIMULATION_WALL_TIME,
    REAL_TIME_FACTOR,
    NUM_METRICS  // Last entry for counting
};

/**
 * Performance Sample - A single measurement point
 */
struct PerformanceSample {
    std::chrono::steady_clock::time_point timestamp;
    std::chrono::duration<double> simulationTime;
    uint64_t neurons;
    uint64_t synapses;
    uint64_t spikes;
    double cpuUsage;
    double memoryUsage;
    uint64_t memoryAllocations;
    uint64_t memoryDeallocations;
    uint64_t simdOperations;
    uint64_t cacheMisses;
    uint64_t flushReductions;
    double numericalError;
    double stabilityScore;
    uint64_t activeThreads;
    uint64_t pendingTasks;
    
    PerformanceSample()
        : timestamp(std::chrono::steady_clock::now())
        , simulationTime(0.0)
        , neurons(0)
        , synapses(0)
        , spikes(0)
        , cpuUsage(0.0)
        , memoryUsage(0.0)
        , memoryAllocations(0)
        , memoryDeallocations(0)
        , simdOperations(0)
        , cacheMisses(0)
        , flushReductions(0)
        , numericalError(0.0)
        , stabilityScore(1.0)
        , activeThreads(0)
        , pendingTasks(0) {}
};

/**
 * Performance Statistics - Statistical analysis of samples
 */
struct PerformanceStats {
    double mean;
    double median;
    double stddev;
    double min;
    double max;
    double p95;  // 95th percentile
    double p99;  // 99th percentile
    uint64_t count;
    
    PerformanceStats() : mean(0.0), median(0.0), stddev(0.0), min(0.0), max(0.0), 
                         p95(0.0), p99(0.0), count(0) {}
};

/**
 * Performance Monitor - Core performance monitoring system
 */
class PerformanceMonitor {
public:
    PerformanceMonitor();
    ~PerformanceMonitor();
    
    // Disable copying
    PerformanceMonitor(const PerformanceMonitor&) = delete;
    PerformanceMonitor& operator=(const PerformanceMonitor&) = delete;
    
    /**
     * Start performance monitoring
     */
    void startMonitoring();
    
    /**
     * Stop performance monitoring
     */
    void stopMonitoring();
    
    /**
     * Record a performance sample
     */
    void recordSample(const PerformanceSample& sample);
    
    /**
     * Get current performance metrics
     */
    double getMetric(PerformanceMetric metric) const;
    
    /**
     * Get performance statistics for a metric
     */
    PerformanceStats getStatistics(PerformanceMetric metric) const;
    
    /**
     * Get recent samples
     */
    std::vector<PerformanceSample> getRecentSamples(size_t count = 100) const;
    
    /**
     * Calculate real-time metrics
     */
    void calculateRealTimeMetrics();
    
    /**
     * Generate performance report
     */
    std::string generateReport() const;
    
    /**
     * Save performance data to file
     */
    bool saveToFile(const std::string& filename) const;
    
    /**
     * Load performance data from file
     */
    bool loadFromFile(const std::string& filename);
    
    /**
     * Check for performance regressions
     */
    std::vector<std::string> checkRegressions(const std::string& baselineFile) const;
    
    /**
     * Enable/disable specific metrics
     */
    void enableMetric(PerformanceMetric metric, bool enable = true);
    
    /**
     * Get monitoring status
     */
    bool isMonitoring() const { return monitoring_; }
    
    /**
     * Get total number of samples collected
     */
    size_t getSampleCount() const { return samples_.size(); }
    
    /**
     * Clear all collected data
     */
    void clear();
    
    /**
     * Get the singleton instance
     */
    static PerformanceMonitor& getInstance() {
        static PerformanceMonitor instance;
        return instance;
    }
    
private:
    bool monitoring_;
    mutable std::mutex mutex_;
    std::vector<PerformanceSample> samples_;
    std::chrono::steady_clock::time_point startTime_;
    uint64_t totalSamples_;
    
    // Atomic counters for metrics
    std::atomic<uint64_t> totalNeurons_{0};
    std::atomic<uint64_t> totalSynapses_{0};
    std::atomic<uint64_t> totalSpikes_{0};
    std::atomic<uint64_t> memoryAllocations_{0};
    std::atomic<uint64_t> memoryDeallocations_{0};
    std::atomic<uint64_t> simdOperations_{0};
    std::atomic<uint64_t> cacheMisses_{0};
    std::atomic<uint64_t> flushReductions_{0};
    std::atomic<double> numericalError_{0.0};
    std::atomic<uint64_t> activeThreads_{0};
    std::atomic<uint64_t> pendingTasks_{0};
    
    // Sampling thread
    std::thread samplingThread_;
    bool stopSampling_;
    std::chrono::milliseconds samplingInterval_;
    
    // Metric enable/disable flags
    std::unordered_map<PerformanceMetric, bool> metricEnabled_;
    
    void samplingThreadFunc();
    double calculateMean(const std::vector<double>& values) const;
    double calculateStddev(const std::vector<double>& values, double mean) const;
    double calculatePercentile(const std::vector<double>& values, double percentile) const;
};

/**
 * Numerical Stability Monitor - Checks for numerical stability issues
 */
class NumericalStabilityMonitor {
public:
    NumericalStabilityMonitor();
    ~NumericalStabilityMonitor() = default;
    
    /**
     * Check for potential numerical stability issues
     */
    bool checkStability(float value, const std::string& operation = "unknown") const;
    
    /**
     * Record numerical error
     */
    void recordError(double error, const std::string& context = "");
    
    /**
     * Get current stability score
     */
    double getStabilityScore() const { return stabilityScore_.load(); }
    
    /**
     * Get numerical error statistics
     */
    struct ErrorStats {
        double mean;
        double max;
        uint64_t count;
    };
    
    ErrorStats getErrorStats() const { return errorStats_; }
    
    /**
     * Check for NaN or Inf values
     */
    static bool isFinite(double value) {
        return std::isfinite(value);
    }
    
    static bool isFinite(float value) {
        return std::isfinite(static_cast<double>(value));
    }
    
    /**
     * Clamp values to safe range
     */
    static float safeClamp(float value, float min, float max) {
        if (std::isnan(value) || std::isinf(value)) {
            return 0.0f;
        }
        return std::clamp(value, min, max);
    }
    
    /**
     * Check for overflow/underflow
     */
    static bool checkOverflow(float a, float b, float result) {
        return std::isinf(result) || std::isnan(result);
    }
    
    /**
     * Get singleton instance
     */
    static NumericalStabilityMonitor& getInstance() {
        static NumericalStabilityMonitor instance;
        return instance;
    }
    
private:
    mutable std::mutex mutex_;
    std::atomic<double> stabilityScore_;
    ErrorStats errorStats_;
    std::vector<double> recentErrors_;
    uint64_t maxErrorsToStore_;
    
    static constexpr double DEFAULT_STABILITY_THRESHOLD = 1e-6;
    static constexpr uint64_t DEFAULT_MAX_ERRORS = 1000;
};

/**
 * Performance Profiler - Time and profile code sections
 */
class PerformanceProfiler {
public:
    PerformanceProfiler();
    ~PerformanceProfiler() = default;
    
    /**
     * Start profiling a code section
     */
    void startProfile(const std::string& name);
    
    /**
     * Stop profiling a code section
     */
    void stopProfile(const std::string& name);
    
    /**
     * Get total time for a section
     */
    double getTotalTime(const std::string& name) const;
    
    /**
     * Get call count for a section
     */
    uint64_t getCallCount(const std::string& name) const;
    
    /**
     * Get average time per call
     */
    double getAverageTime(const std::string& name) const;
    
    /**
     * Generate profiling report
     */
    std::string generateReport() const;
    
    /**
     * Clear all profiling data
     */
    void clear();
    
    /**
     * Get singleton instance
     */
    static PerformanceProfiler& getInstance() {
        static PerformanceProfiler instance;
        return instance;
    }
    
private:
    struct ProfileData {
        double totalTime;
        uint64_t callCount;
        double minTime;
        double maxTime;
        
        ProfileData() : totalTime(0.0), callCount(0), minTime(0.0), maxTime(0.0) {}
    };
    
    mutable std::mutex mutex_;
    std::unordered_map<std::string, ProfileData> profileData_;
    std::chrono::steady_clock::time_point currentProfileStart_;
};

/**
 * Benchmarking Framework - Comprehensive benchmarking suite
 */
class BenchmarkingFramework {
public:
    BenchmarkingFramework();
    ~BenchmarkingFramework() = default;
    
    /**
     * Run a benchmark suite
     */
    struct BenchmarkResult {
        std::string name;
        size_t neurons;
        size_t synapses;
        size_t steps;
        double timeSeconds;
        double neuronsPerSecond;
        double spikesPerSecond;
        double memoryUsageMB;
        double simdEfficiency;
        double numericalStability;
        bool success;
        std::string error;
        
        BenchmarkResult(const std::string& n, size_t neurons, size_t synapses, 
                       size_t steps, double time, bool success = true, 
                       const std::string& err = "")
            : name(n), neurons(neurons), synapses(synapses), steps(steps)
            , timeSeconds(time), neuronsPerSecond(0.0), spikesPerSecond(0.0)
            , memoryUsageMB(0.0), simdEfficiency(0.0), numericalStability(0.0)
            , success(success), error(err) {}
    };
    
    std::vector<BenchmarkResult> runBenchmarkSuite(const std::vector<std::string>& benchmarkNames);
    
    /**
     * Add a benchmark
     */
    void addBenchmark(const std::string& name, std::function<void()> benchmarkFunc);
    
    /**
     * Get all benchmark names
     */
    std::vector<std::string> getBenchmarkNames() const;
    
    /**
     * Get benchmark results
     */
    std::vector<BenchmarkResult> getResults() const { return results_; }
    
    /**
     * Save benchmark results
     */
    bool saveResults(const std::string& filename) const;
    
    /**
     * Load benchmark results
     */
    bool loadResults(const std::string& filename);
    
    /**
     * Generate benchmark comparison report
     */
    std::string generateComparisonReport(const std::vector<BenchmarkResult>& previousResults) const;
    
    /**
     * Get singleton instance
     */
    static BenchmarkingFramework& getInstance() {
        static BenchmarkingFramework instance;
        return instance;
    }
    
private:
    mutable std::mutex mutex_;
    std::vector<BenchmarkResult> results_;
    std::unordered_map<std::string, std::function<void()>> benchmarks_;
};

/**
 * Performance Regression Detector - Detects performance regressions
 */
class PerformanceRegressionDetector {
public:
    PerformanceRegressionDetector();
    ~PerformanceRegressionDetector() = default;
    
    /**
     * Add baseline performance data
     */
    void addBaseline(const std::string& baselineName, 
                     const std::vector<BenchmarkFramework::BenchmarkResult>& results);
    
    /**
     * Compare current results with baseline
     */
    std::vector<std::string> detectRegressions(
        const std::vector<BenchmarkFramework::BenchmarkResult>& currentResults) const;
    
    /**
     * Get all baseline names
     */
    std::vector<std::string> getBaselineNames() const;
    
    /**
     * Save baseline data
     */
    bool saveBaseline(const std::string& filename) const;
    
    /**
     * Load baseline data
     */
    bool loadBaseline(const std::string& filename);
    
    /**
     * Get singleton instance
     */
    static PerformanceRegressionDetector& getInstance() {
        static PerformanceRegressionDetector instance;
        return instance;
    }
    
private:
    mutable std::mutex mutex_;
    std::unordered_map<std::string, std::vector<BenchmarkFramework::BenchmarkResult>> baselines_;
    
    static constexpr double REGRESSION_THRESHOLD = 0.05;  // 5% threshold
};

/**
 * Performance Integration Helper - Integrates with existing performance system
 */
class PerformanceIntegrationHelper {
public:
    PerformanceIntegrationHelper();
    ~PerformanceIntegrationHelper() = default;
    
    /**
     * Initialize performance monitoring with existing system
     */
    void initialize();
    
    /**
     * Update performance metrics from existing system components
     */
    void updateFromExistingSystem(size_t neuronCount, size_t synapseCount,
                                 size_t spikeCount, double timeElapsed);
    
    /**
     * Get performance monitoring status
     */
    bool isPerformanceMonitoringEnabled() const;
    
    /**
     * Get numerical stability status
     */
    bool isNumericalStabilityEnabled() const;
    
    /**
     * Generate integrated performance report
     */
    std::string generateIntegratedReport() const;
    
    /**
     * Get singleton instance
     */
    static PerformanceIntegrationHelper& getInstance() {
        static PerformanceIntegrationHelper instance;
        return instance;
    }
    
private:
    bool performanceMonitoringEnabled_;
    bool numericalStabilityEnabled_;
    bool benchmarkingEnabled_;
    bool regressionDetectionEnabled_;
    mutable std::mutex mutex_;
    std::chrono::steady_clock::timePoint lastUpdateTime_;
};

} // namespace nlm
