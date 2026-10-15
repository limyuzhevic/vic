#pragma once

/**
 * Comprehensive Benchmark Suite for NLM
 * 
 * Enhanced benchmarking system that supports multiple benchmark suites,
 * statistical analysis, performance monitoring, and memory tracking.
 * Maintains backward compatibility with existing benchmark.cpp.
 * 
 * Features:
 * - Multiple benchmark suites (performance, memory, accuracy, scalability)
 * - Configurable benchmark scenarios via config files
 * - Statistical analysis of benchmark results
 * - Real-time performance monitoring
 * - Memory usage tracking and leak detection
 * - Multi-threaded benchmarking
 * - Visualization data export (CSV, JSON)
 * - Integration with existing NLM config system
 */

#include "brain/Brain.hpp"
#include "performance/MemoryPool.hpp"
#include "performance/EventQueue.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <chrono>
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include <thread>
#include <mutex>
#include <algorithm>
#include <numeric>
#include <random>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cmath>

namespace nlm {

// Forward declarations
class BenchmarkSuite;
class PerformanceMonitor;
class MemoryTracker;
class StatisticalAnalyzer;

/**
 * Benchmark result structure containing comprehensive performance metrics
 */
struct BenchmarkResult {
    // Basic metrics (backward compatible)
    size_t neurons;
    size_t synapses;
    size_t steps;
    double totalTime;              // seconds
    double stepsPerSecond;
    double spikesPerStep;
    double synapsesPerNeuron;
    
    // Enhanced metrics
    double cpuTime;                // CPU time only
    double memoryUsage;           // Peak memory usage (MB)
    double cpuUsage;              // Average CPU usage (0-100%)
    double efficiency;            // Normalized efficiency score
    
    // Spike and activity metrics
    size_t totalSpikes;
    size_t activeNeurons;
    size_t peakActiveNeurons;
    double averageFiringRate;
    double stdDeviationFiringRate;
    
    // Memory metrics
    size_t memoryPoolAllocated;
    size_t memoryPoolAvailable;
    size_t eventQueueSize;
    double memoryFragmentation;
    
    // Temporal metrics
    double minStepTime;
    double maxStepTime;
    double medianStepTime;
    double cvStepTime;            // Coefficient of variation
    
    // Resource metrics
    double bandwidthUsage;        // Memory bandwidth (GB/s)
    double cacheMissRate;         // CPU cache miss rate
    
    // Timestamps
    double startTime;
    double endTime;
    
    // Threading metrics
    int numThreadsUsed;
    double parallelEfficiency;
    
    BenchmarkResult()
        : neurons(0), synapses(0), steps(0), totalTime(0.0), stepsPerSecond(0.0)
        , spikesPerStep(0.0), synapsesPerNeuron(0.0), cpuTime(0.0), memoryUsage(0.0)
        , cpuUsage(0.0), efficiency(0.0), totalSpikes(0), activeNeurons(0)
        , peakActiveNeurons(0), averageFiringRate(0.0), stdDeviationFiringRate(0.0)
        , memoryPoolAllocated(0), memoryPoolAvailable(0), eventQueueSize(0)
        , memoryFragmentation(0.0), minStepTime(0.0), maxStepTime(0.0)
        , medianStepTime(0.0), cvStepTime(0.0), bandwidthUsage(0.0), cacheMissRate(0.0)
        , startTime(0.0), endTime(0.0), numThreadsUsed(0), parallelEfficiency(0.0) {}
};

/**
 * Benchmark configuration parameters
 */
struct BenchmarkConfig {
    // Benchmark scenarios
    enum class BenchmarkType {
        PERFORMANCE,    // Performance scaling benchmarks
        MEMORY,         // Memory usage benchmarks
        ACCURACY,       // Accuracy and stability benchmarks
        SCALABILITY,    // Scalability benchmarks
        INTEGRATION,    // System integration benchmarks
        REGRESSION,     // Regression detection benchmarks
        COMPARISON,     // Configuration comparison benchmarks
        CUSTOM         // User-defined benchmarks
    };
    
    // Benchmark scenarios
    enum class Scenario {
        MICRO = 1000,      // 1K neurons
        SMALL = 10000,     // 10K neurons
        MEDIUM = 50000,    // 50K neurons
        LARGE = 100000,    // 100K neurons
        MASSIVE = 1000000 // 1M neurons
    };
    
