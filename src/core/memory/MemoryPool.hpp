#pragma once

/**
 * MemoryPool - Pre-allocated memory pool for fast object allocation
 * 
 * Provides a memory pool for pre-allocated objects to avoid frequent
 * dynamic memory allocation/deallocation overhead. Objects are allocated
 * from fixed-size blocks and returned to the pool when deallocated.
 * 
 * Features:
 * - Fixed-size object allocation
 * - Pre-allocated memory blocks
 * - Lock-free operations
 * - Memory tracking and statistics
 * - Automatic block expansion
 * 
 * Biological motivation:
 * - Like how neural systems maintain populations of neurons/synapses
 * - Reduces allocation overhead during intense simulation periods
 */

#include <cstddef>
#include <cstdint>
#include <vector>
#include <memory>
#include <atomic>
#include <algorithm>
#include <stdexcept>

namespace nlm {

/**
 * Memory pool for fixed-size objects
 * 
 * Template parameter T specifies the object type to allocate.
 * All objects must be trivially destructible for safety.
 */
template<typename T>
class MemoryPool {
public:
    /**
     * Create a memory pool
     * 
     * @param initialSize Initial number of objects to pre-allocate
     * @param maxBlockSize Maximum objects per memory block
     */
    explicit MemoryPool(size_t initialSize = 1024, size_t maxBlockSize = 4096)
        : maxBlockSize_(maxBlockSize)
        , objectsAllocated_(0)
        , blocksAllocated_(0)
        , allocatedCount_(0)
        , deallocatedCount_(0)
    {
        static_assert(std::is_trivially_destructible<T>::value, 
                     "T must be trivially destructible for MemoryPool");
        
        reserve(initialSize);
    }
    
    ~MemoryPool()
    {
        // Free all blocks
        for (auto& block : blocks_) {
            delete[] block.data;
        }
    }
    
    // Disable copying and assignment
    MemoryPool(const MemoryPool&) = delete;
    MemoryPool& operator=(const MemoryPool&) = delete;
    
    /**
     * Allocate an object from the pool
     * 
     * @return Pointer to allocated object (nullptr if pool empty and cannot expand)
     */
    T* allocate()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        
        // Check if we have available objects
        if (!freeList_.empty()) {
            T* obj = freeList_.back();
            freeList_.pop_back();
            allocatedCount_++;
            return obj;
        }
        
        // Try to expand pool
        if (objectsAllocated_ >= maxBlockSize_ * blocks_.size()) {
            // Pool full, try to add new block
            if (!expand()) {
                return nullptr; // Cannot expand
            }
        }
        
        // Get new object from current block
        T* obj = getNewObject();
        if (obj) {
            allocatedCount_++;
            return obj;
        }
        
        return nullptr;
    }
    
    /**
     * Return an object to the pool
     * 
     * @param obj Pointer to object to deallocate
     */
    void deallocate(T* obj)
    {
        if (!obj) return;
        
        std::lock_guard<std::mutex> lock(mutex_);
        
        // Add to free list
        freeList_.push_back(obj);
        deallocatedCount_++;
        
        // Optional: compact free list periodically
        if (freeList_.size() > maxBlockSize_) {
            std::sort(freeList_.begin(), freeList_.end());
            freeList_.erase(std::unique(freeList_.begin(), freeList_.end()), freeList_.end());
        }
    }
    
    /**
     * Check if pool is empty
     */
    bool empty() const
    {
        return allocatedCount_ == 0;
    }
    
    /**
     * Get number of allocated objects
     */
    size_t allocatedCount() const
    {
        return allocatedCount_;
    }
    
    /**
     * Get number of available objects
     */
    size_t availableCount() const
    {
        return freeList_.size();
    }
    
    /**
     * Get total objects allocated
     */
    size_t totalObjectsAllocated() const
    {
        return objectsAllocated_;
    }
    
    /**
     * Get number of memory blocks
     */
    size_t blockCount() const
    {
        return blocks_.size();
    }
    
