#pragma once

/**
 * EventSystem - Optimized event queue for neural simulation
 * 
 * Provides high-performance event processing using a combination of:
 * - Priority queues for time-critical events
 * - Thread-local queues for parallel processing
 * - Event object pooling for zero-allocation processing
 * - Zero-copy optimizations for maximum throughput
 * - Memory-efficient event storage and processing
 * 
 * Features:
 * - Lock-free event scheduling with thread-local buffers
 * - Priority-based event processing (spike > plasticity > development > neuromodulation)
 * - Event object pooling with automatic reuse
 * - Cache-friendly memory layout
 * - Parallel event processing support
 * - Configurable event size and capacity
 * 
 * Biological motivation:
 * - Neural events are discrete and time-critical (spike timing)
 * - Different event types have different urgency (spikes immediate, plasticity delayed)
 * - Event-driven processing saves computation on inactive neurons
 * - Neuromodulation creates global signals affecting many neurons
 */

#include <cstddef>
#include <cstdint>
#include <queue>
#include <vector>
#include <mutex>
#include <atomic>
#include <algorithm>
#include <memory>
#include <optional>

namespace nlm {

// Forward declarations
class EventSystem;

/**
 * Event iterator for event pool
 */
class EventPoolIterator {
public:
    EventPoolIterator() : ptr_(nullptr), index_(0) {}
    EventPoolIterator(Event** ptr) : ptr_(ptr), index_(0) {
        if (ptr_) {
            // Find index in event pool
            for (size_t i = 0; i < maxPoolSize; ++i) {
                if (&eventSystem_.eventPool_[i] == ptr) {
                    index_ = i;
                    break;
                }
            }
        }
    }
    
    EventPoolIterator(std::vector<Event*>::iterator it) : ptr_(nullptr), index_(0) {
        if (it != std::vector<Event*>::end()) {
            ptr_ = *it;
            if (&eventSystem_.eventPool_[0] <= ptr_ && ptr_ < &eventSystem_.eventPool_[maxPoolSize]) {
                index_ = ptr_ - &eventSystem_.eventPool_[0];
            }
        }
    }
    
    EventPoolIterator& operator++() {
        if (ptr_ && index_ < eventSystem_.eventPool_.size()) {
            ptr_ = eventSystem_.eventPool_[++index_];
        } else {
            ptr_ = nullptr;
        }
        return *this;
    }
    
    EventPoolIterator operator++(int) {
        EventPoolIterator temp = *this;
        ++(*this);
        return temp;
    }
    
    Event& operator*() { return *ptr_; }
    Event* operator->() { return ptr_; }
    
    bool operator==(const EventPoolIterator& other) const { return ptr_ == other.ptr_; }
    bool operator!=(const EventPoolIterator& other) const { return ptr_ != other.ptr_; }
    
    static EventPoolIterator create(Event* event) {
        EventPoolIterator it;
        if (event) {
            for (size_t i = 0; i < eventSystem_.eventPool_.size(); ++i) {
                if (eventSystem_.eventPool_[i] == event) {
                    it.ptr_ = event;
                    it.index_ = i;
                    return it;
                }
            }
        }
        return it;
    }
    
private:
    Event** ptr_;
    size_t index_;
    static EventSystem& eventSystem_;
    static constexpr size_t maxPoolSize = 100000;
    
    friend class EventSystem;
};

/**
 * Event system for neural simulation
 */
class EventSystem {
public:
    /**
     * Event types for different neural processes
     */
    enum class EventType {
        Spike,           // Neuron spike event (highest priority)
        Plasticity,      // Synaptic plasticity event (medium priority)
        Structural,      // Structural plasticity event (low priority)
        Development,     // Developmental change (lowest priority)
        Neuromodulation  // Neuromodulatory signal (special handling)
    };
    
    /**
     * Create event system with specified capacity
     */
    explicit EventSystem(size_t maxEvents = 100000);
    
    ~EventSystem();
    
    // Disable copying
    EventSystem(const EventSystem&) = delete;
    EventSystem& operator=(const EventSystem&) = delete;
    
    /**
     * Schedule an event for future processing
     */
    void scheduleEvent(const Event& event);
    
    /**
     * Process the next highest priority event
     */
    Event processNextEvent();
    
    /**
     * Check if there are any pending events
     */
    bool hasEvents() const;
    
    /**
     * Clear completed events (time <= current time)
     */
    void clearCompletedEvents();
    
    /**
     * Optimize memory layout for better cache performance
     */
    void optimizeMemoryLayout();
    
    /**
     * Get current event count
     */
    size_t getEventCount() const;
    
    /**
     * Get average event processing time
     */
    double getAverageProcessingTime() const;
    
    // Configuration methods
    
    /**
     * Set current simulation time for event expiration
     */
    void setCurrentTime(Timestamp time);
    
    /**
     * Enable/disable parallel event processing
     */
    void setParallelProcessing(bool enable);
    
    /**
     * Enable/disable zero-copy optimizations
     */
    void setZeroCopy(bool enable);
    
    /**
     * Set number of worker threads
     */
    void setThreadCount(size_t count);
    
    /**
     * Enable/disable event recycling
     */
    void enableEventRecycling(bool enable);
    
    /**
     * Set maximum number of events
     */
    void setMaxEvents(size_t maxEvents);
    
    /**
     * Update current thread ID for thread-local processing
     */
    void updateCurrentThread();
    
    /**
     * Get iterator to event pool
     */
    EventPoolIterator begin();
    
    /**
     * Get end iterator for event pool
     */
    EventPoolIterator end();
    
    /**
     * Get free event from pool
     */
    EventPoolIterator getFreeEvent();
    
    /**
     * Return event to pool for reuse
     */
    void returnEvent(EventPoolIterator iterator);
    
    /**
     * Create event iterator from event pointer
     */
    static EventPoolIterator createEventIterator(Event* event);
    
private:
    // Event configuration
    size_t maxEvents_;
    Timestamp currentSimulationTime_;
    
    // Event statistics
    std::atomic<size_t> eventCount_;
    double processingTimeTotal_;
    std::atomic<size_t> processingCount_;
    
    // Performance flags
    bool enableZeroCopy_;
    bool enableParallelProcessing_;
    
    // Main event queue (priority queue)
    std::priority_queue<Event> mainQueue_;
    
    // Thread-local queues for parallel processing
    bool parallelEnabled_;
    size_t maxThreadCount_;
    std::vector<std::deque<Event>> threadLocalQueues_;
    std::vector<Event*> threadLocalBuffers_;
    
    // Event pool for zero-allocation
    std::vector<Event*> eventPool_;
    std::vector<size_t> eventPoolIndices_;
    std::vector<size_t> freeEventIndices_;
    size_t eventPoolSize_;
    size_t eventReuseCount_;
    
    // Thread safety
    std::mutex queueMutex_;
    std::mutex poolMutex_;
    std::mutex bufferMutex_;
    
    // Thread tracking
    size_t currentThreadId_;
    
    // Statistics
    std::atomic<SimulationStep> lastProcessingStep_;
};

} // namespace nlm

// Forward declarations for EventPoolIterator
namespace nlm {
    EventSystem& EventPoolIterator::eventSystem_;
}