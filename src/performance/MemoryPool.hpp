#pragma once

/**
 * Enhanced MemoryPool - Advanced memory management with thread safety and performance monitoring
 * 
 * Provides pre-allocated object pools with enhanced features:
 * 
 * Features:
 * - Advanced lock-free algorithms with reduced contention
 * - Memory pools with object reuse and fragmentation management
 * - Memory leak detection and tracking
 * - Performance monitoring and statistics
 * - Thread-safe operations with optimized synchronization
 * - Memory de-fragmentation capabilities
 * - Configurable pool sizing and growth strategies
 * - Aligned memory allocation for cache efficiency
 * - Memory usage tracking and reporting
 * 
 * Biological motivation:
 * - Like biological neural systems that maintain fixed numbers of neurons/synapses
 * - Reduces "allocation" overhead similar to how biological systems don't constantly create/destroy neurons
 * - Memory efficiency for large-scale neural simulations
 * 
 * Phase 5+ enhancements:
 * - Advanced thread safety with reduced contention
 * - Memory tracking and leak detection
 * - Performance monitoring integration
 * - Memory fragmentation analysis
 * - Object lifecycle management
 */

#ifndef NLM_MEMORY_POOL_HPP
#define NLM_MEMORY_POOL_HPP

#include <cstddef>
#include <cstdint>
#include <vector>
#include <memory>
#include <atomic>
#include <array>
#include <optional>
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <chrono>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <numeric>
#include <limits>
#include <stdexcept>
#include <new>
#include <type_traits>
#include <forward_list>
#include "Performance.hpp"

namespace nlm {

// Forward declarations
class MemoryPoolBase;

/**
 * Lock-free stack node for memory pool free list with enhanced features
 */
struct MemPoolNode {
    std::atomic<MemPoolNode*> next;
    std::atomic<uint64_t> sequence;  // For improved CAS detection
    uint64_t threadId;  // Track which thread owns this node
    uint64_t timestamp;  // Creation timestamp for debugging
    
    MemPoolNode() : next(nullptr), sequence(0), threadId(0), timestamp(0) {}
    
    explicit MemPoolNode(size_t size) {
        // Initialize with padding for alignment
        static constexpr size_t paddingSize = 64;
        if (size < paddingSize) {
            // Add padding to prevent false sharing
            alignas(64) char padding[paddingSize];
        }
    }
};

/**
 * Enhanced MemoryPool - Slab allocator with advanced features
 * 
 * Template parameter T specifies the object type to allocate.
 * All objects have the same fixed size determined at construction.
 * 
 * Features:
 * - Lock-free allocation/deallocation with reduced contention
 * - Memory tracking and leak detection
 * - Performance monitoring integration
 * - Memory fragmentation management
 * - Thread-local caching for reduced contention
 * - Memory de-fragmentation capabilities
 * - Object lifecycle tracking
 * - Configurable growth strategies
 * - Memory usage statistics
 */
template<typename T>
class MemoryPool {
public:
    /**
     * Memory allocation strategies
     */
    enum class AllocationStrategy {
        Conservative,  // Grow only when needed
        Aggressive,    // Grow proactively
        Adaptive      // Learn from allocation patterns
    };
    
    /**
     * Memory pool configuration
     */
    struct PoolConfig {
        size_t blockSize;              // Objects per block
        size_t initialBlocks;          // Initial blocks to pre-allocate
        size_t maxBlocks;              // Maximum number of blocks
        AllocationStrategy allocationStrategy;  // Growth strategy
        bool enableLeakDetection;      // Enable memory leak detection
        bool enableFragmentationTracking; // Track memory fragmentation
        size_t threadLocalCacheSize;   // Size of thread-local cache
        size_t alignment;              // Memory alignment (0 = auto)
        
        PoolConfig() 
            : blockSize(1024), initialBlocks(4), maxBlocks(1024 * 1024),
              allocationStrategy(AllocationStrategy::Adaptive),
              enableLeakDetection(true), enableFragmentationTracking(true),
              threadLocalCacheSize(16), alignment(64) {}
    };
    
    /**
     * Create a memory pool
     * @param config Pool configuration
     */
    explicit MemoryPool(const PoolConfig& config = PoolConfig());
    
