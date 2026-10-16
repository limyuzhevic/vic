// PerformanceOptimization.hpp: CUDA performance optimization interface
#pragma once

#include "../core/Types/Types.hpp"
#include "DeviceMemoryManager.hpp"
#include <cstddef>

namespace nlm {
namespace cuda {

// Performance metrics for optimization
enum class PerformanceMetric {
    OPTIMIZATIONS_PERFORMED,
    KERNEL_TIME,
    BANDWIDTH,
    WARP_EFFICIENCY
};

// Configuration for performance optimization
struct PerformanceOptimizationConfig {
    size_t workloadSize;
    bool useMemoryPools;
    bool enableAsynchronousTransfers;
    size_t maxThreadsPerBlock;
    size_t maxBlocksPerGrid;
    size_t sharedMemorySize;
    bool useVectorizedOperations;
    bool enableWarpSynchronous;
    float computeCapabilityMajor;
    float computeCapabilityMinor;
    bool optimizeMemoryAccess;
    bool optimizeSharedMemory;
    bool optimizeAtomicOperations;
    bool enableWarpShuffles;
    bool enableInstructionCache;
    bool enableRegisterPressureOptimization;
};

// Performance optimization class for GPU acceleration
class PerformanceOptimization {
public:
    PerformanceOptimization();
    ~PerformanceOptimization();
    
    // Setup performance optimization on GPU
    void setup(DeviceMemoryManager& memoryManager,
              const PerformanceOptimizationConfig& config);
    
    // Optimize LIF memory access on GPU
    void optimizeLIFMemoryAccess(DeviceMemoryManager& memoryManager,
                                const float* membranePotentials,
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
    
    // Optimize neuron processing with shared memory on GPU
    void optimizeNeuronSharedMemory(DeviceMemoryManager& memoryManager,
                                    float* sharedMem,
                                    const float* membranePotentials,
                                    const float* synapticInputs,
                                    float* membranePotentialsOut,
                                    uint32_t* firingStates,
                                    size_t numNeurons,
                                    float dt,
                                    cudaStream_t stream = nullptr);
    
    // Optimize spike propagation on GPU
    void optimizeSpikePropagation(DeviceMemoryManager& memoryManager,
                                 const uint32_t* sourceFiringStates,
                                 const float* synapseWeights,
                                 float* synapticInputs,
                                 uint32_t* deliveryStatus,
                                 size_t numSynapses,
                                 size_t maxSynapsesPerWarp,
                                 cudaStream_t stream = nullptr);
    
    // Optimize plasticity update on GPU
    void optimizePlasticityUpdate(DeviceMemoryManager& memoryManager,
                                 const float* weightDeltas,
                                 float* updatedWeights,
                                 const uint32_t* plasticityFlags,
                                 size_t numSynapses,
                                 float maxWeightChange,
                                 cudaStream_t stream = nullptr);
    
    // Optimize population processing on GPU
    void optimizePopulationProcessing(DeviceMemoryManager& memoryManager,
                                      const float* membranePotentials,
                                      const float* synapticInputs,
                                      float* membranePotentialsOut,
                                      uint32_t* firingStates,
                                      size_t numNeurons,
                                      size_t populationSize,
                                      float dt,
                                      cudaStream_t stream = nullptr);
    
    // Benchmark performance on GPU
    void benchmarkPerformance(DeviceMemoryManager& memoryManager,
                              float* dummyMemory,
                              size_t arraySize,
                              float* executionTime,
                              float* memoryBandwidth,
                              cudaStream_t stream = nullptr);
    
    // Monitor kernel performance on GPU
    void monitorKernelPerformance(DeviceMemoryManager& memoryManager,
                                 cudaEvent_t startEvent,
                                 cudaEvent_t stopEvent,
                                 float& kernelTime);
    
    // Calculate memory bandwidth on GPU
    void calculateMemoryBandwidth(DeviceMemoryManager& memoryManager,
                                  size_t bytesTransferred,
                                  float timeMs,
                                  float& bandwidthGBps);
    
    // Get performance metrics
    float getPerformanceMetric(PerformanceMetric metric) const;
    
    // Print statistics
    void printStatistics() const;
    
private:
    PerformanceOptimizationConfig config_;
    float optimizationsPerformed_;
    float kernelTime_;
    float memoryBandwidth_;
    float warpEfficiency_;
};

// Global performance optimization instance
extern PerformanceOptimization performanceOptimization;

} // namespace cuda
} // namespace nlm
