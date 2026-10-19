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

class VectorizedPriorityEventQueue {
public:
    static constexpr size_t MAX_PRIORITY_EVENTS = 4096;
    
    struct alignas(32) PriorityEvent {
        uint64_t timestamp;      // Delivery time
        uint8_t priority;        // 0 = highest priority, higher = lower priority
        uint16_t eventType;       // Event type ID for dispatching
        uint64_t data[4];         // Event-specific data (max 4 × 64-bit values)
        uint32_t reserved;        // Padding for cache alignment
        
        // Vectorized comparison operations for SIMD optimization
        bool operator<(const PriorityEvent& other) const {
            // Compare priority first (lower value = higher priority)
            if (priority != other.priority) {
                return priority > other.priority;  // Min-heap: smaller priority = higher priority
            }
            // Compare timestamp (earlier = higher priority)
            return timestamp > other.timestamp;
        }
        
        // Fast comparison for sorting operations
        bool operator<=(const PriorityEvent& other) const {
            return (priority < other.priority) || 
                   (priority == other.priority && timestamp <= other.timestamp);
        }
        
        bool operator>(const PriorityEvent& other) const {
            return (priority > other.priority) ||
                   (priority == other.priority && timestamp > other.timestamp);
        }
        
        bool operator>=(const PriorityEvent& other) const {
            return (priority > other.priority) ||
                   (priority == other.priority && timestamp >= other.timestamp);
        }
    };
    
    VectorizedPriorityEventQueue();
    ~VectorizedPriorityEventQueue();
    
    // Disable copying and assignment
    VectorizedPriorityEventQueue(const VectorizedPriorityEventQueue&) = delete;
    VectorizedPriorityEventQueue& operator=(const VectorizedPriorityEventQueue&) = delete;
    
    /**
     * Push a priority event into the heap
     * @param timestamp Absolute timestamp for delivery
     * @param priority Priority level (0 = highest, higher = lower)
     * @param type Event type identifier
     * @param data Event data (optional, up to 4 uint64_t values)
     * @return true if event was enqueued, false if queue is full
     */
    bool push(uint64_t timestamp, uint8_t priority, uint16_t type, 
              const uint64_t* data = nullptr);
    
    /**
     * Push an event with minimal overhead (optimized path)
     * @param event Event to push (will be copied into heap)
     * @return true if event was enqueued, false if queue is full
     */
    bool push(const PriorityEvent& event);
    
    /**
     * Pop the highest priority event (lowest priority value, earliest timestamp)
     * @return The highest priority event, or empty optional if queue is empty
     */
    std::optional<PriorityEvent> pop();
    
    /**
     * Peek at the highest priority event without removing it
     * @return The highest priority event, or empty optional if queue is empty
     */
    std::optional<PriorityEvent> peek() const;
    
    /**
     * Get all events due before a given timestamp
     * @param currentTime Current simulation time
     * @return Vector of due events (ownership transferred)
     */
    std::vector<PriorityEvent> getDueEvents(uint64_t currentTime);
    
    /**
     * Get count of events with timestamp <= current time
     */
    size_t getDueCount(uint64_t currentTime) const;
    
    /**
     * Check if queue is empty
     */
    bool empty() const { return size_ == 0; }
    
    /**
     * Check if queue is full
     */
    bool full() const { return size_ >= MAX_PRIORITY_EVENTS; }
    
    /**
     * Get current queue size
     */
    size_t size() const { return size_; }
    
    /**
     * Clear all events (fast reset)
     */
    void clear();
    
    /**
     * Get heap statistics
     */
    struct HeapStats {
        size_t size;           // Current number of events
        size_t capacity;       // Maximum capacity
        size_t comparisons;    // Number of comparisons performed
        size_t bubbleUps;      // Number of bubble-up operations
        size_t bubbleDowns;    // Number of bubble-down operations
        float averageDepth;    // Average heap depth
    };
    
    HeapStats getStats() const;
    
    /**
     * Enable/disable performance monitoring
     */
    void setMonitoringEnabled(bool enabled) { monitoringEnabled_ = enabled; }
    bool isMonitoringEnabled() const { return monitoringEnabled_; }
    
    /**
     * Pre-allocate heap space for better performance
     */
    void reserve(size_t additionalEvents);
    
private:
    // Heap implementation using vector with efficient memory layout
    std::vector<PriorityEvent> heap_;
    size_t size_;
    
    // Performance monitoring
    bool monitoringEnabled_;
    mutable size_t comparisons_;
    mutable size_t bubbleUps_;
    mutable size_t bubbleDowns_;
    
    // Vectorized helper functions
    static inline bool compareEvents(const PriorityEvent& a, const PriorityEvent& b) {
        if (a.priority != b.priority) {
            return a.priority > b.priority;  // Min-heap comparison
        }
        return a.timestamp > b.timestamp;  // Timestamp comparison
    }
    
    // Heap operations
    void bubbleUp(size_t index);
    void bubbleDown(size_t index);
    size_t parent(size_t index) const { return (index - 1) / 2; }
    size_t leftChild(size_t index) const { return 2 * index + 1; }
    size_t rightChild(size_t index) const { return 2 * index + 2; }
    
    // Monitoring helpers
    void incrementComparisons() const { if (monitoringEnabled_) comparisons_++; }
    void incrementBubbleUps() const { if (monitoringEnabled_) bubbleUps_++; }
    void incrementBubbleDowns() const { if (monitoringEnabled_) bubbleDowns_++; }
    
    // Heap property validation (for debugging)
    bool validateHeapProperty() const;
    
    // Calculate average depth (for statistics)
    float calculateAverageDepth() const;
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