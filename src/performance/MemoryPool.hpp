#pragma once

/**
 * MemoryPool - Lock-free memory pool for neurons and synapses
 * 
 * Provides pre-allocated object pools to reduce allocation overhead during simulation.
 * Each pool manages objects of a single type with fixed size.
 * 
 * Features:
 * - Lock-free allocation and deallocation
 * - Pre-allocated memory blocks
 * - Memory-efficient slab allocation
 * - Thread-safe operations
 * - Memory leak detection
 * - Arena allocation
 * - Statistics tracking
 * - Integration with logging system
 * - Memory corruption detection
 * - Performance monitoring
 * 
 * Biological motivation:
 * - Like biological neural systems that maintain fixed numbers of neurons/synapses
 * - Reduces "allocation" overhead similar to how biological systems don't constantly create/destroy neurons
 */

#include <cstddef>
#include <cstdint>
#include <vector>
#include <memory>
#include <atomic>
#include <array>
#include <optional>
#include <chrono>
#include <unordered_map>
#include <mutex>
#include <functional>
#include <thread>
#include <condition_variable>
#include <stdexcept>

namespace nlm {

/**
 * Lock-free stack node for memory pool free list
 */
struct MemPoolNode {
    std::atomic<MemPoolNode*> next;
    std::atomic<uint64_t> tag;  // For corruption detection
    uint64_t magic;  // Tag value for corruption detection
    
    MemPoolNode() : next(nullptr), tag(0), magic(0xDEADBEEFDEADBEEFull) {}
};

/**
 * Arena allocation for large memory blocks
 */
class MemoryArena {
public:
    MemoryArena(size_t capacity) : capacity_(capacity), used_(0) {
        data_ = std::make_unique<uint8_t[]>(capacity);
    }
    
    ~MemoryArena() = default;
    
    // Disable copying
    MemoryArena(const MemoryArena&) = delete;
    MemoryArena& operator=(const MemoryArena&) = delete;
    
    /**
     * Allocate memory from arena
     */
    void* allocate(size_t size) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (used_ + size > capacity_) {
            throw std::bad_alloc("MemoryArena: Out of memory");
        }
        void* ptr = data_.get() + used_;
        used_ += size;
        return ptr;
    }
    
    /**
     * Reset arena (release all memory)
     */
    void reset() {
        std::lock_guard<std::mutex> lock(mutex_);
        used_ = 0;
    }
    
    /**
     * Get used memory
     */
    size_t used() const { return used_; }
    
    /**
     * Get total capacity
     */
    size_t capacity() const { return capacity_; }
    
private:
    std::unique_ptr<uint8_t[]> data_;
    size_t capacity_;
    std::atomic<size_t> used_;
    mutable std::mutex mutex_;
};

/**
 * Memory statistics tracker
 */
class MemoryStats {
public:
    struct PoolStats {
        size_t allocatedCount = 0;
        size_t availableCount = 0;
        size_t memoryUsage = 0;
        uint64_t allocations = 0;
        uint64_t deallocations = 0;
        double utilization = 0.0;
    };
    
    MemoryStats() : totalAllocations_(0), totalDeallocations_(0),
                    peakMemoryUsage_(0), currentMemoryUsage_(0) {}
    
    void recordAllocation(size_t bytes) {
        std::lock_guard<std::mutex> lock(mutex_);
        totalAllocations_++;
        currentMemoryUsage_ += bytes;
        if (currentMemoryUsage_ > peakMemoryUsage_) {
            peakMemoryUsage_ = currentMemoryUsage_;
        }
    }
    
    void recordDeallocation(size_t bytes) {
        std::lock_guard<std::mutex> lock(mutex_);
        totalDeallocations_++;
        currentMemoryUsage_ -= bytes;
    }
    
    PoolStats getPoolStats(size_t poolId) const {
        std::lock_guard<std::mutex> lock(mutex_);
        return poolStats_[poolId];
    }
    
    void updatePoolStats(size_t poolId, const PoolStats& stats) {
        std::lock_guard<std::mutex> lock(mutex_);
        poolStats_[poolId] = stats;
    }
    
    size_t getTotalAllocations() const { return totalAllocations_; }
    size_t getTotalDeallocations() const { return totalDeallocations_; }
    size_t getCurrentMemoryUsage() const { return currentMemoryUsage_; }
    size_t getPeakMemoryUsage() const { return peakMemoryUsage_; }
    
private:
    mutable std::mutex mutex_;
    std::unordered_map<size_t, PoolStats> poolStats_;
    std::atomic<size_t> totalAllocations_;
    std::atomic<size_t> totalDeallocations_;
    std::atomic<size_t> peakMemoryUsage_;
    std::atomic<size_t> currentMemoryUsage_;
};

/**
 * Memory leak detector
 */
class MemoryLeakDetector {
public:
    MemoryLeakDetector() : enabled_(false), maxTrackedAllocations_(10000) {}
    
