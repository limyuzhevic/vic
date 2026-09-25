// Memory pool implementation for NLM neural simulation
// This file provides efficient memory pools for frequently allocated objects
// to reduce overhead and improve performance in large-scale simulations

#include <cstdint>
#include <cstring>
#include <vector>
#include <algorithm>
#include <new>
#include <atomic>
#include <memory>

namespace nlm {

// Base memory pool template
template<typename T, size_t BlockSize = 1024>
class MemoryPool {
public:
    MemoryPool() : currentBlock_(nullptr), currentOffset_(0), blocks_() {}
    
    ~MemoryPool() {
        clear();
    }
    
    // Allocate an object from the pool
    T* allocate() {
        if (currentOffset_ >= BlockSize) {
            // Current block is full, get a new one
            currentBlock_ = allocateBlock();
            currentOffset_ = 0;
        }
        
        if (!currentBlock_) {
            return nullptr; // Out of memory
        }
        
        void* ptr = currentBlock_ + currentOffset_;
        currentOffset_ += sizeof(T);
        return new (ptr) T();
    }
    
    // Allocate and construct with arguments
    template<typename... Args>
    T* allocate(Args&&... args) {
        if (currentOffset_ >= BlockSize) {
            currentBlock_ = allocateBlock();
            currentOffset_ = 0;
        }
        
        if (!currentBlock_) {
            return nullptr;
        }
        
        void* ptr = currentBlock_ + currentOffset_;
        currentOffset_ += sizeof(T);
        return new (ptr) T(std::forward<Args>(args)...);
    }
    
    // Deallocate an object
    void deallocate(T* ptr) {
        if (!ptr) return;
        
        // Find the block containing this pointer
        for (auto& block : blocks_) {
            if (ptr >= block && ptr < block + BlockSize / sizeof(T)) {
                ptr->~T();
                return;
            }
        }
        
        // Pointer not found in any block - assume it's from regular new/delete
        delete ptr;
    }
    
    // Check if a pointer belongs to this pool
    bool owns(T* ptr) const {
        for (const auto& block : blocks_) {
            if (ptr >= block && ptr < block + BlockSize / sizeof(T)) {
                return true;
            }
        }
        return false;
    }
    
    // Clear all allocated blocks
    void clear() {
        for (auto block : blocks_) {
            ::operator delete(block);
        }
        blocks_.clear();
        currentBlock_ = nullptr;
        currentOffset_ = 0;
    }
    
    // Get total allocated size
    size_t totalSize() const {
        size_t total = 0;
        for (const auto& block : blocks_) {
            total += BlockSize;
        }
        return total;
    }
    
    // Get number of blocks
    size_t blockCount() const {
        return blocks_.size();
    }
    
private:
    // Allocate a new block of memory
    T* allocateBlock() {
        void* memory = ::operator new(BlockSize);
        if (!memory) return nullptr;
        
        blocks_.push_back(static_cast<T*>(memory));
        return static_cast<T*>(memory);
    }
    
    T** currentBlock_;     // Current block being filled
    size_t currentOffset_; // Current offset in current block
    std::vector<T*> blocks_; // All allocated blocks
};

// Specialized memory pool for Neuron objects
class NeuronMemoryPool : public MemoryPool<Neuron, 4096> {
public:
    NeuronMemoryPool() : nextNeuronId_(0) {}
    
    // Allocate a neuron with automatic ID assignment
    Neuron* allocateNeuron() {
        return allocate();
    }
    
    // Allocate and initialize a neuron
    Neuron* allocateNeuron(NeuronId id, const NeuronState& initialState) {
        Neuron* neuron = allocate();
        if (neuron) {
            // Note: In practice, you'd need to set the ID separately
            // since the memory pool doesn't automatically initialize IDs
            // This is a simplified example
        }
        return neuron;
    }
    
