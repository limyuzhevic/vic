#pragma once

/**
 * Enhanced EventQueue with lock-free implementations and advanced bucketing strategies
 * 
 * Provides high-performance event queue optimization for neural simulation:
 * - Lock-free ring buffers with multiple producer/consumer support
 * - Time-bucketed spike processing with adaptive bucket sizing
 * - NUMA-aware event queue placement
 * - Event compression and deduplication
 * - Priority-based event processing
 * - Memory-efficient event storage
 * 
 * Performance features:
 * - Reduced cache contention through partitioning
 * - SIMD-optimized event processing
 * - Minimized memory allocations
 * - Efficient spike delivery timing
 * - Advanced event routing
 */

#include <cstddef>
#include <cstdint>
#include <atomic>
#include <vector>
#include <array>
#include <optional>
#include <limits>
#include <algorithm>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <unordered_map>
#include <deque>
#include <cstring>

// SIMD support for event processing
#ifdef __AVX2__
    #include <immintrin.h>
#endif

namespace nlm {

// Constants for enhanced event queue sizing
constexpr size_t ENHANCED_SPIKE_QUEUE_CAPACITY = 65536;  // Must be power of 2
constexpr size_t ENHANCED_DELAYED_SPIKE_BUCKETS = 4096;  // More buckets for finer granularity
constexpr size_t MAX_DELAY_STEPS = 1000;                // Maximum synaptic delay in steps
constexpr size_t EVENT_QUEUE_PARTITIONS = 8;            // For multi-producer support
constexpr size_t PARTITION_CACHE_LINE_SIZE = 64;

/**
 * Enhanced spike event with compression and SIMD support
 */
struct alignas(32) EnhancedSpikeEvent {
    uint64_t sourceNeuron;      // Source neuron ID
    uint64_t destNeuron;        // Destination neuron ID
    uint64_t synapseId;         // Synapse ID for direct lookup
    float weight;              // Synaptic weight
    uint32_t delaySteps;       // Delay in simulation steps
    uint32_t eventTimestamp;   // Absolute timestamp for ordering
    uint32_t partitionId;      // For routing to appropriate consumer
    uint32_t flags;            // Event flags (e.g., compressed, deduplicated)
    uint32_t padding[2];       // Padding for alignment
    
    EnhancedSpikeEvent() : sourceNeuron(0), destNeuron(0), synapseId(0),
                          weight(0.0f), delaySteps(0), eventTimestamp(0),
                          partitionId(0), flags(0), padding{} {}
    
    EnhancedSpikeEvent(uint64_t src, uint64_t dst, uint64_t syn,
                      float w, uint32_t delay, uint32_t timestamp,
                      uint32_t partition)
        : sourceNeuron(src), destNeuron(dst), synapseId(syn), weight(w),
          delaySteps(delay), eventTimestamp(timestamp), partitionId(partition),
          flags(0), padding{} {}
} ALIGNAS(32);

/**
 * Enhanced SPSC Ring Buffer with partition support
 * 
 * Multiple independent ring buffers for each producer, reducing contention
 */
class EnhancedSpikeRingBuffer {
public:
    explicit EnhancedSpikeRingBuffer(size_t capacity = ENHANCED_SPIKE_QUEUE_CAPACITY,
                                    size_t partitionSize = 1024);
    ~EnhancedSpikeRingBuffer();
    
    // Disable copying
    EnhancedSpikeRingBuffer(const EnhancedSpikeRingBuffer&) = delete;
    EnhancedSpikeRingBuffer& operator=(const EnhancedSpikeRingBuffer&) = delete;
    
    /**
     * Push a spike event to the specific partition
     * @return true if event was enqueued, false if queue is full
     */
    bool push(const EnhancedSpikeEvent& event, uint32_t partitionId = 0);
    
    /**
     * Pop a spike event from the specific partition
     * @return event if available, empty optional if queue is empty
     */
    std::optional<EnhancedSpikeEvent> pop(uint32_t partitionId = 0);
    
    /**
     * Batch pop multiple events from a partition
     * @return vector of events (may be less than requested if queue empty)
     */
    std::vector<EnhancedSpikeEvent> popBatch(size_t count, uint32_t partitionId = 0);
    
    /**
     * Check if partition is empty
     */
    bool empty(uint32_t partitionId) const;
    
    /**
     * Check if partition is full
     */
    bool full(uint32_t partitionId) const;
    
    /**
     * Get number of events in a partition
     */
    size_t size(uint32_t partitionId) const;
    
    /**
     * Get partition capacity
     */
    size_t capacity() const { return capacity_; }
    
    /**
     * Clear a partition
     */
    void clear(uint32_t partitionId);
    