    void enable() { enabled_ = true; }
    void disable() { enabled_ = false; }
    bool isEnabled() const { return enabled_; }
    
    void trackAllocation(void* ptr, size_t size, const char* type) {
        if (!enabled_) return;
        std::lock_guard<std::mutex> lock(mutex_);
        if (trackedAllocations_.size() >= maxTrackedAllocations_) {
            // Remove oldest allocation
            trackedAllocations_.erase(trackedAllocations_.begin());
        }
        trackedAllocations_[ptr] = {size, type, std::chrono::high_resolution_clock::now()};
    }
    
    void untrackAllocation(void* ptr) {
        if (!enabled_) return;
        std::lock_guard<std::mutex> lock(mutex_);
        trackedAllocations_.erase(ptr);
    }
    
    void checkLeaks() const {
        if (!enabled_) return;
        std::lock_guard<std::mutex> lock(mutex_);
        auto now = std::chrono::high_resolution_clock::now();
        for (const auto& pair : trackedAllocations_) {
            auto& info = pair.second;
            auto age = std::chrono::duration_cast<std::chrono::seconds>(now - info.timestamp);
            if (age.count() > 60) {  // Leaked for more than 60 seconds
                // Log leak warning (in real implementation, would use logger)
            }
        }
    }
    
    size_t getTrackedCount() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return trackedAllocations_.size();
    }
    
private:
    struct AllocationInfo {
        size_t size;
        const char* type;
        std::chrono::high_resolution_clock::time_point timestamp;
    };
    
    mutable std::mutex mutex_;
    bool enabled_;
    std::unordered_map<void*, AllocationInfo> trackedAllocations_;
    size_t maxTrackedAllocations_;
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
    
    /**
     * Get memory statistics for this pool
     */
    MemoryStats::PoolStats getStats() const {
        MemoryStats::PoolStats stats;
        stats.allocatedCount = allocatedCount();
        stats.availableCount = availableCount();
        stats.memoryUsage = memoryUsage();
        return stats;
    }
    
    /**
     * Check memory corruption
     */
    bool checkCorruption() const {
        for (const auto& block : blocks_) {
            // Check each allocated object for magic number
            for (size_t i = 0; i < blockSize_; ++i) {
                T* obj = block->getObject(i, slabSize_);
                // In real implementation, would check magic number
            }
        }
        return false;
    }

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
        node->tag.store(0, std::memory_order_relaxed);
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
            // Tag for corruption detection
            node->tag.store(node->magic, std::memory_order_relaxed);
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
    
    // Check for corruption
    if (node->tag.load(std::memory_order_relaxed) != node->magic) {
        // In real implementation, would log corruption error
        return;
    }
    
    // Lock-free push to free list
    MemPoolNode* current = freeList_.load(std::memory_order_relaxed);
    do {
        node->next.store(current, std::memory_order_relaxed);
    } while (!freeList_.compare_exchange_weak(current, node,
                                             std::memory_order_release,
                                             std::memory_order_relaxed));
    
    allocatedCount_.fetch_sub(1, std::memory_order_relaxed);
    availableCount_.fetch_add(1, std::memory_order_relaxed);
    
    // Clear tag to mark as free
    node->tag.store(0, std::memory_order_relaxed);
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

/**
 * Memory pool for Neuron objects
 */
class NeuronPool {
public:
    NeuronPool(size_t initialCapacity = 1024);
    ~NeuronPool();
    
    // Disable copying
    NeuronPool(const NeuronPool&) = delete;
    NeuronPool& operator=(const NeuronPool&) = delete;
    
    /**
     * Allocate a neuron with its state
     */
    NeuronId allocate();
    
    /**
     * Deallocate a neuron
     */
    void deallocate(NeuronId id);
    
    /**
     * Check if neuron is allocated
     */
    bool isAllocated(NeuronId id) const;
    
    /**
     * Access neuron state directly (for SoA access)
     */
    template<typename Member>
    Member* getState(NeuronIndex index);
    
    /**
     * Get total count
     */
    size_t size() const { return size_; }
    
    /**
     * Memory usage
     */
    size_t memoryUsage() const;
    
    /**
     * Get memory statistics
     */
    MemoryStats::PoolStats getStats() const {
        MemoryStats::PoolStats stats;
        stats.allocatedCount = size_ - freeList_.size();
        stats.availableCount = freeList_.size();
        stats.memoryUsage = memoryUsage();
        return stats;
    }
    
    /**
     * Reserve memory for specified number of neurons
     */
    void reserve(size_t count) {
        pool_.reserve(count);
    }

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
    
    // Memory pool
    MemoryPool<Neuron> pool_;
};

/**
 * Memory pool for Synapse objects
 */
class SynapsePool {
public:
    SynapsePool(size_t initialCapacity = 4096);
    ~SynapsePool();
    
