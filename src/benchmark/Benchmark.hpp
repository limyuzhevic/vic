// Comprehensive benchmarking and performance testing framework
// Tools for measuring and analyzing NLM performance characteristics

#ifndef BENCHMARK_HPP
#define BENCHMARK_HPP

#include <chrono>
#include <iostream>
#include <fstream>
#include <memory>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <thread>
#include <mutex>
#include <atomic>
#include <numeric>
#include <iomanip>
#include <functional>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"

namespace nlm {

// Benchmark measurement types
enum class BenchmarkMetric {
    Time,
    Memory,
    CPU,
    Throughput,
    Latency,
    Accuracy,
    Energy,
    Bandwidth
};

// Benchmark result structure
struct BenchmarkResult {
    std::string name;
    std::string description;
    std::map<BenchmarkMetric, double> metrics;
    std::map<std::string, std::string> metadata;
    double timestamp;
    size_t iterationCount;
    double totalTime;
    
    // Statistical measures
    double mean;
    double median;
    double stddev;
    double min;
    double max;
    std::vector<double> samples;
    
    BenchmarkResult(const std::string& name, const std::string& description)
        : name(name), description(description), timestamp(std::time(nullptr)),
          iterationCount(0), totalTime(0.0), mean(0.0), median(0.0), stddev(0.0),
          min(std::numeric_limits<double>::max()), max(0.0) {}
};

// Benchmark suite for measuring specific functionality
class BenchmarkSuite {
public:
    BenchmarkSuite(const std::string& name, const std::string& description);
    ~BenchmarkSuite();
    
    // Run a benchmark measurement
    template<typename Func>
    BenchmarkResult run(const std::string& benchmarkName, 
                       const std::string& description,
                       Func&& func,
                       size_t iterations = 100);
    
    // Run a benchmark with setup function
    template<typename SetupFunc, typename Func>
    BenchmarkResult runWithSetup(const std::string& benchmarkName,
                                const std::string& description,
                                SetupFunc&& setup,
                                Func&& func,
                                size_t iterations = 100);
    
    // Run brain simulation benchmark
    BenchmarkResult brainSimulation(size_t neuronCount = 1000, 
                                   size_t steps = 1000,
                                   const std::string& configFile = "");
    
    // Run memory benchmark
    BenchmarkResult memoryBenchmark(size_t memorySizeMB = 100, 
                                   const std::string& operation = "allocate");
    
    // Run neuron lookup benchmark
    BenchmarkResult neuronLookupBenchmark(size_t neuronCount = 1000,
                                         size_t lookupCount = 10000);
    
    // Run spike processing benchmark
    BenchmarkResult spikeProcessingBenchmark(size_t neuronCount = 1000,
                                           float firingRate = 0.01f,
                                           size_t steps = 1000);
    
    // Run plasticity benchmark
    BenchmarkResult plasticityBenchmark(size_t neuronCount = 1000,
                                       size_t synapseCount = 5000,
                                       float plasticityRate = 0.01f);
    
    // Run memory benchmark
    BenchmarkResult memorySystemBenchmark(const std::string& memorySystem = "working_memory");
    
    // Run integration benchmark
    BenchmarkResult integrationBenchmark(const std::string& integrationType = "full_brain");
    
    // Get all benchmark results
    const std::vector<BenchmarkResult>& getResults() const { return results_; }
    
    // Get results by name
    const BenchmarkResult* getResult(const std::string& name) const;
    
    // Save results to file
    bool saveResults(const std::string& filepath) const;
    
    // Load results from file
    bool loadResults(const std::string& filepath);
    
    // Clear all results
    void clearResults() { results_.clear(); }
    
    // Generate performance report
    std::string generateReport() const;
    
    // Generate detailed report
    std::string generateDetailedReport() const;
    
    // Compare benchmarks
    static std::string compareBenchmarks(const std::vector<BenchmarkSuite*>& suites);
    
    // Create benchmark profile
    static std::string createBenchmarkProfile(const std::vector<BenchmarkResult>& results);
    
    // Validate benchmark results
    bool validateResults() const;
    
