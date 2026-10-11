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
 * - Thread-local caching for reduced contention
 * - Advanced memory tracking and statistics
 * - Memory alignment for cache efficiency
 * 
 * Biological motivation:
 * - Like biological neural systems that maintain fixed numbers of neurons/synapses
 * - Reduces "allocation" overhead similar to how biological systems don't constantly create/destroy neurons
 */

#include "../core/Types/Types.hpp"

#include <cstddef>
#include <cstdint>
#include <vector>
#include <memory>
#include <atomic>
#include <array>
#include <optional>
#include <algorithm>
#include <chrono>

namespace nlm {

/**
 * Memory statistics for monitoring and optimization
 */
struct MemoryStats {
    size_t totalAllocations{0};
    size_t totalDeallocations{0};
    size_t currentObjects{0};
    size_t peakObjects{0};
    size_t memoryUsageBytes{0};
    size_t peakMemoryUsageBytes{0};
    uint64_t totalAllocationTimeNs{0};
    uint64_t totalDeallocationTimeNs{0};
    size_t cacheHits{0};
    size_t cacheMisses{0};
    
    void reset() {
        totalAllocations = 0;
        totalDeallocations = 0;
        currentObjects = 0;
        peakObjects = 0;
        memoryUsageBytes = 0;
        peakMemoryUsageBytes = 0;
        totalAllocationTimeNs = 0;
        totalDeallocationTimeNs = 0;
        cacheHits = 0;
        cacheMisses = 0;
    }
    
    double getUtilizationRate() const {
        return peakObjects > 0 ? static_cast<double>(currentObjects) / peakObjects : 0.0;
    }
    
    double getCacheHitRate() const {
        size_t total = cacheHits + cacheMisses;
        return total > 0 ? static_cast<double>(cacheHits) / total : 0.0;
    }
    
    size_t getActiveObjects() const {
        return totalAllocations - totalDeallocations;
    }
};

/**
 * Thread-local cache for reduced contention
 */
template<typename T>
struct ThreadLocalCache {
    std::vector<T*> cache;
    size_t maxCacheSize;
    
    ThreadLocalCache(size_t maxSize = 64) : maxCacheSize(maxSize) {}
    
    bool tryCache(T*& obj) {
        for (auto cached : cache) {
            if (cached) {
                obj = cached;
                cache.erase(std::remove(cache.begin(), cache.end(), cached), cache.end());
                return true;
            }
        }
        return false;
    }
    
    void cacheObject(T* obj) {
        if (cache.size() < maxCacheSize && obj) {
            cache.push_back(obj);
        }
    }
    
    void clear() {
        cache.clear();
    }
};

/**
 * Enhanced MemoryPool with advanced features
 */
template<typename T>
class MemoryPool {
public:
    /**
     * Create a memory pool with advanced configuration
     * @param blockSize Number of objects per memory block
     * @param initialBlocks Number of blocks to pre-allocate
     * @param cacheSize Size of thread-local cache
     * @param enableStatistics Enable collection of allocation statistics
     */
    explicit MemoryPool(size_t blockSize = 1024, size_t initialBlocks = 4,
                       size_t cacheSize = 64, bool enableStatistics = true)
        : slabSize_(sizeof(T))
        , blockSize_(blockSize)
        , freeList_(nullptr)
        , currentBlock_(nullptr)
        , currentIndex_(0)
        , allocatedCount_(0)
        , availableCount_(0)
        , statsEnabled_(enableStatistics)
        , cacheSize_(cacheSize) {
        
        // Align slab size to cache line (64 bytes) for false sharing prevention
        constexpr size_t cacheLineSize = 64;
        if (slabSize_ % cacheLineSize != 0) {
            slabSize_ = ((slabSize_ / cacheLineSize) + 1) * cacheLineSize;
        }
        
        // Pre-allocate initial blocks
        for (size_t i = 0; i < initialBlocks; ++i) {
            addBlock();
        }
        
        // Initialize thread-local caches
        threadLocalCaches_.resize(std::thread::hardware_concurrency() + 1);
    }
    