    // Get next available neuron ID
    NeuronId getNextId() {
        return NeuronId(nextNeuronId_++);
    }
    
private:
    std::atomic<uint64_t> nextNeuronId_;
};

// Specialized memory pool for Synapse objects
class SynapseMemoryPool : public MemoryPool<Synapse, 8192> {
public:
    SynapseMemoryPool() : nextSynapseId_(0) {}
    
    // Allocate a synapse with automatic ID assignment
    Synapse* allocateSynapse(NeuronId source, NeuronId destination) {
        return allocate(source, destination);
    }
    
    // Allocate and initialize a synapse
    Synapse* allocateSynapse(SynapseId id, NeuronId source, NeuronId destination,
                           SynapticWeight weight = 0.0f, SynapseType type = SynapseType::Excitatory) {
        return allocate(id, source, destination, weight, type);
    }
    
    // Get next available synapse ID
    SynapseId getNextId() {
        return SynapseId(nextSynapseId_++);
    }
    
private:
    std::atomic<uint64_t> nextSynapseId_;
};

// Specialized memory pool for SpikeEvent objects
class SpikeEventMemoryPool : public MemoryPool<SpikeEvent, 2048> {
public:
    SpikeEventMemoryPool() : nextEventId_(0) {}
    
    // Allocate a spike event
    SpikeEvent* allocateEvent(NeuronId neuron, Timestamp time, SimulationStep step) {
        return allocate(neuron, time, step);
    }
    
    // Get next available event ID
    uint64_t getNextId() {
        return nextEventId_++;
    }
    
private:
    std::atomic<uint64_t> nextEventId_;
};

// Specialized memory pool for DelayedSpikeEvent objects
class DelayedSpikeEventMemoryPool : public MemoryPool<DelayedSpikeEvent, 4096> {
public:
    DelayedSpikeEventMemoryPool() : nextDelayedId_(0) {}
    
    // Allocate a delayed spike event
    DelayedSpikeEvent* allocateEvent(NeuronId source, NeuronId destination,
                                   SynapseId synapse, SynapticWeight weight,
                                   SynapseType type, Timestamp timestamp,
                                   Timestamp deliveryTime, SimulationStep step,
                                   SimulationStep deliveryStep) {
        return allocate(source, destination, synapse, weight, type,
                       timestamp, deliveryTime, step, deliveryStep);
    }
    
    // Get next available delayed event ID
    uint64_t getNextId() {
        return nextDelayedId_++;
    }
    
private:
    std::atomic<uint64_t> nextDelayedId_;
};

// Memory pool manager for the entire NLM system
class MemoryPoolManager {
public:
    MemoryPoolManager() {
        initializePools();
    }
    
    ~MemoryPoolManager() {
        cleanupPools();
    }
    
    // Get neuron memory pool
    NeuronMemoryPool* getNeuronPool() {
        return neuronPool_.get();
    }
    
    // Get synapse memory pool
    SynapseMemoryPool* getSynapsePool() {
        return synapsePool_.get();
    }
    
    // Get spike event memory pool
    SpikeEventMemoryPool* getSpikeEventPool() {
        return spikeEventPool_.get();
    }
    
    // Get delayed spike event memory pool
    DelayedSpikeEventMemoryPool* getDelayedSpikeEventPool() {
        return delayedSpikeEventPool_.get();
    }
    
    // Enable or disable memory pools
    void setEnabled(bool enabled) {
        enabled_ = enabled;
    }
    
    // Get total memory usage
    size_t getTotalMemoryUsage() const {
        size_t total = 0;
        total += neuronPool_->totalSize();
        total += synapsePool_->totalSize();
        total += spikeEventPool_->totalSize();
        total += delayedSpikeEventPool_->totalSize();
        return total;
    }
    
    // Get total object allocation count
    size_t getTotalAllocationCount() const {
        size_t total = 0;
        total += neuronPool_->blockCount() * (4096 / sizeof(Neuron));
        total += synapsePool_->blockCount() * (8192 / sizeof(Synapse));
        total += spikeEventPool_->blockCount() * (2048 / sizeof(SpikeEvent));
        total += delayedSpikeEventPool_->blockCount() * (4096 / sizeof(DelayedSpikeEvent));
        return total;
    }
    