    /**
     * Get memory usage in bytes
     */
    size_t memoryUsage() const
    {
        size_t total = 0;
        for (const auto& block : blocks_) {
            total += block->allocated * sizeof(T);
        }
        return total;
    }
    
    /**
     * Reserve additional objects
     */
    void reserve(size_t additionalObjects)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        
        size_t currentAllocated = objectsAllocated_;
        size_t neededBlocks = (currentAllocated + additionalObjects + maxBlockSize_ - 1) / maxBlockSize_;
        
        while (blocks_.size() < neededBlocks) {
            expand();
        }
    }
    
    /**
     * Clear all allocated objects (keep memory)
     */
    void clear()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        
        // Return all objects to free list
        for (auto& block : blocks_) {
            for (size_t i = 0; i < block->allocated; ++i) {
                freeList_.push_back(block->getObject(i));
            }
        }
        
        objectsAllocated_ = 0;
        allocatedCount_ = 0;
        // Note: deallocatedCount_ not reset to preserve history
    }
    
private:
    /**
     * Memory block containing objects
     */
    struct Block {
        T* data;          // Pointer to object array
        size_t allocated; // Number of allocated objects in this block
        size_t capacity;  // Total capacity of this block
        
        Block(size_t capacity) 
            : data(nullptr), allocated(0), capacity(capacity) {
            if (capacity > 0) {
                data = new T[capacity];
            }
        }
        
        ~Block() {
            delete[] data;
        }
        
        T* getObject(size_t index) {
            return &data[index];
        }
        
        const T* getObject(size_t index) const {
            return &data[index];
        }
    };
    
    /**
     * Get a new object from current block
     */
    T* getNewObject()
    {
        if (!currentBlock_) {
            if (blocks_.empty() || blocks_.back()->allocated >= blocks_.back()->capacity) {
                // Need new block
                if (!expand()) {
                    return nullptr;
                }
            }
            currentBlock_ = blocks_.back().get();
        }
        
        if (currentBlock_->allocated < currentBlock_->capacity) {
            T* obj = currentBlock_->getObject(currentBlock_->allocated++);
            objectsAllocated_++;
            return obj;
        }
        
        currentBlock_ = nullptr;
        return getNewObject();
    }
    
    /**
     * Expand pool with a new block
     */
    bool expand()
    {
        size_t newBlockSize = maxBlockSize_;
        
        // Calculate optimal block size based on current usage
        if (!blocks_.empty()) {
            float utilization = static_cast<float>(blocks_.back()->allocated) / blocks_.back()->capacity;
            if (utilization < 0.5f) {
                newBlockSize = maxBlockSize_ * 2;  // Grow larger if underutilized
            }
        }
        
        auto newBlock = std::make_unique<Block>(newBlockSize);
        if (!newBlock->data) {
            return false; // Failed to allocate memory
        }
        
        blocks_.push_back(std::move(newBlock));
        blocksAllocated_++;
        
        return true;
    }
    
private:
    std::vector<T*> freeList_;          // List of free objects for quick reuse
    std::vector<std::unique_ptr<Block>> blocks_;  // Memory blocks
    Block* currentBlock_;              // Current block being allocated from
    
    size_t maxBlockSize_;              // Maximum objects per block
    std::mutex mutex_;                 // Mutex for thread safety
    
    size_t objectsAllocated_;          // Total objects ever allocated
    size_t blocksAllocated_;           // Total blocks ever allocated
    std::atomic<size_t> allocatedCount_;  // Current number of allocated objects
    std::atomic<size_t> deallocatedCount_; // Total objects deallocated
};

/**
 * Neuron memory pool for efficient neuron allocation
 */
class NeuronMemoryPool {
public:
    NeuronMemoryPool(size_t initialNeurons = 1024, size_t maxBlockSize = 4096)
        : pool_(initialNeurons, maxBlockSize) {}
    
    ~NeuronMemoryPool() = default;
    
    // Disable copying
    NeuronMemoryPool(const NeuronMemoryPool&) = delete;
    NeuronMemoryPool& operator=(const NeuronMemoryPool&) = delete;
    