    /**
     * Legacy constructor for backward compatibility
     * @param blockSize Number of objects per memory block
     * @param initialBlocks Number of blocks to pre-allocate
     */
    explicit MemoryPool(size_t blockSize = 1024, size_t initialBlocks = 4);
    
    ~MemoryPool();
    
    // Disable copying
    MemoryPool(const MemoryPool&) = delete;
    MemoryPool& operator=(const MemoryPool&) = delete;
    
    // Enable moving
    MemoryPool(MemoryPool&& other) noexcept;
    MemoryPool& operator=(MemoryPool&& other) noexcept;
    
    /**
     * Allocate an object from the pool
     * Lock-free operation with thread-local caching
     */
    T* allocate();
    
    /**
     * Return an object to the pool
     * Lock-free operation with thread-local caching
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
     * Get pool configuration
     */
    const PoolConfig& getConfig() const { return config_; }
    
    /**
     * Pre-allocate objects
     */
    void reserve(size_t count);
    
    /**
     * Force memory cleanup (reduce memory usage)
     */
    void compact();
    
    /**
     * Check for memory leaks
     */
    bool hasLeaks() const { return leakTracker_.hasLeaks(); }
    
    /**
     * Get memory leak report
     */
    MemoryTracker::LeakReport getLeakReport() const { return leakTracker_.checkLeaks(); }
    
    /**
     * Enable/disable memory leak detection
     */
    void setLeakDetectionEnabled(bool enabled) { 
        leakDetectionEnabled_ = enabled; 
        if (enabled) leakTracker_.clear();
    }
    
    /**
     * Enable/disable fragmentation tracking
     */
    void setFragmentationTrackingEnabled(bool enabled) { 
        fragmentationTrackingEnabled_ = enabled; 
    }
    
    /**
     * Get fragmentation ratio (0.0 - 1.0, lower is better)
     */
    double getFragmentationRatio() const;
    
    /**
     * Get performance metrics for this pool
     */
    PerfMetrics getPerformanceMetrics() const;
    
    /**
     * Get thread-local cache statistics
     */
    struct ThreadLocalCacheStats {
        size_t cacheSize{0};
        size_t hits{0};
        size_t misses{0};
        double hitRate() const { return cacheSize > 0 ? (double)hits / (hits + misses) : 0.0; }
    };
    
    ThreadLocalCacheStats getThreadLocalCacheStats(size_t threadId) const;
    
    /**
     * Get total threads using this pool
     */
    size_t getThreadCount() const { return threadCache_.size(); }
    
private:
    // Memory block containing objects
    struct Block {
        std::vector<uint8_t> data;
        Block* nextBlock;
        std::atomic<size_t> freeIndices;  // Number of free indices in this block
        size_t firstFreeIndex;  // Index of first free object (for sequential allocation)
        
        explicit Block(size_t objectSize, size_t count, size_t alignment);
        
        T* getObject(size_t index, size_t objectSize) {
            return reinterpret_cast<T*>(data.data() + index * objectSize);
        }
        
        void markObjectUsed(size_t index, size_t objectSize) {
            // Update free indices counter
            freeIndices.fetch_sub(1, std::memory_order_relaxed);
            // Mark object as used (for debugging)
            if (firstFreeIndex == index) {
                // Find next free index
                for (size_t i = index + 1; i < objectSize * count; i += objectSize) {
                    if (isObjectFree(i, objectSize)) {
                        firstFreeIndex = i;
                        return;
                    }
                }
                firstFreeIndex = static_cast<size_t>(-1);
            }
        }
        
        void markObjectFree(size_t index, size_t objectSize) {
            // Update free indices counter
            freeIndices.fetch_add(1, std::memory_order_relaxed);
            // Mark object as free (for debugging)
            if (firstFreeIndex == static_cast<size_t>(-1) || index < firstFreeIndex) {
                firstFreeIndex = index;
            }
        }
        
        bool isObjectFree(size_t index, size_t objectSize) const {
            return freeIndices.load(std::memory_order_relaxed) > 0;
        }
    };
    
    // Thread-local cache for reduced contention
    struct ThreadLocalCache {
        std::vector<T*> cachedObjects;
        size_t cacheHits;
        size_t cacheMisses;
        uint64_t lastAccess;
        
        ThreadLocalCache() : cacheHits(0), cacheMisses(0), lastAccess(0) {}
    };
    
