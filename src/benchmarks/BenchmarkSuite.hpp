#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>
#include <functional>
#include <algorithm>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <mutex>
#include <thread>
#include <future>
#include "../core/Logger.hpp"
#include "Profiler.hpp"
#include "PerformanceAnalyzer.hpp"

namespace nlm {

// Benchmark result for a single measurement
struct BenchmarkResult {
    std::string benchmarkName;
    Timestamp timestamp;
    double executionTime; // milliseconds
    double cpuTime; // milliseconds
    double memoryUsage; // bytes
    double throughput; // operations per second
    double efficiency; // percentage (0-100)
    size_t iterationCount;
    bool passed;
    std::string errorMessage;
    std::map<std::string, double> componentMetrics;
    std::vector<double> rawMeasurements;
};

// Configuration for benchmarking
struct BenchmarkConfig {
    std::string name;
    size_t iterationCount;
    double timeout; // seconds
    bool warmUpEnabled;
    size_t warmUpIterations;
    std::map<std::string, double> parameters;
    std::vector<std::string> enabledComponents;
    bool detailedMetricsEnabled;
    bool profileEnabled;
    
    BenchmarkConfig() : iterationCount(100), timeout(60.0), warmUpEnabled(true), 
                        warmUpIterations(10), detailedMetricsEnabled(true), 
                        profileEnabled(false) {}
};

// Performance baseline for comparison
struct PerformanceBaseline {
    std::string name;
    Timestamp createdTime;
    Timestamp lastUpdated;
    std::map<std::string, double> metrics;
    double stabilityScore; // 0.0 to 1.0
    size_t sampleCount;
    double confidenceInterval; // standard deviation
};

// Benchmark suite for comprehensive performance testing
class BenchmarkSuite {
public:
    BenchmarkSuite();
    ~BenchmarkSuite();

    // Suite management
    bool initialize();
    void shutdown();
    bool addBenchmark(const std::string& name, std::function<void()> benchmarkFunc);
    bool addBenchmark(const BenchmarkConfig& config, std::function<void()> benchmarkFunc);
    
    // Benchmark execution
    BenchmarkResult runBenchmark(const std::string& name);
    BenchmarkResult runBenchmark(const BenchmarkConfig& config);
    std::vector<BenchmarkResult> runBenchmarks(const std::vector<std::string>& benchmarkNames);
    
    // Performance testing
    void runNeuralNetworkBenchmarks();
    void runMemorySystemBenchmarks();
    void runPlasticityBenchmarks();
    void runIntegrationBenchmarks();
    void runIobenchmarks();
    void runScalabilityBenchmarks();
    
    // Performance regression testing
    bool isRegressionDetected(const std::string& benchmarkName, const BenchmarkResult& result);
    double calculateRegressionPercent(const std::string& benchmarkName, const BenchmarkResult& result);
    bool runRegressionTest(const std::string& baselineName);
    
    // Performance comparison
    void setPerformanceBaseline(const std::string& baselineName);
    double compareToBaseline(const std::string& benchmarkName, const BenchmarkResult& result);
    void generateComparisonReport(const std::string& filename);
    
    // Results management
    const BenchmarkResult& getLastResult(const std::string& benchmarkName) const;
    const std::vector<BenchmarkResult>& getBenchmarkHistory(const std::string& benchmarkName) const;
    const std::vector<BenchmarkResult>& getAllResults() const;
    
    // Analysis and reporting
    void analyzePerformanceTrends();
    void identifyPerformancePatterns();
    void generateBenchmarkReport(const std::string& filename);
    void exportResults(const std::string& filename);
    
    // Configuration and settings
    void setIterationCount(size_t count) { defaultConfig.iterationCount = count; }
    void setTimeout(double timeout) { defaultConfig.timeout = timeout; }
    void enableProfiling(bool enable) { defaultConfig.profileEnabled = enable; }
    
    // Statistics and reporting
    size_t getBenchmarkCount() const;
    size_t getSuccessfulRunCount() const;
    size_t getFailedRunCount() const;
    double getAverageExecutionTime(const std::string& benchmarkName) const;
    double getMinExecutionTime(const std::string& benchmarkName) const;
    double getMaxExecutionTime(const std::string& benchmarkName) const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Standardized benchmark functions for NLM components
namespace benchmarks {
    
    // Neural network benchmarks
    void benchmarkNeuronInitialization();
    void benchmarkSynapseCreation();
    void benchmarkSpikePropagation();
    void benchmarkNeuralConnectivity();
    
    // Memory system benchmarks
    void benchmarkWorkingMemoryAccess();
    void benchmarkEpisodicMemoryStorage();
    void benchmarkMemoryConsolidation();
    void benchmarkMemoryRetrieval();
    
    // Plasticity benchmarks
    void benchmarkSTDPComputation();
    void benchmarkHebbianLearning();
    void benchmarkStructuralPlasticity();
    void benchmarkHomeostaticPlasticity();
    
    // Integration benchmarks
    void benchmarkIntegrationLoop();
    void benchmarkSensorProcessing();
    void benchmarkMotorOutput();
    void benchmarkDecisionMaking();
    
    // I/O benchmarks
    void benchmarkDiskIO();
    void benchmarkNetworkIO();
    void benchmarkFileProcessing();
    void benchmarkDataSerialization();
    
    // Scalability benchmarks
    void benchmarkScaleNeurons(size_t neuronCount);
    void benchmarkScaleSynapses(size_t synapseCount);
    void benchmarkScaleMemory(size_t memorySize);
    void benchmarkScaleTimeStep(size_t timeSteps);
    
} // namespace benchmarks

} // namespace nlm