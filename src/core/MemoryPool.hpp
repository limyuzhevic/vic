// Simple memory pool for NLM brain simulation
// Provides efficient allocation/deallocation for frequently created objects

#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <algorithm>
#include <vector>
#include <mutex>

namespace nlm {

/**
 * @brief Memory pool for efficient object allocation
 * 
 * Allocates objects in fixed-size blocks to reduce heap fragmentation
 * and improve allocation/deallocation performance for frequently created
 * objects like neurons, synapses, and spike events in the NLM brain.
 */
template<typename T, size_t BLOCK_SIZE = 1024>
class SimpleMemoryPool {
public:
    static constexpr size_t BLOCK_SIZE_LIMIT = BLOCK_SIZE;
    
    SimpleMemoryPool() noexcept : head_(nullptr), free_list_(nullptr), size_(0) {}
    
    ~SimpleMemoryPool() {
        MemoryBlock* current = head_;
        while (current) {
            MemoryBlock* next = current->next;
            delete current;
            current = next;
        }
    }
    
    // Disable copying and assignment
    SimpleMemoryPool(const SimpleMemoryPool&) = delete;
    SimpleMemoryPool& operator=(const SimpleMemoryPool&) = delete;
    
    // Allocate object (uses default constructor)
    T* allocate() {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (free_list_) {
            // Use free list
            T* result = free_list_;
            free_list_ = free_list_->next;
            return result;
        }
        
        // Need new block
        return allocateNewBlock();
    }
    
    // Allocate with custom constructor
    template<typename... Args>
    T* allocate_and_construct(Args&&... args) {
        T* ptr = allocate();
        new(ptr) T(std::forward<Args>(args)...);
        return ptr;
    }
    
    // Deallocate object (calls destructor)
    void deallocate(T* ptr) noexcept {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (!ptr) return;
        
        ptr->~T();
        ptr->next = free_list_;
        free_list_ = ptr;
    }
    
    // Get current allocated count
    size_t size() const noexcept { return size_; }
    
    // Check if pool has available objects
    bool available() const noexcept { return free_list_ != nullptr; }
    
    // Clear all objects
    void clear() noexcept {
        std::lock_guard<std::mutex> lock(mutex_);
        
        // Call destructors on all objects in blocks
        MemoryBlock* current = head_;
        while (current) {
            for (size_t i = 0; i < current->count; ++i) {
                T* ptr = &current->objects[i];
                ptr->~T();
            }
            current = current->next;
        }
        
        // Clear free list
        free_list_ = nullptr;
        size_ = 0;
    }
    
    // Static instance access
    static SimpleMemoryPool& instance() {
        static SimpleMemoryPool pool;
        return pool;
    }
    
private:
    struct MemoryBlock {
        T objects[BLOCK_SIZE];
        MemoryBlock* next;
        size_t count;
        
        MemoryBlock() : next(nullptr), count(0) {}
    };
    
    T* allocateNewBlock() {
        MemoryBlock* block = new MemoryBlock();
        block->next = head_;
        head_ = block;
        
        // Initialize all objects in block
        for (size_t i = 0; i < BLOCK_SIZE; ++i) {
            new(&block->objects[i]) T();  // Default construct
            if (i == 0) {
                // Link all objects to form free list
                for (size_t j = 0; j < BLOCK_SIZE - 1; ++j) {
                    block->objects[j].next = &block->objects[j + 1];
                }
                block->objects[BLOCK_SIZE - 1].next = nullptr;
                free_list_ = &block->objects[0];
                block->count = BLOCK_SIZE;
                size_ += BLOCK_SIZE;
                return &block->objects[0];
            }
        }
        
        return nullptr;
    }
    
    // Thread safety
    std::mutex mutex_;
    
    // Block management
    MemoryBlock* head_;
    T* free_list_;
    
    // Statistics
    size_t size_;
};

// Convenience typedefs for NLM objects
using NeuronPool = SimpleMemoryPool<Neuron, 2048>;
using SynapsePool = SimpleMemoryPool<Synapse, 4096>;
using SpikeEventPool = SimpleMemoryPool<SpikeEvent, 1024>;

// Global pool instances
extern NeuronPool neuronPool;
extern SynapsePool synapsePool;
extern SpikeEventPool spikeEventPool;

// Helper functions
template<typename T, typename Pool>
T* allocate_object() {
    return Pool::instance().allocate();
}

template<typename T, typename Pool>
void deallocate_object(T* ptr) {
    Pool::instance().deallocate(ptr);
}

// Memory pool statistics
struct MemoryPoolStats {
    size_t neuronPoolSize = 0;
    size_t neuronPoolCapacity = 0;
    size_t synapsePoolSize = 0;
    size_t synapsePoolCapacity = 0;
    size_t spikeEventPoolSize = 0;
    size_t spikeEventPoolCapacity = 0;
    
    size_t totalAllocated() const {
        return neuronPoolSize + synapsePoolSize + spikeEventPoolSize;
    }
    
    size_t totalCapacity() const {
        return neuronPoolCapacity + synapsePoolCapacity + spikeEventPoolCapacity;
    }
};

// Get memory pool statistics
MemoryPoolStats getMemoryPoolStats();

// Clear all memory pools
void clearMemoryPools();

} // namespace nlm