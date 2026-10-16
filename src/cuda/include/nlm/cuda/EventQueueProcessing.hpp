// EventQueueProcessing.hpp: CUDA event queue processing interface
#pragma once

#include "../performance/EventQueue.hpp"
#include "DeviceMemoryManager.hpp"
#include <cstddef>

namespace nlm {
namespace cuda {

// Performance metrics for event queue processing
enum class PerformanceMetric {
    EVENTS_PROCESSED_PER_SECOND,
    QUEUE_MERGES,
    STATISTICS_COMPUTED,
    BANDWIDTH
};

// Configuration for event queue processing
struct EventQueueProcessingConfig {
    size_t queueSize;
    bool useMemoryPools;
    bool enableAsynchronousTransfers;
    size_t maxThreadsPerBlock;
    size_t maxBlocksPerGrid;
    size_t sharedMemorySize;
    bool useVectorizedOperations;
    bool enableWarpSynchronous;
    float computeCapabilityMajor;
    float computeCapabilityMinor;
    size_t maxEventsPerKernel;
    bool enableMergeOptimization;
    bool enableStatisticsComputation;
};

// Event queue processing class for GPU acceleration
class EventQueueProcessing {
public:
    EventQueueProcessing();
    ~EventQueueProcessing();
    
    // Setup event queue processing on GPU
    void setup(DeviceMemoryManager& memoryManager,
              const EventQueueProcessingConfig& config);
    
    // Process spike events on GPU
    void processEvents(DeviceMemoryManager& memoryManager,
                      const QueuedSpikeEvent* events,
                      size_t numEvents,
                      float* synapticInputs,
                      uint32_t* neuronFiringStates,
                      uint64_t currentTime,
                      cudaStream_t stream = nullptr);
    
    // Process delayed spike events on GPU
    void processDelayedEvents(DeviceMemoryManager& memoryManager,
                             const QueuedSpikeEvent* events,
                             const uint64_t* deliveryTimes,
                             size_t numEvents,
                             float* synapticInputs,
                             uint32_t* neuronFiringStates,
                             uint64_t currentTime,
                             cudaStream_t stream = nullptr);
    
    // Merge event queues on GPU
    void mergeEvents(DeviceMemoryManager& memoryManager,
                    const QueuedSpikeEvent* queue1,
                    const QueuedSpikeEvent* queue2,
                    QueuedSpikeEvent* mergedQueue,
                    size_t size1,
                    size_t size2,
                    size_t outputSize,
                    uint64_t mergeThreshold,
                    cudaStream_t stream = nullptr);
    
    // Compute event queue statistics on GPU
    void computeStatistics(DeviceMemoryManager& memoryManager,
                          const QueuedSpikeEvent* events,
                          size_t numEvents,
                          uint32_t* statsOutput,
                          float minWeight,
                          float maxWeight,
                          uint64_t minTime,
                          uint64_t maxTime,
                          cudaStream_t stream = nullptr);
    
    // Get performance metrics
    float getPerformanceMetric(PerformanceMetric metric) const;
    
    // Print statistics
    void printStatistics() const;
    
private:
    EventQueueProcessingConfig config_;
    float eventsProcessedPerSecond_;
    size_t queueMerges_;
    size_t statisticsComputed_;
    float gpuTime_;
    float memoryBandwidth_;
};

// Global event queue processing instance
extern EventQueueProcessing eventQueueProcessing;

} // namespace cuda
} // namespace nlm
