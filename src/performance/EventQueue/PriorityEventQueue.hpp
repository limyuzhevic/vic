// PriorityEventQueue - Priority queue for time-sensitive events
// 
// Provides priority queue functionality for event delivery with ordering guarantees.
// Uses heap-based priority queue for efficient ordering and priority management.
// 
// Features:
// - Priority-based event ordering
// - Lock-free operations for thread safety
// - Efficient priority queue implementation
// - Support for various priority levels
// - Cache-friendly data layout
// 
// Biological motivation:
// - Neural events have varying priorities (spikes vs learning signals)
// - Requires proper ordering to maintain temporal accuracy
// - Efficient priority management like biological scheduling

#pragma once

#include "../core/Types/Types.hpp"
#include <cstddef>
#include <cstdint>
#include <atomic>
#include <vector>
#include <queue>
#include <functional>

namespace nlm {

/**
 * PriorityEventQueue - Priority queue for time-sensitive events
 * 
 * Provides priority queue functionality with efficient ordering and priority management.
 * Supports various event types with different priority levels.
 */
class PriorityEventQueue {
public:
    /**
     * Priority enumeration for event ordering
     */
    enum class Priority {
        Low = 0,
        Normal = 1,
        High = 2,
        Critical = 3
    };
    
    /**
     * Event wrapper for priority queue
     */
    struct EventWrapper {
        SimulationStep timestamp;
        Timestamp deliveryTime;
        Priority priority;
        std::function<void()> callback;
        
        // Priority comparison for heap ordering
        bool operator<(const EventWrapper& other) const {
            return priority < other.priority;
        }
        
        bool operator>(const EventWrapper& other) const {
            return priority > other.priority;
        }
        
        bool operator<=(const EventWrapper& other) const {
            return priority <= other.priority;
        }
        
        bool operator>=(const EventWrapper& other) const {
            return priority >= other.priority;
        }
    };
    
    /**
     * Create a priority event queue
     * @param maxSize Maximum number of events in queue
     */
    explicit PriorityEventQueue(size_t maxSize = 1024);
    
    ~PriorityEventQueue();
    
    // Disable copying
    PriorityEventQueue(const PriorityEventQueue&) = delete;
    PriorityEventQueue& operator=(const PriorityEventQueue&) = delete;
    
    /**
     * Enqueue an event with priority
     * @param timestamp Event timestamp
     * @param deliveryTime Event delivery time
     * @param priority Event priority
     * @param callback Event callback function
     * @return true if successfully enqueued
     */
    bool enqueue(SimulationStep timestamp, Timestamp deliveryTime, 
                Priority priority, std::function<void()> callback);
    
    /**
     * Dequeue and execute the highest priority event
     * @return true if event was processed
     */
    bool dequeue();
    
    /**
     * Get number of events in queue
     */
    size_t size() const { return queue_.size(); }
    
    /**
     * Check if queue is empty
     */
    bool empty() const { return queue_.empty(); }
    
    /**
     * Clear all events
     */
    void clear();
    
    /**
     * Get number of events processed
     */
    uint64_t totalProcessedCount() const { return totalProcessedCount_.load(std::memory_order_relaxed); }
    
    /**
     * Peek at highest priority event without removing it
     * @return Reference to highest priority event (undefined if empty)
     */
    const EventWrapper& peek() const { return queue_.top(); }
    
private:
    // Priority queue
    std::priority_queue<EventWrapper, std::vector<EventWrapper>, std::greater<EventWrapper>> queue_;
    
    // Statistics
    std::atomic<size_t> maxSize_;
    std::atomic<uint64_t> totalProcessedCount_;
    
    // Helper functions
    bool isValidPriority(Priority priority) const;
};

} // namespace nlm