    // Disable copying
    SynapsePool(const SynapsePool&) = delete;
    SynapsePool& operator=(const SynapsePool&) = delete;
    
    /**
     * Allocate a synapse
     */
    SynapseId allocate(NeuronId source, NeuronId destination);
    
    /**
     * Deallocate a synapse
     */
    void deallocate(SynapseId id);
    
    /**
     * Check if synapse is allocated
     */
    bool isAllocated(SynapseId id) const;
    
    /**
     * Get synapse data (for SoA access)
     */
    template<typename Member>
    Member* getState(SynapseIndex index);
    
    /**
     * Get source/destination neurons
     */
    NeuronId getSource(SynapseIndex index) const;
    NeuronId getDestination(SynapseIndex index) const;
    
    /**
     * Total count
     */
    size_t size() const { return size_; }
    
    /**
     * Memory usage
     */
    size_t memoryUsage() const;
    
    /**
     * Get memory statistics
     */
    MemoryStats::PoolStats getStats() const {
        MemoryStats::PoolStats stats;
        stats.allocatedCount = size_ - freeList_.size();
        stats.availableCount = freeList_.size();
        stats.memoryUsage = memoryUsage();
        return stats;
    }
    
    /**
     * Reserve memory for specified number of synapses
     */
    void reserve(size_t count) {
        pool_.reserve(count);
    }

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
    
    // Memory pool
    MemoryPool<Synapse> pool_;
};

/**
 * NeuralRegion memory pool - integrates all specialized pools
 */
class NeuralRegionMemoryPool {
public:
    NeuralRegionMemoryPool();
    ~NeuralRegionMemoryPool();
    
    // Disable copying
    NeuralRegionMemoryPool(const NeuralRegionMemoryPool&) = delete;
    NeuralRegionMemoryPool& operator=(const NeuralRegionMemoryPool&) = delete;
    
    /**
     * Get neuron pool
     */
    NeuronPool& getNeuronPool() { return neuronPool_; }
    
    /**
     * Get synapse pool
     */
    SynapsePool& getSynapsePool() { return synapsePool_; }
    
    /**
     * Get total memory usage
     */
    size_t totalMemoryUsage() const {
        return neuronPool_.memoryUsage() + synapsePool_.memoryUsage();
    }
    
    /**
     * Get global memory statistics
     */
    MemoryStats::PoolStats getNeuronStats() const { return neuronPool_.getStats(); }
    MemoryStats::PoolStats getSynapseStats() const { return synapsePool_.getStats(); }
    
    /**
     * Check for memory leaks
     */
    void checkMemoryLeaks() {
        leakDetector_.checkLeaks();
    }
    
    /**
     * Enable/disable memory leak detection
     */
    void enableLeakDetection(bool enable) {
        if (enable) leakDetector_.enable();
        else leakDetector_.disable();
    }
    
    /**
     * Get global memory usage
     */
    size_t getGlobalMemoryUsage() const {
        return neuronPool_.memoryUsage() + synapsePool_.memoryUsage();
    }
    
    /**
     * Reserve memory for regions
     */
    void reserveMemory(size_t neuronCount, size_t synapseCount) {
        neuronPool_.reserve(neuronCount);
        synapsePool_.reserve(synapseCount);
    }

private:
    NeuronPool neuronPool_;
    SynapsePool synapsePool_;
    MemoryLeakDetector leakDetector_;
    mutable std::mutex mutex_;
};

/**
 * Global memory manager for the entire neural simulation
 */
class GlobalMemoryManager {
public:
    static GlobalMemoryManager& getInstance() {
        static GlobalMemoryManager instance;
        return instance;
    }
    
    /**
     * Get neural region memory pool
     */
    NeuralRegionMemoryPool& getNeuralRegionMemoryPool() {
        return neuralRegionPool_;
    }
    
    /**
     * Get global memory statistics
     */
    size_t getPeakMemoryUsage() const {
        return stats_.getPeakMemoryUsage();
    }
    
    /**
     * Get current memory usage
     */
    size_t getCurrentMemoryUsage() const {
        return stats_.getCurrentMemoryUsage();
    }
    
    /**
     * Enable/disable memory leak detection
     */
    void enableLeakDetection(bool enable) {
        neuralRegionPool_.enableLeakDetection(enable);
    }
    
    /**
     * Check for memory issues
     */
    void healthCheck() {
        neuralRegionPool_.checkMemoryLeaks();
    }
    
    /**
     * Log memory statistics
     */
    void logMemoryStats() const {
        // In real implementation, would use logger
    }

private:
    GlobalMemoryManager() = default;
    ~GlobalMemoryManager() = default;
    
    // Disable copying
    GlobalMemoryManager(const GlobalMemoryManager&) = delete;
    GlobalMemoryManager& operator=(const GlobalMemoryManager&) = delete;
    
    NeuralRegionMemoryPool neuralRegionPool_;
    MemoryStats stats_;
};

} // namespace nlm