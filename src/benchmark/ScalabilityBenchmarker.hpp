#pragma once

#include "BenchmarkRunner.hpp"
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <mutex>
#include <atomic>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <functional>

namespace nlm {

// Scalability analysis results
struct ScalabilityMetrics {
    size_t neuronCount;                // Number of neurons
    size_t synapseCount;                // Number of synapses
    size_t simulationSteps;            // Number of simulation steps
    double executionTime;              // Total execution time in seconds
    double cpuTime;                    // CPU time in seconds
    size_t memoryUsage;                // Peak memory usage in bytes
    double throughput;                 // Operations per second
    double memoryPerNeuron;            // Memory usage per neuron
    double timePerNeuron;              // Time per neuron
    double scalabilityFactor;          // Scaling factor relative to baseline
    double efficiency;                  // Efficiency (scaled performance / ideal scaling)
    double communicationOverhead;      // Communication overhead percentage
    double synchronizationCost;        // Synchronization cost in seconds
    
    // Multi-dimensional scalability
    std::map<std::string, double> dimensionMetrics; // Metrics for different dimensions
    
    // Detailed timing breakdown
    std::map<std::string, double> phaseTimes; // Times for different phases (setup, simulation, teardown)
    
    // Scalability classification
    std::string scalabilityClass;      // Classification (linear, sublinear, superlinear)
    double amdahlsLawLimit;            // Amdahl's law theoretical limit
    double ginsburgsLawLimit;          // Gustafson's law theoretical limit
    
    // Timestamp
    std::chrono::high_resolution_clock::time_point timestamp;
};

// Scalability test configuration
struct ScalabilityConfig {
    std::vector<size_t> neuronScales;           // Different neuron counts to test
    std::vector<size_t> stepScales;             // Different step counts to test
    std::string scalabilityDimension;          // Dimension to scale (neurons, steps, threads, memory)
    bool includeOverheadAnalysis;               // Include overhead analysis
    bool includeCommunicationCost;              // Include communication cost analysis
    bool includeAmdahlsLawAnalysis;              // Include Amdahl's law analysis
    bool includeGustafsonsLawAnalysis;          // Include Gustafson's law analysis
    double measurementInterval;                 // Measurement interval in seconds
    size_t warmUpSteps;                         // Number of warm-up steps
    bool enableParallelExecution;               // Enable parallel execution for different scales
    size_t maxConcurrency;                      // Maximum concurrency for parallel execution
};

class ScalabilityBenchmarker : public Benchmark {
private:
    std::string benchmarkName;
    BenchmarkCategory category;
    ScalabilityConfig config;
    
    // Benchmark configuration
    size_t baselineNeuronCount;
    size_t baselineStepCount;
    
    // Benchmark state
    bool isSetup;
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point endTime;
    std::mutex scalabilityMutex;
    
    // Data collection
    std::vector<ScalabilityMetrics> scalabilityResults;
    std::vector<BenchmarkMetrics> detailedResults;
    
    // Analysis functions
    void calculateAmdahlsLawLimit(double serialFraction) const;
    void calculateGustafsonsLawLimit(size_t numProcessors, size_t baselineWork) const;
    double calculateSerialFraction(const std::vector<ScalabilityMetrics>& results) const;
    double calculateParallelEfficiency(const ScalabilityMetrics& metrics, size_t numThreads) const;
    std::string classifyScalability(double efficiency) const;
    
    // Overhead analysis
    void calculateCommunicationOverhead(const ScalabilityMetrics& metrics) const;
    void calculateSynchronizationCost(const ScalabilityMetrics& metrics) const;
    void calculateMemoryOverhead(const ScalabilityMetrics& metrics) const;
    void calculateCpuOverhead(const ScalabilityMetrics& metrics) const;
    
    // Dimension-specific analysis
    void analyzeNeuronScalability() const;
    void analyzeStepScalability() const;
    void analyzeThreadScalability() const;
    void analyzeMemoryScalability() const;
    
