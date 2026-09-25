#pragma once

/**
 * Benchmarking Framework for NLM
 * 
 * Comprehensive benchmarking tools for neural network performance.
 * Integrates with PerformanceMonitor to provide detailed performance metrics,
 * scalability testing, and comparative analysis.
 * 
 * Features:
 * - Standard benchmarks for neural network operations
 * - Scalability testing (different neuron counts)
 * - Comparative benchmarking (scalar vs SIMD vs AVX)
 * - Memory bandwidth and cache efficiency measurements
 * - Event-driven vs synchronous processing comparison
 * - Automatic performance analysis and optimization recommendations
 * - Flame graph generation capability
 * - Real-time performance visualization
 */

#include <cstddef>
#include <cstdint>
#include <chrono>
#include <vector>
#include <string>
#include <map>
#include <memory>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <thread>
#include <atomic>
#include <unordered_map>
#include <functional>
#include "Monitor.hpp"

// Benchmarking configuration
#define NLM_BENCHMARKS_ENABLED 1

// Forward declarations
namespace nlm {
    class Brain;
    class NeuronPool;
    class SynapsePool;
    class EventQueue;
    class ParallelProcessing;
    class SIMDVectorization;
    class PerformanceMonitor;
}

// Benchmark types
enum class BenchmarkType {
    Micro,        // Microbenchmarks (individual operations)
    Neuron,       // Neuron-centric benchmarks
    Synapse,      // Synapse-centric benchmarks
    Memory,       // Memory bandwidth/caching benchmarks
    Parallel,     // Parallel processing benchmarks
    EventDriven,  // Event-driven processing benchmarks
    Integration,  // Integration benchmarks
    Custom       // Custom benchmarks
};

// Benchmark phases for flame graphs
enum class BenchmarkPhase {
    Initialization,
    Setup,
    Warmup,
    Execution,
    Teardown,
    Analysis
};

// Flame graph event for profiling
struct FlameGraphEvent {
    double timestamp;         // Relative timestamp (start of benchmark)
    BenchmarkPhase phase;     // Current phase
    std::string operation;    // Operation name
    std::string threadId;     // Thread identifier
    double duration;          // Duration in seconds
    size_t stackHash;         // Hash of call stack (for flame graph grouping)
    std::map<std::string, std::string> metadata; // Additional metadata
};

// Performance characteristics
struct PerformanceCharacteristics {
    double neuronsPerSecond;      // Neurons processed per second
    double spikesPerSecond;       // Spikes processed per second
    double memoryBandwidth;       // Memory bandwidth in GB/s
    double cacheHitRate;          // Cache hit rate (0.0 to 1.0)
    double simdUtilization;       // SIMD utilization (0.0 to 1.0)
    double parallelEfficiency;    // Parallel efficiency (0.0 to 1.0)
    double eventProcessingLatency; // Event processing latency in seconds
    double synchronizationOverhead; // Synchronization overhead in seconds
    std::map<std::string, double> customMetrics; // Custom performance metrics
};

// Comparative benchmark results
struct ComparativeResult {
    std::string configuration;     // Configuration name (e.g., "scalar", "AVX2", "SIMD")
    double performance;             // Performance score (higher is better)
    double memoryUsage;             // Memory usage in bytes
    double energyConsumption;       // Energy consumption (arbitrary units)
    double qualityScore;            // Quality score (0.0 to 1.0)
    std::string hardware;           // Target hardware (e.g., "Intel i7-9700K")
    double instructionsPerCycle;    // Instructions per cycle
    double branchPredictionAccuracy; // Branch prediction accuracy (0.0 to 1.0)
};

