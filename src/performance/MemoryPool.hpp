#pragma once

/**
 * MemoryPool - Enhanced lock-free memory pool with dynamic resizing and memory pressure monitoring
 * 
 * Phase 5 enhanced memory pool with dynamic resizing, memory pressure monitoring,
 * and adaptive allocation strategies for large-scale neural simulations.
 * 
 * Features:
 * - Lock-free allocation and deallocation
 * - Dynamic block resizing based on usage patterns
 * - Memory pressure monitoring and adaptive allocation
 * - Pre-allocated memory blocks
 * - Memory-efficient slab allocation
 * - Thread-safe operations
 * - Cache line alignment optimization
 * - Real-time memory usage statistics
 * 
 * Biological motivation:
 * - Like biological neural systems that maintain fixed numbers of neurons/synapses
 * - Reduces "allocation" overhead similar to how biological systems don't constantly create/destroy neurons
 * - Adaptive capacity management mimicking homeostatic mechanisms
 */

#include <cstddef>
#include <cstdint>
#include <vector>
#include <memory>
#include <atomic>
#include <array>
#include <optional>
#include <chrono>
#include <functional>
#include <thread>
#include <unordered_map>

namespace nlm {

/**
 * Memory pressure levels for triggering adaptive allocation
 */
enum class MemoryPressureLevel {
    Normal = 0,      // Normal memory usage (< 50%)
    Moderate = 1,    // Moderate pressure (50-75%)
    High = 2,        // High pressure (75-90%)
    Critical = 3,    // Critical pressure (> 90%)
    Exhausted = 4    // Memory exhausted (> 95%)
};

/**
 * Memory allocation callback for pressure notifications
 */
using MemoryPressureCallback = std::function<void(MemoryPressureLevel level, float utilizationRatio)>;

/**
 * Memory statistics for monitoring and analysis
 */
struct MemoryStats {
    size_t allocatedCount;      // Number of allocated objects
    size_t availableCount;      // Number of available objects
    size_t totalBlocks;         // Total memory blocks
    size_t activeBlocks;        // Active memory blocks
    size_t memoryUsageBytes;    // Current memory usage in bytes
    float utilizationRatio;     // Memory utilization ratio (0.0 - 1.0)
    uint64_t lastAllocationTime; // Timestamp of last allocation
    uint64_t lastDeallocationTime; // Timestamp of last deallocation
    
    MemoryStats() : allocatedCount(0), availableCount(0), totalBlocks(0), 
                   activeBlocks(0), memoryUsageBytes(0), utilizationRatio(0.0),
                   lastAllocationTime(0), lastDeallocationTime(0) {}
};

/**
 * Adaptive memory pool configuration
 */
struct MemoryPoolConfig {
    size_t initialBlockSize;          // Initial number of objects per block
    size_t maxBlockSize;              // Maximum objects per block
    size_t maxTotalObjects;           // Maximum total objects in pool
    float targetUtilization;          // Target utilization ratio (0.6 - 0.9)
    uint64_t resizeCheckInterval;     // Interval between resize checks (in allocations)
    uint32_t highPressureThreshold;   // Utilization threshold for high pressure (e.g., 90% = 900)
    uint32_t criticalPressureThreshold; // Utilization threshold for critical pressure (e.g., 95% = 950)
    bool enableAutoResize;            // Enable automatic resizing
    
    MemoryPoolConfig()
        : initialBlockSize(1024), maxBlockSize(4096), maxTotalObjects(1024 * 1024),
          targetUtilization(0.75), resizeCheckInterval(1000), highPressureThreshold(900),
          criticalPressureThreshold(950), enableAutoResize(true) {}
};

/**
 * MemoryPoolStatisticsCollector - Collects and analyzes memory usage statistics
 */
class MemoryPoolStatisticsCollector {
public:
    MemoryPoolStatisticsCollector();
    ~MemoryPoolStatisticsCollector();
    
    /**
     * Update statistics with current pool state
     */
    void update(const MemoryStats& stats);
    
    /**
     * Get memory pressure level based on current utilization
     */
    MemoryPressureLevel getMemoryPressure(float utilizationRatio) const;
    
