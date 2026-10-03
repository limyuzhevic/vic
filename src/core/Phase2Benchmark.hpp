// Phase 2 Performance Benchmarking
#pragma once

#include "core/Config/Config.hpp"
#include <memory>
#include <string>
#include <vector>
#include <chrono>
#include <iostream>
#include <iomanip>

namespace nlm {
namespace phase2 {

/**
 * Performance metrics structure
 */
struct PerformanceMetrics {
    std::string benchmarkName;
    double executionTime;
    double cpuTime;
    double wallClockTime;
    uint64_t memoryUsage;
    uint64_t peakMemoryUsage;
    uint64_t totalOperations;
    uint64_t operationsPerSecond;
    
    PerformanceMetrics(const std::string& name) : 
        benchmarkName(name),
        executionTime(0.0),
        cpuTime(0.0),
        wallClockTime(0.0),
        memoryUsage(0),
        peakMemoryUsage(0),
        totalOperations(0),
        operationsPerSecond(0) {}
};

/**
 * Benchmarking utilities for Phase 2 tests
 */
class Phase2Benchmark {
public:
    Phase2Benchmark();
    ~Phase2Benchmark();
    
    // Disable copying
    Phase2Benchmark(const Phase2Benchmark&) = delete;
    Phase2Benchmark& operator=(const Phase2Benchmark&) = delete;
    
    // Start a benchmark
    void startBenchmark(const std::string& name);
    
    // Stop a benchmark and record results
    PerformanceMetrics stopBenchmark();
    
    // Run a benchmark with a specific function
    template<typename Func>
    PerformanceMetrics runBenchmark(const std::string& name, Func&& func);
    
    // Run multiple iterations for average
    template<typename Func>
    PerformanceMetrics runBenchmarkIterations(const std::string& name, 
                                              Func&& func,
                                              uint32_t iterations);
    
    // Get all benchmark results
    std::vector<PerformanceMetrics> getResults() const { return results_; }
    
    // Clear previous results
    void clearResults() { results_.clear(); }
    
    // Print benchmark summary
    void printBenchmarkSummary() const;
    
    // Get benchmark summary as string
    std::string getBenchmarkSummary() const;
    
    // Get the global benchmark instance
    static Phase2Benchmark& getInstance();
    
    // Set global benchmark instance
    static void setInstance(std::shared_ptr<Phase2Benchmark> benchmark);
    
    // Configure benchmark settings
    void setMaxIterations(uint32_t maxIterations) { maxIterations_ = maxIterations; }
    void setMinTimeThreshold(double minTime) { minTimeThreshold_ = minTime; }
    void setMemoryTracking(bool enable) { trackMemory_ = enable; }
    
    // Get benchmark settings
    uint32_t getMaxIterations() const { return maxIterations_; }
    double getMinTimeThreshold() const { return minTimeThreshold_; }
    bool getMemoryTracking() const { return trackMemory_; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl_;
    std::vector<PerformanceMetrics> results_;
    uint32_t maxIterations_;
    double minTimeThreshold_;
    bool trackMemory_;
    
    // Internal helper functions
    void recordBenchmark(const PerformanceMetrics& metrics);
    std::string formatTime(double seconds) const;
    std::string formatMemory(uint64_t bytes) const;
};

/**
 * Benchmarking helper functions
 */
namespace BenchmarkUtils {
    
    /**
     * Measure memory usage of a function
     */
    template<typename Func>
    size_t measureMemoryUsage(Func&& func);
    
    /**
     * Measure CPU time of a function
     */
    template<typename Func>
    double measureCPUTime(Func&& func);
    
    /**
     * Measure wall clock time of a function
     */
    template<typename Func>
    double measureWallClockTime(Func&& func);
    
    /**
     * Create benchmark for neural network operations
     */
    template<typename Func>
    PerformanceMetrics benchmarkNeuralOperation(const std::string& name,
                                                 Func&& operation,
                                                 std::shared_ptr<Config> config);
    
    /**
     * Create benchmark for plasticity operations
     */
    template<typename Func>
    PerformanceMetrics benchmarkPlasticityOperation(const std::string& name,
                                                     Func&& operation,
                                                     std::shared_ptr<Config> config);
    
    /**
     * Create benchmark for memory operations
     */
    template<typename Func>
    PerformanceMetrics benchmarkMemoryOperation(const std::string& name,
                                                 Func&& operation,
                                                 std::shared_ptr<Config> config);
    
    /**
     * Compare performance between two implementations
     */
    template<typename Func1, typename Func2>
    struct PerformanceComparison {
        PerformanceMetrics implementation1;
        PerformanceMetrics implementation2;
        double speedupFactor;
        double improvementPercentage;
        std::string winner;
    };
    
    template<typename Func1, typename Func2>
    PerformanceComparison<Func1, Func2> comparePerformance(const std::string& name,
                                                             Func1&& impl1,
                                                             Func2&& impl2,
                                                             std::shared_ptr<Config> config);
    
    /**
     * Profile memory usage patterns
     */
    void profileMemoryUsage(std::shared_ptr<Config> config);
    
    /**
     * Profile CPU usage patterns
     */
    void profileCPUUsage(std::shared_ptr<Config> config);
    
    /**
     * Generate performance report
     */
    std::string generatePerformanceReport(const std::vector<PerformanceMetrics>& metrics);
    
    /**
     * Validate benchmark results
     */
    bool validateBenchmarkResults(const std::vector<PerformanceMetrics>& metrics);
    
} // namespace BenchmarkUtils

} // namespace phase2
} // namespace nlm