#pragma once

/**
 * Benchmark - Performance benchmarking for NLM neural simulation
 * 
 * Provides comprehensive benchmarking capabilities including:
 * - Standard benchmark suite
 * - Performance comparison tools
 * - Scalability analysis
 * - Hardware-specific optimizations
 * - Algorithm performance metrics
 * - Performance regression testing
 * - Baseline performance measurement
 * - Comparative analysis
 * - Performance profiling
 * - Resource usage benchmarking
 * - Memory usage benchmarking
 * - CPU benchmarking
 * - I/O benchmarking
 * 
 * Integration:
 * - Integrates with PerformanceMonitor for metrics collection
 * - Provides benchmarks for Brain.cpp components
 * - Measures performance of neural computation
 * - Compares performance across different configurations
 */

#ifndef NLM_BENCHMARK_HPP
#define NLM_BENCHMARK_HPP

#include <chrono>
#include <deque>
#include <map>
#include <mutex>
#include <string>
#include <thread>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <atomic>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <functional>
#include <numeric>
#include <random>
#include <filesystem>
#include <memory>

namespace nlm {

// Benchmark configuration
struct BenchmarkConfig {
    std::string name;                    // Benchmark name
    size_t neuronCount;                  // Number of neurons
    size_t synapseCount;                 // Number of synapses
    size_t simulationSteps;             // Number of simulation steps
    double timestep;                     // Simulation timestep
    std::string neuralArchitecture;      // Neural architecture type
    std::string optimizationLevel;      // Optimization level
    bool enableMultithreading;           // Enable multithreading
    bool enableSIMD;                     // Enable SIMD
    bool enableMemoryPool;               // Enable memory pool
    bool enableEventDriven;              // Enable event-driven processing
    std::map<std::string, std::string> parameters; // Additional parameters
};

// Benchmark result
struct BenchmarkResult {
    std::string benchmarkName;           // Benchmark name
    double executionTime;                // Total execution time in seconds
    double cpuTime;                      // CPU time in seconds
    size_t memoryPeak;                   // Peak memory usage in bytes
    double memoryEfficiency;             // Memory efficiency (operations per byte)
    double cpuEfficiency;                // CPU efficiency (operations per cycle)
    size_t iterations;                   // Number of iterations
    double avgTimePerIteration;          // Average time per iteration
    double stdDevTime;                    // Standard deviation of execution time
    double minTime;                      // Minimum execution time
    double maxTime;                      // Maximum execution time
    double throughput;                   // Operations per second
    std::map<std::string, double> metrics; // Additional metrics
    std::string hardwareInfo;             // Hardware information
    std::string softwareInfo;             // Software information
    Timestamp timestamp;                  // When benchmark was run
};

// Scalability analysis result
struct ScalabilityResult {
    std::string benchmarkName;           // Benchmark name
    std::vector<size_t> neuronCounts;    // Tested neuron counts
    std::vector<double> executionTimes;  // Corresponding execution times
    std::vector<size_t> memoryUsages;    // Corresponding memory usages
    double scalabilityFactor;            // Scalability factor
    std::string scalingTrend;            // Scaling trend (linear, sublinear, superlinear)
    std::string recommendation;          // Optimization recommendation
    Timestamp timestamp;                  // When analysis was performed
};

// Performance comparison result
struct PerformanceComparison {
    std::string baselineBenchmark;       // Baseline benchmark name
    std::string optimizedBenchmark;      // Optimized benchmark name
    std::string configuration1;         // Configuration 1 description
    std::string configuration2;         // Configuration 2 description
    double performanceImprovement;       // Performance improvement (%)
    double memoryReduction;              // Memory reduction (%)
    double cpuReduction;                 // CPU reduction (%)
    std::string conclusion;              // Conclusion
    Timestamp timestamp;                  // When comparison was performed
};

// Algorithm performance metrics
struct AlgorithmMetrics {
    std::string algorithmName;           // Algorithm name
    double executionTime;                // Execution time
    double memoryUsage;                  // Memory usage
    double computationalComplexity;      // Computational complexity
    double cacheEfficiency;              // Cache efficiency
    double branchPredictionAccuracy;     // Branch prediction accuracy
    std::map<std::string, double> parameters; // Algorithm parameters
    Timestamp timestamp;                  // When metrics were collected
};

class Benchmark {
public:
    Benchmark();
    ~Benchmark();
    