    // Normalize results
    void normalizeResults(const std::vector<BenchmarkSuite*>& referenceSuites);
    
private:
    std::string name_;
    std::string description_;
    std::vector<BenchmarkResult> results_;
    mutable std::mutex mutex_;
    
    // Helper methods
    void calculateStatistics(BenchmarkResult& result) const;
    double calculateMean(const std::vector<double>& samples) const;
    double calculateStddev(const std::vector<double>& samples, double mean) const;
    double calculateMedian(const std::vector<double>& samples) const;
    
    // Time measurement
    template<typename Func>
    double measureTime(Func&& func, size_t iterations) {
        auto start = std::chrono::high_resolution_clock::now();
        func();
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
        return duration.count() / 1000000.0;  // Convert to milliseconds
    }
    
    template<typename Func>
    double measureTimeWithIterations(Func&& func, size_t iterations) {
        auto start = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < iterations; ++i) {
            func();
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
        return duration.count() / 1000000.0 / iterations;  // Convert to milliseconds per iteration
    }
};

// Performance profiler for detailed profiling
class PerformanceProfiler {
public:
    PerformanceProfiler(const std::string& name);
    ~PerformanceProfiler();
    
    // Start profiling
    void start();
    
    // Stop profiling
    double stop();
    
    // Check if profiling is active
    bool isActive() const { return active_; }
    
    // Get total elapsed time
    double getTotalTime() const { return totalTime_; }
    
    // Reset timer
    void reset() { totalTime_ = 0.0; active_ = false; }
    
    // Get statistics
    struct ProfileStats {
        double minTime;
        double maxTime;
        double meanTime;
        double stddevTime;
        size_t sampleCount;
        double totalTime;
    };
    
    ProfileStats getStats() const;
    
    // Create benchmark from profiler
    BenchmarkResult createBenchmark(const std::string& name) const;
    
private:
    std::string name_;
    bool active_;
    double totalTime_;
    std::chrono::high_resolution_clock::time_point startTime_;
    std::vector<double> samples_;
    mutable std::mutex mutex_;
};

// Memory profiler for memory usage tracking
class MemoryProfiler {
public:
    MemoryProfiler(const std::string& name);
    ~MemoryProfiler();
    
    // Start memory profiling
    void start();
    
    // Stop memory profiling
    double stop();  // Returns peak memory usage in MB
    
    // Get current memory usage
    double getCurrentMemoryUsage() const;
    
    // Get peak memory usage
    double getPeakMemoryUsage() const { return peakMemoryUsage_; }
    
    // Reset profiler
    void reset() { peakMemoryUsage_ = 0.0; samples_.clear(); }
    
    // Get memory statistics
    struct MemoryStats {
        double minMemory;
        double maxMemory;
        double meanMemory;
        double stddevMemory;
        size_t sampleCount;
        double peakMemory;
        double currentMemory;
    };
    
    MemoryStats getStats() const;
    
    // Create benchmark from memory profiler
    BenchmarkResult createBenchmark(const std::string& name) const;
    
private:
    std::string name_;
    double peakMemoryUsage_;
    std::vector<double> samples_;
    mutable std::mutex mutex_;
    
    // Platform-specific memory tracking
    double getPlatformMemoryUsage();
};

// Benchmarking utilities
class BenchmarkUtils {
public:
    // Generate configuration for benchmarking
    static std::shared_ptr<Config> generateBenchmarkConfig(size_t neuronCount = 1000,
                                                             float connectionProbability = 0.1f,
                                                             const std::string& benchmarkType = "default");
    
    // Create brain for benchmarking
    static std::shared_ptr<Brain> createBenchmarkBrain(const std::shared_ptr<Config>& config);
    
    // Measure memory usage of an object
    static double measureMemoryUsage(const std::shared_ptr<Brain>& brain);
    
    // Measure CPU usage
    static double measureCPUUsage(const std::function<void()>& func);
    
    // Measure simulation performance
    static double measureSimulationPerformance(std::shared_ptr<Brain>& brain,
                                               size_t steps,
                                               bool enableVisualization = false);
    
