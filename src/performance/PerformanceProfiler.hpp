#pragma once

/**
 * Performance Optimization and Benchmarking Module
 * 
 * This module provides performance analysis tools for NLM, including:
 * - Benchmarking suite for neural network performance
 * - Memory pool optimizations
 * - SIMD vectorization utilities
 * - Parallel processing performance analysis
 * - Simulation speedup measurements
 * - Resource consumption monitoring
 */

#include <chrono>
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <numeric>
#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <cmath>

namespace nlm {

/**
 * Benchmark metrics for neural network performance
 */
struct BenchmarkMetrics {
    // Timing metrics (milliseconds)
    double simulationTimeMs;
    double wallClockTimeMs;
    double setupTimeMs;
    double teardownTimeMs;
    
    // Performance metrics
    double spikesPerSecond;
    double neuronsPerMillisecond;
    double synapsesPerMillisecond;
    double learningRate;  // Weight changes per second
    double energyEfficiency;  // Computation per energy unit
    
    // Memory metrics
    size_t peakMemoryUsage;  // MB
    size_t memoryAllocations;
    size_t memoryDeallocations;
    
    // Computational metrics
    size_t totalOperations;
    double operationsPerSecond;
    double arithmeticIntensity;  // Operations per memory access
    
    // System metrics
    int cpuUsagePercent;
    double memoryUsagePercent;
    size_t threadCount;
    
    BenchmarkMetrics()
        : simulationTimeMs(0.0)
        , wallClockTimeMs(0.0)
        , setupTimeMs(0.0)
        , teardownTimeMs(0.0)
        , spikesPerSecond(0.0)
        , neuronsPerMillisecond(0.0)
        , synapsesPerMillisecond(0.0)
        , learningRate(0.0)
        , energyEfficiency(0.0)
        , peakMemoryUsage(0)
        , memoryAllocations(0)
        , memoryDeallocations(0)
        , totalOperations(0)
        , operationsPerSecond(0.0)
        , arithmeticIntensity(0.0)
        , cpuUsagePercent(0)
        , memoryUsagePercent(0.0)
        , threadCount(0) {}
};

/**
 * Benchmark configuration for performance testing
 */
struct BenchmarkConfig {
    // Scale testing
    std::vector<size_t> neuronCounts;      // Different neuron counts to test
    std::vector<uint64_t> stepCounts;     // Different simulation steps
    std::vector<float> connectionProbabilities; // Different connection densities
    
    // Performance testing modes
    bool testStartupTime;        // Test initialization performance
    bool testSteadyState;        // Test steady-state performance
    bool testScalability;        // Test scaling with problem size
    bool testMemoryEfficiency;   // Test memory usage patterns
    bool testParallelEfficiency; // Test parallel processing efficiency
    
    // Monitoring settings
    bool monitorMemory;          // Track memory usage
    bool monitorCPU;            // Track CPU usage
    bool monitorEnergy;         // Estimate energy consumption
    bool monitorNetworkIO;      // Track network I/O (if applicable)
    
    // Benchmark duration
    double benchmarkDurationSeconds;  // Maximum time per benchmark run
    uint32_t warmupSteps;            // Steps to skip in benchmark
    
    // Output options
    bool generateReports;         // Generate human-readable reports
    bool exportData;             // Export raw data for analysis
    std::string reportFormat;    // "text", "json", "csv", "xml"
    
    BenchmarkConfig()
        : testStartupTime(true)
        , testSteadyState(true)
        , testScalability(true)
        , testMemoryEfficiency(true)
        , testParallelEfficiency(true)
        , monitorMemory(true)
        , monitorCPU(true)
        , monitorEnergy(false)
        , monitorNetworkIO(false)
        , benchmarkDurationSeconds(60.0)
        , warmupSteps(100)
        , generateReports(true)
        , exportData(true)
        , reportFormat("text") {}
};

/**
 * Performance benchmark runner
 */
class BenchmarkRunner {
public:
    BenchmarkRunner();
    ~BenchmarkRunner();
    
    /**
     * Run a complete benchmark suite
     */
    std::vector<BenchmarkMetrics> runBenchmarks(const BenchmarkConfig& config);
    