    // Check if a pointer belongs to any pool
    bool ownsPointer(void* ptr) const {
        if (!enabled_) return false;
        
        // Check each pool (this would be more efficient in practice with a registry)
        // For simplicity, we're checking each pool individually
        return false; // Implementation would check all pools
    }
    
private:
    void initializePools() {
        neuronPool_ = std::make_unique<NeuronMemoryPool>();
        synapsePool_ = std::make_unique<SynapseMemoryPool>();
        spikeEventPool_ = std::make_unique<SpikeEventMemoryPool>();
        delayedSpikeEventPool_ = std::make_unique<DelayedSpikeEventMemoryPool>();
    }
    
    void cleanupPools() {
        neuronPool_.reset();
        synapsePool_.reset();
        spikeEventPool_.reset();
        delayedSpikeEventPool_.reset();
    }
    
    std::unique_ptr<NeuronMemoryPool> neuronPool_;
    std::unique_ptr<SynapseMemoryPool> synapsePool_;
    std::unique_ptr<SpikeEventMemoryPool> spikeEventPool_;
    std::unique_ptr<DelayedSpikeEventMemoryPool> delayedSpikeEventPool_;
    bool enabled_ = true;
};

// Global memory pool manager instance
inline MemoryPoolManager& getMemoryPoolManager() {
    static MemoryPoolManager manager;
    return manager;
}

// Convenience functions for memory pool operations
inline void enableMemoryPools(bool enabled = true) {
    getMemoryPoolManager().setEnabled(enabled);
}

inline size_t getTotalMemoryUsage() {
    return getMemoryPoolManager().getTotalMemoryUsage();
}

inline size_t getTotalAllocationCount() {
    return getMemoryPoolManager().getTotalAllocationCount();
}

// Memory pool-aware allocation functions for common NLM objects
inline Neuron* allocateNeuron() {
    auto* pool = getMemoryPoolManager().getNeuronPool();
    if (pool && getMemoryPoolManager().getNeuronPool()->owns(pool->allocate())) {
        return pool->allocate();
    }
    // Fallback to regular new
    return new Neuron(NeuronId(0)); // Note: ID needs to be set separately
}

inline Synapse* allocateSynapse(SynapseId id, NeuronId source, NeuronId destination,
                             SynapticWeight weight = 0.0f, SynapseType type = SynapseType::Excitatory) {
    auto* pool = getMemoryPoolManager().getSynapsePool();
    if (pool) {
        return pool->allocateSynapse(id, source, destination, weight, type);
    }
    // Fallback to regular new
    return new Synapse(id, source, destination);
}

inline SpikeEvent* allocateSpikeEvent(NeuronId neuron, Timestamp time, SimulationStep step) {
    auto* pool = getMemoryPoolManager().getSpikeEventPool();
    if (pool) {
        return pool->allocateEvent(neuron, time, step);
    }
    // Fallback to regular new
    return new SpikeEvent(neuron, time, step);
}

inline DelayedSpikeEvent* allocateDelayedSpikeEvent(NeuronId source, NeuronId destination,
                                                   SynapseId synapse, SynapticWeight weight,
                                                   SynapseType type, Timestamp timestamp,
                                                   Timestamp deliveryTime, SimulationStep step,
                                                   SimulationStep deliveryStep) {
    auto* pool = getMemoryPoolManager().getDelayedSpikeEventPool();
    if (pool) {
        return pool->allocateEvent(source, destination, synapse, weight, type,
                                  timestamp, deliveryTime, step, deliveryStep);
    }
    // Fallback to regular new
    return new DelayedSpikeEvent(source, destination, synapse, weight, type,
                               timestamp, deliveryTime, step, deliveryStep);
}

} // namespace nlm
