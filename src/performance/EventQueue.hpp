#pragma once

/**
 * EventQueue - Lock-free event queue for spike processing
 * 
 * Provides high-performance event queue using ring buffers.
 * Designed for event-driven neural simulation where most neurons are inactive.
 * 
 * Features:
 * - Lock-free single-producer single-consumer (SPSC) ring buffer
 * - Bounded capacity with overflow handling
 * - Cache-efficient circular buffer
 * - Support for immediate and delayed events
 * 
 * Biological motivation:
 * - Spike events are discrete, infrequent signals
 * - Synaptic transmission has delays (1-20ms in biological systems)
 * - Event-driven processing saves computation on silent neurons
 */

#include <cstddef>
#include <cstdint>
#include <atomic>
#include <vector>
#include <array>
#include <optional>
#include <limits>
#include <algorithm>

namespace nlm {

/**
 * @file EventQueue.hpp
 * 
 * Event queue system for NLM
 * 
 * This file provides the event queue system for NLM, implementing:
 * - Spike event management
 * - Lock-free ring buffer
 * - Delayed spike queue
 * - Priority event queue
 * - Event processing pipeline
 * 
 * The event queue system in NLM:
 * - **SpikeEvent**: Spike event with minimal data for queue processing
 * - **SpikeRingBuffer**: SPSC ring buffer for immediate spike events
 * - **DelayedSpikeQueue**: Time-bucketed delayed spike queue
 * - **PriorityEventQueue**: Priority event queue for time-sensitive events
 * - **Event processing**: Event processing pipeline
 * 
 * Key features:
 * - **Spike event management**: Spike event management
 * - **Lock-free ring buffer**: Lock-free ring buffer
 * - **Delayed spike queue**: Delayed spike queue
 * - **Priority event queue**: Priority event queue
 * - **Event processing**: Event processing pipeline
 * 
 * Event characteristics in NLM:
 * - **Spike event management**: Spike event management
 * - **Lock-free ring buffer**: Lock-free ring buffer
 * - **Delayed spike queue**: Delayed spike queue
 * - **Priority event queue**: Priority event queue
 * - **Event processing**: Event processing pipeline
 * 
 * @note Critical for performance optimization
 * @note Used extensively in event systems
 * @note Essential for spike processing
 * @see EventQueue.cpp - For implementation details
 * 
 * @author NLM Development Team
 * @date 2026
 * @version 1.0
 */

// Constants for event queue sizing
constexpr size_t SPIKE_QUEUE_CAPACITY = 16384;  // Must be power of 2
constexpr size_t DELAYED_SPIKE_BUCKETS = 1024;  // Number of time buckets
constexpr size_t MAX_DELAY_STEPS = 100;  // Maximum synaptic delay in steps

/**
 * @struct QueuedSpikeEvent
 * 
 * Neural implementation of spike event with minimal data for queue processing.
 * 
 * This structure implements the spike event, providing
 * spike event with minimal data for queue processing. It is essential for:
 * - **Spike event**: Spike event with minimal data
 * - **Event storage**: Event storage
 * - **Event transmission**: Event transmission
 * - **Spike processing**: Spike processing
 * - **Event queuing**: Event queuing
 * 
 * Spike event characteristics in NLM:
 * - **Spike event**: Spike event with minimal data
 * - **Event storage**: Event storage
 * - **Event transmission**: Event transmission
 * - **Spike processing**: Spike processing
 * - **Event queuing**: Event queuing
 * 
 * Key components:
 * - **Source neuron**: Source neuron
 * - **Destination neuron**: Destination neuron
 * - **Synapse ID**: Synapse ID
 * - **Weight**: Weight
 * - **Excitatory**: Excitatory
 * 
 * @note Spike event with minimal data for queue processing
 * 
 * @author NLM Development Team
 */
struct alignas(16) QueuedSpikeEvent {
    /** Source neuron */
    uint64_t sourceNeuron;
    
    /** Destination neuron */
    uint64_t destNeuron;
    
    /** Synapse ID */
    uint64_t synapseId;
    