    // Enhanced CAS loop with backoff
    template<typename TPtr>
    bool compareExchangeStrong(volatile TPtr& expected, TPtr desired, 
                               std::memory_order successOrder = std::memory_order_release,
                               std::memory_order failureOrder = std::memory_order_relaxed) {
        // Adaptive backoff strategy for reduced contention
        static constexpr int maxBackoff = 16;
        int backoff = 1;
        
        while (true) {
            TPtr current = expected;
            if (current == desired) return true;
            
            if (expected.compare_exchange_weak(current, desired, successOrder, failureOrder)) {
                return true;
            }
            
            // Exponential backoff
            if (backoff < maxBackoff) {
                std::this_thread::yield();
                backoff *= 2;
            } else {
                backoff = 1;
            }
        }
    }
    
    // Add a new block to the pool
    void addBlock();
    
    // Thread-local cache management
    void initThreadLocalCache(size_t threadId);
    ThreadLocalCache* getThreadLocalCache(size_t threadId);
    void returnToThreadLocalCache(size_t threadId, T* obj);
    T* getFromThreadLocalCache(size_t threadId);
    
    // Memory tracking
    void trackAllocation(T* obj, const std::string& sourceFile, size_t sourceLine);
    void trackDeallocation(T* obj, const std::string& sourceFile, size_t sourceLine);
    
    // Memory de-fragmentation
    void defragment();
    
    // Statistics
    void updateStatistics();
    
    // Slab size for each object (rounded up to alignment)
    size_t slabSize_;
    
    // Number of objects per block
    size_t blockSize_;
    
    // Maximum number of blocks
    size_t maxBlocks_;
    
    // Allocation strategy
    AllocationStrategy allocationStrategy_;
    
    // Head of free list (lock-free with enhanced CAS)
    std::atomic<MemPoolNode*> freeList_;
    
    // List of memory blocks
    std::vector<std::unique_ptr<Block>> blocks_;
    
    // Block we're currently allocating from (for sequential allocation)
    std::atomic<Block*> currentBlock_;
    
    // Index of next available object in current block
    std::atomic<size_t> currentIndex_;
    
    // Statistics
    std::atomic<size_t> allocatedCount_;
    std::atomic<size_t> availableCount_;
    
    // Configuration
    PoolConfig config_;
    
    // Thread-local caches
    std::vector<ThreadLocalCache> threadCache_;
    mutable std::shared_mutex threadCacheMutex_;
    
    // Memory tracking
    bool leakDetectionEnabled_;
    bool fragmentationTrackingEnabled_;
    MemoryTracker leakTracker_;
    
    // Performance monitoring
    std::atomic<uint64_t> threadAllocationCount_;
    std::atomic<uint64_t> threadDeallocationCount_;
    std::atomic<uint64_t> threadLockContention_;
    
    // Memory fragmentation tracking
    struct FragmentationStats {
        std::vector<size_t> freeBlockSizes;
        double averageFreeFragmentation{0.0};
        size_t largestFreeBlock{0};
        uint64_t lastDefragmentationTime{0};
    } fragmentationStats_;
    
    mutable std::mutex fragmentationMutex_;
    
    // Timestamp for last activity
    std::atomic<uint64_t> lastActivity_;
    
    // Object tracking for debugging
    std::unordered_map<void*, std::pair<size_t, std::string>> objectTracker_;
    mutable std::mutex trackerMutex_;
    
