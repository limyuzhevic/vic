// MemoryPoolBase - Generic lock-free slab allocator
// 
// Template MemoryPool for fixed-size objects with lock-free allocation.
// Used as base for specialized pools (NeuronPool, SynapsePool).
// 
// Features:
// - Lock-free allocation and deallocation
// - Pre-allocated memory blocks
// - Memory-efficient slab allocation
// - Thread-safe operations
// 
// Biological motivation:
// - Like biological neural systems that maintain fixed numbers of neurons/synapses
// - Reduces allocation overhead similar to how biological systems don't constantly create/destroy neurons

#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>
#include <memory>
#include <atomic>

namespace nlm {

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
class MemoryPoolBase {
public:
    /**
     * Create a memory pool
     * @param blockSize Number of objects per memory block
     * @param initialBlocks Number of blocks to pre-allocate
     */
    explicit MemoryPoolBase(size_t blockSize = 1024, size_t initialBlocks = 4);
    
    ~MemoryPoolBase();
    
    // Disable copying
    MemoryPoolBase(const MemoryPoolBase&) = delete;
    MemoryPoolBase& operator=(const MemoryPoolBase&) = delete;
    
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
MemoryPoolBase<T>::Block::Block(size_t objectSize, size_t count)
    : data(objectSize * count)
    , nextBlock(nullptr)
{
    // Data is left uninitialized for performance
}

template<typename T>
MemoryPoolBase<T>::MemoryPoolBase(size_t blockSize, size_t initialBlocks)
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
MemoryPoolBase<T>::~MemoryPoolBase() {
    // Blocks are automatically freed via unique_ptr
}

template<typename T>
void MemoryPoolBase<T>::addBlock() {
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
T* MemoryPoolBase<T>::allocate() {
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
void MemoryPoolBase<T>::deallocate(T* obj) {
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
size_t MemoryPoolBase<T>::memoryUsage() const {
    size_t total = 0;
    for (const auto& block : blocks_) {
        total += block->data.capacity() * sizeof(uint8_t);
    }
    return total;
}

template<typename T>
void MemoryPoolBase<T>::reserve(size_t count) {
    while (availableCount_.load(std::memory_order_relaxed) < count) {
        addBlock();
    }
}

} // namespace nlm
