// EventQueueKernels.hpp: CUDA headers for event queue processing
// Kernel declarations and type definitions for GPU-accelerated event queue computation

#pragma once

#include "../performance/EventQueue.hpp"
#include "CudaConfig.hpp"
#include <cstddef>

namespace nlm {
namespace cuda {

// Kernel configuration constants
static constexpr size_t EVENT_QUEUE_KERNEL_THREADS_PER_BLOCK = 256;
static constexpr size_t EVENT_QUEUE_MAX_BLOCKS_PER_GRID = 65535;
static constexpr size_t EVENT_QUEUE_SHARED_MEMORY_SIZE = 24 * 1024;  // 24 KB
static constexpr size_t EVENT_QUEUE_BATCH_SIZE = 1024;

// Forward declarations
class EventQueueProcessing;

// CUDA kernel functions for event queue processing
namespace EventQueueKernels {
    // Parallel event queue processing kernel
    __global__ void kernelEventQueueProcessing(const CudaQueuedSpikeEvent* events,
                                              size_t numEvents,
                                              float* synapticInputs,
                                              uint32_t* neuronFiringStates,
                                              uint64_t currentTime,
                                              size_t maxEventsPerThread);
    
    // Batch event queue processing kernel
    __global__ void kernelEventQueueBatchProcessing(const CudaQueuedSpikeEvent* events,
                                                   size_t numEvents,
                                                   float* synapticInputs,
                                                   uint32_t* neuronFiringStates,
                                                   uint64_t currentTime,
                                                   size_t batchStart,
                                                   size_t batchEnd);
    
    // Delayed spike delivery kernel
    __global__ void kernelDelayedSpikeDelivery(const CudaQueuedSpikeEvent* events,
                                             const uint64_t* deliveryTimes,
                                             float* synapticInputs,
                                             uint32_t* neuronFiringStates,
                                             uint64_t currentTime,
                                             unsigned int numEvents,
                                             uint32_t* deliveryStatus);
    
    // Event queue merging kernel
    __global__ void kernelEventQueueMerge(const CudaQueuedSpikeEvent* queue1,
                                         const CudaQueuedSpikeEvent* queue2,
                                         CudaQueuedSpikeEvent* mergedQueue,
                                         size_t size1,
                                         size_t size2,
                                         size_t outputSize,
                                         uint64_t mergeThreshold);
    
    // Event queue statistics kernel
    __global__ void kernelEventQueueStatistics(const CudaQueuedSpikeEvent* events,
                                              size_t numEvents,
                                              uint32_t* statsOutput,
                                              float minWeight,
                                              float maxWeight,
                                              uint64_t minTime,
                                              uint64_t maxTime);
}

// Event queue data structure for CUDA
struct CudaQueuedSpikeEvent {
    uint64_t sourceNeuron;
    uint64_t destNeuron;
    uint64_t synapseId;
    float weight;
    uint32_t isExcitatory;
    uint32_t padding[2];  // For alignment
    
    // Constructors
    __host__ __device__ CudaQueuedSpikeEvent();
    __host__ __device__ CudaQueuedSpikeEvent(const QueuedSpikeEvent& event);
    __host__ __device__ QueuedSpikeEvent toQueuedSpikeEvent() const;
};

// Event queue processing class for CUDA operations
class EventQueueProcessing {
public:
    // Process events on GPU
    static void processEvents(const QueuedSpikeEvent* hostEvents,
                             size_t numEvents,
                             float* synapticInputs,
                             uint32_t* neuronFiringStates,
                             uint64_t currentTime,
                             cudaStream_t stream = nullptr);
    
    // Process delayed events on GPU
    static void processDelayedEvents(const QueuedSpikeEvent* hostEvents,
                                     const uint64_t* hostDeliveryTimes,
                                     float* synapticInputs,
                                     uint32_t* neuronFiringStates,
                                     uint64_t currentTime,
                                     size_t numEvents,
                                     uint32_t* deliveryStatus,
                                     cudaStream_t stream = nullptr);
    
    // Merge event queues on GPU
    static void mergeEventQueues(const QueuedSpikeEvent* queue1,
                                const QueuedSpikeEvent* queue2,
                                QueuedSpikeEvent* mergedQueue,
                                size_t size1,
                                size_t size2,
                                size_t outputSize,
                                uint64_t mergeThreshold,
                                cudaStream_t stream = nullptr);
    
    // Compute event statistics on GPU
    static void computeEventStatistics(const QueuedSpikeEvent* hostEvents,
                                       size_t numEvents,
                                       uint32_t* hostStats,
                                       float minWeight,
                                       float maxWeight,
                                       uint64_t minTime,
                                       uint64_t maxTime,
                                       cudaStream_t stream = nullptr);
    
    // Process event queue (wrapper)
    static void processEventQueue(const QueuedSpikeEvent* events,
                                 size_t numEvents,
                                 float* synapticInputs,
                                 uint32_t* neuronFiringStates,
                                 uint64_t currentTime,
                                 cudaStream_t stream = nullptr);
    
    // Process delayed events (wrapper)
    static void processDelayedEvents(const QueuedSpikeEvent* events,
                                     const uint64_t* deliveryTimes,
                                     float* synapticInputs,
                                     uint32_t* neuronFiringStates,
                                     uint64_t currentTime,
                                     size_t numEvents,
                                     uint32_t* deliveryStatus,
                                     cudaStream_t stream = nullptr);
    
    // Merge event queues (wrapper)
    static void mergeEventQueues(const QueuedSpikeEvent* queue1,
                                const QueuedSpikeEvent* queue2,
                                QueuedSpikeEvent* mergedQueue,
                                size_t size1,
                                size_t size2,
                                size_t outputSize,
                                uint64_t mergeThreshold,
                                cudaStream_t stream = nullptr);
    
    // Compute event statistics (wrapper)
    static void computeEventStatistics(const QueuedSpikeEvent* events,
                                       size_t numEvents,
                                       uint32_t* statsOutput,
                                       float minWeight,
                                       float maxWeight,
                                       uint64_t minTime,
                                       uint64_t maxTime,
                                       cudaStream_t stream = nullptr);
    
    // Get performance statistics
    static void getPerformanceStats(float& kernelTime, float& memoryBandwidth);
    
private:
    // Private constructor (event queue processing is a namespace)
    EventQueueProcessing() = default;
    ~EventQueueProcessing() = default;
};

} // namespace cuda
} // namespace nlm