    /** Weight */
    float weight;
    
    /** Is excitatory */
    uint8_t isExcitatory;
    
    /** Padding for cache alignment */
    uint8_t padding[7];  // Pad to 16 bytes for cache alignment
    
    /**
     * @brief Default constructor
     * 
     * Creates a default queued spike event with default parameters.
     * The queued spike event is initialized with default values for
     * all event fields.
     */
    QueuedSpikeEvent() : sourceNeuron(0), destNeuron(0), synapseId(0), 
                         weight(0.0f), isExcitatory(1), padding{} {}
    
    /**
     * @brief Parameterized constructor
     * 
     * Creates a queued spike event with specified parameters.
     * The queued spike event is initialized with specified source,
     * destination, synapse, weight, and excitatory flags.
     */
    QueuedSpikeEvent(uint64_t src, uint64_t dst, uint64_t syn, float w, bool exc)
        : sourceNeuron(src), destNeuron(dst), synapseId(syn), weight(w), 
          isExcitatory(exc ? 1 : 0), padding{} {}
};

/**
 * @class SpikeRingBuffer
 * 
 * Neural implementation of SPSC ring buffer for immediate spike events.
 * 
 * This class implements the SPSC ring buffer, providing
 * SPSC ring buffer for immediate spike events. It is essential for:
 * - **Spike event storage**: Spike event storage
 * - **Lock-free processing**: Lock-free processing
 * - **Ring buffer**: Ring buffer
 * - **Event queuing**: Event queuing
 * - **Event dequeuing**: Event dequeuing
 * 
 * Ring buffer characteristics in NLM:
 * - **Spike event storage**: Spike event storage
 * - **Lock-free processing**: Lock-free processing
 * - **Ring buffer**: Ring buffer
 * - **Event queuing**: Event queuing
 * - **Event dequeuing**: Event dequeuing
 * 
 * Key components:
 * - **Buffer capacity**: Buffer capacity
 * - **Buffer mask**: Buffer mask
 * - **Buffer storage**: Buffer storage
 * - **Head index**: Head index
 * - **Tail index**: Tail index
 * 
 * @note Lock-free single-producer single-consumer (SPSC) ring buffer
 * @see EventQueue.cpp - For implementation details
 * 
 * @author NLM Development Team
 */
class SpikeRingBuffer {
public:
    /**
     * @brief Default constructor
     * 
     * Creates a default spike ring buffer with default parameters.
     * The spike ring buffer is initialized with default values for
     * all buffer fields.
     */
    explicit SpikeRingBuffer(size_t capacity = SPIKE_QUEUE_CAPACITY);
    
    /**
     * @brief Destructor
     * 
     * Cleans up spike ring buffer resources and performs any necessary
     * cleanup operations. The spike ring buffer is left in a valid but
     * non-operational state after destruction.
     */
    ~SpikeRingBuffer();
    
    /**
     * @brief Push a spike event to the queue
     * 
     * Pushes a spike event to the queue. This is used for:
     * - Event queuing
     * - Event storage
     * - Event processing
     * - Event control
     */
    bool push(const QueuedSpikeEvent& event);
    
    /**
     * @brief Pop a spike event from the queue
     * 
     * Pops a spike event from the queue. This is used for:
     * - Event dequeuing
     * - Event retrieval
     * - Event processing
     * - Event control
     */
    std::optional<QueuedSpikeEvent> pop();
    
    /**
     * @brief Check if queue is empty
     * 
     * Checks if the queue is empty. This is used for:
     * - Empty checking
     * - Queue status checking
     * - Event processing
     * - Event control
     */
    bool empty() const;
    
    /**
     * @brief Check if queue is full
     * 
     * Checks if the queue is full. This is used for:
     * - Full checking
     * - Queue status checking
     * - Event processing
     * - Event control
     */
    bool full() const;
    
    /**
     * @brief Get number of events in queue
     * 
     * Returns the number of events in queue. This is used for:
     * - Event count access
     * - Event count management
     * - Event processing
     * - Event control
     */
    size_t size() const;
    