    /**
     * Allocate a neuron
     */
    Neuron* allocate()
    {
        return pool_.allocate();
    }
    
    /**
     * Deallocate a neuron
     */
    void deallocate(Neuron* neuron)
    {
        pool_.deallocate(neuron);
    }
    
    /**
     * Get number of allocated neurons
     */
    size_t allocatedCount() const
    {
        return pool_.allocatedCount();
    }
    
    /**
     * Get memory usage in bytes
     */
    size_t memoryUsage() const
    {
        return pool_.memoryUsage();
    }
    
    /**
     * Get total objects allocated
     */
    size_t totalObjectsAllocated() const
    {
        return pool_.totalObjectsAllocated();
    }
    
    /**
     * Reserve additional neurons
     */
    void reserve(size_t neurons)
    {
        pool_.reserve(neurons);
    }
    
    /**
     * Clear pool (keep memory for reuse)
     */
    void clear()
    {
        pool_.clear();
    }
    
    /**
     * Get pool statistics
     */
    struct Statistics {
        size_t allocatedCount;
        size_t availableCount;
        size_t totalAllocated;
        size_t blockCount;
        size_t memoryUsage;
        float utilization;
    };
    
    Statistics getStatistics() const
    {
        Statistics stats;
        stats.allocatedCount = pool_.allocatedCount();
        stats.availableCount = pool_.availableCount();
        stats.totalAllocated = pool_.totalObjectsAllocated();
        stats.blockCount = pool_.blockCount();
        stats.memoryUsage = pool_.memoryUsage();
        
        if (stats.totalAllocated > 0) {
            stats.utilization = static_cast<float>(stats.allocatedCount) / 
                              static_cast<float>(stats.totalAllocated);
        } else {
            stats.utilization = 0.0f;
        }
        
        return stats;
    }
    
private:
    MemoryPool<Neuron> pool_;
};

/**
 * Synapse memory pool for efficient synapse allocation
 */
class SynapseMemoryPool {
public:
    SynapseMemoryPool(size_t initialSynapses = 4096, size_t maxBlockSize = 8192)
        : pool_(initialSynapses, maxBlockSize) {}
    
    ~SynapseMemoryPool() = default;
    
    // Disable copying
    SynapseMemoryPool(const SynapseMemoryPool&) = delete;
    SynapseMemoryPool& operator=(const SynapseMemoryPool&) = delete;
    
    /**
     * Allocate a synapse
     */
    Synapse* allocate()
    {
        return pool_.allocate();
    }
    
    /**
     * Deallocate a synapse
     */
    void deallocate(Synapse* synapse)
    {
        pool_.deallocate(synapse);
    }
    
    /**
     * Get number of allocated synapses
     */
    size_t allocatedCount() const
    {
        return pool_.allocatedCount();
    }
    
    /**
     * Get memory usage in bytes
     */
    size_t memoryUsage() const
    {
        return pool_.memoryUsage();
    }
    
    /**
     * Reserve additional synapses
     */
    void reserve(size_t synapses)
    {
        pool_.reserve(synapses);
    }
    
    /**
     * Clear pool (keep memory for reuse)
     */
    void clear()
    {
        pool_.clear();
    }
    
private:
    MemoryPool<Synapse> pool_;
};

/**
 * Working memory pool for efficient memory trace storage
 */
class WorkingMemoryPool {
public:
    WorkingMemoryPool(size_t initialCapacity = 512, size_t maxBlockSize = 2048)
        : pool_(initialCapacity, maxBlockSize) {}
    
    ~WorkingMemoryPool() = default;
    
    // Disable copying
    WorkingMemoryPool(const WorkingMemoryPool&) = delete;
    WorkingMemoryPool& operator=(const WorkingMemoryPool&) = delete;
    
    /**
     * Allocate a memory trace entry
     */
    struct MemoryTrace {
        NeuronId neuronId;
        float activation;
        float importance;
        float recency;
        uint32_t timestamp;
        bool isActive;
        
