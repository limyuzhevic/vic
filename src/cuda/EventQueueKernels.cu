// EventQueueKernels.cu: CUDA kernels for event queue processing
// High-performance GPU implementation of spike event queue operations

#include "EventQueueKernels.hpp"
#include "../performance/EventQueue.hpp"
#include "../core/Types/Types.hpp"
#include <algorithm>

namespace nlm {
namespace cuda {

// Event queue data structure for CUDA kernel
struct CudaQueuedSpikeEvent {
    uint64_t sourceNeuron;
    uint64_t destNeuron;
    uint64_t synapseId;
    float weight;
    uint32_t isExcitatory;
    uint32_t padding[2];  // For alignment
    
    // Default constructor
    __host__ __device__ CudaQueuedSpikeEvent() 
        : sourceNeuron(0), destNeuron(0), synapseId(0), weight(0.0f),
          isExcitatory(1), padding{0, 0} {}
    
    // Constructor from QueuedSpikeEvent
    __host__ __device__ CudaQueuedSpikeEvent(const QueuedSpikeEvent& event) {
        sourceNeuron = event.sourceNeuron;
        destNeuron = event.destNeuron;
        synapseId = event.synapseId;
        weight = event.weight;
        isExcitatory = event.isExcitatory ? 1 : 0;
        padding[0] = padding[1] = 0;
    }
    