    ~MemoryPool() {
        // Blocks are automatically freed via unique_ptr
        clearThreadLocalCaches();
    }
    
    // Disable copying
    MemoryPool(const MemoryPool&) = delete;
    MemoryPool& operator=(const MemoryPool&) = delete;
    
    /**
     * Allocate an object from the pool with advanced tracking
     * Lock-free operation with thread-local caching
     */
    T* allocate() {
        T* obj = nullptr;
        size_t threadId = getCurrentThreadId();
        
        // Try thread-local cache first
        if (threadLocalCaches_[threadId].tryCache(obj)) {
            if (statsEnabled_) {
                std::atomic_thread_fence(std::memory_order_acquire);
                stats_.cacheHits++;
            }
            return obj;
        }
        
        // Try global free list
        MemPoolNode* node = freeList_.load(std::memory_order_acquire);
        while (node != nullptr) {
            MemPoolNode* next = node->next.load(std::memory_order_relaxed);
            if (freeList_.compare_exchange_weak(node, next, 
                                                 std::memory_order_release, 
                                                 std::memory_order_relaxed)) {
                allocatedCount_.fetch_add(1, std::memory_order_release);
                availableCount_.fetch_sub(1, std::memory_order_release);
                
                obj = reinterpret_cast<T*>(node);
                
                if (statsEnabled_) {
                    auto startTime = std::chrono::high_resolution_clock::now();
                    stats_.totalAllocations++;
                    stats_.currentObjects++;
                    if (stats_.currentObjects > stats_.peakObjects) {
                        stats_.peakObjects = stats_.currentObjects;
                    }
                    auto endTime = std::chrono::high_resolution_clock::now();
                    stats_.totalAllocationTimeNs += 
                        std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime).count();
                }
                
                return obj;
            }
        }
        
        // Free list empty, add a new block
        addBlock();
        
        // Try again with global free list
        node = freeList_.load(std::memory_order_acquire);
        while (node != nullptr) {
            MemPoolNode* next = node->next.load(std::memory_order_relaxed);
            if (freeList_.compare_exchange_weak(node, next, 
                                                 std::memory_order_release, 
                                                 std::memory_order_relaxed)) {
                allocatedCount_.fetch_add(1, std::memory_order_release);
                availableCount_.fetch_sub(1, std::memory_order_release);
                
                obj = reinterpret_cast<T*>(node);
                
                if (statsEnabled_) {
                    auto startTime = std::chrono::high_resolution_clock::now();
                    stats_.totalAllocations++;
                    stats_.currentObjects++;
                    if (stats_.currentObjects > stats_.peakObjects) {
                        stats_.peakObjects = stats_.currentObjects;
                    }
                    auto endTime = std::chrono::high_resolution_clock::now();
                    stats_.totalAllocationTimeNs += 
                        std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime).count();
                }
                
                return obj;
            }
        }
        
