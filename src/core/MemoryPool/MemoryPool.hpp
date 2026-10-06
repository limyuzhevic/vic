// Memory pool implementation for NLM performance optimization
#ifndef NLM_MEMORY_POOL_H
#define NLM_MEMORY_POOL_H

#include <vector>
#include <cstddef>
#include <memory>
#include <mutex>
#include <atomic>
#include <algorithm>

namespace nlm {

// Memory pool for efficient allocation/deallocation of fixed-size objects
// Reduces memory fragmentation and improves cache locality

template<typename T, size_t PoolSize = 1024, size_t Alignment = 16>
class MemoryPool {
public:
    MemoryPool() : currentBlock_(nullptr), currentOffset_(0), totalAllocated_(0), freeCount_(0) {
        // Pre-allocate first block
        addBlock();
    }
    
    ~MemoryPool() {
        // Clean up all blocks
        Block* current = blocks_.load(std::memory_order_acquire);
        while (current) {
            Block* next = current->next;
            delete current;
            current = next;
        }
    }
    
    // Allocate an object from the pool
    T* allocate() {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (currentOffset_ >= PoolSize * sizeof(T)) {
            // Current block is full, get next block
            addBlock();
            currentOffset_ = 0;
        }
        
        T* ptr = reinterpret_cast<T*>(currentBlock_->data + currentOffset_);
        currentOffset_ += sizeof(T);
        totalAllocated_++;
        
        return ptr;
    }
    
    // Deallocate an object
    void deallocate(T* ptr) {
        // In a simple pool implementation, we can't free individual objects
        // They remain allocated until the pool is destroyed
        // Track free count for statistics
        freeCount_++;
    }
    
    // Construct an object in-place
    template<typename... Args>
    T* construct(Args&&... args) {
        T* ptr = allocate();
        try {
            new (ptr) T(std::forward<Args>(args)...);
            return ptr;
        } catch (...) {
            deallocate(ptr);
            throw;
        }
    }
    
    // Destroy and deallocate an object
    void destroy(T* ptr) {
        if (ptr) {
            ptr->~T();
            deallocate(ptr);
        }
    }
    
    // Get current usage statistics
    size_t getCurrentUsage() const {
        return totalAllocated_ - freeCount_;
    }
    
    // Get total allocations
    size_t getTotalAllocated() const {
        return totalAllocated_;
    }
    
    // Get number of free blocks
    size_t getFreeCount() const {
        return freeCount_;
    }
    
    // Get number of blocks
    size_t getBlockCount() const {
        size_t count = 0;
        Block* current = blocks_.load(std::memory_order_acquire);
        while (current) {
            count++;
            current = current->next;
        }
        return count;
    }
    
    // Memory statistics
    struct Statistics {
        size_t totalAllocated = 0;
        size_t freeCount = 0;
        size_t blockCount = 0;
        size_t currentUsage() const { return totalAllocated - freeCount; }
    };
    
    Statistics getStatistics() const {
        Statistics stats;
        stats.totalAllocated = totalAllocated_;
        stats.freeCount = freeCount_;
        stats.blockCount = getBlockCount();
        return stats;
    }
    
    // Check if pool is using excessive memory
    bool isMemoryEfficient() const {
        Statistics stats = getStatistics();
        return stats.currentUsage() < PoolSize * 0.9; // Less than 90% used
    }
    
private:
    struct Block {
        alignas(Alignment) unsigned char data[PoolSize * sizeof(T)];
        Block* next;
        
        Block() : next(nullptr) {}
    };
    
    void addBlock() {
        Block* newBlock = new Block();
        newBlock->next = blocks_.load(std::memory_order_relaxed);
        blocks_.store(newBlock, std::memory_order_release);
    }
    
    std::atomic<Block*> blocks_;
    mutable std::mutex mutex_;
    Block* currentBlock_;
    size_t currentOffset_;
    std::atomic<size_t> totalAllocated_;
    std::atomic<size_t> freeCount_;
};

// Typed memory pool for common types
using NeuronPool = MemoryPool<Neuron, 2048>;
using SynapsePool = MemoryPool<Synapse, 4096>;
using PopulationPool = MemoryPool<NeuralPopulation, 512>;
using RegionPool = MemoryPool<NeuralRegion, 256>;
using SpikeEventPool = MemoryPool<SpikeEvent, 2048>;

// Memory pool manager for managing multiple pools
class MemoryPoolManager {
public:
    static MemoryPoolManager& getInstance() {
        static MemoryPoolManager instance;
        return instance;
    }
    
    // Get or create a neuron pool
    NeuronPool& getNeuronPool() {
        std::lock_guard<std::mutex> lock(mutex_);
        return neuronPool_;
    }
    
