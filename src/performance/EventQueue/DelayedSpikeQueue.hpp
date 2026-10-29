// DelayedSpikeQueue - Time-bucketed delayed spike scheduler
// 
// Provides efficient scheduling of delayed spike events with O(1) delivery.
// Uses time-bucketing to avoid per-event searching and improve cache locality.
// 
// Features:
// - Time-bucketed scheduling for O(1) delivery
// - Lock-free operations for thread safety
// - Pre-allocated time buckets
// - Efficient memory usage
// - Cache-friendly data layout
// 
// Biological motivation:
// - Synaptic delays are critical for neural timing
// - Requires fast, predictable delivery to maintain temporal accuracy
// - Memory-efficient like biological synaptic transmission

#pragma once

#include "../core/Types/Types.hpp"
#include <cstddef>
#include <cstdint>
#include <atomic>
#include <vector>
#include <algorithm>

namespace nlm {

/**
 * DelayedSpikeQueue - Time-bucketed delayed spike scheduler
 * 
 * Efficiently schedules and delivers delayed spike events using time buckets.
 * Provides O(1) delivery for spikes with the same delay.
 */
class DelayedSpikeQueue {
public:
    /**
     * Create a delayed spike queue
     * @param maxDelay Maximum delay in simulation steps
     * @param timeBucketSize Number of time buckets (power of two)
     */
    explicit DelayedSpikeQueue(size_t maxDelay = 1000, size_t timeBucketSize = 64);
    
    ~DelayedSpikeQueue();
    
    // Disable copying
    DelayedSpikeQueue(const DelayedSpikeQueue&) = delete;
    DelayedSpikeQueue& operator=(const DelayedSpikeQueue&) = delete;
    
    /**
     * Schedule a delayed spike event
     * @param spikeEvent Spike event with delivery time
     * @return true if successfully scheduled
     */
    bool schedule(const DelayedSpikeEvent& spikeEvent);
    
    /**
     * Deliver spikes due at or before current time
     * @param currentStep Current simulation step
     * @param currentTime Current simulation time
     * @param spikeCallback Callback function for delivered spikes
     */
    template<typename Callback>
    void deliverUntil(SimulationStep currentStep, Timestamp currentTime, Callback spikeCallback);
    
    /**
     * Get number of pending events
     */
    size_t pendingCount() const { return pendingCount_.load(std::memory_order_relaxed); }
    
    /**
     * Clear all pending events
     */
    void clear();
    
    /**
     * Get number of events delivered
     */
    uint64_t totalDeliveredCount() const { return totalDeliveredCount_.load(std::memory_order_relaxed); }
    
    /**
     * Get current time bucket index
     */
    size_t currentBucket(SimulationStep currentStep, Timestamp timestep) const;
    
private:
    // Time bucket structure
    struct TimeBucket {
        std::vector<DelayedSpikeEvent> events;
        
        TimeBucket() {}
        
        void clear() {
            events.clear();
            events.shrink_to_fit();
        }
    };
    
    // Time bucketing configuration
    size_t maxDelay_;
    size_t timeBucketSize_;
    size_t numBuckets_;
    
    // Current time bucket
    std::atomic<size_t> currentBucketIndex_;
    
    // Time buckets (circular buffer)
    std::vector<std::unique_ptr<TimeBucket>> buckets_;
    
    // Statistics
    std::atomic<size_t> pendingCount_;
    std::atomic<uint64_t> totalDeliveredCount_;
    
    // Helper functions
    size_t bucketIndex(SimulationStep deliveryStep, size_t currentStep) const;
    void deliverBucket(size_t bucketIndex, SimulationStep currentStep, Timestamp currentTime, 
                      Timestamp timestep, std::vector<DelayedSpikeEvent>& outEvents);
};

} // namespace nlm