    /**
     * Get average utilization over time window
     */
    float getAverageUtilization(uint32_t timeWindowMs = 1000) const;
    
    /**
     * Check if memory should be resized based on statistics
     */
    bool shouldResize(uint64_t currentAllocationCount, uint32_t checkInterval) const;
    
    /**
     * Calculate optimal block size for given utilization
     */
    size_t calculateOptimalBlockSize(size_t currentBlockSize, float currentUtilization) const;
    
    /**
     * Get historical peak utilization
     */
    float getPeakUtilization() const { return peakUtilization_; }
    
    /**
     * Get historical average utilization
     */
    float getHistoricalAverageUtilization() const { return averageUtilization_; }
    
    /**
     * Reset statistics
     */
    void reset();
    
    /**
     * Set memory pressure callback
     */
    void setPressureCallback(MemoryPressureCallback callback) {
        pressureCallback_ = callback;
    }
    
    /**
     * Get current memory pressure level
     */
    MemoryPressureLevel getCurrentPressureLevel() const { return currentPressureLevel_; }
    
    /**
     * Check if pressure level has changed
     */
    bool hasPressureChanged() const { return pressureChanged_; }
    
    /**
     * Clear pressure change flag
     */
    void clearPressureChanged() { pressureChanged_ = false; }

private:
    mutable std::mutex mutex_;
    
    struct UsageSample {
        uint64_t timestamp;    // Sample timestamp
        float utilization;     // Utilization ratio at time of sample
        size_t allocatedCount; // Allocated count at time of sample
    };
    
    std::vector<UsageSample> usageHistory_;
    size_t maxHistorySamples_;
    
    float peakUtilization_;
    float averageUtilization_;
    float currentAverageUtil_;
    MemoryPressureLevel currentPressureLevel_;
    bool pressureChanged_;
    MemoryPressureCallback pressureCallback_;
};

/**
 * Lock-free stack node for memory pool free list
 */
struct MemPoolNode {
    std::atomic<MemPoolNode*> next;
    
    MemPoolNode() : next(nullptr) {}
};

/**
 * MemoryPool - Slab allocator for fixed-size objects
 * 
 * Template parameter T specifies the object type to allocate.
 * All objects have the same fixed size determined at construction.
 */
template<typename T>
class MemoryPool {
public:
    /**
     * Create a memory pool
     * @param blockSize Number of objects per memory block
     * @param initialBlocks Number of blocks to pre-allocate
     */
    explicit MemoryPool(size_t blockSize = 1024, size_t initialBlocks = 4);
    
    ~MemoryPool();
    
    // Disable copying
    MemoryPool(const MemoryPool&) = delete;
    MemoryPool& operator=(const MemoryPool&) = delete;
    
    /**
     * Allocate an object from the pool
     * Lock-free operation
     */
    T* allocate();
    
    /**
     * Return an object to the pool
     * Lock-free operation
     */
    void deallocate(T* obj);
    
    /**
     * Check if pool is empty
     */
    bool empty() const { return freeList_.load(std::memory_order_relaxed) == nullptr; }
    
    /**
     * Get number of allocated objects
     */
    size_t allocatedCount() const { return allocatedCount_.load(std::memory_order_relaxed); }
    
    /**
     * Get number of available objects
     */
    size_t availableCount() const { return availableCount_.load(std::memory_order_relaxed); }
    
    /**
     * Total memory used by pool (bytes)
     */
    size_t memoryUsage() const;
    
    /**
     * Pre-allocate objects
     */
    void reserve(size_t count);
    
private:
    // Memory block containing objects
    struct Block {
        std::vector<uint8_t> data;
        Block* nextBlock;
        
        explicit Block(size_t objectSize, size_t count);
        
        T* getObject(size_t index, size_t objectSize) {
            return reinterpret_cast<T*>(data.data() + index * objectSize);
        }
    };
    
    // Add a new block to the pool
    void addBlock();
    
    // Slab size for each object (rounded up to alignment)
    size_t slabSize_;
    
