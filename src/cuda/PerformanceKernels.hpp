// PerformanceKernels.hpp: CUDA headers for performance optimization
// Kernel declarations and type definitions for GPU-accelerated performance optimization

#pragma once

#include "../core/Types/Types.hpp"
#include "CudaConfig.hpp"
#include <cstddef>

namespace nlm {
namespace cuda {

// Kernel configuration constants
static constexpr size_t PERFORMANCE_KERNEL_THREADS_PER_BLOCK = 256;
static constexpr size_t PERFORMANCE_MAX_BLOCKS_PER_GRID = 65535;
static constexpr size_t PERFORMANCE_SHARED_MEMORY_SIZE = 64 * 1024;  // 64 KB
static constexpr size_t SHARED_MEMORY_SIZE = 48 * 1024;  // 48 KB

// Forward declarations
class PerformanceOptimization;

// CUDA kernel functions for performance optimization
namespace PerformanceKernels {
    // Optimized LIF integration with memory coalescing
    __global__ void kernelLIFOptimizedMemory(const float* membranePotentials,
                                           const float* synapticInputs,
                                           const float* externalInputs,
                                           const float* restingPotentials,
                                           const float* thresholds,
                                           const float* resetPotentials,
                                           const float* leakConductances,
                                           float* membranePotentialsOut,
                                           uint32_t* firingStates,
                                           uint32_t* refractoryCounters,
                                           size_t numNeurons,
                                           float dt);
    
    // Shared memory based neuron processing
    __global__ void kernelNeuronSharedMemory(float* sharedMem,
                                            const float* membranePotentials,
                                            const float* synapticInputs,
                                            float* membranePotentialsOut,
                                            uint32_t* firingStates,
                                            size_t numNeurons,
                                            float dt);
    
    // Warp-level synchronous spike detection
    __global__ void kernelWarpSpikeDetection(const float* membranePotentials,
                                            const float* thresholds,
                                            uint32_t* firingStates,
                                            uint32_t* spikeCount,
                                            size_t numNeurons);
    
    // Optimized spike propagation with shared memory
    __global__ void kernelOptimizedSpikePropagation(const uint32_t* sourceFiringStates,
                                                   const float* synapseWeights,
                                                   float* synapticInputs,
                                                   uint32_t* deliveryStatus,
                                                   size_t numSynapses,
                                                   size_t maxSynapsesPerWarp);
    
    // Memory-efficient plasticity update
    __global__ void kernelMemoryEfficientPlasticity(const float* weightDeltas,
                                                   float* updatedWeights,
                                                   const uint32_t* plasticityFlags,
                                                   size_t numSynapses,
                                                   float maxWeightChange);
    
    // Optimized neuron population processing
    __global__ void kernelNeuronPopulationProcessing(const float* membranePotentials,
                                                    const float* synapticInputs,
                                                    float* membranePotentialsOut,
                                                    uint32_t* firingStates,
                                                    size_t numNeurons,
                                                    size_t populationSize,
                                                    float dt);
    
    // Performance benchmarking kernel
    __global__ void kernelPerformanceBenchmark(float* dummyMemory,
                                              size_t arraySize,
                                              float* executionTime,
                                              float* memoryBandwidth);
}

// Performance optimization class for CUDA operations
class PerformanceOptimization {
public:
    // Optimize LIF memory access
    static void optimizeLIFMemoryAccess(const float* membranePotentials,
                                       const float* synapticInputs,
                                       const float* externalInputs,
                                       const float* restingPotentials,
                                       const float* thresholds,
                                       const float* resetPotentials,
                                       const float* leakConductances,
                                       float* membranePotentialsOut,
                                       uint32_t* firingStates,
                                       uint32_t* refractoryCounters,
                                       size_t numNeurons,
                                       float dt,
                                       cudaStream_t stream = nullptr);
    
    // Optimize neuron processing with shared memory
    static void optimizeNeuronSharedMemory(float* sharedMem,
                                          const float* membranePotentials,
                                          const float* synapticInputs,
                                          float* membranePotentialsOut,
                                          uint32_t* firingStates,
                                          size_t numNeurons,
                                          float dt,
                                          cudaStream_t stream = nullptr);
    
    // Optimize spike propagation
    static void optimizeSpikePropagation(const uint32_t* sourceFiringStates,
                                        const float* synapseWeights,
                                        float* synapticInputs,
                                        uint32_t* deliveryStatus,
                                        size_t numSynapses,
                                        size_t maxSynapsesPerWarp,
                                        cudaStream_t stream = nullptr);
    
    // Optimize plasticity update
    static void optimizePlasticityUpdate(const float* weightDeltas,
                                        float* updatedWeights,
                                        const uint32_t* plasticityFlags,
                                        size_t numSynapses,
                                        float maxWeightChange,
                                        cudaStream_t stream = nullptr);
    
    // Optimize population processing
    static void optimizePopulationProcessing(const float* membranePotentials,
                                            const float* synapticInputs,
                                            float* membranePotentialsOut,
                                            uint32_t* firingStates,
                                            size_t numNeurons,
                                            size_t populationSize,
                                            float dt,
                                            cudaStream_t stream = nullptr);
    
    // Benchmark performance
    static void benchmarkPerformance(float* dummyMemory,
                                     size_t arraySize,
                                     float* executionTime,
                                     float* memoryBandwidth,
                                     cudaStream_t stream = nullptr);
    
    // Monitor kernel performance
    static void monitorKernelPerformance(cudaEvent_t startEvent,
                                         cudaEvent_t stopEvent,
                                         float& kernelTime);
    
    // Calculate memory bandwidth
    static void calculateMemoryBandwidth(size_t bytesTransferred,
                                         float timeMs,
                                         float& bandwidthGBps);
    
    // Get performance statistics
    static void getPerformanceStats(float& kernelTime, float& memoryBandwidth);
    
private:
    // Private constructor (performance optimization is a namespace)
    PerformanceOptimization() = default;
    ~PerformanceOptimization() = default;
};

} // namespace cuda
} // namespace nlm
