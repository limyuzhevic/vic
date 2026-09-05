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

// Constants for event queue sizing
constexpr size_t SPIKE_QUEUE_CAPACITY = 16384;  // Must be power of 2
constexpr size_t DELAYED_SPIKE_BUCKETS = 1024;  // Number of time buckets
constexpr size_t MAX_DELAY_STEPS = 100;  // Maximum synaptic delay in steps

/**
 * SpikeEvent with minimal data for queue processing
 */
struct alignas(16) QueuedSpikeEvent {
    uint64_t sourceNeuron;
    uint64_t destNeuron;
    uint64_t synapseId;
    float weight;
    uint8_t isExcitatory;
    uint8_t padding[7];  // Pad to 16 bytes for cache alignment
    
    QueuedSpikeEvent() : sourceNeuron(0), destNeuron(0), synapseId(0), 
                         weight(0.0f), isExcitatory(1), padding{} {}
    
    QueuedSpikeEvent(uint64_t src, uint64_t dst, uint64_t syn, float w, bool exc)
        : sourceNeuron(src), destNeuron(dst), synapseId(syn), weight(w), 
          isExcitatory(exc ? 1 : 0), padding{} {}
};

/**
 * SPSC Ring Buffer for immediate spike events
 * 
 * Lock-free single-producer single-consumer queue.
 * Uses atomic head/tail indices with modulo arithmetic.
 */
class SpikeRingBuffer {
public:
    explicit SpikeRingBuffer(size_t capacity = SPIKE_QUEUE_CAPACITY);
    ~SpikeRingBuffer();
    
    // Disable copying
    SpikeRingBuffer(const SpikeRingBuffer&) = delete;
    SpikeRingBuffer& operator=(const SpikeRingBuffer&) = delete;
    
    /**
     * Push a spike event to the queue
     * @return true if event was enqueued, false if queue is full
     */
    bool push(const QueuedSpikeEvent& event);
    
    /**
     * Pop a spike event from the queue
     * @return event if available, empty optional if queue is empty
     */
    std::optional<QueuedSpikeEvent> pop();
    
    /**
     * Check if queue is empty
     */
    bool empty() const;
    
    /**
     * Check if queue is full
     */
    bool full() const;
    
    /**
     * Get number of events in queue
     */
    size_t size() const;
    
    /**
     * Get queue capacity
     */
    size_t capacity() const { return capacity_; }
    
    /**
     * Clear all events
     */
    void clear();
    
private:
    const size_t capacity_;
    const size_t mask_;  // Bit mask for fast modulo (capacity must be power of 2)
    
    // Storage
    std::vector<QueuedSpikeEvent> buffer_;
    
    // Atomic indices
    std::atomic<size_t> head_;   // Written by consumer
    std::atomic<size_t> tail_;   // Written by producer
};

/**
 * Time-bucketed delayed spike queue
 * 
 * Organizes delayed spikes into time buckets for efficient processing.
 * Each bucket contains spikes scheduled for delivery at that simulation step.
 * 
 * Biological motivation:
 * - Synaptic delays vary (1-20ms typically)
 * - Spikes are delivered after their delay period
 * - Grouping by delivery time enables efficient batch processing
 */
class DelayedSpikeQueue {
public:
    DelayedSpikeQueue(size_t numBuckets = DELAYED_SPIKE_BUCKETS, 
                      size_t maxDelaySteps = MAX_DELAY_STEPS);
    ~DelayedSpikeQueue();
    
    // Disable copying
    DelayedSpikeQueue(const DelayedSpikeQueue&) = delete;
    DelayedSpikeQueue& operator=(const DelayedSpikeQueue&) = delete;
    
    /**
     * Schedule a spike for delivery at a future step
     */
    void schedule(uint64_t sourceNeuron, uint64_t destNeuron, uint64_t synapseId,
                  float weight, bool isExcitatory, uint64_t deliveryStep);
    
    /**
     * Get all spikes scheduled for delivery at current step
     * @return Vector of spikes (ownership transferred)
     */
    std::vector<QueuedSpikeEvent> getDueSpikes(uint64_t currentStep);
    
    /**
     * Get count of pending delayed spikes
     */
    size_t pendingCount() const;
    
    /**
     * Get count of spikes in a specific bucket
     */
    size_t bucketSize(uint64_t step) const;
    
    /**
     * Clear all pending spikes
     */
    void clear();
    
    /**
     * Advance time bucket (call at end of simulation step)
     * Removes old buckets and rotates window
     */
    void advanceTime(uint64_t currentStep);
    
private:
    struct alignas(64) SpikeBucket {
        // Each bucket is a small ring buffer for spikes due at that step
        static constexpr size_t BUCKET_CAPACITY = 256;
        
        QueuedSpikeEvent events[BUCKET_CAPACITY];
        std::atomic<size_t> count;
        std::atomic<size_t> readIndex;
        std::atomic<size_t> writeIndex;
        
        SpikeBucket() : count(0), readIndex(0), writeIndex(0) {
            // Zero the events array
            for (auto& e : events) {
                new (&e) QueuedSpikeEvent();
            }
        }
        
        bool push(const QueuedSpikeEvent& event) {
            if (count.load(std::memory_order_relaxed) >= BUCKET_CAPACITY) {
                return false;
            }
            size_t idx = writeIndex.fetch_add(1, std::memory_order_relaxed) % BUCKET_CAPACITY;
            events[idx] = event;
            count.fetch_add(1, std::memory_order_release);
            return true;
        }
        
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
        
        size_t size() const { return count.load(std::memory_order_acquire); }
    };
    
    size_t numBuckets_;
    size_t maxDelaySteps_;
    uint64_t currentStep_;
    uint64_t oldestStep_;
    
    // Circular buffer of buckets
    std::vector<SpikeBucket> buckets_;
    std::vector<size_t> pendingCounts_;
};

/**
 * Priority Event Queue for time-sensitive events
 * 
 * Uses a small priority queue for events that need strict ordering.
 * Used for neuromodulatory events, plasticity updates, etc.
 */
class PriorityEventQueue {
public:
    static constexpr size_t MAX_PRIORITY_EVENTS = 4096;
    
    struct PriorityEvent {
        uint64_t timestamp;
        uint8_t priority;  // 0 = highest priority
        uint16_t eventType;  // Event type ID
        uint64_t data[4];  // Event-specific data
        
        bool operator<(const PriorityEvent& other) const {
            // Lower priority value = higher priority in min-heap
            if (priority != other.priority) return priority > other.priority;
            return timestamp > other.timestamp;
        }
    };
    
    PriorityEventQueue();
    
    /**
     * Push a priority event
     */
    void push(uint64_t timestamp, uint8_t priority, uint16_t type, const uint64_t* data = nullptr);
    
    /**
     * Pop the highest priority event
     */
    std::optional<PriorityEvent> pop();
    
    /**
     * Get all events due before a timestamp
     */
    std::vector<PriorityEvent> getDueEvents(uint64_t currentTime);
    
    /**
     * Check if empty
     */
    bool empty() const { return events_.empty(); }
    
    /**
     * Clear all events
     */
    void clear() { events_.clear(); }
    
    /**
     * Get count
     */
    size_t size() const { return events_.size(); }

private:
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