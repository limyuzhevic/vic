// Comprehensive Benchmarking and Profiling Tools
// Advanced performance measurement and analysis for NLM neural simulations

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <thread>
#include <future>

namespace nlm {

class PerformanceProfiler {
public:
    PerformanceProfiler();
    ~PerformanceProfiler();
    
    void startMeasurement(const std::string& operationName);
    void stopMeasurement(const std::string& operationName);
    double getElapsedTime(const std::string& operationName) const;
    
    // Memory profiling
    void startMemoryMeasurement();
    void stopMemoryMeasurement();
    size_t getMemoryUsage() const;
    
    // CPU profiling
    void startCPUMeasurement();
    void stopCPUMeasurement();
    double getCPUUsage() const;
    
    // Detailed profiling data
    struct MeasurementData {
        std::string operationName;
        std::chrono::duration<double> elapsedTime;
        size_t memoryUsed;
        double cpuUsage;
        std::chrono::system_clock::time_point timestamp;
        size_t callCount;
    };
    
    // Report generation
    std::string generateReport() const;
    void saveReport(const std::string& filename) const;
    void printReport() const;
    
    // Performance analysis
    double getAverageTime(const std::string& operationName) const;
    double getMaxTime(const std::string& operationName) const;
    double getMinTime(const std::string& operationName) const;
    size_t getTotalCalls(const std::string& operationName) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

class NeuralNetworkBenchmarker {
public:
    NeuralNetworkBenchmarker();
    ~NeuralNetworkBenchmarker();
    
    void attach(std::shared_ptr<class Brain> brain);
    
    // Core benchmarks
    void benchmarkNeurons(size_t neuronCount = 1000, int iterations = 10);
    void benchmarkSynapses(size_t synapseCount = 5000, int iterations = 10);
    void benchmarkPlasticity(int iterations = 10);
    void benchmarkMemory(size_t memorySize = 100000, int iterations = 5);
    
    // Advanced benchmarks
    void benchmarkSimulationSpeed(double timestep = 0.001, size_t steps = 10000);
    void benchmarkScalability(const std::vector<size_t>& neuronCounts);
    void benchmarkPlasticityRules(int trials = 100);
    void benchmarkNetworkTopology(size_t regionCount = 4, size_t neuronsPerRegion = 250);
    
    // Stress tests
    void stressTestMaximumNeurons(size_t maxNeurons = 50000);
    void stressTestMaximumConnections(size_t maxConnections = 500000);
    void stressTestHighPlasticity(int duration = 1000);
    void stressTestComplexTopology(size_t regions = 8, size_t neurons = 2000);
    
    // Profiling benchmarks
    void profileNeuronDynamics(int steps = 1000);
    void profileSpikePropagation(int spikes = 1000);
    void profileSynapticIntegration(int integrations = 10000);
    void profilePlasticityUpdate(int updates = 1000);
    
    // Benchmark results
    struct BenchmarkResult {
        std::string testName;
        double elapsedTime;
        double operationsPerSecond;
        size_t memoryUsed;
        double cpuUsage;
        std::string status; // "PASSED", "FAILED", "SKIPPED"
        std::string details;
        std::map<std::string, double> metrics;
    };
    
    std::vector<BenchmarkResult> getBenchmarkResults() const;
    void clearResults();
    
    // Report generation
    std::string generateBenchmarkReport() const;
    void saveBenchmarkResults(const std::string& filename) const;
    void printBenchmarkSummary() const;
    
    // Analysis
    double calculateEfficiency(const BenchmarkResult& result) const;
    bool isBenchmarkViable(const BenchmarkResult& result, double threshold = 1.0) const;
    std::string getPerformanceGrade(double opsPerSecond) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

class MemoryProfiler {
public:
    MemoryProfiler();
    ~MemoryProfiler();
    
    void attach(std::shared_ptr<class Brain> brain);
    
    // Memory tracking
    void trackMemoryAllocation(size_t size, const std::string& context = "");
    void trackMemoryDeallocation(size_t size, const std::string& context = "");
    