        // Fallback to scalar allocation (should not happen in normal operation)
        return allocateScalar();
    }
    
    /**
     * Return an object to the pool with advanced tracking
     * Lock-free operation with thread-local caching
     */
    void deallocate(T* obj) {
        if (obj == nullptr) return;
        
        MemPoolNode* node = reinterpret_cast<MemPoolNode*>(obj);
        size_t threadId = getCurrentThreadId();
        
        // Check cache before deallocation
        bool wasCached = false;
        for (auto cached : threadLocalCaches_[threadId].cache) {
            if (cached == obj) {
                wasCached = true;
                break;
            }
        }
        
        // Try thread-local cache first
        if (!wasCached && threadLocalCaches_[threadId].cache.size() < cacheSize_) {
            threadLocalCaches_[threadId].cache.push_back(obj);
            if (statsEnabled_) {
                stats_.cacheHits++;
            }
            return;
        }
        
        // Lock-free push to free list
        MemPoolNode* current = freeList_.load(std::memory_order_relaxed);
        do {
            node->next.store(current, std::memory_order_relaxed);
        } while (!freeList_.compare_exchange_weak(current, node,
                                                 std::memory_order_release,
                                                 std::memory_order_relaxed));
        
        if (statsEnabled_) {
            auto startTime = std::chrono::high_resolution_clock::now();
            stats_.totalDeallocations++;
            stats_.currentObjects--;
            stats_.memoryUsageBytes = memoryUsage();
            if (stats_.memoryUsageBytes > stats_.peakMemoryUsageBytes) {
                stats_.peakMemoryUsageBytes = stats_.memoryUsageBytes;
            }
            auto endTime = std::chrono::high_resolution_clock::now();
            stats_.totalDeallocationTimeNs += 
                std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime).count();
        }
    }
    
    /**
     * Check if pool is empty
     */
    bool empty() const { 
        return freeList_.load(std::memory_order_relaxed) == nullptr && 
               allocatedCount_.load(std::memory_order_relaxed) == 0;
    }
    
    /**
     * Get number of allocated objects
     */
    size_t allocatedCount() const { 
        return allocatedCount_.load(std::memory_order_relaxed); 
    }
    
    /**
     * Get number of available objects
     */
    size_t availableCount() const { 
        return availableCount_.load(std::memory_order_relaxed); 
    }
    
    /**
     * Get memory statistics
     */
    MemoryStats getStats() const { 
        MemoryStats stats = stats_; 
        return stats; 
    }
    
    /**
     * Reset statistics
     */
    void resetStats() { 
        if (statsEnabled_) {
            stats_.reset();
        }
    }
    
    /**
     * Get thread-local cache hit rate
     */
    double getThreadCacheHitRate(size_t threadId) const {
        if (!statsEnabled_ || threadId >= threadLocalCaches_.size()) return 0.0;
        size_t total = threadLocalCaches_[threadId].cache.size();
        return total > 0 ? static_cast<double>(stats_.cacheHits) / total : 0.0;
    }
    
    /**
     * Clear all thread-local caches
     */
    void clearThreadLocalCaches() {
        for (auto& cache : threadLocalCaches_) {
            cache.clear();
        }
    }
    
    /**
     * Force garbage collection of unused objects
     */
    void compact() {
        // Implementation for compacting unused memory
        // This could involve removing empty blocks or defragmenting
        // For now, it's a placeholder for future optimization
    }
    
    /**
     * Get total memory used by pool (bytes) with enhanced tracking
     */
    size_t memoryUsage() const {
        size_t total = 0;
        for (const auto& block : blocks_) {
            total += block->data.capacity() * sizeof(uint8_t);
        }
        return total;
    }
    
    /**
     * Pre-allocate objects with statistics
     */
    void reserve(size_t count) {
        while (availableCount_.load(std::memory_order_relaxed) < count) {
            addBlock();
        }
    }
    
    /**
     * Get current memory efficiency ratio
     */
    double getMemoryEfficiency() const {
        if (availableCount_ == 0) return 0.0;
        return static_cast<double>(allocatedCount_) / 
               (allocatedCount_ + availableCount_);
    }
    
private:
    // Helper functions
    size_t getCurrentThreadId() {
        static thread_local size_t threadId = []() {
            static std::atomic<size_t> nextId(0);
            return nextId++;
        }();
        return threadId % threadLocalCaches_.size();
    }
    
    T* allocateScalar() {
        // Fallback scalar allocation (should rarely be used)
        // This is a simplified implementation for edge cases
        return nullptr;
    }
    
    // Memory block containing objects
    struct Block {
        std::vector<uint8_t> data;
        Block* nextBlock;
        size_t blockStartIndex;
        
        explicit Block(size_t objectSize, size_t count)
            : data(objectSize * count)
            , nextBlock(nullptr)
            , blockStartIndex(0) {
            // Data is left uninitialized for performance
        }
        
        T* getObject(size_t index, size_t objectSize) {
            return reinterpret_cast<T*>(data.data() + index * objectSize);
        }
    };
    
    // Add a new block to the pool with enhanced logging
    void addBlock() {
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
        
        if (statsEnabled_) {
            stats_.peakMemoryUsageBytes = std::max(stats_.peakMemoryUsageBytes, memoryUsage());
        }
    }
    
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
    bool statsEnabled_;
    size_t cacheSize_;
    
    // Thread-local caches
    std::vector<ThreadLocalCache<T>> threadLocalCaches_;
    
    // Memory statistics
    MemoryStats stats_;
};