    // Performance requirements
    double targetStepsPerSecond;
    double targetMemoryMB;
    double targetEfficiency;
    
    // Benchmark parameters
    size_t warmupSteps;        // Steps to discard as warmup
    size_t measurementSteps;   // Steps to measure
    size_t steadyStateSteps;   // Minimum steps for steady state
    
    // Multi-threading
    int numThreads;            // Number of threads (0 = auto-detect)
    bool enableParallel;       // Enable parallel processing
    
    // Monitoring
    bool monitorMemory;        // Enable memory monitoring
    bool monitorCpu;           // Enable CPU monitoring
    bool monitorEvents;        // Monitor spike events
    bool monitorFragmentation; // Monitor memory fragmentation
    
    // Export
    bool exportResults;        // Enable results export
    std::string exportPath;    // Export directory
    std::string exportFormat;  // CSV, JSON, or both
    
    // Statistics
    bool computeStatistics;    // Compute statistical analysis
    bool computeRegression;    // Detect performance regression
    
    // Time measurement
    bool wallClockTime;        // Use wall-clock time
    bool cpuTimeOnly;          // Use CPU time only
    
    // Configuration
    std::string configFile;    // Benchmark configuration file
    bool validateConfig;       // Validate configuration parameters
    
    BenchmarkConfig()
        : targetStepsPerSecond(0.0), targetMemoryMB(0.0), targetEfficiency(0.0)
        , warmupSteps(100), measurementSteps(1000), steadyStateSteps(500)
        , numThreads(0), enableParallel(true), monitorMemory(true)
        , monitorCpu(true), monitorEvents(true), monitorFragmentation(true)
        , exportResults(false), exportFormat("csv"), computeStatistics(true)
        , computeRegression(true), wallClockTime(true), cpuTimeOnly(false)
        , validateConfig(true) {}
};

/**
 * Statistical analysis utilities for benchmark results
 */
class StatisticalAnalyzer {
public:
    // Basic statistics
    static double mean(const std::vector<double>& values);
    static double median(const std::vector<double>& values);
    static double standardDeviation(const std::vector<double>& values);
    static double coefficientOfVariation(const std::vector<double>& values);
    static double min(const std::vector<double>& values);
    static double max(const std::vector<double>& values);
    
    // Confidence intervals
    static std::pair<double, double> confidenceInterval(
        const std::vector<double>& values, double confidenceLevel = 0.95);
    
    // Outlier detection
    static std::vector<size_t> detectOutliers(const std::vector<double>& values, 
                                             double zThreshold = 2.0);
    
    // Distribution fitting (normal, exponential, etc.)
    static std::string fitDistribution(const std::vector<double>& values);
    
    // Performance regression detection
    static bool detectRegression(const std::vector<double>& baseline,
                                 const std::vector<double>& current,
                                 double threshold = 0.05);
    
    // Statistical tests
    static double tTest(const std::vector<double>& sample1, const std::vector<double>& sample2);
    static double fTest(const std::vector<double>& sample1, const std::vector<double>& sample2);
    
    // Benchmarking statistics
    static double efficiencyScore(double achieved, double target);
    static double memoryEfficiency(double used, double available);
    static double parallelEfficiency(int threads, double sequentialTime);
};

/**
 * Performance monitoring system for real-time metrics collection
 */
class PerformanceMonitor {
public:
    PerformanceMonitor();
    ~PerformanceMonitor();
    
    // Start/stop monitoring
    void start();
    void stop();
    bool isRunning() const { return running_; }
    
    // Metric collection
    void recordMetric(const std::string& name, double value);
    double getMetric(const std::string& name) const;
    
    // Brain-specific monitoring
    void recordBrainMetrics(std::shared_ptr<Brain> brain);
    void recordMemoryMetrics();
    void recordSystemMetrics();
    
    // Time measurements
    void recordTime(const std::string& name, double duration);
    
    // Get monitoring summary
    std::unordered_map<std::string, double> getSummary() const;
    std::string getDetailedReport() const;
    
private:
    bool running_;
    double startTime_;
    mutable std::mutex metricMutex_;
    std::unordered_map<std::string, std::vector<double>> metrics_;
    std::unordered_map<std::string, std::vector<double>> timeMetrics_;
    
