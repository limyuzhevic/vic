// NLM Performance Monitoring and Profiling
// Provides comprehensive performance monitoring for NLM brain simulations
// 
// This module provides tools for:
// 
// 1. Real-time performance monitoring
//    - CPU usage tracking
//    - Memory usage monitoring
//    - Spike rate monitoring
//    - Synapse activity tracking
// 
// 2. Performance profiling
//    - Step-by-step performance analysis
//    - Bottleneck identification
//    - Resource consumption tracking
//    - Memory usage profiling
// 
// 3. Benchmark and testing
//    - Performance benchmarking
//    - Scalability testing
//    - Load testing
//    - Stress testing
// 
// 4. Automated performance analysis
//    - Performance trend analysis
//    - Anomaly detection
//    - Performance alerting
//    - Optimization recommendations
// 
// This is particularly useful for:
// - Researchers optimizing brain performance
// - Developers identifying performance bottlenecks
// - System administrators monitoring large-scale simulations
// - Users tuning parameters for specific use cases
// - Automated performance testing pipelines

#ifndef NLM_PERFORMANCE_MONITORING_H
#define NLM_PERFORMANCE_MONITORING_H

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <map>
#include <functional>
#include <thread>
#include <atomic>
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace nlm {

/**
 * Performance metrics collector
 * 
 * Collects and analyzes performance metrics from NLM brain simulations.
 * Provides real-time monitoring and historical analysis.
 */
class PerformanceMetrics {
public:
    /**
     * Performance sample collected at a specific time
     */
    struct PerformanceSample {
        double timestamp;           // Time since monitoring start (seconds)
        uint64_t simulationStep;    // Current simulation step
        double simulationTime;      // Current simulation time (seconds)
        
        // Brain metrics
        uint64_t neuronCount;       // Number of neurons
        uint64_t synapseCount;      // Number of synapses
        uint64_t activeNeuronCount; // Number of active neurons
        uint64_t totalSpikeCount;   // Total spike count
        uint64_t firingNeuronCount; // Neurons currently firing
        
        // Performance metrics
        double cpuUsage;            // CPU usage percentage (0.0-100.0)
        double memoryUsageMB;       // Memory usage in MB
        double memoryUsagePercent;  // Memory usage percentage
        double spikeRate;           // Spikes per second
        double firingRate;          // Firing rate (spikes/sec/neuron)
        
        // Activity metrics
        double noveltyLevel;        // Novelty detection level
        double curiosityLevel;      // Curiosity level
        double dopamineLevel;       // Dopamine modulation level
        
        // Network metrics
        double averageFiringRate;   // Average firing rate across neurons
        double maxFiringRate;       // Maximum firing rate
        double minFiringRate;       // Minimum firing rate
        
        PerformanceSample()
            : timestamp(0.0), simulationStep(0), simulationTime(0.0)
            , neuronCount(0), synapseCount(0), activeNeuronCount(0)
            , totalSpikeCount(0), firingNeuronCount(0)
            , cpuUsage(0.0), memoryUsageMB(0.0), memoryUsagePercent(0.0)
            , spikeRate(0.0), firingRate(0.0), noveltyLevel(0.0)
            , curiosityLevel(0.0), dopamineLevel(0.0)
            , averageFiringRate(0.0), maxFiringRate(0.0), minFiringRate(0.0) {}
    };
    
    /**
     * Performance analysis results
     */
    struct PerformanceAnalysis {
        double totalDuration;        // Total monitoring duration (seconds)
        uint64_t totalSteps;        // Total simulation steps
        uint64_t totalSpikes;       // Total spikes recorded
        
        // Performance statistics
        double avgCpuUsage;         // Average CPU usage
        double peakCpuUsage;        // Peak CPU usage
        double avgMemoryUsage;      // Average memory usage (MB)
        double peakMemoryUsage;     // Peak memory usage (MB)
        double avgSpikeRate;        // Average spike rate
        double peakSpikeRate;       // Peak spike rate
        double avgFiringRate;       // Average firing rate
        
        // Network stability
        double stabilityScore;      // Stability metric (0.0-1.0)
        double convergenceRate;     // How quickly system converges
        double activityVariance;    // Variance in activity levels
        
        // Resource efficiency
        double efficiencyScore;     // Overall efficiency (0.0-1.0)
        double scalabilityFactor;   // How well system scales with size
        
        PerformanceAnalysis()
            : totalDuration(0.0), totalSteps(0), totalSpikes(0)
            , avgCpuUsage(0.0), peakCpuUsage(0.0), avgMemoryUsage(0.0)
            , peakMemoryUsage(0.0), avgSpikeRate(0.0), peakSpikeRate(0.0)
            , avgFiringRate(0.0), stabilityScore(0.0), convergenceRate(0.0)
            , activityVariance(0.0), efficiencyScore(0.0), scalabilityFactor(0.0) {}
    };
    
    PerformanceMetrics();
    ~PerformanceMetrics();
    
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
     * Get latest performance sample
     */
    PerformanceSample getLatestSample() const;
    
    /**
     * Get all recorded samples
     */
    const std::vector<PerformanceSample>& getAllSamples() const { return samples_; }
    
    /**
     * Analyze performance data
     */
    PerformanceAnalysis analyze() const;
    
    /**
     * Get monitoring status
     */
    bool isMonitoring() const { return isMonitoring_; }
    
    /**
     * Clear all recorded data
     */
    void clear();
    
    /**
     * Get monitoring duration
     */
    double getDuration() const;
    
    /**
     * Get sample count
     */
    size_t getSampleCount() const { return samples_.size(); }
    
private:
    bool isMonitoring_;
    std::chrono::steady_clock::time_point startTime_;
    std::vector<PerformanceSample> samples_;
    mutable std::mutex samplesMutex_;
};

/**
 * Performance profiler for detailed analysis
 * 
 * Provides detailed profiling capabilities for NLM brain simulations,
 * identifying bottlenecks and optimizing performance.
 */
class PerformanceProfiler {
public:
    PerformanceProfiler();
    ~PerformanceProfiler();
    
    /**
     * Start profiling
     */
    void startProfiling(const std::string& description = "");
    
    /**
     * Stop profiling
     */
    void stopProfiling();
    
    /**
     * Record a timing event
     */
    void recordEvent(const std::string& eventName);
    
    /**
     * Record a custom metric
     */
    void recordMetric(const std::string& metricName, double value);
    
    /**
     * Get profiling results
     */
    std::string getResults() const;
    
    /**
     * Get profiling duration
     */
    double getDuration() const;
    
    /**
     * Check if profiling is active
     */
    bool isProfiling() const { return isProfiling_; }
    
    /**
     * Reset profiler
     */
    void reset();
    
    /**
     * Get timing data for specific event
     */
    const std::vector<std::chrono::steady_clock::time_point>& getEventTimes(const std::string& eventName) const;
    
    /**
     * Get metric values for specific metric
     */
    const std::vector<double>& getMetricValues(const std::string& metricName) const;
    
private:
    bool isProfiling_;
    std::chrono::steady_clock::time_point startTime_;
    std::map<std::string, std::vector<std::chrono::steady_clock::time_point>> eventTimes_;
    std::map<std::string, std::vector<double>> metricValues_;
};

/**
 * Performance benchmark tool
 * 
 * Provides benchmarking capabilities for NLM brain simulations,
 * allowing comparison of performance across different configurations
 * and parameter sets.
 */
class PerformanceBenchmark {
public:
    struct BenchmarkResult {
        std::string testName;        // Name of the test
        double duration;             // Test duration (seconds)
        uint64_t steps;              // Number of steps processed
        uint64_t totalSpikes;        // Total spikes generated
        double cpuUsage;             // Average CPU usage
        double memoryUsage;          // Average memory usage (MB)
        double spikeRate;            // Average spike rate
        double efficiency;           // Efficiency metric (0.0-1.0)
        std::map<std::string, double> additionalMetrics; // Additional metrics
        
        BenchmarkResult()
            : duration(0.0), steps(0), totalSpikes(0)
            , cpuUsage(0.0), memoryUsage(0.0), spikeRate(0.0), efficiency(0.0) {}
    };
    
    PerformanceBenchmark();
    ~PerformanceBenchmark();
    
    /**
     * Run a performance benchmark
     */
    BenchmarkResult runBenchmark(const std::string& testName, 
                                 std::function<void()> benchmarkFunction);
    
    /**
     * Run multiple benchmarks
     */
    std::vector<BenchmarkResult> runBenchmarks(
        const std::vector<std::pair<std::string, std::function<void()>>>& benchmarks);
    
    /**
     * Generate benchmark report
     */
    std::string generateReport(const std::vector<BenchmarkResult>& results) const;
    
    /**
     * Compare benchmarks
     */
    std::string compareBenchmarks(const std::vector<BenchmarkResult>& results) const;
    
    /**
     * Save benchmark results
     */
    bool saveResults(const std::string& filename, const std::vector<BenchmarkResult>& results) const;
    
    /**
     * Load benchmark results
     */
    bool loadResults(const std::string& filename, std::vector<BenchmarkResult>& results) const;
    
private:
    std::vector<BenchmarkResult> previousResults_;
};

/**
 * Performance monitoring manager
 * 
 * Main interface for performance monitoring, profiling, and benchmarking.
 * Coordinates multiple performance tools and provides a unified interface.
 */
class PerformanceMonitor {
public:
    PerformanceMonitor();
    ~PerformanceMonitor();
    
    /**
     * Get performance metrics instance
     */
    PerformanceMetrics& getMetrics() { return metrics_; }
    
    /**
     * Get performance profiler instance
     */
    PerformanceProfiler& getProfiler() { return profiler_; }
    
    /**
     * Get performance benchmark instance
     */
    PerformanceBenchmark& getBenchmark() { return benchmark_; }
    
    /**
     * Start performance monitoring
     */
    void startMonitoring();
    
    /**
     * Stop performance monitoring
     */
    void stopMonitoring();
    
    /**
     * Get overall performance analysis
     */
    PerformanceMetrics::PerformanceAnalysis getAnalysis() const;
    
    /**
     * Generate performance report
     */
    std::string generateReport() const;
    
    /**
     * Check if performance monitoring is active
     */
    bool isMonitoring() const { return metrics_.isMonitoring(); }
    
    /**
     * Reset all performance data
     */
    void reset();
    
private:
    PerformanceMetrics metrics_;
    PerformanceProfiler profiler_;
    PerformanceBenchmark benchmark_;
};

} // namespace nlm

#endif // NLM_PERFORMANCE_MONITORING_H