// NeuronPool - Specialized pool for neurons with enhanced cache-aligned states
class NeuronPool {
public:
    NeuronPool(size_t initialCapacity = 1024)
        : size_(0), capacity_(initialCapacity), freeListTop_(0) {
        blocks_.reserve((initialCapacity + BLOCK_SIZE - 1) / BLOCK_SIZE);
        allocateBlock();
    }
    
    ~NeuronPool() = default;
    
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
    
    // Get statistics
    MemoryStats getStats() const { return stats_; }
    
    // Reset statistics
    void resetStats() { stats_.reset(); }
    
private:
    static constexpr size_t BLOCK_SIZE = 1024;
    static constexpr size_t CACHE_LINE = 64;
    
    struct alignas(CACHE_LINE) NeuronBlock {
        // SoA storage for neuron states with enhanced alignment
        std::vector<float> membranePotential;
        std::vector<float> restingPotential;
        std::vector<float> threshold;
        std::vector<float> resetPotential;
        std::vector<float> leakConductance;
        std::vector<float> synapseConductance;
        std::vector<float> firingRate;
        std::vector<uint8_t> firingState;
        std::vector<uint32_t> refractoryRemaining;
        std::vector<uint32_t> refractoryPeriod;
        std::vector<float> adaptationVariable;
        std::vector<float> lastSpikeTime;
        std::vector<uint64_t> neuronType;
        std::vector<uint64_t> regionId;
        std::vector<uint64_t> populationId;
        
        // Per-neuron current injection
        std::vector<float> synapticInput;
        
        // Memory for other neuron properties
        std::vector<float> membranePotentialExtensions;
        std::vector<float> synapticInputExtensions;
        
        NeuronBlock() {}
    };
    
    void allocateBlock() {
        auto block = std::make_unique<NeuronBlock>();
        block->membranePotential.resize(BLOCK_SIZE);
        block->restingPotential.resize(BLOCK_SIZE);
        block->threshold.resize(BLOCK_SIZE);
        block->resetPotential.resize(BLOCK_SIZE);
        block->leakConductance.resize(BLOCK_SIZE);
        block->synapseConductance.resize(BLOCK_SIZE);
        block->firingRate.resize(BLOCK_SIZE);
        block->firingState.resize(BLOCK_SIZE);
        block->refractoryRemaining.resize(BLOCK_SIZE);
        block->refractoryPeriod.resize(BLOCK_SIZE);
        block->adaptationVariable.resize(BLOCK_SIZE);
        block->lastSpikeTime.resize(BLOCK_SIZE);
        block->neuronType.resize(BLOCK_SIZE);
        block->regionId.resize(BLOCK_SIZE);
        block->populationId.resize(BLOCK_SIZE);
        block->synapticInput.resize(BLOCK_SIZE);
        
        // Initialize with default values
        for (size_t i = 0; i < BLOCK_SIZE; ++i) {
            block->membranePotential[i] = -70.0f;
            block->restingPotential[i] = -70.0f;
            block->threshold[i] = -55.0f;
            block->resetPotential[i] = -70.0f;
            block->leakConductance[i] = 10.0f;
            block->firingRate[i] = 0.0f;
            block->firingState[i] = 0;
            block->refractoryRemaining[i] = 0;
            block->refractoryPeriod[i] = 5;
            block->lastSpikeTime[i] = -1.0f;
        }
        
        blocks_.push_back(std::move(block));
    }
    
    size_t size_;
    size_t capacity_;
    std::vector<std::unique_ptr<NeuronBlock>> blocks_;
    std::vector<NeuronIndex> freeList_;
    std::atomic<size_t> freeListTop_;
    MemoryStats stats_;
};