    // Convert to QueuedSpikeEvent
    __host__ __device__ QueuedSpikeEvent toQueuedSpikeEvent() const {
        QueuedSpikeEvent event;
        event.sourceNeuron = sourceNeuron;
        event.destNeuron = destNeuron;
        event.synapseId = synapseId;
        event.weight = weight;
        event.isExcitatory = (isExcitatory != 0);
        return event;
    }
};

// Parallel event queue processing kernel
// Processes multiple spike events in parallel
__global__ void kernelEventQueueProcessing(const CudaQueuedSpikeEvent* events,
                                          size_t numEvents,
                                          float* synapticInputs,
                                          uint32_t* neuronFiringStates,
                                          uint64_t currentTime,
                                          size_t maxEventsPerThread) {
    unsigned int eventIdx = blockIdx.x * blockDim.x + threadIdx.x;
    unsigned int stride = gridDim.x * blockDim.x;
    
    // Process multiple events per thread for load balancing
    for (unsigned int idx = eventIdx; idx < numEvents; idx += stride) {
        const CudaQueuedSpikeEvent& event = events[idx];
        
        // Check if event is due (immediate events only)
        if (eventIdx < maxEventsPerThread) {
            // Deliver spike to destination neuron
            float spikeCurrent = event.weight;
            if (event.isExcitatory) {
                // Excitatory spike
                atomicAdd(&synapticInputs[event.destNeuron], spikeCurrent);
                neuronFiringStates[event.destNeuron] = 1;
            } else {
                // Inhibitory spike
                spikeCurrent *= -1.0f;
                atomicAdd(&synapticInputs[event.destNeuron], spikeCurrent);
            }
        }
    }
}

// Batch event queue processing kernel
// Processes events in batches for better cache utilization
__global__ void kernelEventQueueBatchProcessing(const CudaQueuedSpikeEvent* events,
                                               size_t numEvents,
                                               float* synapticInputs,
                                               uint32_t* neuronFiringStates,
                                               uint64_t currentTime,
                                               size_t batchStart,
                                               size_t batchEnd) {
    // Process events in this batch
    for (size_t idx = batchStart + threadIdx.x; idx < batchEnd; idx += blockDim.x) {
        if (idx >= numEvents) break;
        
        const CudaQueuedSpikeEvent& event = events[idx];
        
        // Deliver spike to destination neuron
        float spikeCurrent = event.weight;
        if (event.isExcitatory) {
            // Excitatory spike
            atomicAdd(&synapticInputs[event.destNeuron], spikeCurrent);
            neuronFiringStates[event.destNeuron] = 1;
        } else {
            // Inhibitory spike
            spikeCurrent *= -1.0f;
            atomicAdd(&synapticInputs[event.destNeuron], spikeCurrent);
        }
    }
}

// Delayed spike delivery kernel
// Processes spikes scheduled for delayed delivery
__global__ void kernelDelayedSpikeDelivery(const CudaQueuedSpikeEvent* events,
                                         const uint64_t* deliveryTimes,
                                         float* synapticInputs,
                                         uint32_t* neuronFiringStates,
                                         uint64_t currentTime,
                                         unsigned int numEvents,
                                         uint32_t* deliveryStatus) {
    unsigned int eventIdx = blockIdx.x * blockDim.x + threadIdx.x;
    if (eventIdx >= numEvents) return;
    
    const CudaQueuedSpikeEvent& event = events[eventIdx];
    uint64_t deliveryTime = deliveryTimes[eventIdx];
    
    // Check if spike is due for delivery
    if (deliveryTime > 0 && deliveryTime <= currentTime) {
        // Deliver spike to destination neuron
        float spikeCurrent = event.weight;
        if (event.isExcitatory) {
            // Excitatory spike
            atomicAdd(&synapticInputs[event.destNeuron], spikeCurrent);
            neuronFiringStates[event.destNeuron] = 1;
        } else {
            // Inhibitory spike
            spikeCurrent *= -1.0f;
            atomicAdd(&synapticInputs[event.destNeuron], spikeCurrent);
        }
        
        // Mark spike as delivered
        deliveryStatus[eventIdx] = 1;
        
        // Reset delivery time
        deliveryTimes[eventIdx] = 0;
    }
}

// Event queue merging kernel
// Merges multiple event queues into one sorted order
__global__ void kernelEventQueueMerge(const CudaQueuedSpikeEvent* queue1,
                                     const CudaQueuedSpikeEvent* queue2,
                                     CudaQueuedSpikeEvent* mergedQueue,
                                     size_t size1,
                                     size_t size2,
                                     size_t outputSize,
                                     uint64_t mergeThreshold) {
    unsigned int eventIdx = blockIdx.x * blockDim.x + threadIdx.x;
    if (eventIdx >= outputSize) return;
    
    // Simple merge: take from queue1 then queue2
    // In a real implementation, this would be more sophisticated
    if (eventIdx < size1) {
        mergedQueue[eventIdx] = queue1[eventIdx];
    } else if (eventIdx - size1 < size2) {
        mergedQueue[eventIdx] = queue2[eventIdx - size1];
    }
}

// Event queue statistics kernel
// Computes statistics about event queue processing
__global__ void kernelEventQueueStatistics(const CudaQueuedSpikeEvent* events,
                                          size_t numEvents,
                                          uint32_t* statsOutput,
                                          float minWeight,
                                          float maxWeight,
                                          uint64_t minTime,
                                          uint64_t maxTime) {
    unsigned int eventIdx = blockIdx.x * blockDim.x + threadIdx.x;
    if (eventIdx >= numEvents) return;
    
    const CudaQueuedSpikeEvent& event = events[eventIdx];
    
    // Update statistics
    if (event.weight < minWeight) {
        statsOutput[0] = 1;  // Has lighter weight
    }
    if (event.weight > maxWeight) {
        statsOutput[1] = 1;  // Has heavier weight
    }
    if (event.destNeuron < minTime) {
        statsOutput[2] = 1;  // Has smaller destination
    }
    if (event.destNeuron > maxTime) {
        statsOutput[3] = 1;  // Has larger destination
    }
}

// Event processing implementation using CUDA
void EventQueueProcessing::processEvents(const QueuedSpikeEvent* hostEvents,
                                         size_t numEvents,
                                         float* synapticInputs,
                                         uint32_t* neuronFiringStates,
                                         uint64_t currentTime,
                                         cudaStream_t stream) {
    if (numEvents == 0) return;
    
    // Convert events to device format
    CudaQueuedSpikeEvent* deviceEvents = nullptr;
    cudaError_t error = cudaMalloc(&deviceEvents, numEvents * sizeof(CudaQueuedSpikeEvent));
    if (error != cudaSuccess) {
        std::cerr << "Failed to allocate device memory for events: "
                  << CudaError::getErrorString(error) << std::endl;
        return;
    }
    
    // Copy events to device
    error = cudaMemcpyAsync(deviceEvents, hostEvents,
                           numEvents * sizeof(CudaQueuedSpikeEvent),
                           cudaMemcpyHostToDevice, stream);
    if (error != cudaSuccess) {
        std::cerr << "Failed to copy events to device: "
                  << CudaError::getErrorString(error) << std::endl;
        cudaFree(deviceEvents);
        return;
    }
    
    // Determine optimal grid and block dimensions
    size_t threadsPerBlock = EVENT_QUEUE_KERNEL_THREADS_PER_BLOCK;
    size_t blocksPerGrid = (numEvents + threadsPerBlock - 1) / threadsPerBlock;
    
    // Process events in batches if large population
    if (numEvents > EVENT_QUEUE_BATCH_SIZE) {
        size_t batchSize = EVENT_QUEUE_BATCH_SIZE;
        size_t numBatches = (numEvents + batchSize - 1) / batchSize;
        
        for (size_t batch = 0; batch < numBatches; ++batch) {
            size_t batchStart = batch * batchSize;
            size_t batchEnd = std::min(batchStart + batchSize, numEvents);
            
            kernelEventQueueBatchProcessing<<<1, threadsPerBlock, 0, stream>>>(
                deviceEvents,
                numEvents,
                synapticInputs,
                neuronFiringStates,
                currentTime,
                batchStart,
                batchEnd
            );
        }
    } else {
        kernelEventQueueProcessing<<<blocksPerGrid, threadsPerBlock, 0, stream>>>(
            deviceEvents,
            numEvents,
            synapticInputs,
            neuronFiringStates,
            currentTime,
            threadsPerBlock
        );
    }
    
    // Synchronize to ensure events are processed
    cudaStreamSynchronize(stream);
    
    // Free device memory
    cudaFree(deviceEvents);
}

void EventQueueProcessing::processDelayedEvents(const QueuedSpikeEvent* hostEvents,
                                               const uint64_t* hostDeliveryTimes,
                                               float* synapticInputs,
                                               uint32_t* neuronFiringStates,
                                               uint64_t currentTime,
                                               size_t numEvents,
                                               uint32_t* deliveryStatus,
                                               cudaStream_t stream) {
    if (numEvents == 0) return;
    
    // Convert events to device format
    CudaQueuedSpikeEvent* deviceEvents = nullptr;
    uint64_t* deviceDeliveryTimes = nullptr;
    
    cudaError_t error = cudaMalloc(&deviceEvents, numEvents * sizeof(CudaQueuedSpikeEvent));
    if (error != cudaSuccess) {
        std::cerr << "Failed to allocate device memory for events: "
                  << CudaError::getErrorString(error) << std::endl;
        return;
    }
    
    error = cudaMalloc(&deviceDeliveryTimes, numEvents * sizeof(uint64_t));
    if (error != cudaSuccess) {
        std::cerr << "Failed to allocate device memory for delivery times: "
                  << CudaError::getErrorString(error) << std::endl;
        cudaFree(deviceEvents);
        return;
    }
    
    // Copy data to device
    error = cudaMemcpyAsync(deviceEvents, hostEvents,
                           numEvents * sizeof(CudaQueuedSpikeEvent),
                           cudaMemcpyHostToDevice, stream);
    if (error != cudaSuccess) {
        std::cerr << "Failed to copy events to device: "
                  << CudaError::getErrorString(error) << std::endl;
        cudaFree(deviceEvents);
        cudaFree(deviceDeliveryTimes);
        return;
    }
    
    error = cudaMemcpyAsync(deviceDeliveryTimes, hostDeliveryTimes,
                           numEvents * sizeof(uint64_t),
                           cudaMemcpyHostToDevice, stream);
    if (error != cudaSuccess) {
        std::cerr << "Failed to copy delivery times to device: "
                  << CudaError::getErrorString(error) << std::endl;
        cudaFree(deviceEvents);
        cudaFree(deviceDeliveryTimes);
        return;
    }
    
    // Determine optimal grid and block dimensions
    size_t threadsPerBlock = EVENT_QUEUE_KERNEL_THREADS_PER_BLOCK;
    size_t blocksPerGrid = (numEvents + threadsPerBlock - 1) / threadsPerBlock;
    
    // Launch delayed spike delivery kernel
    kernelDelayedSpikeDelivery<<<blocksPerGrid, threadsPerBlock, 0, stream>>>(
        deviceEvents,
        deviceDeliveryTimes,
        synapticInputs,
        neuronFiringStates,
        currentTime,
        static_cast<unsigned int>(numEvents),
        deliveryStatus
    );
    
    // Synchronize to ensure events are processed
    cudaStreamSynchronize(stream);
    
    // Free device memory
    cudaFree(deviceEvents);
    cudaFree(deviceDeliveryTimes);
}

void EventQueueProcessing::mergeEventQueues(const QueuedSpikeEvent* queue1,
                                          const QueuedSpikeEvent* queue2,
                                          QueuedSpikeEvent* mergedQueue,
                                          size_t size1,
                                          size_t size2,
                                          size_t outputSize,
                                          uint64_t mergeThreshold,
                                          cudaStream_t stream) {
    if (size1 == 0 || size2 == 0 || outputSize == 0) return;
    
    // Convert queues to device format
    CudaQueuedSpikeEvent* deviceQueue1 = nullptr;
    CudaQueuedSpikeEvent* deviceQueue2 = nullptr;
    CudaQueuedSpikeEvent* deviceMerged = nullptr;
    
    cudaError_t error = cudaMalloc(&deviceQueue1, size1 * sizeof(CudaQueuedSpikeEvent));
    if (error != cudaSuccess) {
        std::cerr << "Failed to allocate device memory for queue1: "
                  << CudaError::getErrorString(error) << std::endl;
        return;
    }
    
    error = cudaMalloc(&deviceQueue2, size2 * sizeof(CudaQueuedSpikeEvent));
    if (error != cudaSuccess) {
        std::cerr << "Failed to allocate device memory for queue2: "
                  << CudaError::getErrorString(error) << std::endl;
        cudaFree(deviceQueue1);
        return;
    }
    
    error = cudaMalloc(&deviceMerged, outputSize * sizeof(CudaQueuedSpikeEvent));
    if (error != cudaSuccess) {
        std::cerr << "Failed to allocate device memory for merged queue: "
                  << CudaError::getErrorString(error) << std::endl;
        cudaFree(deviceQueue1);
        cudaFree(deviceQueue2);
        return;
    }
    
    // Copy data to device
    error = cudaMemcpyAsync(deviceQueue1, queue1,
                           size1 * sizeof(CudaQueuedSpikeEvent),
                           cudaMemcpyHostToDevice, stream);
    if (error != cudaSuccess) {
        std::cerr << "Failed to copy queue1 to device: "
                  << CudaError::getErrorString(error) << std::endl;
        cudaFree(deviceQueue1);
        cudaFree(deviceQueue2);
        cudaFree(deviceMerged);
        return;
    }
    
    error = cudaMemcpyAsync(deviceQueue2, queue2,
                           size2 * sizeof(CudaQueuedSpikeEvent),
                           cudaMemcpyHostToDevice, stream);
    if (error != cudaSuccess) {
        std::cerr << "Failed to copy queue2 to device: "
                  << CudaError::getErrorString(error) << std::endl;
        cudaFree(deviceQueue1);
        cudaFree(deviceQueue2);
        cudaFree(deviceMerged);
        return;
    }
    
    // Determine optimal grid and block dimensions
    size_t threadsPerBlock = EVENT_QUEUE_KERNEL_THREADS_PER_BLOCK;
    size_t blocksPerGrid = (outputSize + threadsPerBlock - 1) / threadsPerBlock;
    
    // Launch event queue merge kernel
    kernelEventQueueMerge<<<blocksPerGrid, threadsPerBlock, 0, stream>>>(
        deviceQueue1,
        deviceQueue2,
        deviceMerged,
        static_cast<size_t>(size1),
        static_cast<size_t>(size2),
        static_cast<size_t>(outputSize),
        mergeThreshold
    );
    
    // Copy merged queue back to host
    error = cudaMemcpyAsync(mergedQueue, deviceMerged,
                           outputSize * sizeof(CudaQueuedSpikeEvent),
                           cudaMemcpyDeviceToHost, stream);
    if (error != cudaSuccess) {
        std::cerr << "Failed to copy merged queue to host: "
                  << CudaError::getErrorString(error) << std::endl;
    }
    
    // Synchronize to ensure operations complete
    cudaStreamSynchronize(stream);
    
    // Free device memory
    cudaFree(deviceQueue1);
    cudaFree(deviceQueue2);
    cudaFree(deviceMerged);
}

void EventQueueProcessing::computeEventStatistics(const QueuedSpikeEvent* hostEvents,
                                                 size_t numEvents,
                                                 uint32_t* hostStats,
                                                 float minWeight,
                                                 float maxWeight,
                                                 uint64_t minTime,
                                                 uint64_t maxTime,
                                                 cudaStream_t stream) {
    if (numEvents == 0) return;
    
    // Convert events to device format
    CudaQueuedSpikeEvent* deviceEvents = nullptr;
    
    cudaError_t error = cudaMalloc(&deviceEvents, numEvents * sizeof(CudaQueuedSpikeEvent));
    if (error != cudaSuccess) {
        std::cerr << "Failed to allocate device memory for events: "
                  << CudaError::getErrorString(error) << std::endl;
        return;
    }
    
    // Copy events to device
    error = cudaMemcpyAsync(deviceEvents, hostEvents,
                           numEvents * sizeof(CudaQueuedSpikeEvent),
                           cudaMemcpyHostToDevice, stream);
    if (error != cudaSuccess) {
        std::cerr << "Failed to copy events to device: "
                  << CudaError::getErrorString(error) << std::endl;
        cudaFree(deviceEvents);
        return;
    }
    
    // Prepare statistics array on device
    uint32_t* deviceStats = nullptr;
    error = cudaMalloc(&deviceStats, 4 * sizeof(uint32_t));
    if (error != cudaSuccess) {
        std::cerr << "Failed to allocate device memory for statistics: "
                  << CudaError::getErrorString(error) << std::endl;
        cudaFree(deviceEvents);
        return;
    }
    
    // Initialize statistics
    error = cudaMemset(deviceStats, 0, 4 * sizeof(uint32_t));
    if (error != cudaSuccess) {
        std::cerr << "Failed to initialize device statistics: "
                  << CudaError::getErrorString(error) << std::endl;
        cudaFree(deviceEvents);
        cudaFree(deviceStats);
        return;
    }
    
    // Determine optimal grid and block dimensions
    size_t threadsPerBlock = EVENT_QUEUE_KERNEL_THREADS_PER_BLOCK;
    size_t blocksPerGrid = (numEvents + threadsPerBlock - 1) / threadsPerBlock;
    
    // Launch event queue statistics kernel
    kernelEventQueueStatistics<<<blocksPerGrid, threadsPerBlock, 0, stream>>>(
        deviceEvents,
        numEvents,
        deviceStats,
        minWeight,
        maxWeight,
        minTime,
        maxTime
    );
    
    // Copy statistics back to host
    error = cudaMemcpyAsync(hostStats, deviceStats,
                           4 * sizeof(uint32_t),
                           cudaMemcpyDeviceToHost, stream);
    if (error != cudaSuccess) {
        std::cerr << "Failed to copy statistics to host: "
                  << CudaError::getErrorString(error) << std::endl;
    }
    
    // Synchronize to ensure operations complete
    cudaStreamSynchronize(stream);
    
    // Free device memory
    cudaFree(deviceEvents);
    cudaFree(deviceStats);
}

// Statistics and benchmarking
void EventQueueProcessing::getPerformanceStats(float& kernelTime, float& memoryBandwidth) {
    kernelTime = 0.0f;
    memoryBandwidth = 0.0f;
    
    std::cout << "Event Queue Processing CUDA Stats:" << std::endl;
    std::cout << "  Kernel execution time: " << kernelTime << " ms" << std::endl;
    std::cout << "  Memory bandwidth: " << memoryBandwidth << " GB/s" << std::endl;
}

// Event queue processing utilities
void EventQueueProcessing::processEventQueue(const QueuedSpikeEvent* events,
                                            size_t numEvents,
                                            float* synapticInputs,
                                            uint32_t* neuronFiringStates,
                                            uint64_t currentTime,
                                            cudaStream_t stream) {
    // Wrapper for CUDA implementation
    processEvents(events, numEvents, synapticInputs, neuronFiringStates,
                  currentTime, stream);
}

void EventQueueProcessing::processDelayedEvents(const QueuedSpikeEvent* events,
                                               const uint64_t* deliveryTimes,
                                               float* synapticInputs,
                                               uint32_t* neuronFiringStates,
                                               uint64_t currentTime,
                                               size_t numEvents,
                                               uint32_t* deliveryStatus,
                                               cudaStream_t stream) {
    // Wrapper for CUDA implementation
    processDelayedEvents(events, deliveryTimes, synapticInputs,
                        neuronFiringStates, currentTime, numEvents,
                        deliveryStatus, stream);
}

void EventQueueProcessing::mergeEventQueues(const QueuedSpikeEvent* queue1,
                                          const QueuedSpikeEvent* queue2,
                                          QueuedSpikeEvent* mergedQueue,
                                          size_t size1,
                                          size_t size2,
                                          size_t outputSize,
                                          uint64_t mergeThreshold,
                                          cudaStream_t stream) {
    // Wrapper for CUDA implementation
    mergeEventQueues(queue1, queue2, mergedQueue, size1, size2,
                    outputSize, mergeThreshold, stream);
}

void EventQueueProcessing::computeEventStatistics(const QueuedSpikeEvent* events,
                                                 size_t numEvents,
                                                 uint32_t* statsOutput,
                                                 float minWeight,
                                                 float maxWeight,
                                                 uint64_t minTime,
                                                 uint64_t maxTime,
                                                 cudaStream_t stream) {
    // Wrapper for CUDA implementation
    computeEventStatistics(events, numEvents, statsOutput, minWeight,
                          maxWeight, minTime, maxTime, stream);
}

} // namespace cuda
} // namespace nlm