    // Compare two benchmarks
    static std::string compareBenchmarks(const BenchmarkResult& result1,
                                         const BenchmarkResult& result2,
                                         const std::string& metric = "time");
    
    // Format benchmark results
    static std::string formatBenchmarkResult(const BenchmarkResult& result);
    
    // Save benchmark report
    static bool saveBenchmarkReport(const std::vector<BenchmarkSuite*>& suites,
                                    const std::string& filepath);
    
    // Load benchmark report
    static bool loadBenchmarkReport(std::vector<BenchmarkSuite*>& suites,
                                    const std::string& filepath);
    
    // Generate benchmark summary
    static std::string generateBenchmarkSummary(const std::vector<BenchmarkSuite*>& suites);
    
    // Create benchmark directory
    static bool createBenchmarkDirectory(const std::string& directory);
    
    // Clean up benchmark files
    static bool cleanupBenchmarkFiles(const std::string& directory);
    
    // Get system information
    static std::string getSystemInfo();
    
    // Get benchmark system info
    static std::string getBenchmarkSystemInfo();
    
    // Check if benchmarking is available
    static bool isBenchmarkingAvailable();
    
    // Initialize benchmarking
    static bool initializeBenchmarking();
    
    // Cleanup benchmarking
    static bool cleanupBenchmarking();
};

// Global benchmark manager
class BenchmarkManager {
public:
    static BenchmarkManager& getInstance() {
        static BenchmarkManager instance;
        return instance;
    }
    
    // Add benchmark suite
    void addSuite(std::unique_ptr<BenchmarkSuite> suite);
    
    // Remove benchmark suite
    bool removeSuite(const std::string& name);
    
    // Get benchmark suite
    BenchmarkSuite* getSuite(const std::string& name);
    
    // Get all benchmark suites
    const std::vector<BenchmarkSuite*>& getSuites() const { return suites_; }
    
    // Run all benchmarks
    void runAllBenchmarks();
    
    // Save all results
    bool saveAllResults(const std::string& directory) const;
    
    // Load all results
    bool loadAllResults(const std::string& directory);
    
    // Clear all results
    void clearAllResults() {
        for (auto& suite : suites_) {
            suite->clearResults();
        }
    }
    
    // Generate comprehensive report
    std::string generateComprehensiveReport() const;
    
    // Create benchmark profile
    std::string createBenchmarkProfile() const;
    
    // Compare all benchmarks
    std::string compareAllBenchmarks() const;
    
    // Export benchmarks to different formats
    bool exportToJSON(const std::string& filepath) const;
    bool exportToCSV(const std::string& filepath) const;
    bool exportToYAML(const std::string& filepath) const;
    
private:
    BenchmarkManager() = default;
    ~BenchmarkManager() = default;
    
    std::vector<std::unique_ptr<BenchmarkSuite>> suites_;
    mutable std::mutex mutex_;
};

// Convenience functions for quick benchmarking
namespace Bench {
    // Quick brain simulation benchmark
    BenchmarkResult brainSimulation(size_t neuronCount = 1000, size_t steps = 1000);
    
    // Quick neuron lookup benchmark
    BenchmarkResult neuronLookup(size_t neuronCount = 1000, size_t lookupCount = 10000);
    
    // Quick spike processing benchmark
    BenchmarkResult spikeProcessing(size_t neuronCount = 1000, float firingRate = 0.01f, size_t steps = 1000);
    
    // Quick memory benchmark
    BenchmarkResult memory(size_t memorySizeMB = 100);
    
    // Quick plasticity benchmark
    BenchmarkResult plasticity(size_t neuronCount = 1000, size_t synapseCount = 5000);
    
    // Quick integration benchmark
    BenchmarkResult integration(const std::string& integrationType = "full_brain");
    
    // Run all quick benchmarks
    std::vector<BenchmarkResult> runAllQuickBenchmarks();
    
    // Generate quick report
    std::string generateQuickReport();
    
    // Save quick benchmark results
    bool saveQuickBenchmarkResults(const std::string& filepath);
    
    // Load quick benchmark results
    bool loadQuickBenchmarkResults(const std::string& filepath);
}

} // namespace nlm

#endif // BENCHMARK_HPP
