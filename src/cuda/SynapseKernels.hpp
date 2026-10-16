// SynapseKernels.hpp: CUDA headers for synaptic operations
// Kernel declarations and type definitions for GPU-accelerated synaptic computation

#pragma once

#include "../core/Types/Types.hpp"
#include "CudaConfig.hpp"
#include <cstddef>

namespace nlm {
namespace cuda {

// Kernel configuration constants
static constexpr size_t SYNAPSE_KERNEL_THREADS_PER_BLOCK = 256;
static constexpr size_t SYNAPSE_MAX_BLOCKS_PER_GRID = 65535;
static constexpr size_t SYNAPSE_SHARED_MEMORY_SIZE = 32 * 1024;  // 32 KB

// Forward declarations
class SynapseProcessing;

// CUDA kernel functions for synaptic operations
namespace SynapseKernels {
    // Synaptic current calculation kernel
    __global__ void kernelSynapseCurrentCalculation(CudaSynapse* synapses,
                                                    const float* neuronMembranePotentials,
                                                    const uint32_t* neuronFiringStates,
                                                    float* synapticInputs,
                                                    float* synapticConductances,
                                                    float currentTime,
                                                    unsigned int numSynapses);
    
    // Batch synaptic current computation kernel
    __global__ void kernelSynapseBatchCurrent(const CudaSynapse* synapses,
                                             const float* neuronMembranePotentials,
                                             const uint32_t* neuronFiringStates,
                                             float* synapticInputs,
                                             size_t numSynapses,
                                             size_t batchStart,
                                             size_t batchEnd);
    
    // Spike propagation kernel
    __global__ void kernelSynapseSpikePropagation(const CudaSynapse* synapses,
                                                 const uint32_t* sourceFiringStates,
                                                 float* synapticInputs,
                                                 uint64_t* spikeDeliveryTimes,
                                                 float currentTime,
                                                 unsigned int numSynapses);
    
    // Delayed spike delivery kernel
    __global__ void kernelDelayedSpikeDelivery(const CudaSynapse* synapses,
                                             const uint64_t* spikeDeliveryTimes,
                                             const float* neuronMembranePotentials,
                                             float* synapticInputs,
                                             float currentTime,
                                             unsigned int numSynapses,
                                             uint32_t* deliveryStatus);
    
    // Event queue processing kernel
    __global__ void kernelEventQueueProcessing(const CudaQueuedSpikeEvent* events,
                                              size_t numEvents,
                                              float* synapticInputs,
                                              uint32_t* neuronFiringStates,
                                              uint64_t currentTime,
                                              size_t maxEventsPerThread);
    
    // Event queue batch processing kernel
    __global__ void kernelEventQueueBatchProcessing(const CudaQueuedSpikeEvent* events,
                                                   size_t numEvents,
                                                   float* synapticInputs,
                                                   uint32_t* neuronFiringStates,
                                                   uint64_t currentTime,
                                                   size_t batchStart,
                                                   size_t batchEnd);
}

// Synapse data structure for CUDA
struct CudaSynapse {
    uint32_t sourceNeuron;
    uint32_t destinationNeuron;
    float weight;
    float delay;
    uint32_t type;
    uint32_t plasticityFlags;
    float efficacy;
    float eligibilityTrace;
    
    // Constructors
    __host__ __device__ CudaSynapse();
    __host__ __device__ CudaSynapse(const Synapse* synapse);
    __host__ __device__ Synapse toSynapse() const;
};

// Synapse processing class for CUDA operations
class SynapseProcessing {
public:
    // Process synaptic currents on GPU
    static void processCurrents(CudaSynapse* deviceSynapses,
                               const float* neuronMembranePotentials,
                               const uint32_t* neuronFiringStates,
                               float* synapticInputs,
                               float* synapticConductances,
                               float currentTime,
                               size_t numSynapses,
                               cudaStream_t stream = nullptr);
    
    // Batch process synaptic currents
    static void batchProcessCurrents(const CudaSynapse* deviceSynapses,
                                    const float* neuronMembranePotentials,
                                    const uint32_t* neuronFiringStates,
                                    float* synapticInputs,
                                    size_t numSynapses,
                                    size_t batchStart,
                                    size_t batchEnd,
                                    cudaStream_t stream = nullptr);
    
    // Process spike propagation
    static void processSpikePropagation(const CudaSynapse* deviceSynapses,
                                       const uint32_t* sourceFiringStates,
                                       float* synapticInputs,
                                       uint64_t* spikeDeliveryTimes,
                                       float currentTime,
                                       size_t numSynapses,
                                       cudaStream_t stream = nullptr);
    
    // Process delayed spike delivery
    static void processDelayedSpikeDelivery(const CudaSynapse* deviceSynapses,
                                           const uint64_t* spikeDeliveryTimes,
                                           const float* neuronMembranePotentials,
                                           float* synapticInputs,
                                           float currentTime,
                                           size_t numSynapses,
                                           uint32_t* deliveryStatus,
                                           cudaStream_t stream = nullptr);
    
    // Process event queue
    static void processEventQueue(const QueuedSpikeEvent* hostEvents,
                                 size_t numEvents,
                                 CudaSynapse* deviceSynapses,
                                 float* synapticInputs,
                                 cudaStream_t stream = nullptr);
    
    // Copy synapse data between host and device
    static void copyToDevice(const Synapse* hostSynapses,
                            CudaSynapse* deviceSynapses,
                            size_t count,
                            cudaStream_t stream = nullptr);
    
    static void copyFromDevice(CudaSynapse* deviceSynapses,
                              Synapse* hostSynapses,
                              size_t count,
                              cudaStream_t stream = nullptr);
    
    // Get performance statistics
    static void getPerformanceStats(float& kernelTime, float& memoryBandwidth);
    
private:
    // Private constructor (synapse processing is a namespace)
    SynapseProcessing() = default;
    ~SynapseProcessing() = default;
};

} // namespace cuda
} // namespace nlm