    /**
     * Run specific benchmark scenario
     */
    BenchmarkMetrics runBenchmark(size_t neuronCount, uint64_t steps, 
                                float connectionProbability = 0.1f);
    
    /**
     * Measure startup time (initialization performance)
     */
    double measureStartupTime(size_t neuronCount);
    
    /**
     * Measure steady-state performance
     */
    BenchmarkMetrics measureSteadyState(size_t neuronCount, uint64_t steps);
    
    /**
     * Measure scalability with problem size
     */
    std::vector<BenchmarkMetrics> measureScalability(const std::vector<size_t>& neuronCounts,
                                                     uint64_t baseSteps);
    
    /**
     * Measure memory efficiency
     */
    double measureMemoryEfficiency(size_t neuronCount, uint64_t steps);
    
    /**
     * Measure parallel processing efficiency
     */
    double measureParallelEfficiency(size_t neuronCount, uint64_t steps,
                                    const std::vector<size_t>& threadCounts);
    
    /**
     * Generate benchmark report
     */
    std::string generateReport(const std::vector<BenchmarkMetrics>& results) const;
    
    /**
     * Export benchmark data to file
     */
    bool exportBenchmarkData(const std::vector<BenchmarkMetrics>& results,
                            const std::string& filename) const;
    
    /**
     * Load benchmark configuration from file
     */
    bool loadBenchmarkConfig(const std::string& filename, BenchmarkConfig& config);
    
    /**
     * Save benchmark configuration to file
     */
    bool saveBenchmarkConfig(const BenchmarkConfig& config, const std::string& filename) const;
    
    /**
     * Get system information for benchmarking
     */
    std::string getSystemInfo() const;
    
    /**
     * Get current CPU usage
     */
    double getCurrentCPUUsage() const;
    
    /**
     * Get current memory usage
     */
    size_t getCurrentMemoryUsage() const;
    
    /**
     * Get number of available CPU cores
     */
    static size_t getCPUCoreCount();
    
    /**
     * Get peak memory usage since program start
     */
    static size_t getPeakMemoryUsage();
    
    /**
     * Reset peak memory usage counter
     */
    static void resetPeakMemoryUsage();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helper methods
    BenchmarkMetrics runSingleBenchmark(size_t neuronCount, uint64_t steps,
                                       float connectionProbability,
                                       bool enableVisualization = false);
    
    void collectSystemMetrics(BenchmarkMetrics& metrics) const;
    void collectBenchmarkMetrics(BenchmarkMetrics& metrics, const Brain* brain,
                                uint64_t steps, double wallClockTime) const;
    
    double measureOperationTime(std::function<void()> operation) const;
    
    std::vector<BenchmarkMetrics> runBenchmarksSequentially(const BenchmarkConfig& config);
    std::vector<BenchmarkMetrics> runBenchmarksInParallel(const BenchmarkConfig& config);
    
    static std::string formatTime(double seconds) const;
    static std::string formatMemory(size_t bytes) const;
};

/**
 * Memory pool performance analyzer
 */
class MemoryPoolAnalyzer {
public:
    MemoryPoolAnalyzer();
    ~MemoryPoolAnalyzer();
    
    /**
     * Analyze memory pool performance
     */
    struct PoolAnalysis {
        std::string poolName;
        size_t totalAllocations;
        size_t totalDeallocations;
        size_t peakActiveObjects;
        size_t currentActiveObjects;
        double allocationTimeAverage;
        double deallocationTimeAverage;
        double memoryUsage;
        size_t blockSize;
        size_t numBlocks;
        double fragmentationRatio;
        bool isWorking;
    };
    
    std::vector<PoolAnalysis> analyzeMemoryPools(const Brain* brain) const;
    
    /**
     * Test memory pool allocation performance
     */
    double testAllocationPerformance(size_t poolSize, size_t numAllocations);
    
    /**
     * Test memory pool deallocation performance
     */
    double testDeallocationPerformance(size_t poolSize, size_t numDeallocations);
    
    /**
     * Test memory pool mixed workload
     */
    double testMixedWorkloadPerformance(size_t poolSize, size_t numOperations);
    
    /**
     * Analyze memory fragmentation
     */
    double analyzeMemoryFragmentation() const;
    