    /**
     * @brief Get queue capacity
     * 
     * Returns the queue capacity. This is used for:
     * - Capacity access
     * - Capacity management
     * - Event processing
     * - Event control
     */
    size_t capacity() const { return capacity_; }
    
    /**
     * @brief Clear all events
     * 
     * Clears all events. This is used for:
     * - Event clearing
     * - Queue clearing
     * - Event processing
     * - Event control
     */
    void clear();
    
private:
    /** Buffer capacity */
    const size_t capacity_;
    
    /** Bit mask for fast modulo (capacity must be power of 2) */
    const size_t mask_;  // Bit mask for fast modulo (capacity must be power of 2)
    
    /** Storage */
    std::vector<QueuedSpikeEvent> buffer_;
    
    /** Atomic head index (written by consumer) */
    std::atomic<size_t> head_;
    
    /** Atomic tail index (written by producer) */
    std::atomic<size_t> tail_;
};

/**
 * @class DelayedSpikeQueue
 * 
 * Neural implementation of time-bucketed delayed spike queue.
 * 
 * This class implements the delayed spike queue, providing
 * time-bucketed delayed spike queue. It is essential for:
 * - **Delayed spike processing**: Delayed spike processing
 * - **Time bucketing**: Time bucketing
 * - **Bucket processing**: Bucket processing
 * - **Event scheduling**: Event scheduling
 * - **Event delivery**: Event delivery
 * 
 * Delayed spike characteristics in NLM:
 * - **Delayed spike processing**: Delayed spike processing
 * - **Time bucketing**: Time bucketing
 * - **Bucket processing**: Bucket processing
 * - **Event scheduling**: Event scheduling
 * - **Event delivery**: Event delivery
 * 
 * Key components:
 * - **Number of buckets**: Number of buckets
 * - **Max delay steps**: Max delay steps
 * - **Current step**: Current step
 * - **Oldest step**: Oldest step
 * - **Bucket storage**: Bucket storage
 * 
 * @note Time-bucketed delayed spike queue
 * @see EventQueue.cpp - For implementation details
 * 
 * @author NLM Development Team
 */
class DelayedSpikeQueue {
public:
    /**
     * @brief Default constructor
     * 
     * Creates a default delayed spike queue with default parameters.
     * The delayed spike queue is initialized with default values for
     * all queue fields.
     */
    DelayedSpikeQueue(size_t numBuckets = DELAYED_SPIKE_BUCKETS, 
                      size_t maxDelaySteps = MAX_DELAY_STEPS);
    
    /**
     * @brief Destructor
     * 
     * Cleans up delayed spike queue resources and performs any necessary
     * cleanup operations. The delayed spike queue is left in a valid but
     * non-operational state after destruction.
     */
    ~DelayedSpikeQueue();
    
    /**
     * @brief Schedule a spike for delivery at a future step
     * 
     * Schedules a spike for delivery at a future step. This is used for:
     * - Event scheduling
     * - Delayed spike processing
     * - Event queuing
     * - Event delivery
     */
    void schedule(uint64_t sourceNeuron, uint64_t destNeuron, uint64_t synapseId,
                   float weight, bool isExcitatory, uint64_t deliveryStep);
    
    /**
     * @brief Get all spikes scheduled for delivery at current step
     * 
     * Returns all spikes scheduled for delivery at current step. This is used for:
     * - Spike retrieval
     * - Event processing
     * - Spike delivery
     * - Event control
     */
    std::vector<QueuedSpikeEvent> getDueSpikes(uint64_t currentStep);
    
    /**
     * @brief Get count of pending delayed spikes
     * 
     * Returns the count of pending delayed spikes. This is used for:
     * - Pending count access
     * - Pending count management
     * - Event processing
     * - Event control
     */
    size_t pendingCount() const;
    
    /**
     * @brief Get count of spikes in a specific bucket
     * 
     * Returns the count of spikes in a specific bucket. This is used for:
     * - Bucket size access
     * - Bucket management
     * - Event processing
     * - Event control
     */
    size_t bucketSize(uint64_t step) const;
    
