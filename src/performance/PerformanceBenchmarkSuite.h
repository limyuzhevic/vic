// PerformanceBenchmarkSuite.h
// Comprehensive benchmark suite for NLM brain system

#ifndef NLM_PERFORMANCE_BENCHMARK_SUITE_H
#define NLM_PERFORMANCE_BENCHMARK_SUITE_H

#include "performance/PerformanceProfiler.h"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "prediction/PredictionSystem.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "core/Config/Config.hpp"
#include <vector>
#include <string>
#include <chrono>
#include <memory>
#include <iostream>
#include <iomanip>

// Forward declarations
namespace nlm {
    class Brain;
    class Neuron;
    class Synapse;
    class WorkingMemory;
    class EpisodicMemory;
    class PredictionSystem;
    class Neuromodulator;
}

// Benchmark configuration
struct BenchmarkConfig {
    std::string name;
    std::string description;
    
    // Scale factors
    size_t neuronCount = 100;
    size_t synapseCount = 1000;
    size_t steps = 1000;
    
    // Test parameters
    bool useDifferentNetworkSizes = true;  // Test multiple sizes
    bool useDifferentStepCounts = true;    // Test different step counts
    bool testMemorySystems = true;         // Test memory systems specifically
    bool testPrediction = true;            // Test prediction system
    bool testNeuromodulation = true;       // Test neuromodulation
    bool testPlasticity = true;            // Test plasticity
    bool testDevelopment = true;           // Test development
    
    // Brain configuration
    std::shared_ptr<nlm::Config> brainConfig;
    
    BenchmarkConfig(const std::string& name = "Benchmark", 
                   const std::string& description = "",
                   size_t neurons = 100,
                   size_t synapses = 1000,
                   size_t steps = 1000)
        : name(name), description(description), neuronCount(neurons), 
          synapseCount(synapses), steps(steps) {
        
        if (!brainConfig) {
            brainConfig = std::make_shared<nlm::Config>();
        }
        
        brainConfig->set("neuron_count", static_cast<int64_t>(neuronCount), nlm::ConfigSource::Default);
        brainConfig->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        brainConfig->set("connection_probability", 0.1f, nlm::ConfigSource::Default);
        brainConfig->set("stdp_ltp_weight", 0.01f, nlm::ConfigSource::Default);
        brainConfig->set("stdp_ltd_weight", 0.012f, nlm::ConfigSource::Default);
    }
};

// Individual benchmark result
struct BenchmarkResult {
    std::string benchmarkName;
    std::string category;
    size_t neurons;
    size_t synapses;
    size_t steps;
    double totalTime;        // seconds
    double timePerStep;      // microseconds per step
    double stepsPerSecond;   // steps per second
    double memoryUsage;      // bytes
    double cacheEfficiency;  // percentage
    double instructionCount; // approximate instruction count
    
    // Component-specific metrics
    double neuronLIFTime;    // time spent in neuron LIF integration
    double synapticTime;     // time spent in synaptic transmission
    double memoryTime;       // time spent in memory operations
    double predictionTime;   // time spent in prediction
    double neuromodTime;     // time spent in neuromodulation
    double plasticityTime;   // time spent in plasticity
    
    // Statistics
    double stddevTime;       // standard deviation
    double minTime;          // minimum time
    double maxTime;          // maximum time
    double throughput;       // operations per second
    
    BenchmarkResult() : neurons(0), synapses(0), steps(0), totalTime(0.0),
                       timePerStep(0.0), stepsPerSecond(0.0), memoryUsage(0.0),
                       cacheEfficiency(0.0), instructionCount(0.0),
                       neuronLIFTime(0.0), synapticTime(0.0), memoryTime(0.0),
                       predictionTime(0.0), neuromodTime(0.0), plasticityTime(0.0),
                       stddevTime(0.0), minTime(0.0), maxTime(0.0), throughput(0.0) {}
    
    BenchmarkResult(const std::string& name, const std::string& category, 
                   size_t neurons, size_t synapses, size_t steps)
        : benchmarkName(name), category(category), neurons(neurons), 
          synapses(synapses), steps(steps) {}
};