    /**
     * Get load statistics for monitoring
     */
    struct PartitionStats {
        uint64_t pushCount;
        uint64_t popCount;
        uint64_t overflowCount;
        uint64_t underflowCount;
        size_t currentSize;
        double utilizationRate;
        
        PartitionStats() : pushCount(0), popCount(0), overflowCount(0),
                          underflowCount(0), currentSize(0), utilizationRate(0.0) {}
    };
    
    PartitionStats getPartitionStats(uint32_t partitionId) const;
    
    /**
     * Get overall load statistics
     */
    std::vector<PartitionStats> getAllPartitionStats() const;
    
    /**
     * Adaptive bucket sizing for dynamic load balancing
     */
    void adaptiveBucketResizing();
    
    /**
     * Get optimal partition for current load
     */
    uint32_t getOptimalPartition() const;
    
private:
    size_t capacity_;
    size_t partitionSize_;
    
    // Per-partition storage
    struct Partition {
        EnhancedSpikeEvent* buffer;        // Allocated memory for events
        std::atomic<size_t> head;        // Consumer index
        std::atomic<size_t> tail;        // Producer index
        std::atomic<uint64_t> pushCount; // Statistics
        std::atomic<uint64_t> popCount;
        std::atomic<uint64_t> overflowCount;
        std::atomic<uint64_t> underflowCount;
        bool initialized;
        
        Partition() : buffer(nullptr), head(0), tail(0),
                     pushCount(0), popCount(0), overflowCount(0),
                     underflowCount(0), initialized(false) {}
        
        ~Partition() {
            if (buffer) {
                free(buffer);
            }
        }
        
        void initialize(size_t size, size_t alignment) {
            if (!initialized) {
                buffer = static_cast<EnhancedSpikeEvent*>(aligned_alloc(alignment, size * sizeof(EnhancedSpikeEvent)));
                head.store(0, std::memory_order_relaxed);
                tail.store(0, std::memory_order_relaxed);
                initialized = true;
            }
        }
        
        size_t mask() const { return capacity_ - 1; }  // capacity must be power of 2
    };
    
    // Array of partitions
    std::vector<Partition> partitions_;
    
    // Cache-efficient indexing
    struct alignas(PARTITION_CACHE_LINE_SIZE) CacheLine {
        std::atomic<uint64_t> lastAccess;
        uint64_t padding[PARTITION_CACHE_LINE_SIZE / 8 - 1];
        
        CacheLine() : lastAccess(0) {}
    };
    
    std::vector<CacheLine> accessStats_;
    
    // Partition assignment strategy
    static uint32_t hashNeuronId(uint64_t neuronId, uint32_t numPartitions) {
        // Simple hash function for neuron ID to partition mapping
        return static_cast<uint32_t>(neuronId * 0x9E3779B1) % numPartitions;
    }
    
    // SIMD-optimized batch operations
    #ifdef __AVX2__
    static void SIMDStoreEvents(EnhancedSpikeEvent* dest, const EnhancedSpikeEvent* src, size_t count);
    static void SIMDLoadEvents(EnhancedSpikeEvent* dest, const EnhancedSpikeEvent* src, size_t count);
    #endif
};

/**
 * Advanced time-bucketed delayed spike queue with optimization
 * 
 * Provides efficient delayed spike processing with:
 * - Fine-grained time bucketing (micro-second precision)
 * - NUMA-aware bucket placement
 * - Event compression for storage efficiency
 * - Priority-based processing
 * - Memory pool integration
 */
class EnhancedDelayedSpikeQueue {
public:
    EnhancedDelayedSpikeQueue(size_t numBuckets = ENHANCED_DELAYED_SPIKE_BUCKETS, 
                            size_t maxDelaySteps = MAX_DELAY_STEPS,
                            size_t numPartitions = EVENT_QUEUE_PARTITIONS);
    ~EnhancedDelayedSpikeQueue();
    
    // Disable copying
    EnhancedDelayedSpikeQueue(const EnhancedDelayedSpikeQueue&) = delete;
    EnhancedDelayedSpikeQueue& operator=(const EnhancedDelayedSpikeQueue&) = delete;
    
    /**
     * Schedule a spike for delivery at a future step with enhanced metadata
     */
    void schedule(uint64_t sourceNeuron, uint64_t destNeuron, uint64_t synapseId,
                  float weight, uint32_t delaySteps, uint64_t deliveryStep,
                  uint32_t partitionId = 0);
    
    /**
     * Schedule batch of spikes for better performance
     */
    void scheduleBatch(const std::vector<EnhancedSpikeEvent>& events);
    
    /**
     * Get all spikes scheduled for delivery at current step
     * @return vector of events (ownership transferred)
     */
    std::vector<EnhancedSpikeEvent> getDueSpikes(uint64_t currentStep);
    