    // Get or create a synapse pool
    SynapsePool& getSynapsePool() {
        std::lock_guard<std::mutex> lock(mutex_);
        return synapsePool_;
    }
    
    // Get or create a population pool
    PopulationPool& getPopulationPool() {
        std::lock_guard<std::mutex> lock(mutex_);
        return populationPool_;
    }
    
    // Get or create a region pool
    RegionPool& getRegionPool() {
        std::lock_guard<std::mutex> lock(mutex_);
        return regionPool_;
    }
    
    // Get or create a spike event pool
    SpikeEventPool& getSpikeEventPool() {
        std::lock_guard<std::mutex> lock(mutex_);
        return spikeEventPool_;
    }
    
    // Report memory pool statistics
    void reportStatistics() {
        std::cout << "=== Memory Pool Statistics ===" << std::endl;
        std::cout << "Neuron pool: " << neuronPool_.getStatistics().currentUsage() << "/" << neuronPool_.getStatistics().totalAllocated << std::endl;
        std::cout << "Synapse pool: " << synapsePool_.getStatistics().currentUsage() << "/" << synapsePool_.getStatistics().totalAllocated << std::endl;
        std::cout << "Population pool: " << populationPool_.getStatistics().currentUsage() << "/" << populationPool_.getStatistics().totalAllocated << std::endl;
        std::cout << "Region pool: " << regionPool_.getStatistics().currentUsage() << "/" << regionPool_.getStatistics().totalAllocated << std::endl;
        std::cout << "Spike event pool: " << spikeEventPool_.getStatistics().currentUsage() << "/" << spikeEventPool_.getStatistics().totalAllocated << std::endl;
        
        std::cout << std::endl;
        std::cout << "Total pools: " << getPoolCount() << std::endl;
    }
    
    // Get total number of pools
    size_t getPoolCount() const {
        return 5; // All pools
    }
    
    // Force cleanup of all pools
    void cleanup() {
        std::lock_guard<std::mutex> lock(mutex_);
        // In this implementation, pools are singletons, so cleanup is automatic
        // Just report that cleanup would occur
        std::cout << "Memory pool cleanup would occur here" << std::endl;
    }
    
private:
    MemoryPoolManager() {}
    
    NeuronPool neuronPool_;
    SynapsePool synapsePool_;
    PopulationPool populationPool_;
    RegionPool regionPool_;
    SpikeEventPool spikeEventPool_;
    mutable std::mutex mutex_;
};

// RAII wrapper for memory pool allocations
template<typename T, typename Pool>
class PoolAllocation {
public:
    PoolAllocation(Pool& pool) : pool_(pool), ptr_(nullptr) {}
    
    template<typename... Args>
    PoolAllocation(Pool& pool, Args&&... args) : pool_(pool), ptr_(nullptr) {
        ptr_ = pool_.construct(std::forward<Args>(args)...);
    }
    
    ~PoolAllocation() {
        if (ptr_) {
            ptr_->~T();
            pool_.deallocate(ptr_);
        }
    }
    
    T* get() const { return ptr_; }
    T* release() {
        T* temp = ptr_;
        ptr_ = nullptr;
        return temp;
    }
    
    void reset() {
        if (ptr_) {
            ptr_->~T();
            pool_.deallocate(ptr_);
            ptr_ = nullptr;
        }
    }
    
    // Allow access to the underlying pointer
    operator T*() { return ptr_; }
    operator const T*() const { return ptr_; }
    
    T& operator*() { return *ptr_; }
    const T& operator*() const { return *ptr_; }
    
    T* operator->() { return ptr_; }
    const T* operator->() const { return ptr_; }
    
    bool valid() const { return ptr_ != nullptr; }
    
private:
    Pool& pool_;
    T* ptr_;
};

// Convenience functions for common allocations
namespace MemoryAllocators {
    inline auto allocateNeuron(MemoryPoolManager& manager) {
        return PoolAllocation<Neuron, NeuronPool>(manager.getNeuronPool());
    }
    
    inline auto allocateSynapse(MemoryPoolManager& manager) {
        return PoolAllocation<Synapse, SynapsePool>(manager.getSynapsePool());
    }
    
    inline auto allocatePopulation(MemoryPoolManager& manager) {
        return PoolAllocation<NeuralPopulation, PopulationPool>(manager.getPopulationPool());
    }
    
    inline auto allocateRegion(MemoryPoolManager& manager) {
        return PoolAllocation<NeuralRegion, RegionPool>(manager.getRegionPool());
    }
    
    inline auto allocateSpikeEvent(MemoryPoolManager& manager) {
        return PoolAllocation<SpikeEvent, SpikeEventPool>(manager.getSpikeEventPool());
    }
}

} // namespace nlm

#endif // NLM_MEMORY_POOL_H