        MemoryTrace() : neuronId(0), activation(0.0f), importance(0.0f), 
                       recency(0.0f), timestamp(0), isActive(false) {}
    };
    
    MemoryTrace* allocateTrace()
    {
        return pool_.allocate();
    }
    
    /**
     * Deallocate a memory trace entry
     */
    void deallocateTrace(MemoryTrace* trace)
    {
        pool_.deallocate(trace);
    }
    
    /**
     * Get number of allocated traces
     */
    size_t allocatedCount() const
    {
        return pool_.allocatedCount();
    }
    
    /**
     * Get memory usage in bytes
     */
    size_t memoryUsage() const
    {
        return pool_.memoryUsage();
    }
    
    /**
     * Clear pool (keep memory for reuse)
     */
    void clear()
    {
        pool_.clear();
    }
    
private:
    MemoryPool<MemoryTrace> pool_;
};

/**
 * Plasticity pool for efficient plasticity structure storage
 */
class PlasticityPool {
public:
    PlasticityPool(size_t initialSize = 2048, size_t maxBlockSize = 4096)
        : pool_(initialSize, maxBlockSize) {}
    
    ~PlasticityPool() = default;
    
    // Disable copying
    PlasticityPool(const PlasticityPool&) = delete;
    PlasticityPool& operator=(const PlasticityPool&) = delete;
    
    /**
     * Plasticity state for a synapse
     */
    struct PlasticityState {
        float weight;                    // Current synaptic weight
        float eligibilityTrace;          // STDP eligibility trace
        float preSpikeTime;              // Last pre-synaptic spike time
        float postSpikeTime;             // Last post-synaptic spike time
        float learningRate;              // Current learning rate
        float neuromodulationFactor;     // Neuromodulatory influence
        
        PlasticityState()
            : weight(0.0f), eligibilityTrace(0.0f), preSpikeTime(-1.0f), 
              postSpikeTime(-1.0f), learningRate(0.01f), neuromodulationFactor(1.0f) {}
    };
    
    PlasticityState* allocateState()
    {
        return pool_.allocate();
    }
    
    /**
     * Deallocate plasticity state
     */
    void deallocateState(PlasticityState* state)
    {
        pool_.deallocate(state);
    }
    
    /**
     * Get number of allocated states
     */
    size_t allocatedCount() const
    {
        return pool_.allocatedCount();
    }
    
    /**
     * Get memory usage in bytes
     */
    size_t memoryUsage() const
    {
        return pool_.memoryUsage();
    }
    
    /**
     * Reserve additional states
     */
    void reserve(size_t states)
    {
        pool_.reserve(states);
    }
    
    /**
     * Clear pool (keep memory for reuse)
     */
    void clear()
    {
        pool_.clear();
    }
    
private:
    MemoryPool<PlasticityState> pool_;
};

/**
 * Global memory manager for coordinating all memory pools
 */
class MemoryManager {
public:
    MemoryManager()
    {
        // Initialize all pools
        neuronPool_ = std::make_unique<NeuronMemoryPool>(1024, 4096);
        synapsePool_ = std::make_unique<SynapseMemoryPool>(4096, 8192);
        workingMemoryPool_ = std::make_unique<WorkingMemoryPool>(512, 2048);
        plasticityPool_ = std::make_unique<PlasticityPool>(2048, 4096);
    }
    
    ~MemoryManager() = default;
    
    // Disable copying
    MemoryManager(const MemoryManager&) = delete;
    MemoryManager& operator=(const MemoryManager&) = delete;
    
    /**
     * Get neuron memory pool
     */
    NeuronMemoryPool* getNeuronPool()
    {
        return neuronPool_.get();
    }
    
    /**
     * Get synapse memory pool
     */
    SynapseMemoryPool* getSynapsePool()
    {
        return synapsePool_.get();
    }
    
    /**
     * Get working memory pool
     */
    WorkingMemoryPool* getWorkingMemoryPool()
    {
        return workingMemoryPool_.get();
    }
    
    /**
     * Get plasticity pool
     */
    PlasticityPool* getPlasticityPool()
    {
        return plasticityPool_.get();
    }
    