    /**
     * Get due spikes with partition awareness for parallel processing
     */
    std::vector<std::pair<uint32_t, EnhancedSpikeEvent>> getDueSpikesWithPartition(uint64_t currentStep);
    
    /**
     * Get count of pending delayed spikes
     */
    size_t pendingCount() const;
    
    /**
     * Get count of spikes in a specific bucket
     */
    size_t bucketSize(uint64_t step, uint32_t partitionId = 0) const;
    
    /**
     * Clear all pending spikes
     */
    void clear();
    
    /**
     * Advance time bucket (call at end of simulation step)
     * Removes old buckets and rotates window
     */
    void advanceTime(uint64_t currentStep);
    
    /**
     * Get bucket statistics for monitoring
     */
    struct BucketStats {
        uint64_t step;
        size_t eventCount;
        size_t memoryUsage;
        double averageDelay;
        uint32_t partitionId;
        
        BucketStats() : step(0), eventCount(0), memoryUsage(0),
                       averageDelay(0.0), partitionId(0) {}
    };
    
    std::vector<BucketStats> getBucketStats(uint64_t startStep, uint64_t endStep) const;
    
    /**
     * Optimize bucket usage based on access patterns
     */
    void optimizeBucketUsage();
    
    /**
     * Compress old buckets to free memory
     */
    void compressOldBuckets(uint64_t cutoffStep);
    
    /**
     * Get memory statistics
     */
    struct MemoryStats {
        size_t totalMemoryUsage;
        size_t peakMemoryUsage;
        size_t currentBucketMemory;
        double compressionRatio;
        size_t activeBuckets;
        
        MemoryStats() : totalMemoryUsage(0), peakMemoryUsage(0),
                       currentBucketMemory(0), compressionRatio(1.0),
                       activeBuckets(0) {}
    };
    
    MemoryStats getMemoryStats() const;
    
private:
    size_t numBuckets_;
    size_t maxDelaySteps_;
    size_t numPartitions_;
    uint64_t currentStep_;
    uint64_t oldestStep_;
    
    // Per-partition, per-bucket storage
    struct alignas(PARTITION_CACHE_LINE_SIZE) SpikeBucket {
        // Enhanced bucket with compression and metadata
        static constexpr size_t BUCKET_CAPACITY = 512;  // Optimized for cache efficiency
        
        EnhancedSpikeEvent events[BUCKET_CAPACITY];
        std::atomic<size_t> count;
        std::atomic<size_t> readIndex;
        std::atomic<size_t> writeIndex;
        std::atomic<uint64_t> totalDeliveryTime;
        uint32_t partitionId;
        bool compressed;
        double compressionRatio;
        
        SpikeBucket() : count(0), readIndex(0), writeIndex(0),
                       totalDeliveryTime(0), partitionId(0),
                       compressed(false), compressionRatio(1.0) {
            // Initialize events array
            for (auto& e : events) {
                new (&e) EnhancedSpikeEvent();
            }
        }
        
        bool push(const EnhancedSpikeEvent& event) {
            if (count.load(std::memory_order_relaxed) >= BUCKET_CAPACITY) {
                return false;  // Bucket full
            }
            
            size_t idx = writeIndex.fetch_add(1, std::memory_order_relaxed) % BUCKET_CAPACITY;
            events[idx] = event;
            count.fetch_add(1, std::memory_order_release);
            return true;
        }
        
        std::vector<EnhancedSpikeEvent> drain() {
            std::vector<EnhancedSpikeEvent> result;
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
    
    // Circular buffer of buckets for each partition
    std::vector<std::vector<SpikeBucket>> buckets_;
    std::vector<std::vector<size_t>> pendingCounts_;
    
    // Memory pools for bucket efficiency
    struct BucketMemoryPool {
        std::vector<SpikeBucket> buckets;
        size_t capacity;
        size_t used;
        
        BucketMemoryPool(size_t numBuckets, size_t partitions) {
            capacity = numBuckets * partitions;
            buckets.reserve(capacity);
            used = 0;
        }
        
        SpikeBucket& getBucket(uint64_t step, uint32_t partitionId) {
            size_t index = (step % capacity) * numPartitions_ + partitionId;
            if (used >= buckets.size()) {
                buckets.emplace_back();
            }
            return buckets[index];
        }
        
        void returnBucket(SpikeBucket& bucket) {
            // Return bucket to pool (simplified - could implement free list)
        }
    };
    
    std::vector<BucketMemoryPool> partitionPools_;
    
    // Statistics and monitoring
    struct QueueStats {
        uint64_t totalScheduled;
        uint64_t totalDelivered;
        uint64_t totalOverflow;
        double efficiencyRatio;
        
        QueueStats() : totalScheduled(0), totalDelivered(0),
                      totalOverflow(0), efficiencyRatio(0.0) {}
    };
    
    mutable std::vector<QueueStats> partitionStats_;
    
    // Event compression utilities
    static float compressEvent(const EnhancedSpikeEvent& event);
    static void decompressEvent(EnhancedSpikeEvent& event, float compressedValue);
    
    // SIMD-optimized batch operations
    #ifdef __AVX2__
    static void SIMDStoreEvents(EnhancedSpikeEvent* dest, const EnhancedSpikeEvent* src, size_t count);
    static void SIMDLoadEvents(EnhancedSpikeEvent* dest, const EnhancedSpikeEvent* src, size_t count);
    static void SIMDCompressEvents(float* dest, const EnhancedSpikeEvent* src, size_t count);
    static void SIMDDecompressEvents(EnhancedSpikeEvent* dest, const float* src, size_t count);
    #endif
};

/**
 * Priority Event Queue for critical events
 * 
 * Provides highest-priority processing for time-sensitive events like
 * neuromodulatory signals, plasticity updates, and system events.
 */
class PriorityEventQueue {
public:
    static constexpr size_t MAX_PRIORITY_EVENTS = 8192;
    static constexpr size_t MAX_PRIORITY_LEVELS = 16;
    