    /**
     * Get memory pool statistics
     */
    std::string getMemoryPoolStatistics() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    PoolAnalysis analyzeSinglePool(const std::string& poolName, const Brain* brain) const;
    
    double measureAllocationTime(size_t size);
    double measureDeallocationTime(size_t size);
};

/**
 * SIMD vectorization performance analyzer
 */
class SIMDPerformanceAnalyzer {
public:
    SIMDPerformanceAnalyzer();
    ~SIMDPerformanceAnalyzer();
    
    /**
     * Analyze SIMD performance
     */
    struct SIMDMetrics {
        bool simdEnabled;
        size_t vectorWidth;  // Bytes per SIMD register
        size_t neuronVectorizationFactor;
        size_t synapseVectorizationFactor;
        double vectorizationEfficiency;  // Fraction of operations using SIMD
        double simdSpeedup;  // Speedup compared to scalar
        double memoryBandwidthUtilization;
        size_t operationsPerSIMDInstruction;
        bool hasSSE;
        bool hasSSE2;
        bool hasSSE3;
        bool hasSSSE3;
        bool hasSSE4_1;
        bool hasSSE4_2;
        bool hasAVX;
        bool hasAVX2;
        bool hasAVX512;
        bool hasNEON;
    };
    
    SIMDMetrics analyzeSIMDPerformance(const Brain* brain) const;
    
    /**
     * Compare scalar vs SIMD performance
     */
    double compareScalarVsSIMD(size_t neuronCount, uint64_t steps);
    
    /**
     * Measure SIMD overhead
     */
    double measureSIMDOverhead(size_t operationCount);
    
    /**
     * Get SIMD capabilities of current system
     */
    SIMDMetrics getSIMDCapabilities() const;
    
    /**
     * Generate SIMD performance report
     */
    std::string generateSIMDReport(const SIMDMetrics& metrics) const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    SIMDMetrics getProcessorFeatures() const;
    
    double benchmarkSIMDOperations(size_t count);
    double benchmarkScalarOperations(size_t count);
};

/**
 * Parallel processing performance analyzer
 */
class ParallelPerformanceAnalyzer {
public:
    ParallelPerformanceAnalyzer();
    ~ParallelPerformanceAnalyzer();
    
    /**
     * Analyze parallel processing performance
     */
    struct ParallelMetrics {
        size_t threadCount;
        double totalSpeedup;  // Compared to single-threaded
        double efficiency;   // Speedup / threadCount
        double parallelFraction;  // Amdahl's law calculation
        double overheadTime;      // Thread creation/synchronization overhead
        double loadBalancing;     // How evenly work is distributed
        double cacheEffectiveness; // Cache hit rate in parallel execution
        double communicationOverhead; // Time spent on inter-thread communication
        bool isParallel;        // Whether parallel processing is enabled
        size_t maxConcurrentThreads;  // Maximum threads used simultaneously
    };
    
    ParallelMetrics analyzeParallelPerformance(const Brain* brain,
                                               const std::vector<size_t>& threadCounts);
    
    /**
     * Measure thread creation/destruction overhead
     */
    double measureThreadOverhead(size_t numThreads);
    
    /**
     * Measure work distribution efficiency
     */
    double measureWorkDistribution(size_t neuronCount, size_t threadCount);
    
    /**
     * Measure cache performance in parallel execution
     */
    double measureCachePerformance(size_t neuronCount, size_t threadCount);
    
    /**
     * Get thread pool statistics
     */
    std::string getThreadPoolStatistics() const;
    
    /**
     * Generate parallel performance report
     */
    std::string generateParallelReport(const ParallelMetrics& metrics) const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    ParallelMetrics analyzeSingleThreadCount(size_t threadCount, const Brain* brain);
    
    double benchmarkWithThreadCount(size_t threadCount, size_t neuronCount,
                                   uint64_t steps);
};

/**
 * Complete performance profiling suite
 */
class PerformanceProfiler {
public:
    PerformanceProfiler();
    ~PerformanceProfiler();
    
    /**
     * Run complete performance profile
     */
    struct FullProfile {
        BenchmarkMetrics benchmarkMetrics;
        MemoryPoolAnalyzer::PoolAnalysis memoryPoolMetrics;
        SIMDPerformanceAnalyzer::SIMDMetrics simdMetrics;
        ParallelPerformanceAnalyzer::ParallelMetrics parallelMetrics;
        std::string profileTimestamp;
        double totalProfileTime;
    };
    
