#pragma once

#include <string>
#include <vector>
#include <memory>
#include "../benchmarks/Benchmark.hpp"

namespace nlm {
namespace benchmark {
namespace core {

// Brain performance benchmark
class BrainPerformanceBenchmark {
public:
    struct BrainMetrics {
        double neuronStepTime;        // Average time per neuron step
        double spikeProcessingTime;   // Average time for spike processing
        double synapticUpdateTime;    // Average time for synaptic updates
        double plasticityTime;        // Average time for plasticity operations
        double memoryAccessTime;      // Average time for memory access
        double cacheMissRate;         // Cache miss rate
        double branchPredictionRate;  // Branch prediction success rate
        double instructionPerCycle;   // Instructions per cycle
    };
    
    BenchmarkMetrics benchmarkNeuronStep(size_t neuronCount, size_t steps);
    BenchmarkMetrics benchmarkSpikeProcessing(size_t neuronCount, size_t spikeRate);
    BenchmarkMetrics benchmarkSynapticUpdates(size_t connectionDensity);
    BenchmarkMetrics benchmarkPlasticity(size_t neuronCount, size_t steps);
    BenchmarkMetrics benchmarkMemoryAccess(size_t neuronCount);
    BenchmarkMetrics benchmarkCacheEfficiency(size_t neuronCount);
    BenchmarkMetrics benchmarkBranchPrediction(size_t neuronCount);
    
    std::vector<BrainMetrics> runComprehensiveBrainBenchmark(const std::vector<size_t>& neuronCounts);
};

// Memory allocation benchmark
class MemoryAllocationBenchmark {
public:
    struct MemoryMetrics {
        double poolAllocatorTime;     // Time for pool allocator operations
        double heapAllocatorTime;     // Time for heap allocator operations
        double memoryAllocationTime;  // Total allocation time
        double memoryDeallocationTime; // Total deallocation time
        double memoryCopyTime;        // Time for memory copy operations
        double memoryBandwidth;       // Memory bandwidth utilization
        double cacheLineUtilization;   // Cache line utilization efficiency
        double fragmentationRatio;    // Memory fragmentation ratio
    };
    
    BenchmarkMetrics benchmarkPoolVsHeapAllocation(size_t objectCount);
    BenchmarkMetrics benchmarkMemoryCopy(size_t size);
    BenchmarkMetrics benchmarkMemoryAccessPattern(const std::string& pattern);
    BenchmarkMetrics benchmarkCacheLineUtilization(size_t dataSize);
    BenchmarkMetrics benchmarkMemoryFragmentation(size_t iterations);
    
    std::vector<MemoryMetrics> runMemoryBenchmarkSuite();
};

// SIMD effectiveness benchmark
class SIMDBenchmark {
public:
    struct SIMDMETRICS {
        double scalarOperationsTime;      // Time for scalar operations
        double vectorOperationsTime;      // Time for vectorized operations
        double simdSpeedup;               // SIMD vs scalar speedup
        double vectorizationEfficiency;   // SIMD vectorization efficiency
        double registerPressure;          // CPU register pressure
        double instructionMix;            // Ratio of SIMD to scalar instructions
        double memoryBandwidthLimiting;   // Memory bandwidth limitation
        double computeBound;              // Whether compute-bound
        double memoryBound;               // Whether memory-bound
    };
    
    BenchmarkMetrics benchmarkScalarVsVector(const std::vector<float>& data);
    BenchmarkMetrics benchmarkSIMDWidth(size_t simdWidth);
    BenchmarkMetrics benchmarkInstructionMix(size_t operations);
    BenchmarkMetrics benchmarkRegisterPressure(size_t workingSetSize);
    BenchmarkMetrics benchmarkMemoryBandwidth(size_t dataSize);
    BenchmarkMetrics benchmarkComputeBound(const std::vector<float>& data);
    BenchmarkMetrics benchmarkMemoryBound(const std::vector<float>& data);
    
    std::vector<SIMDMETRICS> runSIMDBenchmarkSuite();
};

// Integration benchmark
class IntegrationBenchmark {
public:
    struct IntegrationMetrics {
        double sensoryProcessingTime;    // Time for sensory processing
        double brainProcessingTime;      // Time for brain computation
        double motorOutputTime;          // Time for motor output generation
        double perceptionTime;           // Time for perception
        double actionSelectionTime;      // Time for action selection
        double learningTime;             // Time for learning
        double adaptationTime;           // Time for adaptation
        double synchronizationTime;      // Time for system synchronization
    };
    
    BenchmarkMetrics benchmarkSensoryProcessing(size_t sensoryInputSize);
    BenchmarkMetrics benchmarkBrainComputation(size_t neuronCount);
    BenchmarkMetrics benchmarkMotorOutput(size_t actionCount);
    BenchmarkMetrics benchmarkPerception(size_t observationSize);
    BenchmarkMetrics benchmarkActionSelection(size_t actionSpace);
    BenchmarkMetrics benchmarkLearning(size_t learningDataSize);
    BenchmarkMetrics benchmarkAdaptation(size_t adaptationSteps);
    BenchmarkMetrics benchmarkSystemSynchronization(size_t threadCount);
    
    std::vector<IntegrationMetrics> runIntegrationBenchmarkSuite();
};

} // namespace core
} // namespace benchmark
} // namespace nlm

#include "CoreBenchmarks.inl"