    /**
     * @brief Clear all pending spikes
     * 
     * Clears all pending spikes. This is used for:
     * - Spike clearing
     * - Queue clearing
     * - Event processing
     * - Event control
     */
    void clear();
    
    /**
     * @brief Advance time bucket
     * 
     * Advances time bucket (call at end of simulation step). This is used for:
     * - Time advancement
     * - Bucket management
     * - Event processing
     * - Event control
     */
    void advanceTime(uint64_t currentStep);
    
private:
    /**
     * @struct SpikeBucket
     * 
     * Neural implementation of spike bucket.
     * 
     * This structure implements the spike bucket, providing
     * spike bucket for event queuing.
     */
    struct alignas(64) SpikeBucket {
        /**
         * Bucket capacity
         */
        static constexpr size_t BUCKET_CAPACITY = 256;
        
        /** Event storage */
        QueuedSpikeEvent events[BUCKET_CAPACITY];
        
        /** Atomic event count */
        std::atomic<size_t> count;
        
        /** Atomic read index */
        std::atomic<size_t> readIndex;
        
        /** Atomic write index */
        std::atomic<size_t> writeIndex;
        
        /**
         * @brief Default constructor
         * 
         * Creates a default spike bucket with default parameters.
         */
        SpikeBucket() : count(0), readIndex(0), writeIndex(0) {
            // Zero the events array
            for (auto& e : events) {
                new (&e) QueuedSpikeEvent();
            }
        }
        
        /**
         * @brief Push a spike event to bucket
         * 
         * Pushes a spike event to bucket. This is used for:
         * - Event queuing
         * - Bucket processing
         */
        bool push(const QueuedSpikeEvent& event) {
            if (count.load(std::memory_order_relaxed) >= BUCKET_CAPACITY) {
                return false;
            }
            size_t idx = writeIndex.fetch_add(1, std::memory_order_relaxed) % BUCKET_CAPACITY;
            events[idx] = event;
            count.fetch_add(1, std::memory_order_release);
            return true;
        }
        
        /**
         * @brief Drain all spikes from bucket
         * 
         * Drains all spikes from bucket. This is used for:
         * - Spike draining
         * - Event retrieval
         */
        std::vector<QueuedSpikeEvent> drain() {
            std::vector<QueuedSpikeEvent> result;
            size_t cnt = count.load(std::memory_order_acquire);
            result.reserve(cnt);
            for (size_t i = 0; i < cnt; ++i) {
                size_t idx = readIndex.fetch_add(1, std::memory_order_relaxed) % BUCKET_CAPACITY;
                result.push_back(events[idx]);
            }
            count.store(0, std::memory_order_release);
            return result;
        }
        
        /**
         * @brief Get bucket size
         * 
         * Returns the bucket size. This is used for:
         * - Bucket size access
         * - Bucket management
         */
        size_t size() const { return count.load(std::memory_order_acquire); }
    };
    
    /** Number of buckets */
    size_t numBuckets_;
    
    /** Maximum delay steps */
    size_t maxDelaySteps_;
    
    /** Current step */
    uint64_t currentStep_;
    
    /** Oldest step */
    uint64_t oldestStep_;
    
    /** Circular buffer of buckets */
    std::vector<SpikeBucket> buckets_;
    
    /** Pending bucket counts */
    std::vector<size_t> pendingCounts_;
};

/**
 * @class PriorityEventQueue
 * 
 * Neural implementation of priority event queue for time-sensitive events.
 * 
 * This class implements the priority event queue, providing
 * priority event queue for time-sensitive events. It is essential for:
 * - **Priority event processing**: Priority event processing
 * - **Event ordering**: Event ordering
 * - **Priority management**: Priority management
 * - **Time-sensitive processing**: Time-sensitive processing
 * - **Event prioritization**: Event prioritization
 * 
 * Priority event characteristics in NLM:
 * - **Priority event processing**: Priority event processing
 * - **Event ordering**: Event ordering
 * - **Priority management**: Priority management
 * - **Time-sensitive processing**: Time-sensitive processing
 * - **Event prioritization**: Event prioritization
 * 
 * Key components:
 * - **Max priority events**: Max priority events
 * - **Priority event**: Priority event
 * - **Event storage**: Event storage
 * - **Event pushing**: Event pushing
 * - **Event popping**: Event popping
 * 
 * @note Priority event queue for time-sensitive events
 * @see EventQueue.cpp - For implementation details
 * 
 * @author NLM Development Team
 */
class PriorityEventQueue {
public:
    /**
     * Maximum priority events
     */
    static constexpr size_t MAX_PRIORITY_EVENTS = 4096;
    