    // Benchmark management
    void startBenchmark(const std::string& benchmarkName);
    void endBenchmark(const std::string& benchmarkName);
    bool isBenchmarking() const { return isBenchmarking_; }
    
    // Standard benchmarks
    BenchmarkResult runNeuronCreationBenchmark(const BenchmarkConfig& config);
    BenchmarkResult runSynapseCreationBenchmark(const BenchmarkConfig& config);
    BenchmarkResult runSimulationBenchmark(const BenchmarkConfig& config);
    BenchmarkResult runMemoryAllocationBenchmark(const BenchmarkConfig& config);
    BenchmarkResult runPlasticityBenchmark(const BenchmarkConfig& config);
    BenchmarkResult runPredictionBenchmark(const BenchmarkConfig& config);
    BenchmarkResult runCognitiveBenchmark(const BenchmarkConfig& config);
    BenchmarkResult runDevelopmentBenchmark(const BenchmarkConfig& config);
    BenchmarkResult runNeuromodulationBenchmark(const BenchmarkConfig& config);
    
    // Custom benchmark
    BenchmarkResult runCustomBenchmark(const std::string& benchmarkName,
                                       std::function<void()> benchmarkFunction,
                                       const BenchmarkConfig& config);
    
    // Scalability analysis
    ScalabilityResult analyzeScalability(const std::string& benchmarkName,
                                         const std::vector<size_t>& neuronCounts);
    
    // Performance comparison
    PerformanceComparison comparePerformance(const std::string& baselineBenchmark,
                                             const std::string& optimizedBenchmark,
                                             const std::string& config1,
                                             const std::string& config2);
    
    // Algorithm performance
    AlgorithmMetrics benchmarkAlgorithm(const std::string& algorithmName,
                                        std::function<void()> algorithmFunction,
                                        const std::map<std::string, double>& parameters);
    
    // Hardware-specific benchmarks
    BenchmarkResult runCPUBenchmark() const;
    BenchmarkResult runMemoryBenchmark() const;
    BenchmarkResult runIOBenchmark() const;
    BenchmarkResult runNetworkBenchmark() const;
    
    // Export/import benchmark results
    void exportBenchmarks(const std::string& filename) const;
    void importBenchmarks(const std::string& filename);
    
    // Statistics
    std::vector<BenchmarkResult> getBenchmarkResults() const;
    BenchmarkResult getBenchmarkResult(const std::string& benchmarkName) const;
    size_t getBenchmarkCount() const;
    
    // Clear benchmark results
    void clearBenchmarkResults();
    
    // Baseline management
    void setBaseline(const std::string& benchmarkName);
    std::string getBaseline() const { return baselineBenchmark_; }
    double compareToBaseline(const std::string& benchmarkName) const;
    
private:
    bool isBenchmarking_;
    Timestamp benchmarkStartTime_;
    Timestamp benchmarkEndTime_;
    
    // Benchmark results
    std::vector<BenchmarkResult> benchmarkResults_;
    
    // Configuration
    std::map<std::string, BenchmarkConfig> benchmarkConfigs_;
    
    // Baseline
    std::string baselineBenchmark_;
    
    // Statistics
    double totalBenchmarkTime_;
    size_t totalBenchmarkCount_;
    
    mutable std::mutex mutex_;
    
    // Helper methods
    void recordBenchmarkResult(const BenchmarkResult& result);
    BenchmarkResult createDefaultBenchmarkResult(const std::string& benchmarkName) const;
    double calculateStatistics(const std::vector<double>& values) const;
    std::string generateHardwareInfo() const;
    std::string generateSoftwareInfo() const;
    std::string determineScalingTrend(const std::vector<double>& times) const;
    double calculateScalabilityFactor(const std::vector<double>& times) const;
};

#endif // NLM_BENCHMARK_HPP