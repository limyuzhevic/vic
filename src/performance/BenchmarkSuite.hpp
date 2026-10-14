#pragma once

/**
 * BenchmarkSuite - Performance benchmarking for NLM
 * 
 * Provides comprehensive performance benchmarking including:
 * - Neuron stepping benchmarks
 * - Memory pool benchmarks
 * - Parallel processing benchmarks
 * - Checkpoint performance benchmarks
 * - Hardware-specific benchmarks
 * - Regression testing
 * - Automated performance comparison
 * 
 * Features:
 * - Multiple benchmark suites
 * - Real-time performance measurement
 * - Statistical analysis and confidence intervals
 * - Performance regression detection
 * - Hardware-specific optimizations
 * - Automated reporting
 */

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <chrono>
#include <atomic>
#include <mutex>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <random>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <thread>
#include <future>
#include <numeric>
#include <cmath>
#include <type_traits>
#include <array>

namespace nlm {

// Benchmark configuration
struct BenchmarkConfig {
    uint32_t numIterations;               // Number of iterations per benchmark
    uint32_t warmupIterations;            // Warmup iterations
    std::chrono::milliseconds timeout;   // Timeout per benchmark
    bool enableDetailedStats;             // Enable detailed statistics
    uint32_t precision;                   // Decimal precision for output
    std::string outputFormat;             // Output format (csv, json, text)
    bool enableHardwareCounters;          // Enable hardware performance counters
    bool enableMemoryProfiling;           // Enable memory profiling
    bool enableCPUP profiling;            // Enable CPU profiling
    double confidenceLevel;               // Statistical confidence level (0-1)
    bool detectRegressions;               // Detect performance regressions
    uint32_t regressionThresholdPercent; // Regression threshold (%)
    std::string regressionBaseline;      // Baseline for regression detection
};

// Benchmark results
struct BenchmarkResult {
    std::string name;                     // Benchmark name
    std::string description;              // Benchmark description
    uint64_t iterations;                  // Number of iterations
    double totalTimeMs;                   // Total execution time (ms)
    double avgTimeMs;                     // Average time per iteration (ms)
    double minTimeMs;                     // Minimum time (ms)
    double maxTimeMs;                     // Maximum time (ms)
    double stdDevMs;                      // Standard deviation (ms)
    double cvMs;                          // Coefficient of variation
    double opsPerSecond;                  // Operations per second
    double memoryUsageMB;                 // Peak memory usage (MB)
    double cpuUsagePercent;               // CPU usage (%)
    uint64_t flops;                       // Floating point operations
    uint64_t memoryTrafficBytes;          // Memory traffic (bytes)
    std::chrono::high_resolution_clock::time_point startTime;  // Start time
    std::chrono::high_resolution_clock::time_point endTime;    // End time
    std::string hardwareInfo;             // Hardware information
    std::string compilerInfo;             // Compiler information
    std::unordered_map<std::string, double> customMetrics;   // Custom metrics
};

// Benchmark suite for specific functionality
class BenchmarkSuite {
public:
    BenchmarkSuite(const BenchmarkConfig& config);
    ~BenchmarkSuite();
    
    // Disable copying
    BenchmarkSuite(const BenchmarkSuite&) = delete;
    BenchmarkSuite& operator=(const BenchmarkSuite&) = delete;
    
    /**
     * Run a single benchmark
     */
    bool run(const std::string& name,
             std::function<void()> benchmarkFunc,
             const std::string& description = "");
    
    /**
     * Run a benchmark with measurements
     */
    bool runWithMeasurements(const std::string& name,
                             std::function<void()> benchmarkFunc,
                             std::function<void(BenchmarkResult&)> measureFunc,
                             const std::string& description = "");
    
    /**
     * Run multiple benchmarks in sequence
     */
    std::vector<BenchmarkResult> runBatch(const std::vector<std::pair<std::string, std::function<void()>>>& benchmarks);
    
    /**
     * Get benchmark results
     */
    const std::vector<BenchmarkResult>& getResults() const { return results_; }
    
    /**
     * Get specific benchmark result
     */
    const BenchmarkResult* getResult(const std::string& name) const;
    
    /**
     * Clear all results
     */
    void clearResults() { results_.clear(); }
    
    /**
     * Save results to file
     */
    bool saveResults(const std::string& filepath) const;
    
    /**
     * Load results from file
     */
    bool loadResults(const std::string& filepath);
    
    /**
     * Compare with baseline results
     */
    std::vector<BenchmarkResult> compareWithBaseline(const std::vector<BenchmarkResult>& baseline,
                                                     double regressionThresholdPercent = 5.0) const;
    
    /**
     * Generate performance report
     */
    std::string generateReport() const;
    