    struct alignas(32) PriorityEvent {
        uint64_t timestamp;
        uint8_t priority;      // 0 = highest priority
        uint16_t eventType;    // Event type ID
        uint64_t data[4];      // Event-specific data
        uint32_t partitionId;
        
        bool operator<(const PriorityEvent& other) const {
            // Lower priority value = higher priority in min-heap
            if (priority != other.priority) return priority > other.priority;
            return timestamp > other.timestamp;
        }
    };
    
    PriorityEventQueue(size_t numPartitions = EVENT_QUEUE_PARTITIONS);
    ~PriorityEventQueue();
    
    // Disable copying
    PriorityEventQueue(const PriorityEventQueue&) = delete;
    PriorityEventQueue& operator=(const PriorityEventQueue&) = delete;
    
    /**
     * Push a priority event
     */
    void push(uint64_t timestamp, uint8_t priority, uint16_t type, 
              const uint64_t* data = nullptr, uint32_t partitionId = 0);
    
    /**
     * Push batch of events for efficiency
     */
    void pushBatch(const std::vector<PriorityEvent>& events);
    
    /**
     * Pop the highest priority event
     */
    std::optional<PriorityEvent> pop();
    
    /**
     * Get all events due before a timestamp
     */
    std::vector<PriorityEvent> getDueEvents(uint64_t currentTime);
    
    /**
     * Check if queue is empty
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
    
    /**
     * Get partition statistics
     */
    struct PartitionStats {
        uint64_t pushCount;
        uint64_t popCount;
        uint64_t overflowCount;
        size_t currentSize;
        
        PartitionStats() : pushCount(0), popCount(0), overflowCount(0),
                          currentSize(0) {}
    };
    
    std::vector<PartitionStats> getPartitionStats() const;
    
private:
    std::vector<std::priority_queue<PriorityEvent>> partitionQueues_;
    std::vector<PartitionStats> partitionStats_;
    size_t numPartitions_;
};

/**
 * Enhanced Event Router for intelligent event distribution
 * 
 * Provides intelligent routing of events to appropriate consumers
 * based on event type, priority, and destination neuron.
 */
class EventRouter {
public:
    EventRouter(size_t numPartitions = EVENT_QUEUE_PARTITIONS);
    
    /**
     * Route an event to the appropriate partition
     */
    uint32_t routeEvent(const EnhancedSpikeEvent& event) const;
    
    /**
     * Route an event to the appropriate priority queue
     */
    uint32_t routePriorityEvent(const PriorityEventQueue::PriorityEvent& event) const;
    
    /**
     * Update routing based on access patterns
     */
    void learnFromAccess(const EnhancedSpikeEvent& event);
    
    /**
     * Get partition load statistics
     */
    std::vector<double> getPartitionLoads() const;
    
    /**
     * Rebalance partitions based on current load
     */
    void rebalancePartitions();
    
private:
    size_t numPartitions_;
    std::vector<double> partitionLoads_;
    mutable std::mutex loadMutex_;
    
    // Routing tables
    std::vector<std::vector<uint32_t>> sourceToPartitionMap_;  // Neuron ID -> Partition
    std::vector<std::vector<uint32_t>> eventTypeToPartitionMap_;
    
    // Load learning
    struct AccessPattern {
        uint64_t neuronId;
        uint32_t eventType;
        uint64_t accessCount;
        uint64_t lastAccess;
        
        AccessPattern() : neuronId(0), eventType(0), accessCount(0),
                         lastAccess(0) {}
    };
    
    std::vector<std::vector<AccessPattern>> accessPatterns_;
};

} // namespace nlm
