#ifndef NLM_BENCHMARK_H
#define NLM_BENCHMARK_H

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <chrono>
#include <vector>
#include <string>
#include <iostream>
#include <memory>
#include <algorithm>
#include <iomanip>

namespace nlm {

// Benchmark metrics structure
struct BenchmarkMetrics {
    size_t neurons;
    size_t synapses;
    size_t steps;
    double totalTime;
    double avgStepTime;
    double stepsPerSecond;
    size_t totalSpikes;
    double avgFiringRate;
    double memoryUsageMB;
    std::vector<double> stepTimes;  // For detailed analysis
    
    BenchmarkMetrics(size_t n = 0, size_t s = 0, size_t st = 0)
        : neurons(n), synapses(s), steps(st), totalTime(0.0), avgStepTime(0.0), 
          stepsPerSecond(0.0), totalSpikes(0), avgFiringRate(0.0), memoryUsageMB(0.0) {}
};

// Benchmark class for performance testing
class Benchmark {
public:
    Benchmark() = default;
    ~Benchmark() = default;
    
    // Run benchmark with given configuration
    BenchmarkMetrics runBenchmark(std::shared_ptr<Config> config, 
                                  size_t steps = 1000,
                                  bool detailedTiming = true,
                                  bool logProgress = true);
    
    // Run comparison benchmark (different configurations)
    std::vector<BenchmarkMetrics> runComparison(const std::vector<std::shared_ptr<Config>>& configs,
                                                 size_t steps = 1000);
    
    // Run performance profile (analyze different aspects)
    struct PerformanceProfile {
        double neuronSetupTime;
        double stepExecutionTime;
        double plasticityTime;
        double memoryAccessTime;
        double spikePropagationTime;
    };
    
    PerformanceProfile runPerformanceProfile(std::shared_ptr<Config> config, size_t steps = 500);
    
    // Memory usage measurement
    double measureMemoryUsage() const;
    
    // Performance statistics
    struct PerformanceStats {
        double min, max, mean, median, stddev, p95, p99;
        
        PerformanceStats() : min(0), max(0), mean(0), median(0), stddev(0), p95(0), p99(0) {}
    };
    
    PerformanceStats calculateStatistics(const std::vector<double>& values) const;
    
    // Generate benchmark report
    std::string generateReport(const BenchmarkMetrics& metrics) const;
    std::string generateComparisonReport(const std::vector<BenchmarkMetrics>& metrics) const;
    
    // Validate configuration for benchmarking
    static bool validateBenchmarkConfig(std::shared_ptr<Config> config);
    
    // Get default benchmark configurations
    static std::vector<std::shared_ptr<Config>> getBenchmarkConfigs();

private:
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point endTime;
    
    void recordStartTime() {
        startTime = std::chrono::high_resolution_clock::now();
    }
    
    void recordEndTime() {
        endTime = std::chrono::high_resolution_clock::now();
    }
    
    double getElapsedSeconds() const {
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
        return duration.count() / 1000000.0;
    }
};

// Benchmark suite for comprehensive testing
class BenchmarkSuite {
public:
    BenchmarkSuite() = default;
    ~BenchmarkSuite() = default;
    
    // Run all benchmarks
    void runAllBenchmarks(std::shared_ptr<Config> config, size_t steps = 1000);
    
    // Run specific benchmarks
    void runConnectivityBenchmark(std::shared_ptr<Config> config, size_t steps);
    void runPlasticityBenchmark(std::shared_ptr<Config> config, size_t steps);
    void runScalingBenchmark(std::shared_ptr<Config> config, size_t steps);
    void runMemoryBenchmark(std::shared_ptr<Config> config, size_t steps);
    void runNeuromodulationBenchmark(std::shared_ptr<Config> config, size_t steps);
    
    // Generate comprehensive benchmark report
    std::string generateFullReport() const;
    
    // Save benchmark results to file
    bool saveResults(const std::string& filename) const;
    
    // Load benchmark results from file
    bool loadResults(const std::string& filename);
    
    // Getters for benchmark results
    const std::vector<BenchmarkMetrics>& getBenchmarkResults() const { return results; }
    void setBenchmarkResults(const std::vector<BenchmarkMetrics>& metrics) { results = metrics; }

private:
    std::vector<BenchmarkMetrics> results;
    Benchmark benchmark;
};

// Simple timing utility
class Timer {
public:
    Timer() : startTime(std::chrono::high_resolution_clock::now()) {}
    ~Timer() = default;
    
    void reset() {
        startTime = std::chrono::high_resolution_clock::now();
    }
    
    double elapsedSeconds() const {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
        return duration.count() / 1000000.0;
    }
    
    long long elapsedMicroseconds() const {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
        return duration.count();
    }

private:
    std::chrono::high_resolution_clock::time_point startTime;
};

// Progress indicator for long-running benchmarks
class BenchmarkProgress {
public:
    BenchmarkProgress(size_t totalSteps, bool showPercentage = true, bool showETA = true)
        : totalSteps(totalSteps), showPercentage(showPercentage), showETA(showETA),
          startTime(std::chrono::high_resolution_clock::now()), lastUpdateTime(startTime) {}
    
    void update(size_t currentStep, const std::string& message = "") {
        currentStep++;
        
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime).count();
        
        if (showPercentage) {
            double percentage = (100.0 * currentStep) / totalSteps;
            NLM_LOG_INFO("Progress: " + std::to_string(currentStep) + "/" + std::to_string(totalSteps) +
                        " (" + std::to_string(percentage) + "%)");
        }
        
        if (showETA && currentStep > 0) {
            double eta = (elapsed / currentStep) * (totalSteps - currentStep);
            int etaMinutes = static_cast<int>(eta / 60);
            int etaSeconds = static_cast<int>(eta) % 60;
            NLM_LOG_INFO("ETA: " + std::to_string(etaMinutes) + "m " + std::to_string(etaSeconds) + "s");
        }
        
        if (!message.empty()) {
            NLM_LOG_INFO("Status: " + message);
        }
        
        lastUpdateTime = currentTime;
    }
    
    void complete() {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto totalElapsed = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime).count();
        
        NLM_LOG_INFO("Benchmark completed in " + std::to_string(totalElapsed) + " seconds");
    }

private:
    size_t totalSteps;
    size_t currentStep = 0;
    bool showPercentage;
    bool showETA;
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point lastUpdateTime;
};

} // namespace nlm

#endif // NLM_BENCHMARK_H