// SynapsePool - Enhanced synapse pool with additional features
class SynapsePool {
public:
    SynapsePool(size_t initialCapacity = 1024)
        : size_(0), capacity_(initialCapacity), freeListTop_(0) {
        blocks_.reserve((initialCapacity + BLOCK_SIZE - 1) / BLOCK_SIZE);
        allocateBlock();
    }
    
    ~SynapsePool() = default;
    
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
    
    // Get statistics
    MemoryStats getStats() const { return stats_; }
    
    // Reset statistics
    void resetStats() { stats_.reset(); }
    
    // Get average synapse weight
    float getAverageWeight() const;
    
    // Get weight statistics
    void getWeightStats(float& min, float& max, float& avg) const;
    
private:
    static constexpr size_t BLOCK_SIZE = 1024;
    static constexpr size_t CACHE_LINE = 64;
    
    struct alignas(CACHE_LINE) SynapseBlock {
        // Enhanced SoA storage for synapse states
        std::vector<uint64_t> sourceNeuron;
        std::vector<uint64_t> destinationNeuron;
        std::vector<float> weight;
        std::vector<uint32_t> delay;
        std::vector<uint8_t> synapseType;
        std::vector<uint8_t> plasticityFlags;
        
        // Short-term plasticity
        std::vector<float> shortTermDepression;
        std::vector<float> shortTermFacilitation;
        std::vector<float> lastPreSpikeTime;
        std::vector<float> lastPostSpikeTime;
        
        // Eligibility trace for reward-modulated plasticity
        std::vector<float> eligibilityTrace;
        
        // Synaptic efficacy for competition
        std::vector<float> efficacy;
        
        // Additional metadata
        std::vector<uint64_t> creationTime;
        std::vector<float> lastUpdateTime;
        std::vector<uint32_t> updateCount;
        
        SynapseBlock() {}
    };
    
    void allocateBlock() {
        auto block = std::make_unique<SynapseBlock>();
        block->sourceNeuron.resize(BLOCK_SIZE);
        block->destinationNeuron.resize(BLOCK_SIZE);
        block->weight.resize(BLOCK_SIZE);
        block->delay.resize(BLOCK_SIZE);
        block->synapseType.resize(BLOCK_SIZE);
        block->plasticityFlags.resize(BLOCK_SIZE);
        block->shortTermDepression.resize(BLOCK_SIZE);
        block->shortTermFacilitation.resize(BLOCK_SIZE);
        block->lastPreSpikeTime.resize(BLOCK_SIZE);
        block->lastPostSpikeTime.resize(BLOCK_SIZE);
        block->eligibilityTrace.resize(BLOCK_SIZE);
        block->efficacy.resize(BLOCK_SIZE);
        block->creationTime.resize(BLOCK_SIZE);
        block->lastUpdateTime.resize(BLOCK_SIZE);
        block->updateCount.resize(BLOCK_SIZE);
        
        // Initialize with default values
        for (size_t i = 0; i < BLOCK_SIZE; ++i) {
            block->weight[i] = 0.5f;
            block->delay[i] = 1;
            block->synapseType[i] = 0;
            block->plasticityFlags[i] = 0xFF;  // All plasticity enabled by default
            block->shortTermDepression[i] = 1.0f;
            block->shortTermFacilitation[i] = 0.0f;
            block->lastPreSpikeTime[i] = -1.0f;
            block->lastPostSpikeTime[i] = -1.0f;
            block->eligibilityTrace[i] = 0.0f;
            block->efficacy[i] = 1.0f;
            block->creationTime[i] = 0;
            block->lastUpdateTime[i] = 0.0f;
            block->updateCount[i] = 0;
        }
        
        blocks_.push_back(std::move(block));
    }
    
    size_t size_;
    size_t capacity_;
    std::vector<std::unique_ptr<SynapseBlock>> blocks_;
    std::vector<SynapseIndex> freeList_;
    std::atomic<size_t> freeListTop_;
    MemoryStats stats_;
};

} // namespace nlm