// NLM Brain Performance Benchmarks Header
// Comprehensive benchmarking for Brain system performance

#pragma once

#include <cstddef>
#include <string>
#include <vector>
#include <map>
#include <chrono>

namespace nlm {
namespace benchmarks {

// Memory pool statistics
struct MemoryPoolStats {
    size_t neuronCount;
    size_t totalMemory;
    size_t peakMemory;
    size_t allocationCount;
    double stepsPerSecond;
    size_t memoryPerNeuron;
    double memoryEfficiency;
};

// Allocation benchmark results
struct AllocationBenchmarkResults {
    size_t allocationCount;
    double totalTime;
    double allocationsPerSecond;
    size_t averageAllocationSize;
    size_t peakMemory;
};

// Memory pressure test results
struct MemoryPressureResults {
    size_t neuronCount;
    size_t totalSteps;
    double totalTime;
    double stepsPerSecond;
    double spikesPerStep;
    size_t pendingSpikes;
    size_t totalSpikes;
};

// Spike propagation benchmark results
struct SpikePropagationResults {
    size_t neuronCount;
    size_t totalSteps;
    double totalTime;
    double stepsPerSecond;
    double synapseActivationTime;
    double neuronActivationTime;
    double spikesPerStep;
};

// Synaptic update benchmark results
struct SynapticUpdateResults {
    size_t synapseCount;
    size_t totalSteps;
    double totalTime;
    double stepsPerSecond;
    double stdpUpdateTime;
    double hebbianUpdateTime;
    size_t totalSpikeCount;
};

// Scalability benchmark results
struct ScalabilityResults {
    size_t neuronCount;
    size_t totalSteps;
    double totalTime;
    double stepsPerSecond;
    size_t synapseCount;
    size_t spikeCount;
    double efficiency; // synapses/sec
};

// Steps per second benchmark results
struct StepsPerSecondResults {
    size_t neuronCount;
    size_t steps;
    double totalTime;
    double stepsPerSecond;
    double spikesPerStep;
};

// Memory pool efficiency benchmark results
struct MemoryPoolEfficiencyResults {
    size_t neuronCount;
    size_t totalSteps;
    double totalTime;
    double stepsPerSecond;
    double peakMemoryKB;
    double totalMemoryKB;
    size_t allocationCount;
    size_t memoryPerNeuron;
    double efficiencyScore;
};

// SIMD efficiency benchmark results
struct SIMDEfficiencyResults {
    size_t neuronCount;
    size_t totalSteps;
    double totalTime;
    double stepsPerSecond;
    double totalPhaseTime;
    double synapseInitTime;
    double neuronInitTime;
    double synapseConnectTime;
    double neuronConnectTime;
    double synapseActivationTime;
    double neuronActivationTime;
    double peakMemoryKB;
    size_t allocationCount;
    double simdUtilizationScore;
};

class MemoryUsageBenchmarks {
public:
    static void runMemoryPoolBenchmarks();
    static void runAllocationBenchmarks();
    static void runMemoryPressureTests();
    
private:
    static void printMemoryPoolResults(const std::vector<MemoryPoolStats>& results);
    static void printAllocationResults(const std::vector<AllocationBenchmarkResults>& results);
    static void printMemoryPressureResults(const MemoryPressureResults& result);
};

class ComputationBenchmarks {
public:
    static void runSpikePropagationBenchmarks();
    static void runSynapticUpdateBenchmarks();
    static void runScalabilityBenchmarks();
    
private:
    static void printSpikePropagationResults(const std::vector<SpikePropagationResults>& results);
    static void printSynapticUpdateResults(const std::vector<SynapticUpdateResults>& results);
    static void printScalabilityResults(const std::vector<ScalabilityResults>& results);
};

class SimulationSpeedBenchmarks {
public:
    static void runStepsPerSecondBenchmarks();
    static void runMemoryPoolEfficiencyBenchmarks();
    static void runSIMDEfficiencyBenchmarks();
    
private:
    static void printStepsPerSecondResults(const std::vector<StepsPerSecondResults>& results);
    static void printMemoryPoolEfficiencyResults(const std::vector<MemoryPoolEfficiencyResults>& results);
    static void printSIMDEfficiencyResults(const std::vector<SIMDEfficiencyResults>& results);
};

} // namespace benchmarks
} // namespace nlm