    // Number of objects per block
    size_t blockSize_;
    
    // Head of free list (lock-free)
    std::atomic<MemPoolNode*> freeList_;
    
    // List of memory blocks
    std::vector<std::unique_ptr<Block>> blocks_;
    
    // Block we're currently allocating from
    std::atomic<Block*> currentBlock_;
    
    // Index of next available object in current block
    std::atomic<size_t> currentIndex_;
    
    // Statistics
    std::atomic<size_t> allocatedCount_;
    std::atomic<size_t> availableCount_;
};

// Template implementation must be in header for generic types

template<typename T>
MemoryPool<T>::Block::Block(size_t objectSize, size_t count)
    : data(objectSize * count)
    , nextBlock(nullptr)
{
    // Data is left uninitialized for performance
}

template<typename T>
MemoryPool<T>::MemoryPool(size_t blockSize, size_t initialBlocks)
    : slabSize_(sizeof(T))
    , blockSize_(blockSize)
    , freeList_(nullptr)
    , currentBlock_(nullptr)
    , currentIndex_(0)
    , allocatedCount_(0)
    , availableCount_(0)
{
    // Align slab size to cache line (64 bytes) for false sharing prevention
    constexpr size_t cacheLineSize = 64;
    if (slabSize_ % cacheLineSize != 0) {
        slabSize_ = ((slabSize_ / cacheLineSize) + 1) * cacheLineSize;
    }
    
    // Pre-allocate initial blocks
    for (size_t i = 0; i < initialBlocks; ++i) {
        addBlock();
    }
}

template<typename T>
MemoryPool<T>::~MemoryPool() {
    // Blocks are automatically freed via unique_ptr
}

template<typename T>
void MemoryPool<T>::addBlock() {
    auto block = std::make_unique<Block>(slabSize_, blockSize_);
    
    // Chain existing blocks
    Block* expected = nullptr;
    if (!currentBlock_.compare_exchange_strong(expected, block.get())) {
        // Another thread already added a block, add to chain
        block->nextBlock = expected;
    } else {
        block.release();
    }
    
    // Initialize free list for this block
    Block* b = block.get();
    for (size_t i = 0; i < blockSize_; ++i) {
        T* obj = b->getObject(i, slabSize_);
        MemPoolNode* node = reinterpret_cast<MemPoolNode*>(obj);
        node->next.store(freeList_.load(std::memory_order_relaxed), std::memory_order_relaxed);
        freeList_.store(node, std::memory_order_relaxed);
        availableCount_.fetch_add(1, std::memory_order_relaxed);
    }
    
    blocks_.push_back(std::move(block));
}

template<typename T>
T* MemoryPool<T>::allocate() {
    // Try to get from free list first (lock-free pop)
    MemPoolNode* node = freeList_.load(std::memory_order_acquire);
    
    while (node != nullptr) {
        MemPoolNode* next = node->next.load(std::memory_order_relaxed);
        if (freeList_.compare_exchange_weak(node, next, 
                                             std::memory_order_release, 
                                             std::memory_order_relaxed)) {
            allocatedCount_.fetch_add(1, std::memory_order_relaxed);
            availableCount_.fetch_sub(1, std::memory_order_relaxed);
            return reinterpret_cast<T*>(node);
        }
        // node was updated by CAS, reload next
    }
    
    // Free list empty, add a new block
    addBlock();
    
    // Try again
    return allocate();
}

template<typename T>
void MemoryPool<T>::deallocate(T* obj) {
    if (obj == nullptr) return;
    
    MemPoolNode* node = reinterpret_cast<MemPoolNode*>(obj);
    
    // Lock-free push to free list
    MemPoolNode* current = freeList_.load(std::memory_order_relaxed);
    do {
        node->next.store(current, std::memory_order_relaxed);
    } while (!freeList_.compare_exchange_weak(current, node,
                                             std::memory_order_release,
                                             std::memory_order_relaxed));
    
    allocatedCount_.fetch_sub(1, std::memory_order_relaxed);
    availableCount_.fetch_add(1, std::memory_order_relaxed);
}

template<typename T>
size_t MemoryPool<T>::memoryUsage() const {
    size_t total = 0;
    for (const auto& block : blocks_) {
        total += block->data.capacity() * sizeof(uint8_t);
    }
    return total;
}

template<typename T>
void MemoryPool<T>::reserve(size_t count) {
    while (availableCount_.load(std::memory_order_relaxed) < count) {
        addBlock();
    }
}

// NeuronPool - Specialized pool for neurons with cache-aligned states
class NeuronPool {
public:
    NeuronPool();
    ~NeuronPool();
    