// Scaling benchmark results
struct ScalingResult {
    size_t neuronCount;            // Number of neurons
    size_t synapseCount;           // Number of synapses
    double neuronsPerSecond;      // Neurons per second
    double spikesPerSecond;       // Spikes per second
    double memoryUsage;            // Memory usage in bytes
    double memoryPerNeuron;       // Memory per neuron in bytes
    double memoryBandwidth;        // Memory bandwidth in GB/s
    double efficiency;             // Efficiency score (neurons/sec per watt or per core)
    double parallelSpeedup;        // Parallel speedup compared to single thread
    double realtimeFactor;         // Real-time factor (simulated time / wall time)
    std::vector<PerformanceCharacteristics> phases; // Performance per phase
};

// Benchmark configuration
struct BenchmarkConfig {
    std::string name;              // Benchmark name
    BenchmarkType type;             // Benchmark type
    size_t neuronCount;             // Target neuron count
    size_t synapseCount;            // Target synapse count
    size_t steps;                   // Number of simulation steps
    double timeStep;                // Simulation time step
    size_t warmupSteps;             // Number of warmup steps
    bool measureMemory;             // Whether to measure memory usage
    bool measureEnergy;             // Whether to measure energy consumption
    bool generateFlameGraph;        // Whether to generate flame graph
    std::map<std::string, std::string> customSettings; // Custom settings
};

// Main benchmarking framework
class Benchmark {
public:
    Benchmark();
    ~Benchmark();
    
    // Singleton pattern
    static Benchmark& getInstance();
    
    // Configuration
    void configure(const BenchmarkConfig& config);
    void enable(bool enable = true);
    bool isEnabled() const { return enabled_; }
    
    // Integration with NLM systems
    void integrateWithBrain(nlm::Brain* brain);
    void integrateWithNeuronPool(nlm::NeuronPool* neuronPool);
    void integrateWithSynapsePool(nlm::SynapsePool* synapsePool);
    void integrateWithEventQueue(nlm::EventQueue* eventQueue);
    void integrateWithParallelProcessing(nlm::ParallelProcessing* parallelProc);
    void integrateWithSIMD(nlm::SIMDVectorization* simd);
    void integrateWithPerformanceMonitor(nlm::PerformanceMonitor* monitor);
    
    // Benchmark execution
    BenchmarkResult run(const std::string& name);
    void runAll();
    void stopCurrentBenchmark();
    
    // Specific benchmark categories
    BenchmarkResult runMicroBenchmarks();
    BenchmarkResult runNeuronBenchmarks();
    BenchmarkResult runSynapseBenchmarks();
    BenchmarkResult runMemoryBenchmarks();
    BenchmarkResult runParallelBenchmarks();
    BenchmarkResult runEventDrivenBenchmarks();
    BenchmarkResult runIntegrationBenchmarks();
    
    // Comparative benchmarks
    std::vector<ComparativeResult> runComparativeBenchmarks(const std::vector<std::string>& configurations);
    std::vector<ScalingResult> runScalingBenchmarks(const std::vector<size_t>& neuronCounts);
    
    // Configuration-based benchmarks
    BenchmarkResult runConfigurationBenchmark(const std::string& configName);
    void runConfigurationSeries(const std::vector<std::string>& configNames);
    
    // Benchmarking utilities
    void startPhase(BenchmarkPhase phase);
    void endPhase(BenchmarkPhase phase);
    void recordEvent(const std::string& operation, double duration, BenchmarkPhase phase = BenchmarkPhase::Execution);
    void recordMemoryAccess(size_t bytes, bool cacheHit);
    void recordEventLatency(double latency, const std::string& eventType);
    void recordSynchronizationOverhead(double overhead);
    
    // Flame graph generation
    void generateFlameGraph(const std::string& filename) const;
    void generateDetailedReport(const std::string& filename) const;
    void generatePerformanceSummary(const std::string& filename) const;
    
    // Analysis and reporting
    std::string analyzePerformance(const BenchmarkResult& result) const;
    std::string analyzeScaling(const std::vector<ScalingResult>& results) const;
    std::string analyzeComparative(const std::vector<ComparativeResult>& results) const;
    
    // Performance tuning
    std::vector<std::string> getOptimizationRecommendations() const;
    bool applyAutoOptimizations();
    