// Brain component benchmarks
class BrainComponentBenchmarks {
public:
    static BenchmarkResult benchmarkBrainInitialization(size_t neuronCount, size_t steps);
    static BenchmarkResult benchmarkBrainStep(size_t neuronCount, size_t steps);
    static BenchmarkResult benchmarkNeuronLIF(size_t neuronCount);
    static BenchmarkResult benchmarkSynapticTransmission(size_t synapseCount);
    static BenchmarkResult benchmarkMemorySystem(size_t neurons);
    static BenchmarkResult benchmarkPrediction(size_t neurons);
    static BenchmarkResult benchmarkNeuromodulation(size_t neurons);
    static BenchmarkResult benchmarkPlasticity(size_t synapses);
    static BenchmarkResult benchmarkDevelopment(size_t neurons);
    static BenchmarkResult benchmarkIntegrationTest(size_t neuronCount, size_t steps);
};

// Memory profiler
class MemoryProfiler {
public:
    struct MemoryStats {
        size_t totalAllocations;
        size_t totalDeallocations;
        size_t netLeaks;
        size_t peakUsage;
        size_t currentUsage;
        double allocationRate;
        double deallocationRate;
        std::string leakAnalysis;
        
        MemoryStats() : totalAllocations(0), totalDeallocations(0), netLeaks(0),
                       peakUsage(0), currentUsage(0), allocationRate(0.0),
                       deallocationRate(0.0) {}
    };
    
    static MemoryStats profileMemoryUsage(const std::string& operation, 
                                          size_t size, double duration);
    static MemoryStats analyzeMemoryLeaks();
    static void trackMemoryAllocation(size_t size, void* address,
                                      const std::string& sourceFile = "",
                                      int sourceLine = 0);
    static void trackMemoryDeallocation(void* address);
};

// CPU profiler
class CPUProfiler {
public:
    struct CPUStats {
        uint64_t instructionsRetired;
        uint64_t cacheMissesL1;
        uint64_t cacheMissesL2;
        uint64_t cacheMissesL3;
        uint64_t cycles;
        double instructionsPerCycle;
        double cacheMissRate;
        double efficiency;
        
        CPUStats() : instructionsRetired(0), cacheMissesL1(0), cacheMissesL2(0),
                    cacheMissesL3(0), cycles(0), instructionsPerCycle(0.0),
                    cacheMissRate(0.0), efficiency(0.0) {}
    };
    
    static CPUStats profileCPUCycle(const std::string& operation, double duration);
    static CPUStats getAverageCPUStats(const std::vector<CPUStats>& stats);
    static std::vector<CPUStats> profileCPULoad(size_t durationSeconds);
};

// Performance Benchmark Suite
class PerformanceBenchmarkSuite {
private:
    PerformanceProfiler profiler;
    PerformanceProfilerConfig profilerConfig;
    std::vector<BenchmarkResult> results;
    std::vector<std::string> benchmarkLog;
    
    // Helper methods
    void printBenchmarkHeader(const BenchmarkConfig& config) const;
    void printBenchmarkResult(const BenchmarkResult& result) const;
    void logBenchmark(const std::string& message);
    double calculateThroughput(size_t operations, double time) const;
    
public:
    PerformanceBenchmarkSuite(const PerformanceProfilerConfig& config = PerformanceProfilerConfig());
    ~PerformanceBenchmarkSuite();
    
    // Core benchmark operations
    void runBenchmarkSuite(const std::vector<BenchmarkConfig>& configs);
    void runIndividualBenchmarks();
    
    // Component-specific benchmark suites
    void runBrainBenchmarks();
    void runMemoryBenchmarks();
    void runPredictionBenchmarks();
    void runNeuromodulationBenchmarks();
    void runPlasticityBenchmarks();
    void runDevelopmentBenchmarks();
    void runIntegrationBenchmarks();
    
    // Benchmark levels
    void runSmallScaleBenchmarks();
    void runMediumScaleBenchmarks();
    void runLargeScaleBenchmarks();
    
    // Analysis and reporting
    void analyzeResults();
    void generatePerformanceReport() const;
    void exportBenchmarkResults(const std::string& filename) const;
    
    // Configuration
    void setProfilerConfig(const PerformanceProfilerConfig& config);
    const PerformanceProfiler& getProfiler() const { return profiler; }
    
    // Results access
    const std::vector<BenchmarkResult>& getResults() const { return results; }
    void clearResults() { results.clear(); }
    
    // Performance analysis
    BenchmarkResult getBestBenchmark(const std::string& category) const;
    BenchmarkResult getWorstBenchmark(const std::string& category) const;
    BenchmarkResult getBenchmarkByName(const std::string& name) const;
    
    // Singleton pattern for global access
    static PerformanceBenchmarkSuite& getInstance(const PerformanceProfilerConfig& config = PerformanceProfilerConfig());
    static void cleanupInstance();
};