    // Statistical analysis
    double calculateCoefficientOfVariation(const std::vector<double>& data) const;
    double calculateMean(const std::vector<double>& data) const;
    double calculateMedian(const std::vector<double>& data) const;
    double calculateStandardDeviation(const std::vector<double>& data) const;
    
public:
    ScalabilityBenchmarker(size_t baselineNeurons = 1000,
                          size_t baselineSteps = 1000,
                          const ScalabilityConfig& config = ScalabilityConfig());
    ~ScalabilityBenchmarker() override = default;
    
    // Benchmark interface implementation
    std::string getName() const override;
    BenchmarkCategory getCategory() const override;
    void setup() override;
    BenchmarkMetrics run() override;
    void teardown() override;
    bool validate() const override;
    
    // Configuration
    void setBaselineNeuronCount(size_t count);
    void setBaselineStepCount(size_t steps);
    void setScalabilityConfig(const ScalabilityConfig& config);
    
    size_t getBaselineNeuronCount() const;
    size_t getBaselineStepCount() const;
    const ScalabilityConfig& getScalabilityConfig() const;
    
    // Scalability testing
    ScalabilityMetrics runNeuronScalabilityTest(size_t neuronCount) const;
    ScalabilityMetrics runStepScalabilityTest(size_t stepCount) const;
    std::vector<ScalabilityMetrics> runFullScalabilitySuite() const;
    
    // Dimension-specific scalability tests
    std::vector<ScalabilityMetrics> runNeuronScalabilitySuite(const std::vector<size_t>& neuronCounts) const;
    std::vector<ScalabilityMetrics> runStepScalabilitySuite(const std::vector<size_t>& stepCounts) const;
    std::vector<ScalabilityMetrics> runThreadScalabilitySuite(const std::vector<size_t>& threadCounts) const;
    
    // Scalability analysis
    double calculateStrongScaling(const std::vector<ScalabilityMetrics>& results) const;
    double calculateWeakScaling(const std::vector<ScalabilityMetrics>& results) const;
    double calculateSpeedup(const ScalabilityMetrics& baseline, const ScalabilityMetrics& test) const;
    double calculateEfficiency(const ScalabilityMetrics& metrics, size_t numThreads) const;
    
    // Bottleneck analysis
    void identifyBottlenecks(const std::vector<ScalabilityMetrics>& results) const;
    void analyzeMemoryBottlenecks(const std::vector<ScalabilityMetrics>& results) const;
    void analyzeCpuBottlenecks(const std::vector<ScalabilityMetrics>& results) const;
    void analyzeCommunicationBottlenecks(const std::vector<ScalabilityMetrics>& results) const;
    
    // Prediction and optimization
    double predictPerformance(size_t neuronCount, size_t stepCount) const;
    size_t predictOptimalNeuronCount(size_t memoryLimit) const;
    size_t predictOptimalStepCount(double timeLimit) const;
    std::vector<size_t> getRecommendedScalabilityConfiguration(size_t targetPerformance) const;
    
    // Analysis results
    std::vector<ScalabilityMetrics> getScalabilityResults() const;
    std::vector<BenchmarkMetrics> getDetailedResults() const;
    ScalabilityMetrics getBestConfiguration() const;
    
    // Export and reporting
    void exportScalabilityResults(const std::string& filename, const std::string& format = "json") const;
    std::string formatScalabilityResults(const std::string& format = "text") const;
    
    // Utility functions
    static std::string scalabilityClassToString(const std::string& className);
    static double calculateAmdahlsLaw(double serialFraction, size_t numProcessors);
    static double calculateGustafsonsLaw(double parallelFraction, size_t numProcessors, size_t baselineWork);
    static std::vector<size_t> generateScaleFactors(size_t start, size_t end, double factor = 2.0);
    
private:
    BenchmarkMetrics convertScalabilityMetricsToBenchmark(const ScalabilityMetrics& scalability) const;
    void collectScalabilityData(size_t neuronCount, size_t stepCount);
    ScalabilityMetrics runSingleScalabilityTest(size_t neuronCount, size_t stepCount) const;
};

} // namespace nlm