    /**
     * Get total memory usage across all pools
     */
    size_t getTotalMemoryUsage() const
    {
        size_t total = 0;
        if (neuronPool_) total += neuronPool_->memoryUsage();
        if (synapsePool_) total += synapsePool_->memoryUsage();
        if (workingMemoryPool_) total += workingMemoryPool_->memoryUsage();
        if (plasticityPool_) total += plasticityPool_->memoryUsage();
        return total;
    }
    
    /**
     * Get total object count across all pools
     */
    size_t getTotalObjectCount() const
    {
        size_t total = 0;
        if (neuronPool_) total += neuronPool_->allocatedCount();
        if (synapsePool_) total += synapsePool_->allocatedCount();
        if (workingMemoryPool_) total += workingMemoryPool_->allocatedCount();
        if (plasticityPool_) total += plasticityPool_->allocatedCount();
        return total;
    }
    
    /**
     * Reserve additional objects across all pools
     */
    void reserve(size_t neurons, size_t synapses, size_t memoryTraces, size_t plasticityStates)
    {
        if (neuronPool_) neuronPool_->reserve(neurons);
        if (synapsePool_) synapsePool_->reserve(synapses);
        if (workingMemoryPool_) workingMemoryPool_->reserve(memoryTraces);
        if (plasticityPool_) plasticityPool_->reserve(plasticityStates);
    }
    
    /**
     * Clear all pools (keep memory for reuse)
     */
    void clear()
    {
        if (neuronPool_) neuronPool_->clear();
        if (synapsePool_) synapsePool_->clear();
        if (workingMemoryPool_) workingMemoryPool_->clear();
        if (plasticityPool_) plasticityPool_->clear();
    }
    
    /**
     * Get memory manager statistics
     */
    struct Statistics {
        struct PoolStats {
            size_t allocatedCount;
            size_t memoryUsage;
            float utilization;
        };
        
        PoolStats neuronPool;
        PoolStats synapsePool;
        PoolStats workingMemoryPool;
        PoolStats plasticityPool;
        
        size_t totalMemoryUsage;
        size_t totalObjectCount;
    };
    
    Statistics getStatistics() const
    {
        Statistics stats;
        
        if (neuronPool_) {
            auto neuronStats = neuronPool_->getStatistics();
            stats.neuronPool.allocatedCount = neuronStats.allocatedCount;
            stats.neuronPool.memoryUsage = neuronStats.memoryUsage;
            stats.neuronPool.utilization = neuronStats.utilization;
        }
        
        if (synapsePool_) {
            auto synapseStats = synapsePool_->getStatistics();
            stats.synapsePool.allocatedCount = synapseStats.allocatedCount;
            stats.synapsePool.memoryUsage = synapseStats.memoryUsage;
            stats.synapsePool.utilization = synapseStats.utilization;
        }
        
        if (workingMemoryPool_) {
            auto workingStats = workingMemoryPool_->getStatistics();
            stats.workingMemoryPool.allocatedCount = workingStats.allocatedCount;
            stats.workingMemoryPool.memoryUsage = workingStats.memoryUsage;
            stats.workingMemoryPool.utilization = workingStats.utilization;
        }
        
        if (plasticityPool_) {
            auto plasticStats = plasticityPool_->getStatistics();
            stats.plasticityPool.allocatedCount = plasticStats.allocatedCount;
            stats.plasticityPool.memoryUsage = plasticStats.memoryUsage;
            stats.plasticityPool.utilization = plasticStats.utilization;
        }
        
        stats.totalMemoryUsage = getTotalMemoryUsage();
        stats.totalObjectCount = getTotalObjectCount();
        
        return stats;
    }
    
private:
    std::unique_ptr<NeuronMemoryPool> neuronPool_;
    std::unique_ptr<SynapseMemoryPool> synapsePool_;
    std::unique_ptr<WorkingMemoryPool> workingMemoryPool_;
    std::unique_ptr<PlasticityPool> plasticityPool_;
};

} // namespace nlm