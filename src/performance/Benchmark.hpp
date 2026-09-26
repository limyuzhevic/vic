#pragma once

/**
 * Benchmark Suite
 * 
 * Comprehensive benchmarking infrastructure for measuring performance
 * improvements across different optimization strategies, scales, and configurations.
 * 
 * Features:
 * - Multi-scale benchmarking (small, medium, large networks)
 * - Configurable optimization levels
 * - Before/after performance comparison
 * - Statistical analysis of benchmark results
 * - Performance trend analysis
 * - Automated benchmark suites
 * 
 * Benchmark Categories:
 * - Neural computation benchmarks
 * - Memory optimization benchmarks
 * - Cache efficiency benchmarks
 * - Algorithmic complexity benchmarks
 * - I/O performance benchmarks
 */

#include <chrono>
#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#include <map>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <random>
#include <iomanip>
#include <memory>

namespace nlm {

class Benchmark {
public:
    struct BenchmarkConfig {
        std::string name;
        uint32_t neurons;
        uint32_t synapses;
        uint32_t steps;
        double timeStep;
        uint32_t threadCount;
        bool enableSIMD;
        bool enableMemoryPool;
        bool enableEventDriven;
        bool enableMultithreading;
        bool enableSparseConnectivity;
        bool enableCacheOptimizations;
        bool enableAlgorithmicOptimizations;
    };
    
    struct BenchmarkResult {
        std::string benchmarkName;
        BenchmarkConfig config;
        uint64_t startTime;
        uint64_t endTime;
        uint64_t duration;  // nanoseconds
        double realTimeDuration;  // actual elapsed time in seconds
        
        // Performance metrics
        double neuronsPerSecond;
        double spikesPerSecond;
        double synapticEventsPerSecond;
        double memoryUsageMB;
        double cpuUsage;
        
        // Detailed measurements
        uint64_t totalMemoryAllocations;
        uint64_t totalAllocatedBytes;
        uint64_t totalCacheMisses;
        uint64_t totalBranchMispredictions;
        uint64_t totalFLOPs;
        
        // Per-step statistics
        std::vector<double> stepTimes;  // milliseconds per step
        std::vector<uint32_t> activeNeuronCounts;
        std::vector<double> firingRates;
        
        // Optimization effectiveness
        double memoryOptimizationFactor;
        double cacheEfficiency;
        double simdSpeedup;
        double multithreadingEfficiency;
        
        BenchmarkResult() : startTime(0), endTime(0), duration(0),
                           realTimeDuration(0.0),
                           neuronsPerSecond(0.0), spikesPerSecond(0.0),
                           synapticEventsPerSecond(0.0), memoryUsageMB(0.0),
                           cpuUsage(0.0), totalMemoryAllocations(0),
                           totalAllocatedBytes(0), totalCacheMisses(0),
                           totalBranchMispredictions(0), totalFLOPs(0),
                           memoryOptimizationFactor(0.0), cacheEfficiency(0.0),
                           simdSpeedup(0.0), multithreadingEfficiency(0.0) {}
    };
    
    class BenchmarkSuite {
    public:
        BenchmarkSuite(const std::string& name) : name_(name) {}
        
        // Add a benchmark configuration
        void addBenchmark(const std::string& benchName, const BenchmarkConfig& config);
        
        // Run all benchmarks
        std::vector<BenchmarkResult> runAll();
        
        // Run a specific benchmark
        BenchmarkResult runBenchmark(const std::string& benchName);
        
        // Generate report
        void generateReport(const std::string& filename, const std::vector<BenchmarkResult>& results) const;
        
        // Get suite name
        const std::string& getName() const { return name_; }
        
        // Get benchmark names
        std::vector<std::string> getBenchmarkNames() const;
        
        // Get configurations
        const std::map<std::string, BenchmarkConfig>& getConfigs() const { return configs_; }
        
    private:
        std::string name_;
        std::map<std::string, BenchmarkConfig> configs_;
        
        // Generate configuration variations
        void generateScaleVariations();
        void generateOptimizationVariations();
        void generateArchitectureVariations();
        
        // Generate benchmark config for specific scale
        BenchmarkConfig generateScaleConfig(const std::string& baseName, uint32_t neurons, uint32_t synapses) const;
        
        // Generate optimization level config
        BenchmarkConfig generateOptimizationConfig(const std::string& baseName, const BenchmarkConfig& baseConfig,
                                                  const std::string& optimization) const;
    };
    
    /**
     * Create standard benchmark suite for performance testing
     */
    static BenchmarkSuite createStandardSuite();
    
    /**
     * Create micro-benchmark suite for fine-grained measurements
     */
    static BenchmarkSuite createMicroBenchmarkSuite();
    
    /**
     * Calculate statistical analysis
     */
    struct StatisticalAnalysis {
        double mean;
        double median;
        double mode;
        double stddev;
        double min;
        double max;
        double iqr;  // Interquartile range
        
        StatisticalAnalysis() : mean(0.0), median(0.0), mode(0.0), stddev(0.0),
                               min(0.0), max(0.0), iqr(0.0) {}
    };
    
    static StatisticalAnalysis analyze(const std::vector<double>& data);
    
    /**
     * Generate optimization comparison report
     */
    static void generateOptimizationComparison(const std::string& baselineFilename,
                                             const std::string& optimizedFilename,
                                             const std::string& outputFilename);
    
    /**
     * Create benchmark configuration file
     */
    static void saveConfig(const std::string& filename, const BenchmarkConfig& config);
    
    /**
     * Load benchmark configuration file
     */
    static BenchmarkConfig loadConfig(const std::string& filename);
    
private:
    // Neural network generation for benchmarking
    static void generateRandomNetwork(BenchmarkConfig& config);
    static void generateScaleFreeNetwork(BenchmarkConfig& config);
    static void generateClusterNetwork(BenchmarkConfig& config);
    
    // Performance measurement helpers
    static uint64_t getCurrentTimestamp();
    static double getRealTime();
    
    // Statistical helpers
    static double calculateMean(const std::vector<double>& data);
    static double calculateStdDev(const std::vector<double>& data, double mean);
};

} // namespace nlm