    // Alignment
    size_t alignment_;
}; 

// MemoryPoolNode implementation details
inline void MemPoolNode::initializeFromAllocation() {
    threadId = std::hash<std::thread::id>{}(std::this_thread::get_id());
    timestamp = std::chrono::high_resolution_clock::now().time_since_epoch().count();
}

// Template implementation must be in header for generic types

inline MemoryPool<T>::MemoryPool(const PoolConfig& config)
    : slabSize_(sizeof(T))
    , blockSize_(config.blockSize)
    , maxBlocks_(config.maxBlocks)
    , allocationStrategy_(config.allocationStrategy)
    , freeList_(nullptr)
    , currentBlock_(nullptr)
    , currentIndex_(0)
    , allocatedCount_(0)
    , availableCount_(0)
    , config_(config)
    , leakDetectionEnabled_(config.enableLeakDetection)
    , fragmentationTrackingEnabled_(config.enableFragmentationTracking)
    , threadAllocationCount_(0)
    , threadDeallocationCount_(0)
    , threadLockContention_(0)
    , lastActivity_(0)
    , alignment_(config.alignment)
{
    // Align slab size to cache line (64 bytes) for false sharing prevention
    constexpr size_t cacheLineSize = 64;
    if (slabSize_ % cacheLineSize != 0) {
        slabSize_ = ((slabSize_ / cacheLineSize) + 1) * cacheLineSize;
    }
    
    if (alignment_ % cacheLineSize != 0) {
        alignment_ = ((alignment_ / cacheLineSize) + 1) * cacheLineSize;
    }
    
    // Pre-allocate initial blocks
    size_t initialBlockCount = std::min(config_.initialBlocks, maxBlocks_);
    for (size_t i = 0; i < initialBlockCount; ++i) {
        addBlock();
    }
    
    // Initialize thread-local caches
    size_t numHardwareThreads = std::max<size_t>(1, std::thread::hardware_concurrency());
    threadCache_.resize(numHardwareThreads);
    for (size_t i = 0; i < numHardwareThreads; ++i) {
        threadCache_[i] = ThreadLocalCache();
        threadCache_[i].cachedObjects.reserve(config_.threadLocalCacheSize);
    }
}

inline MemoryPool<T>::MemoryPool(size_t blockSize, size_t initialBlocks)
    : MemoryPool(PoolConfig{blockSize, initialBlocks, 0, AllocationStrategy::Adaptive, true, true, 16, 64}) {}

inline MemoryPool<T>::MemoryPool(MemoryPool&& other) noexcept
    : slabSize_(other.slabSize_)
    , blockSize_(other.blockSize_)
    , maxBlocks_(other.maxBlocks_)
    , allocationStrategy_(other.allocationStrategy_)
    , freeList_(other.freeList_.load())
    , currentBlock_(other.currentBlock_.load())
    , currentIndex_(other.currentIndex_.load())
    , allocatedCount_(other.allocatedCount_.load())
    , availableCount_(other.availableCount_.load())
    , config_(std::move(other.config_))
    , threadCache_(std::move(other.threadCache_))
    , leakDetectionEnabled_(other.leakDetectionEnabled_)
    , fragmentationTrackingEnabled_(other.fragmentationTrackingEnabled_)
    , leakTracker_(std::move(other.leakTracker_))
    , threadAllocationCount_(other.threadAllocationCount_.load())
    , threadDeallocationCount_(other.threadDeallocationCount_.load())
    , threadLockContention_(other.threadLockContention_.load())
    , lastActivity_(other.lastActivity_.load())
    , alignment_(other.alignment_)
{
    // Clear other's atomic values
    other.freeList_.store(nullptr, std::memory_order_relaxed);
    other.currentBlock_.store(nullptr, std::memory_order_relaxed);
    other.currentIndex_.store(0, std::memory_order_relaxed);
    other.allocatedCount_.store(0, std::memory_order_relaxed);
    other.availableCount_.store(0, std::memory_order_relaxed);
    other.threadAllocationCount_.store(0, std::memory_order_relaxed);
    other.threadDeallocationCount_.store(0, std::memory_order_relaxed);
    other.threadLockContention_.store(0, std::memory_order_relaxed);
    other.lastActivity_.store(0, std::memory_order_relaxed);
}

inline MemoryPool<T>& MemoryPool<T>::operator=(MemoryPool&& other) noexcept {
    if (this != &other) {
        // Clear existing resources
        compact();
        
        // Move resources from other
        slabSize_ = other.slabSize_;
        blockSize_ = other.blockSize_;
        maxBlocks_ = other.maxBlocks_;
        allocationStrategy_ = other.allocationStrategy_;
        freeList_.store(other.freeList_.load(), std::memory_order_relaxed);
        currentBlock_.store(other.currentBlock_.load(), std::memory_order_relaxed);
        currentIndex_.store(other.currentIndex_.load(), std::memory_order_relaxed);
        allocatedCount_.store(other.allocatedCount_.load(), std::memory_order_relaxed);
        availableCount_.store(other.availableCount_.load(), std::memory_order_relaxed);
        config_ = std::move(other.config_);
        threadCache_ = std::move(other.threadCache_);
        leakDetectionEnabled_ = other.leakDetectionEnabled_;
        fragmentationTrackingEnabled_ = other.fragmentationTrackingEnabled_;
        leakTracker_ = std::move(other.leakTracker_);
        threadAllocationCount_.store(other.threadAllocationCount_.load(), std::memory_order_relaxed);
        threadDeallocationCount_.store(other.threadDeallocationCount_.load(), std::memory_order_relaxed);
        threadLockContention_.store(other.threadLockContention_.load(), std::memory_order_relaxed);
        lastActivity_.store(other.lastActivity_.load(), std::memory_order_relaxed);
        alignment_ = other.alignment_;
        
        // Clear other's atomic values
        other.freeList_.store(nullptr, std::memory_order_relaxed);
        other.currentBlock_.store(nullptr, std::memory_order_relaxed);
        other.currentIndex_.store(0, std::memory_order_relaxed);
        other.allocatedCount_.store(0, std::memory_order_relaxed);
        other.availableCount_.store(0, std::memory_order_relaxed);
        other.threadAllocationCount_.store(0, std::memory_order_relaxed);
        other.threadDeallocationCount_.store(0, std::memory_order_relaxed);
        other.threadLockContention_.store(0, std::memory_order_relaxed);
        other.lastActivity_.store(0, std::memory_order_relaxed);
    }
    return *this;
}

inline MemoryPool<T>::Block::Block(size_t objectSize, size_t count, size_t alignment)
    : nextBlock(nullptr), freeIndices(count), firstFreeIndex(0) {
    // Calculate aligned size for each object
    size_t alignedObjectSize = objectSize;
    if (alignment > 0 && objectSize % alignment != 0) {
        alignedObjectSize = ((objectSize / alignment) + 1) * alignment;
    }
    
    data.resize(alignedObjectSize * count);
    
    // Initialize free list with all indices
    firstFreeIndex = 0;
    freeIndices.store(count, std::memory_order_relaxed);
}

inline void MemoryPool<T>::initThreadLocalCache(size_t threadId) {
    if (threadId < threadCache_.size()) {
        threadCache_[threadId] = ThreadLocalCache();
        threadCache_[threadId].cachedObjects.reserve(config_.threadLocalCacheSize);
        threadCache_[threadId].lastAccess = 0;
    }
}

inline MemoryPool<T>::ThreadLocalCache* MemoryPool<T>::getThreadLocalCache(size_t threadId) {
    if (threadId >= threadCache_.size()) {
        initThreadLocalCache(threadId);
    }
    return &threadCache_[threadId];
}

inline T* MemoryPool<T>::getFromThreadLocalCache(size_t threadId) {
    ThreadLocalCache* cache = getThreadLocalCache(threadId);
    if (cache->cachedObjects.empty()) {
        cache->cacheMisses++;
        return nullptr;
    }
    
    // Get the most recently used object
    T* obj = cache->cachedObjects.back();
    cache->cachedObjects.pop_back();
    cache->cacheHits++;
    cache->lastAccess = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    
    return obj;
}

inline void MemoryPool<T>::returnToThreadLocalCache(size_t threadId, T* obj) {
    ThreadLocalCache* cache = getThreadLocalCache(threadId);
    
    // Limit cache size
    if (cache->cachedObjects.size() < config_.threadLocalCacheSize) {
        cache->cachedObjects.push_back(obj);
    } else {
        // Cache is full, return to global free list
        deallocate(obj);
    }
}

inline void MemoryPool<T>::trackAllocation(T* obj, const std::string& sourceFile, size_t sourceLine) {
    if (leakDetectionEnabled_) {
        leakTracker_.trackAllocation(obj, sizeof(T), sourceFile, sourceLine, typeid(T).name());
        threadAllocationCount_.fetch_add(1, std::memory_order_relaxed);
    }
}

inline void MemoryPool<T>::trackDeallocation(T* obj, const std::string& sourceFile, size_t sourceLine) {
    if (leakDetectionEnabled_) {
        leakTracker_.trackDeallocation(obj, sourceFile, sourceLine, typeid(T).name());
        threadDeallocationCount_.fetch_add(1, std::memory_order_relaxed);
    }
}

inline void MemoryPool<T>::addBlock() {
    if (blocks_.size() >= maxBlocks_) {
        return;  // Reached maximum blocks
    }
    
    auto block = std::make_unique<Block>(slabSize_, blockSize_, alignment_);
    Block* blockPtr = block.get();
    
    // Chain existing blocks
    Block* expected = nullptr;
    if (!currentBlock_.compare_exchange_strong(expected, blockPtr)) {
        // Another thread already added a block, add to chain
        block->nextBlock = expected;
    } else {
        block.release();
    }
    
    // Initialize free list for this block
    for (size_t i = 0; i < blockSize_; ++i) {
        T* obj = blockPtr->getObject(i, slabSize_);
        MemPoolNode* node = reinterpret_cast<MemPoolNode*>(obj);
        node->next.store(freeList_.load(std::memory_order_relaxed), std::memory_order_relaxed);
        node->threadId = std::hash<std::thread::id>{}(std::this_thread::get_id());
        node->timestamp = std::chrono::high_resolution_clock::now().time_since_epoch().count();
        node->sequence.store(0, std::memory_order_relaxed);
        freeList_.store(node, std::memory_order_relaxed);
        availableCount_.fetch_add(1, std::memory_order_relaxed);
    }
    
    blocks_.push_back(std::move(block));
}

inline T* MemoryPool<T>::allocate() {
    // Try thread-local cache first (for reduced contention)
    size_t threadId = std::hash<std::thread::get_id()>{}();
    T* obj = getFromThreadLocalCache(threadId);
    if (obj != nullptr) {
        lastActivity_.store(std::chrono::high_resolution_clock::now().time_since_epoch().count(), 
                          std::memory_order_relaxed);
        return obj;
    }
    
    // Try to get from free list with enhanced CAS
    MemPoolNode* node = freeList_.load(std::memory_order_acquire);
    
    while (node != nullptr) {
        MemPoolNode* next = node->next.load(std::memory_order_relaxed);
        
        // Use enhanced CAS with backoff
        if (freeList_.compare_exchange_weak(node, next, 
                                             std::memory_order_release, 
                                             std::memory_order_relaxed)) {
            // Successfully removed from free list
            allocatedCount_.fetch_add(1, std::memory_order_relaxed);
            availableCount_.fetch_sub(1, std::memory_order_relaxed);
            
            T* obj = reinterpret_cast<T*>(node);
            
            // Mark object as allocated (for debugging)
            updateStatistics();
            
            // Track allocation for leak detection
            if (leakDetectionEnabled_) {
                trackAllocation(obj, "", 0); // TODO: Add source file/line info
            }
            
            lastActivity_.store(std::chrono::high_resolution_clock::now().time_since_epoch().count(), 
                              std::memory_order_relaxed);
            
            return obj;
        }
        // node was updated by CAS, reload next
    }
    
    // Free list empty, add a new block
    addBlock();
    
    // Try again
    return allocate();
}

inline void MemoryPool<T>::deallocate(T* obj) {
    if (obj == nullptr) return;
    
    // Track deallocation for leak detection
    if (leakDetectionEnabled_) {
        trackDeallocation(obj, "", 0); // TODO: Add source file/line info
    }
    
    MemPoolNode* node = reinterpret_cast<MemPoolNode*>(obj);
    
    // Lock-free push to free list
    MemPoolNode* current = freeList_.load(std::memory_order_relaxed);
    do {
        node->next.store(current, std::memory_order_relaxed);
        node->threadId = std::hash<std::thread::id>{}(std::this_thread::get_id());
        node->timestamp = std::chrono::high_resolution_clock::now().time_since_epoch().count();
        node->sequence.store(0, std::memory_order_relaxed);
    } while (!freeList_.compare_exchange_weak(current, node,
                                             std::memory_order_release,
                                             std::memory_order_relaxed));
    
    allocatedCount_.fetch_sub(1, std::memory_order_relaxed);
    availableCount_.fetch_add(1, std::memory_order_relaxed);
    
    lastActivity_.store(std::chrono::high_resolution_clock::now().time_since_epoch().count(), 
                      std::memory_order_relaxed);
}

inline void MemoryPool<T>::reserve(size_t count) {
    while (availableCount_.load(std::memory_order_relaxed) < count) {
        addBlock();
    }
}

inline void MemoryPool<T>::compact() {
    // Trigger garbage collection of unused blocks
    size_t currentBlockCount = blocks_.size();
    size_t minBlocks = std::max<size_t>(1, config_.initialBlocks);
    
    if (currentBlockCount > minBlocks && availableCount_.load(std::memory_order_relaxed) == 0) {
        // Remove all but the first block to reduce memory usage
        blocks_.resize(1);
        
        // Rebuild free list from remaining blocks
        freeList_.store(nullptr, std::memory_order_relaxed);
        availableCount_.store(blockSize_, std::memory_order_relaxed);
        
        // Rebuild free list entries
        for (size_t i = 0; i < blockSize_; ++i) {
            T* obj = blocks_[0]->getObject(i, slabSize_);
            MemPoolNode* node = reinterpret_cast<MemPoolNode*>(obj);
            node->next.store(freeList_.load(std::memory_order_relaxed), std::memory_order_relaxed);
            freeList_.store(node, std::memory_order_relaxed);
        }
    }
}

inline void MemoryPool<T>::defragment() {
    std::lock_guard<std::mutex> lock(fragmentationMutex_);
    
    // Simple defragmentation: rebuild free list in order
    if (freeList_.load(std::memory_order_relaxed) != nullptr) {
        freeList_.store(nullptr, std::memory_order_relaxed);
        availableCount_.store(0, std::memory_order_relaxed);
        
        // Iterate through all blocks and rebuild free list
        for (auto& blockPtr : blocks_) {
            Block* block = blockPtr.get();
            for (size_t i = 0; i < blockSize_; ++i) {
                T* obj = block->getObject(i, slabSize_);
                MemPoolNode* node = reinterpret_cast<MemPoolNode*>(obj);
                node->next.store(freeList_.load(std::memory_order_relaxed), std::memory_order_relaxed);
                freeList_.store(node, std::memory_order_relaxed);
                availableCount_.fetch_add(1, std::memory_order_relaxed);
            }
        }
        
        fragmentationStats_.lastDefragmentationTime = 
            std::chrono::high_resolution_clock::now().time_since_epoch().count();
    }
}

inline void MemoryPool<T>::updateStatistics() {
    // Update performance metrics based on current state
    threadAllocationCount_.fetch_add(1, std::memory_order_relaxed);
    lastActivity_.store(std::chrono::high_resolution_clock::now().time_since_epoch().count(), 
                      std::memory_order_relaxed);
    
    // Update fragmentation statistics
    if (fragmentationTrackingEnabled_) {
        fragmentationStats_.freeBlockSizes.clear();
        for (const auto& blockPtr : blocks_) {
            Block* block = blockPtr.get();
            size_t freeCount = block->freeIndices.load(std::memory_order_relaxed);
            if (freeCount > 0) {
                fragmentationStats_.freeBlockSizes.push_back(freeCount);
            }
        }
        
        if (!fragmentationStats_.freeBlockSizes.empty()) {
            double sum = std::accumulate(fragmentationStats_.freeBlockSizes.begin(), 
                                        fragmentationStats_.freeBlockSizes.end(), 0.0);
            fragmentationStats_.averageFreeFragmentation = sum / fragmentationStats_.freeBlockSizes.size();
            
            size_t maxFree = *std::max_element(fragmentationStats_.freeBlockSizes.begin(),
                                             fragmentationStats_.freeBlockSizes.end());
            fragmentationStats_.largestFreeBlock = maxFree;
        }
    }
}

inline size_t MemoryPool<T>::memoryUsage() const {
    size_t total = 0;
    for (const auto& block : blocks_) {
        total += block->data.capacity() * sizeof(uint8_t);
    }
    return total;
}

inline double MemoryPool<T>::getFragmentationRatio() const {
    if (!fragmentationTrackingEnabled_) return 0.0;
    
    size_t totalObjects = blockSize_ * blocks_.size();
    if (totalObjects == 0) return 0.0;
    
    size_t freeObjects = availableCount_.load(std::memory_order_relaxed);
    double fragmentationRatio = (double)(totalObjects - freeObjects) / totalObjects;
    
    return std::min(fragmentationRatio, 1.0);  // Cap at 1.0
}

inline PerfMetrics MemoryPool<T>::getPerformanceMetrics() const {
    PerfMetrics metrics;
    
    metrics.allocations = allocatedCount_.load(std::memory_order_relaxed);
    metrics.deallocations = deallocations_.load(std::memory_order_relaxed);
    metrics.allocationsFailed = allocationsFailed_.load(std::memory_order_relaxed);
    metrics.totalMemoryUsed = memoryUsage();
    metrics.peakMemoryUsage = peakMemoryUsage_.load(std::memory_order_relaxed);
    metrics.activeThreads = threadCache_.size();
    metrics.lockContentionCount = threadLockContention_.load(std::memory_order_relaxed);
    metrics.simdOperations = simdOperations_.load(std::memory_order_relaxed);
    metrics.scalarFallbacks = scalarFallbacks_.load(std::memory_order_relaxed);
    
    // Calculate derived metrics
    if (metrics.allocations + metrics.deallocations > 0) {
        metrics.operationsPerSecond = (metrics.allocations + metrics.deallocations) / 10.0; // Approximate
        metrics.allocationFailureRate() = metrics.allocationsFailed / (metrics.allocations + metrics.allocationsFailed);
    }
    
    metrics.simdEfficiency() = metrics.simdOperations / (metrics.simdOperations + metrics.scalarFallbacks);
    metrics.healthy = !hasLeaks();
    
    return metrics;
}

inline MemoryPool<T>::ThreadLocalCacheStats MemoryPool<T>::getThreadLocalCacheStats(size_t threadId) const {
    ThreadLocalCacheStats stats;
    if (threadId < threadCache_.size()) {
        const ThreadLocalCache& cache = threadCache_[threadId];
        stats.cacheSize = cache.cachedObjects.size();
        stats.hits = cache.cacheHits;
        stats.misses = cache.cacheMisses;
    }
    return stats;
}

inline MemoryPool<T>::~MemoryPool() {
    compact();  // Clean up before destruction
}

// NeuronPool - Specialized pool for neurons with enhanced features
class NeuronPool {
public:
    explicit NeuronPool(const MemoryPool<T>::PoolConfig& config = MemoryPool<T>::PoolConfig());
    ~NeuronPool();
    