    // System monitoring (simplified - would use platform-specific APIs)
    void monitorCpuUsage();
    void monitorMemoryUsage();
    void monitorSystemEvents();
};

/**
 * Memory tracking system for detailed memory analysis
 */
class MemoryTracker {
public:
    MemoryTracker();
    ~MemoryTracker();
    
    // Memory pool tracking
    void recordMemoryPoolStats(const MemoryPool<NeuronId>* pool, const std::string& name);
    void recordMemoryPoolStats(const MemoryPool<SynapseId>* pool, const std::string& name);
    
    // Memory allocation tracking
    void recordAllocation(size_t size, const std::string& allocator);
    void recordDeallocation(size_t size, const std::string& allocator);
    
    // Event queue tracking
    void recordEventQueueStats(const SpikeRingBuffer& queue);
    
    // Memory leak detection
    bool detectMemoryLeak(double threshold = 0.01);
    std::vector<std::string> getMemoryLeakReport() const;
    
    // Fragmentation analysis
    double analyzeFragmentation() const;
    std::string getFragmentationReport() const;
    
    // Memory usage summary
    size_t getTotalMemoryUsage() const;
    size_t getPeakMemoryUsage() const;
    std::unordered_map<std::string, size_t> getDetailedMemoryUsage() const;
    
private:
    mutable std::mutex memoryMutex_;
    std::unordered_map<std::string, size_t> allocations_;
    std::unordered_map<std::string, size_t> deallocations_;
    std::vector<std::pair<std::string, size_t>> poolStats_;
    size_t peakMemoryUsage_;
    double fragmentationScore_;
};

/**
 * Enhanced benchmark suite with comprehensive testing capabilities
 */
class BenchmarkSuite {
public:
    BenchmarkSuite();
    ~BenchmarkSuite();
    
    // Configuration
    void configure(const BenchmarkConfig& config) { config_ = config; }
    const BenchmarkConfig& getConfig() const { return config_; }
    
    // Benchmark execution
    BenchmarkResult runBenchmark(size_t neuronCount, size_t steps);
    std::vector<BenchmarkResult> runBenchmarkSuite(const std::vector<std::pair<size_t, size_t>>& scenarios);
    std::vector<BenchmarkResult> runMemoryBenchmarks();
    std::vector<BenchmarkResult> runAccuracyBenchmarks();
    std::vector<BenchmarkResult> runScalabilityBenchmarks();
    
    // Multi-threaded benchmarking
    std::vector<BenchmarkResult> runParallelBenchmarks(const std::vector<std::pair<size_t, size_t>>& scenarios);
    
    // Configuration comparison
    std::vector<BenchmarkResult> compareConfigurations(const std::vector<std::shared_ptr<Config>>& configs);
    
    // Regression testing
    bool runRegressionTest(const std::string& baselineFile, double threshold);
    
    // Export results
    bool exportResults(const std::string& filename, const std::string& format = "csv");
    bool exportDetailedReport(const std::string& filename);
    
    // Statistical analysis
    std::unordered_map<std::string, double> analyzeResults(const std::vector<BenchmarkResult>& results);
    std::string generateStatisticalReport(const std::vector<BenchmarkResult>& results);
    
    // Configuration loading
    bool loadConfigFromFile(const std::string& filename);
    bool loadConfigFromArgs(int argc, char** argv);
    void printHelp() const;
    
private:
    BenchmarkConfig config_;
    std::unique_ptr<PerformanceMonitor> monitor_;
    std::unique_ptr<MemoryTracker> memoryTracker_;
    std::unique_ptr<StatisticalAnalyzer> statsAnalyzer_;
    
    std::shared_ptr<Brain> createBrainWithConfig(size_t neuronCount);
    void setupPerformanceInfrastructure();
    void warmupBenchmark(std::shared_ptr<Brain> brain, size_t steps);
    void runBenchmarkInternal(std::shared_ptr<Brain> brain, size_t steps, BenchmarkResult& result);
};

// Global benchmark runner instance
BenchmarkSuite& getBenchmarkSuite();

} // namespace nlm