    /**
     * @brief Priority event
     * 
     * Priority event for queue processing.
     */
    struct PriorityEvent {
        /** Timestamp */
        uint64_t timestamp;
        
        /** Priority (0 = highest priority) */
        uint8_t priority;
        
        /** Event type ID */
        uint16_t eventType;
        
        /** Event-specific data */
        uint64_t data[4];
        
        /**
         * @brief Comparison operator
         * 
         * Comparison operator for priority queue.
         */
        bool operator<(const PriorityEvent& other) const {
            // Lower priority value = higher priority in min-heap
            if (priority != other.priority) return priority > other.priority;
            return timestamp > other.timestamp;
        }
    };
    
    /**
     * @brief Default constructor
     * 
     * Creates a default priority event queue with default parameters.
     * The priority event queue is initialized with default values for
     * all queue fields.
     */
    PriorityEventQueue();
    
    /**
     * @brief Push a priority event
     * 
     * Pushes a priority event. This is used for:
     * - Event pushing
     * - Event storage
     * - Event processing
     * - Event control
     */
    void push(uint64_t timestamp, uint8_t priority, uint16_t type, const uint64_t* data = nullptr);
    
    /**
     * @brief Pop the highest priority event
     * 
     * Pops the highest priority event. This is used for:
     * - Event popping
     * - Event retrieval
     * - Event processing
     * - Event control
     */
    std::optional<PriorityEvent> pop();
    
    /**
     * @brief Get all events due before a timestamp
     * 
     * Returns all events due before a timestamp. This is used for:
     * - Event retrieval
     * - Event processing
     * - Event scheduling
     * - Event control
     */
    std::vector<PriorityEvent> getDueEvents(uint64_t currentTime);
    
    /**
     * @brief Check if empty
     * 
     * Checks if the queue is empty. This is used for:
     * - Empty checking
     * - Queue status checking
     * - Event processing
     * - Event control
     */
    bool empty() const { return events_.empty(); }
    
    /**
     * @brief Clear all events
     * 
     * Clears all events. This is used for:
     * - Event clearing
     * - Queue clearing
     * - Event processing
     * - Event control
     */
    void clear() { events_.clear(); }
    