    /**
     * Set hardware-specific benchmarking
     */
    void enableHardwareBenchmarking(bool enable) { enableHardwareBenchmarking_ = enable; }
    
    /**
     * Set memory profiling
     */
    void enableMemoryProfiling(bool enable) { enableMemoryProfiling_ = enable; }
    
    /**
     * Set CPU profiling
     */
    void enableCPUPProfiling(bool enable) { enableCPUPProfiling_ = enable; }

protected:
    BenchmarkConfig config_;
    std::vector<BenchmarkResult> results_;
    bool enableHardwareBenchmarking_;
    bool enableMemoryProfiling_;
    bool enableCPUPProfiling_;
    std::mutex resultsMutex_;
    
    // Timing utilities
    double measureTime(std::function<void()> func) const;
    
    // Memory measurement
    double measureMemoryUsage(std::function<void()> func) const;
    
    // CPU measurement
    double measureCPUUsage(std::function<void()> func) const;
    
    // Hardware-specific measurements
    double measureHardwareSpecific(std::function<void()> func) const;
    
    // Statistical analysis
    double calculateMean(const std::vector<double>& values) const;
    double calculateStdDev(const std::vector<double>& values, double mean) const;
    double calculateCV(const std::vector<double>& values, double mean) const;
    
    // Baseline comparison
    bool isRegression(double baseline, double current, double thresholdPercent) const;
};

/**
 * Neuron stepping benchmarks
 */
class NeuronSteppingBenchmark : public BenchmarkSuite {
public:
    NeuronSteppingBenchmark(const BenchmarkConfig& config);
    
    /**
     * Benchmark single neuron stepping
     */
    BenchmarkResult benchmarkSingleNeuron(size_t neuronCount, uint32_t steps);
    
    /**
     * Benchmark batch neuron stepping
     */
    BenchmarkResult benchmarkBatchNeurons(size_t neuronCount, uint32_t steps, size_t batchSize = 0);
    
    /**
     * Benchmark active neuron processing
     */
    BenchmarkResult benchmarkActiveNeurons(size_t totalNeurons, size_t activeNeurons, uint32_t steps);
    
    /**
     * Benchmark spike propagation
     */
    BenchmarkResult benchmarkSpikePropagation(size_t neuronCount, size_t connectionDensity, uint32_t steps);
};

/**
 * Memory pool benchmarks
 */
class MemoryPoolBenchmark : public BenchmarkSuite {
public:
    MemoryPoolBenchmark(const BenchmarkConfig& config);
    
    /**
     * Benchmark memory pool allocation
     */
    BenchmarkResult benchmarkAllocation(size_t objectSize, size_t objectCount, size_t blockSize = 1024);
    
    /**
     * Benchmark memory pool deallocation
     */
    BenchmarkResult benchmarkDeallocation(size_t objectSize, size_t objectCount);
    
    /**
     * Benchmark memory pool mixed operations
     */
    BenchmarkResult benchmarkMixedOperations(size_t objectSize, size_t objectCount, 
                                            size_t allocRate, size_t deallocRate);
    
    /**
     * Benchmark memory pool fragmentation
     */
    BenchmarkResult benchmarkFragmentation(size_t objectSize, size_t totalObjects, size_t allocationPattern);
};

/**
 * Parallel processing benchmarks
 */
class ParallelProcessingBenchmark : public BenchmarkSuite {
public:
    ParallelProcessingBenchmark(const BenchmarkConfig& config);
    
    /**
     * Benchmark parallel for-loop
     */
    BenchmarkResult benchmarkParallelFor(size_t workSize, size_t grainSize = 256);
    
    /**
     * Benchmark thread pool
     */
    BenchmarkResult benchmarkThreadPool(size_t taskCount, size_t tasksPerThread);
    
    /**
     * Benchmark parallel neuron processing
     */
    BenchmarkResult benchmarkParallelNeuronProcessing(size_t neuronCount, size_t threadCount);
    
    /**
     * Benchmark synchronization
     */
    BenchmarkResult benchmarkSynchronization(size_t threadCount, size_t iterationCount);
};

/**
 * Checkpoint performance benchmarks
 */
class CheckpointBenchmark : public BenchmarkSuite {
public:
    CheckpointBenchmark(const BenchmarkConfig& config);
    
    /**
     * Benchmark checkpoint save
     */
    BenchmarkResult benchmarkSave(size_t neuronCount, size_t synapseCount, 
                                 const std::string& compression = "zstd");
    
    /**
     * Benchmark checkpoint load
     */
    BenchmarkResult benchmarkLoad(size_t neuronCount, size_t synapseCount,
                                 const std::string& compression = "zstd");
    
