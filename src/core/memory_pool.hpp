#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>
#include <memory>
#include <atomic>
#include <optional>

namespace nlm {

/**
 * MemoryPool - Lock-free memory pool for neural components
 * 
 * Template class for efficient object pooling to reduce allocation overhead
 * during neural network simulation. Uses slab allocation for better cache locality
 * and reduced memory fragmentation.
 * 
 * @tparam T Type of objects to pool
 */
template<typename T>
class MemoryPool {
public:
    /**
     * Create a memory pool
     * 
     * @param blockSize Number of objects per memory block
     * @param initialBlocks Number of blocks to pre-allocate
     */
    explicit MemoryPool(size_t blockSize = 1024, size_t initialBlocks = 4)
        : slabSize_(sizeof(T))
        , blockSize_(blockSize)
        , freeList_(nullptr)
        , currentBlock_(nullptr)
        , currentIndex_(0)
        , allocatedCount_(0)
        , availableCount_(0) {
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
    
    ~MemoryPool() {
        // Blocks are automatically freed via unique_ptr
    }
    
    // Disable copying
    MemoryPool(const MemoryPool&) = delete;
    MemoryPool& operator=(const MemoryPool&) = delete;
    
    /**
     * Allocate an object from the pool
     * Lock-free operation
     */
    T* allocate() {
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
    
    /**
     * Return an object to the pool
     * Lock-free operation
     */
    void deallocate(T* obj) {
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
    
    /**
     * Check if pool is empty
     */
    bool empty() const { return freeList_.load(std::memory_order_relaxed) == nullptr; }
    
    /**
     * Get number of allocated objects
     */
    size_t size() const { return allocatedCount_.load(std::memory_order_relaxed); }
    
    /**
     * Get number of available objects
     */
    size_t availableCount() const { return availableCount_.load(std::memory_order_relaxed); }
    
    /**
     * Total memory used by pool (bytes)
     */
    size_t memoryUsage() const {
        size_t total = 0;
        for (const auto& block : blocks_) {
            total += block->data.capacity() * sizeof(uint8_t);
        }
        return total;
    }
    
    /**
     * Pre-allocate objects
     */
    void reserve(size_t count) {
        while (availableCount_.load(std::memory_order_relaxed) < count) {
            addBlock();
        }
    }
    
    /**
     * Clear the pool (mark all objects as free)
     */
    void clear() {
        // Mark all objects in all blocks as free
        for (auto& block : blocks_) {
            Block* b = block.get();
            for (size_t i = 0; i < blockSize_; ++i) {
                T* obj = b->getObject(i, slabSize_);
                MemPoolNode* node = reinterpret_cast<MemPoolNode*>(obj);
                node->next.store(freeList_.load(std::memory_order_relaxed), std::memory_order_relaxed);
            }
        }
        
        // Reset statistics
        freeList_.store(nullptr, std::memory_order_relaxed);
        allocatedCount_.store(0, std::memory_order_relaxed);
        availableCount_.store(blockSize_ * blocks_.size(), std::memory_order_relaxed);
    }
    
private:
    // Memory block containing objects
    struct Block {
        std::vector<uint8_t> data;
        Block* nextBlock;
        
        explicit Block(size_t objectSize, size_t count)
            : data(objectSize * count), nextBlock(nullptr) {}
        
        T* getObject(size_t index, size_t objectSize) {
            return reinterpret_cast<T*>(data.data() + index * objectSize);
        }
    };
    
    // Add a new block to the pool
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
    
    // Statistics
    std::atomic<size_t> allocatedCount_;
    std::atomic<size_t> availableCount_;
};

// Forward declaration for use in specialized pools
struct MemPoolNode {
    std::atomic<MemPoolNode*> next;
    
    MemPoolNode() : next(nullptr) {}
};

} // namespace nlm