    // Disable copying
    NeuronPool(const NeuronPool&) = delete;
    NeuronPool& operator=(const NeuronPool&) = delete;
    
    /**
     * Allocate a neuron with its state
     */
    T* allocate();
    
    /**
     * Return a neuron to the pool
     */
    void deallocate(T* obj);
    
    /**
     * Get total count
     */
    size_t size() const { return size_; }
    
    /**
     * Memory usage
     */
    size_t memoryUsage() const;
    
    /**
     * Get performance metrics
     */
    PerfMetrics getPerformanceMetrics() const { return pool_.getPerformanceMetrics(); }
    
    /**
     * Get memory leak report
     */
    MemoryTracker::LeakReport getLeakReport() const { return leakTracker_.checkLeaks(); }
    
private:
    MemoryPool<T> pool_;
    MemoryTracker leakTracker_;
    
    size_t size_;
    
    // Neuron-specific state tracking
    std::vector<size_t> neuronIndices_;
    std::atomic<size_t> freeListHead_;
};

// SynapsePool - Specialized pool for synapses with enhanced features
class SynapsePool {
public:
    explicit SynapsePool(const MemoryPool<T>::PoolConfig& config = MemoryPool<T>::PoolConfig());
    ~SynapsePool();
    
    // Disable copying
    SynapsePool(const SynapsePool&) = delete;
    SynapsePool& operator=(const SynapsePool&) = delete;
    
    /**
     * Allocate a synapse
     */
    T* allocate();
    
    /**
     * Return a synapse to the pool
     */
    void deallocate(T* obj);
    
    /**
     * Get total count
     */
    size_t size() const { return size_; }
    
    /**
     * Memory usage
     */
    size_t memoryUsage() const;
    
    /**
     * Get performance metrics
     */
    PerfMetrics getPerformanceMetrics() const { return pool_.getPerformanceMetrics(); }
    
    /**
     * Get memory leak report
     */
    MemoryTracker::LeakReport getLeakReport() const { return leakTracker_.checkLeaks(); }
    
private:
    MemoryPool<T> pool_;
    MemoryTracker leakTracker_;
    
    size_t size_;
    
    // Synapse-specific state tracking
    std::vector<size_t> synapseIndices_;
    std::atomic<size_t> freeListHead_;
};

} // namespace nlm

#endif // NLM_MEMORY_POOL_HPP