    /**
     * Benchmark differential checkpoint
     */
    BenchmarkResult benchmarkDifferential(size_t neuronCount, size_t synapseCount,
                                         uint64_t changeRate);
    
    /**
     * Benchmark incremental checkpoint
     */
    BenchmarkResult benchmarkIncremental(size_t neuronCount, size_t synapseCount,
                                        uint64_t stepInterval);
};

/**
 * Hardware-specific benchmarks
 */
class HardwareBenchmark : public BenchmarkSuite {
public:
    HardwareBenchmark(const BenchmarkConfig& config);
    
    /**
     * Benchmark SIMD optimizations
     */
    BenchmarkResult benchmarkSIMD(size_t neuronCount, uint32_t steps);
    
    /**
     * Benchmark cache efficiency
     */
    BenchmarkResult benchmarkCacheEfficiency(size_t accessPattern, size_t workingSetSize);
    
    /**
     * Benchmark memory bandwidth
     */
    BenchmarkResult benchmarkMemoryBandwidth(size_t dataSize, size_t iterationCount);
    
    /**
     * Benchmark threading efficiency
     */
    BenchmarkResult benchmarkThreadingEfficiency(size_t workSize, size_t threadCount);
};

/**
 * Regression testing suite
 */
class RegressionTestSuite : public BenchmarkSuite {
public:
    RegressionTestSuite(const BenchmarkConfig& config);
    
    /**
     * Add regression baseline
     */
    void addBaseline(const std::string& name, const BenchmarkResult& baseline);
    
    /**
     * Test for performance regressions
     */
    std::vector<std::pair<std::string, bool>> testForRegressions(const std::vector<BenchmarkResult>& currentResults) const;
    
    /**
     * Generate regression report
     */
    std::string generateRegressionReport(const std::vector<BenchmarkResult>& currentResults) const;
    
    /**
     * Get all baselines
     */
    const std::unordered_map<std::string, BenchmarkResult>& getBaselines() const { return baselines_; }

private:
    std::unordered_map<std::string, BenchmarkResult> baselines_;
    std::mutex baselinesMutex_;
};

/**
 * Global benchmark runner for easy use
 */
class BenchmarkRunner {
public:
    static BenchmarkRunner& getInstance() {
        static BenchmarkRunner instance;
        return instance;
    }
    
    /**
     * Configure global benchmark settings
     */
    void configure(const BenchmarkConfig& config) {
        config_ = config;
    }
    
    /**
     * Run all benchmarks
     */
    std::vector<BenchmarkResult> runAllBenchmarks();
    
    /**
     * Run specific benchmark suite
     */
    std::vector<BenchmarkResult> runBenchmarkSuite(BenchmarkSuite& suite);
    
    /**
     * Save benchmark results
     */
    bool saveResults(const std::string& filepath) const;
    
    /**
     * Get global configuration
     */
    const BenchmarkConfig& getConfig() const { return config_; }

private:
    BenchmarkRunner() : config_() {}
    ~BenchmarkRunner() = default;
    
    BenchmarkConfig config_;
    mutable std::mutex configMutex_;
};

// Convenience functions
namespace BenchmarkUtils {
    /**
     * Generate unique benchmark name
     */
    static std::string generateBenchmarkName(const std::string& prefix, 
                                             const std::string& suffix = "") {
        auto now = std::chrono::system_clock::now();
        auto timestamp = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << prefix << "_" << timestamp;
        if (!suffix.empty()) {
            ss << "_" << suffix;
        }
        return ss.str();
    }
    
    /**
     * Format benchmark results for output
     */
    static std::string formatResults(const BenchmarkResult& result) {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(3);
        ss << "Benchmark: " << result.name << "\n";
        ss << "  Description: " << result.description << "\n";
        ss << "  Iterations: " << result.iterations << "\n";
        ss << "  Average Time: " << result.avgTimeMs << " ms\n";
        ss << "  Min Time: " << result.minTimeMs << " ms\n";
        ss << "  Max Time: " << result.maxTimeMs << " ms\n";
        ss << "  Standard Deviation: " << result.stdDevMs << " ms\n";
        ss << "  Operations/sec: " << result.opsPerSecond << "\n";
        ss << "  Memory Usage: " << result.memoryUsageMB << " MB\n";
        ss << "  CPU Usage: " << result.cpuUsagePercent << "%\n";
        ss << "  FLOPS: " << result.flops << "\n";
        ss << "  Memory Traffic: " << result.memoryTrafficBytes << " bytes\n";
        return ss.str();
    }
    
    /**
     * Calculate performance score
     */
    static double calculatePerformanceScore(const BenchmarkResult& result) {
        return result.opsPerSecond / std::max(result.avgTimeMs, 0.001);
    }
}

} // namespace nlm