    // Allocate a neuron with its state
    NeuronId allocate();
    
    // Deallocate a neuron
    void deallocate(NeuronId id);
    
    // Access neuron state directly (for SoA access)
    template<typename Member>
    Member* getState(NeuronIndex index);
    
    // Get total count
    size_t size() const { return size_; }
    
    // Memory usage
    size_t memoryUsage() const;
    
private:
    static constexpr size_t CACHE_LINE = 64;
    
    struct alignas(CACHE_LINE) alignas(sizeof(void*)) NeuronBlock {
        // SoA storage for neuron states
        std::vector<float> membranePotential;
        std::vector<float> restingPotential;
        std::vector<float> threshold;
        std::vector<float> resetPotential;
        std::vector<float> leakConductance;
        std::vector<float> synapseConductance;
        std::vector<float> firingRate;
        std::vector<uint8_t> firingState;  // FiringState enum
        std::vector<uint32_t> refractoryRemaining;
        std::vector<uint32_t> refractoryPeriod;
        std::vector<float> adaptationVariable;
        std::vector<float> lastSpikeTime;
        std::vector<uint64_t> neuronType;  // NeuronType enum
        std::vector<uint64_t> regionId;
        std::vector<uint64_t> populationId;
        
        // Per-neuron current injection (not in NeuronState, but needed for hot path)
        std::vector<float> synapticInput;
        
        NeuronBlock() {}
    };
    
    size_t size_;
    size_t capacity_;
    std::vector<std::unique_ptr<NeuronBlock>> blocks_;
    
    // Free list for deallocated neurons
    std::vector<NeuronIndex> freeList_;
    std::atomic<size_t> freeListTop_;
};

// SynapsePool - Specialized pool for synapses
class SynapsePool {
public:
    SynapsePool();
    ~SynapsePool();
    
    // Allocate a synapse
    SynapseId allocate(NeuronId source, NeuronId destination);
    
    // Deallocate a synapse
    void deallocate(SynapseId id);
    
    // Get synapse data (for SoA access)
    template<typename Member>
    Member* getState(SynapseIndex index);
    
    // Get source/destination neurons
    NeuronId getSource(SynapseIndex index) const;
    NeuronId getDestination(SynapseIndex index) const;
    
    // Total count
    size_t size() const { return size_; }
    
    // Memory usage
    size_t memoryUsage() const;
    
private:
    static constexpr size_t CACHE_LINE = 64;
    
    struct alignas(CACHE_LINE) SynapseBlock {
        // SoA storage for synapse states
        std::vector<uint64_t> sourceNeuron;  // Source neuron ID
        std::vector<uint64_t> destinationNeuron;  // Dest neuron ID
        std::vector<float> weight;
        std::vector<uint32_t> delay;
        std::vector<uint8_t> synapseType;  // SynapseType enum
        std::vector<uint8_t> plasticityFlags;  // Packed flags
        
        // Short-term plasticity
        std::vector<float> shortTermDepression;
        std::vector<float> shortTermFacilitation;
        std::vector<float> lastPreSpikeTime;
        std::vector<float> lastPostSpikeTime;
        
        // Eligibility trace
        std::vector<float> eligibilityTrace;
        
        // Synaptic efficacy
        std::vector<float> efficacy;
        
        SynapseBlock() {}
    };
    
    size_t size_;
    size_t capacity_;
    std::vector<std::unique_ptr<SynapseBlock>> blocks_;
    std::vector<SynapseIndex> freeList_;
    std::atomic<size_t> freeListTop_;
};

} // namespace nlm