    FullProfile runCompleteProfile(const BenchmarkConfig& config = BenchmarkConfig());
    
    /**
     * Run specific performance tests
     */
    BenchmarkMetrics runBenchmarkTest(const BenchmarkConfig& config);
    MemoryPoolAnalyzer::PoolAnalysis runMemoryPoolTest(const Brain* brain);
    SIMDPerformanceAnalyzer::SIMDMetrics runSIMDTest(const Brain* brain);
    ParallelPerformanceAnalyzer::ParallelMetrics runParallelTest(const Brain* brain,
                                                                const std::vector<size_t>& threadCounts);
    
    /**
     * Generate comprehensive performance report
     */
    std::string generateCompleteReport(const FullProfile& profile) const;
    
    /**
     * Export profile data to files
     */
    bool exportProfile(const FullProfile& profile, const std::string& outputDir);
    
    /**
     * Load previous profile from file
     */
    bool loadProfile(const std::string& filename, FullProfile& profile);
    
    /**
     * Save profile to file
     */
    bool saveProfile(const FullProfile& profile, const std::string& filename) const;
    
    /**
     * Compare profiles
     */
    std::string compareProfiles(const FullProfile& profile1, const FullProfile& profile2) const;
    
    /**
     * Get profile summary
     */
    std::string getProfileSummary(const FullProfile& profile) const;
    
    /**
     * Reset all performance counters
     */
    void resetCounters();
    
    /**
     * Start performance monitoring
     */
    void startMonitoring();
    
    /**
     * Stop performance monitoring
     */
    FullProfile stopMonitoring();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    BenchmarkRunner benchmarkRunner_;
    MemoryPoolAnalyzer memoryPoolAnalyzer_;
    SIMDPerformanceAnalyzer simdAnalyzer_;
    ParallelPerformanceAnalyzer parallelAnalyzer_;
    
    FullProfile createProfile(const BenchmarkMetrics& benchmarkMetrics,
                              const MemoryPoolAnalyzer::PoolAnalysis& memoryPoolMetrics,
                              const SIMDPerformanceAnalyzer::SIMDMetrics& simdMetrics,
                              const ParallelPerformanceAnalyzer::ParallelMetrics& parallelMetrics);
};

/**
 * Performance report generator
 */
class PerformanceReportGenerator {
public:
    PerformanceReportGenerator();
    ~PerformanceReportGenerator();
    
    /**
     * Generate human-readable performance report
     */
    std::string generateHumanReadableReport(const PerformanceProfiler::FullProfile& profile) const;
    
    /**
     * Generate JSON report
     */
    std::string generateJSONReport(const PerformanceProfiler::FullProfile& profile) const;
    
    /**
     * Generate CSV report
     */
    std::string generateCSVReport(const PerformanceProfiler::FullProfile& profile) const;
    
    /**
     * Generate XML report
     */
    std::string generateXMLReport(const PerformanceProfiler::FullProfile& profile) const;
    
    /**
     * Generate LaTeX report
     */
    std::string generateLaTeXReport(const PerformanceProfiler::FullProfile& profile) const;
    
    /**
     * Generate performance comparison report
     */
    std::string generateComparisonReport(const std::vector<PerformanceProfiler::FullProfile>& profiles) const;
    
    /**
     * Save report to file
     */
    bool saveReport(const std::string& content, const std::string& filename) const;
    
    /**
     * Get available report formats
     */
    std::vector<std::string> getAvailableFormats() const;
    
    /**
     * Validate report content
     */
    bool validateReport(const std::string& content, const std::string& format) const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string formatNumber(double value, int precision = 6) const;
    std::string formatPercentage(double value, int precision = 2) const;
    std::string formatTime(double seconds) const;
    std::string formatMemory(size_t bytes) const;
    std::string escapeHTML(const std::string& text) const;
    std::string escapeCSV(const std::string& text) const;
    std::string escapeXML(const std::string& text) const;
    std::string escapeLaTeX(const std::string& text) const;
}; 

} // namespace nlm