    // Memory usage analysis
    size_t getCurrentMemoryUsage() const;
    size_t getPeakMemoryUsage() const;
    size_t getAverageMemoryUsage() const;
    
    // Memory leaks detection
    bool detectMemoryLeaks() const;
    std::vector<std::string> getMemoryLeakContexts() const;
    
    // Memory efficiency
    double calculateMemoryEfficiency() const;
    std::string getMemoryUsageReport() const;
    
    // Allocation patterns
    struct AllocationInfo {
        size_t size;
        std::string context;
        std::chrono::system_clock::time_point timestamp;
        bool isFreed;
        std::chrono::duration<double> lifetime;
    };
    
    std::vector<AllocationInfo> getAllocationHistory() const;
    size_t getTotalAllocatedMemory() const;
    size_t getTotalDeallocatedMemory() const;
    
    // Export capabilities
    void exportMemoryProfile(const std::string& filename) const;
    void exportAllocationTimeline(const std::string& filename) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

class ScalabilityTester {
public:
    ScalabilityTester();
    ~ScalabilityTester();
    
    void attach(std::shared_ptr<class Brain> brain);
    
    // Scale tests
    void testLinearScaling(const std::vector<size_t>& neuronCounts);
    void testQuadraticScaling(const std::vector<size_t>& synapseCounts);
    void testMemoryScaling(const std::vector<size_t>& memorySizes);
    
    // Performance scaling analysis
    struct ScalingResult {
        size_t neurons;
        double timePerNeuron;
        double memoryPerNeuron;
        double connectionsPerNeuron;
        double efficiency;
    };
    
    std::vector<ScalingResult> analyzeScaling(const std::vector<size_t>& neuronCounts) const;
    
    // Scaling laws
    std::string getScalingLawName(size_t neurons) const;
    double calculateScalingExponent(const std::vector<ScalingResult>& results) const;
    
    // Bottleneck detection
    bool detectMemoryBottleneck() const;
    bool detectCPUBottleneck() const;
    bool detectNetworkBottleneck() const;
    
    // Optimization recommendations
    std::vector<std::string> getOptimizationRecommendations() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Benchmark configuration
struct BenchmarkConfig {
    size_t neuronCount = 1000;
    size_t synapseCount = 5000;
    double simulationTimestep = 0.001;
    size_t simulationSteps = 1000;
    int plasticityIterations = 10;
    size_t memoryTestSize = 100000;
    std::string outputFormat = "JSON";
    bool enableDetailedProfiling = true;
    bool saveIntermediateResults = false;
    std::string resultsDirectory = "benchmark_results";
    
    // Performance thresholds
    double minPerformanceThreshold = 1.0; // ops/second
    size_t maxMemoryUsage = 1024 * 1024 * 1024; // 1GB
    double maxCPUMemoryUsage = 0.9; // 90%
    
    // Quality criteria
    double minAccuracyThreshold = 0.95;
    double minStabilityThreshold = 0.99;
};

// Benchmark utilities
namespace BenchmarkUtils {
    // File operations
    bool ensureBenchmarkDirectory(const std::string& directory);
    std::string generateBenchmarkFilename(const std::string& prefix, 
                                          const std::string& extension);
    
    // Data formatting
    std::string formatBenchmarkResult(const BenchmarkResult& result);
    std::string formatPerformanceMetric(double value, const std::string& unit);
    std::string formatMemorySize(size_t bytes);
    std::string formatTime(double seconds);
    
    // Report generation
    std::string generateHTMLReport(const std::vector<BenchmarkResult>& results);
    std::string generateJSONReport(const std::vector<BenchmarkResult>& results);
    std::string generateCSVReport(const std::vector<BenchmarkResult>& results);
    
    // Analysis
    double calculateThroughput(const BenchmarkResult& result);
    double calculateEfficiency(const BenchmarkResult& result);
    std::string gradePerformance(double opsPerSecond);
    
    // Validation
    bool validateBenchmarkConfig(const BenchmarkConfig& config);
    std::string getBenchmarkValidationError(const BenchmarkConfig& config);
}

} // namespace nlm