    // Accessors
    std::vector<BenchmarkResult> getBenchmarkHistory() const { return benchmarkHistory_; }
    std::vector<ScalingResult> getScalingResults() const { return scalingResults_; }
    std::vector<ComparativeResult> getComparativeResults() const { return comparativeResults_; }
    PerformanceCharacteristics getCurrentCharacteristics() const { return currentCharacteristics_; }
    
    // Export
    void exportResults(const std::string& directory) const;
    void clearHistory();

private:
    struct BenchmarkInfo {
        std::string name;
        BenchmarkConfig config;
        std::chrono::high_resolution_clock::time_point startTime;
        std::chrono::high_resolution_clock::time_point cpuStart;
        bool active;
    };
    
    // Configuration
    bool enabled_;
    BenchmarkConfig currentConfig_;
    
    // Integration pointers
    nlm::Brain* brain_;
    nlm::NeuronPool* neuronPool_;
    nlm::SynapsePool* synapsePool_;
    nlm::EventQueue* eventQueue_;
    nlm::ParallelProcessing* parallelProc_;
    nlm::SIMDVectorization* simd_;
    nlm::PerformanceMonitor* monitor_;
    
    // Benchmark execution state
    BenchmarkInfo currentBenchmark_;
    BenchmarkPhase currentPhase_;
    double phaseStartTime_;
    std::vector<FlameGraphEvent> flameGraphEvents_;
    
    // Performance tracking
    std::vector<BenchmarkResult> benchmarkHistory_;
    std::vector<ScalingResult> scalingResults_;
    std::vector<ComparativeResult> comparativeResults_;
    PerformanceCharacteristics currentCharacteristics_;
    
    // Resource tracking
    std::atomic<size_t> totalMemoryAccessed_;
    std::atomic<size_t> cacheHits_;
    std::atomic<size_t> totalCacheMisses_;
    std::atomic<size_t> eventsProcessed_;
    std::atomic<double> totalEventLatency_;
    std::atomic<double> totalSynchronizationOverhead_;
    
    // Thread safety
    mutable std::mutex dataMutex_;
    std::condition_variable benchmarkCondition_;
    
    // Utility functions
    void initializeBenchmark(const std::string& name);
    BenchmarkResult finalizeBenchmark();
    double calculatePerformanceScore(const BenchmarkResult& result) const;
    double calculateScalingEfficiency(const ScalingResult& result) const;
    double calculateComparativeScore(const ComparativeResult& result) const;
    
    // Benchmark implementations
    BenchmarkResult runMicrobenchmarksImpl();
    BenchmarkResult runNeuronBenchmarksImpl();
    BenchmarkResult runSynapseBenchmarksImpl();
    BenchmarkResult runMemoryBenchmarksImpl();
    BenchmarkResult runParallelBenchmarksImpl();
    BenchmarkResult runEventDrivenBenchmarksImpl();
    BenchmarkResult runIntegrationBenchmarksImpl();
    
    // Comparative benchmark implementations
    std::vector<ComparativeResult> runComparativeBenchmarksImpl(const std::vector<std::string>& configurations);
    std::vector<ScalingResult> runScalingBenchmarksImpl(const std::vector<size_t>& neuronCounts);
    
    // Configuration benchmark implementations
    BenchmarkResult runConfigurationBenchmarkImpl(const std::string& configName);
    
    // Flame graph generation
    void writeFlameGraph(const std::string& filename) const;
    void writeDetailedReport(const std::string& filename) const;
    void writePerformanceSummary(const std::string& filename) const;
    
    // Analysis functions
    std::string analyzeMemoryPerformance(const BenchmarkResult& result) const;
    std::string analyzeCPUPerformance(const BenchmarkResult& result) const;
    std::string analyzeParallelPerformance(const BenchmarkResult& result) const;
    std::string analyzeEventDrivenPerformance(const BenchmarkResult& result) const;
};