    /**
     * @brief Get count
     * 
     * Returns the count. This is used for:
     * - Count access
     * - Count management
     * - Event processing
     * - Event control
     */
    size_t size() const { return events_.size(); }
    
private:
    /** Event storage */
    std::vector<PriorityEvent> events_;
};

// Inline implementations for performance

inline SpikeRingBuffer::SpikeRingBuffer(size_t capacity)
    : capacity_(capacity)
    , mask_(capacity - 1)  // capacity MUST be power of 2
    , buffer_(capacity_)
    , head_(0)
    , tail_(0)
{
    // Verify capacity is power of 2
    assert((capacity & (capacity - 1)) == 0 && "SpikeRingBuffer capacity must be power of 2");
}

inline SpikeRingBuffer::~SpikeRingBuffer() = default;

inline bool SpikeRingBuffer::push(const QueuedSpikeEvent& event) {
    size_t tail = tail_.load(std::memory_order_relaxed);
    size_t nextTail = (tail + 1) & mask_;
    
    if (nextTail == head_.load(std::memory_order_acquire)) {
        return false;  // Queue full
    }
    
    buffer_[tail] = event;
    tail_.store(nextTail, std::memory_order_release);
    return true;
}

inline std::optional<QueuedSpikeEvent> SpikeRingBuffer::pop() {
    size_t head = head_.load(std::memory_order_relaxed);
    
    if (head == tail_.load(std::memory_order_acquire)) {
        return std::nullopt;  // Queue empty
    }
    
    QueuedSpikeEvent event = buffer_[head];
    head_.store((head + 1) & mask_, std::memory_order_release);
    return event;
}

inline bool SpikeRingBuffer::empty() const {
    return head_.load(std::memory_order_acquire) == tail_.load(std::memory_order_acquire);
}

inline bool SpikeRingBuffer::full() const {
    size_t nextTail = (tail_.load(std::memory_order_acquire) + 1) & mask_;
    return nextTail == head_.load(std::memory_order_acquire);
}

inline size_t SpikeRingBuffer::size() const {
    size_t head = head_.load(std::memory_order_acquire);
    size_t tail = tail_.load(std::memory_order_acquire);
    return (tail - head) & mask_;
}

inline void SpikeRingBuffer::clear() {
    head_.store(0, std::memory_order_release);
    tail_.store(0, std::memory_order_release);
}

inline DelayedSpikeQueue::DelayedSpikeQueue(size_t numBuckets, size_t maxDelaySteps)
    : numBuckets_(numBuckets)
    , maxDelaySteps_(maxDelaySteps)
    , currentStep_(0)
    , oldestStep_(0)
    , buckets_(numBuckets)
    , pendingCounts_(numBuckets, 0)
{
    assert(numBuckets >= maxDelaySteps && "numBuckets must be >= maxDelaySteps");
}

inline DelayedSpikeQueue::~DelayedSpikeQueue() = default;

inline void DelayedSpikeQueue::schedule(uint64_t sourceNeuron, uint64_t destNeuron, 
                                        uint64_t synapseId, float weight, 
                                        bool isExcitatory, uint64_t deliveryStep) {
    if (deliveryStep < currentStep_) return;  // Already due, skip
    
    size_t bucketIdx = (deliveryStep % numBuckets_);
    QueuedSpikeEvent event(sourceNeuron, destNeuron, synapseId, weight, isExcitatory);
    
    if (buckets_[bucketIdx].push(event)) {
        pendingCounts_[bucketIdx].fetch_add(1, std::memory_order_relaxed);
    }
}

inline std::vector<QueuedSpikeEvent> DelayedSpikeQueue::getDueSpikes(uint64_t currentStep) {
    currentStep_ = currentStep;
    std::vector<QueuedSpikeEvent> result;
    
    // Process all buckets up to current step
    while (oldestStep_ <= currentStep) {
        size_t bucketIdx = oldestStep_ % numBuckets_;
        auto spikes = buckets_[bucketIdx].drain();
        result.insert(result.end(), spikes.begin(), spikes.end());
        pendingCounts_[bucketIdx].store(0, std::memory_order_relaxed);
        ++oldestStep_;
    }
    
    return result;
}

inline size_t DelayedSpikeQueue::pendingCount() const {
    size_t total = 0;
    for (size_t c : pendingCounts_) {
        total += c;
    }
    return total;
}

inline size_t DelayedSpikeQueue::bucketSize(uint64_t step) const {
    return buckets_[step % numBuckets_].size();
}

inline void DelayedSpikeQueue::clear() {
    for (auto& bucket : buckets_) {
        bucket = SpikeBucket();  // Reset bucket to default state
    }
    std::fill(pendingCounts_.begin(), pendingCounts_.end(), 0);
    oldestStep_ = currentStep_;
}

inline void DelayedSpikeQueue::advanceTime(uint64_t currentStep) {
    // Advance the oldest step marker
    while (oldestStep_ <= currentStep && oldestStep_ <= currentStep - numBuckets_) {
        size_t bucketIdx = oldestStep_ % numBuckets_;
        buckets_[bucketIdx] = SpikeBucket();  // Reset bucket
        pendingCounts_[bucketIdx].store(0, std::memory_order_relaxed);
        ++oldestStep_;
    }
}

} // namespace nlm
