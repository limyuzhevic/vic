#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>
#include <memory>
#include <atomic>
#include <algorithm>
#include <cstring>

namespace nlm {

// MemoryPoolNode - Lock-free stack node for memory pool free list
struct MemPoolNode {
    std::atomic<MemPoolNode*> next;
    
    MemPoolNode() : next(nullptr) {}
};

// MemoryPool - Simple slab allocator for neural components
// Uses slab allocation for better cache locality and reduced fragmentation
template<typename T>
class MemoryPool {
public:
    explicit MemoryPool(size_t blockSize = 1024, size_t initialBlocks = 4)
        : blockSize_(blockSize)
        , freeList_(nullptr)
        , allocatedCount_(0)
        , availableCount_(0) {
        // Pre-allocate initial blocks
        for (size_t i = 0; i < initialBlocks; ++i) {
            addBlock();
        }
    }
    
    ~MemoryPool() = default;
    
    // Disable copying
    MemoryPool(const MemoryPool&) = delete;
    MemoryPool& operator=(const MemoryPool&) = delete;
    
    // Allocate an object from the pool
    T* allocate() {
        // Try to get from free list first
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
    
    // Return an object to the pool
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
    
    // Check if pool is empty
    bool empty() const { return freeList_.load(std::memory_order_relaxed) == nullptr; }
    
    // Get number of allocated objects
    size_t size() const { return allocatedCount_.load(std::memory_order_relaxed); }
    
    // Get number of available objects
    size_t availableCount() const { return availableCount_.load(std::memory_order_relaxed); }
    
    // Total memory used by pool (bytes)
    size_t memoryUsage() const {
        size_t total = 0;
        for (const auto& block : blocks_) {
            total += block->data.size() * sizeof(uint8_t);
        }
        return total;
    }
    
    // Pre-allocate objects
    void reserve(size_t count) {
        while (availableCount_.load(std::memory_order_relaxed) < count) {
            addBlock();
        }
    }
    
    // Clear the pool
    void clear() {
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
    };
    
    // Add a new block to the pool
    void addBlock() {
        auto block = std::make_unique<Block>(sizeof(T), blockSize_);
        
        // Chain existing blocks
        Block* expected = nullptr;
        if (!currentBlock_.compare_exchange_strong(expected, block.get())) {
            block->nextBlock = expected;
        } else {
            block.release();
        }
        
        // Initialize free list for this block
        Block* b = block.get();
        for (size_t i = 0; i < blockSize_; ++i) {
            T* obj = b->getObject(i, sizeof(T));
            MemPoolNode* node = reinterpret_cast<MemPoolNode*>(obj);
            node->next.store(freeList_.load(std::memory_order_relaxed), std::memory_order_relaxed);
            freeList_.store(node, std::memory_order_relaxed);
            availableCount_.fetch_add(1, std::memory_order_relaxed);
        }
        
        blocks_.push_back(std::move(block));
    }
    
    // For simplicity, just add the needed methods
    T* getObject(size_t index, size_t objectSize) {
        return reinterpret_cast<T*>(data.data() + index * objectSize);
    }
    
    // Block we're currently allocating from
    std::atomic<Block*> currentBlock_{nullptr};
    
    // Number of objects per block
    size_t blockSize_;
    
    // Head of free list (lock-free)
    std::atomic<MemPoolNode*> freeList_;
    
    // List of memory blocks
    std::vector<std::unique_ptr<Block>> blocks_;
    
    // Statistics
    std::atomic<size_t> allocatedCount_;
    std::atomic<size_t> availableCount_;
};

// NeuronPool - Specialized memory pool for neurons
class NeuronPool {
public:
    NeuronPool();
    ~NeuronPool();
    
    // Allocate a neuron with its state
    NeuronId allocate();
    
    // Deallocate a neuron
    void deallocate(NeuronId id);
    
    // Get total count
    size_t size() const { return size_; }
    
    // Memory usage
    size_t memoryUsage() const;
    
    // Clear all neurons
    void clear();
    
private:
    // Simple neuron storage
    struct NeuronStorage {
        NeuronId id;
        NeuronType type;
        NeuronState state;
        RegionId regionId;
        PopulationId populationId;
        std::vector<Timestamp> spikeHistory;
        std::vector<SynapseHandle> incomingSynapses;
        std::vector<SynapseHandle> outgoingSynapses;
        PlasticityFlags plasticityFlags;
    };
    
    size_t size_;
    size_t capacity_;
    std::vector<std::unique_ptr<NeuronStorage>> neurons_;
    std::vector<NeuronIndex> freeList_;
    std::atomic<size_t> freeListTop_;
    
    // Add a new neuron to the pool
    void addNeuron();
};

// SynapsePool - Specialized memory pool for synapses
class SynapsePool {
public:
    SynapsePool();
    ~SynapsePool();
    
    // Allocate a synapse
    SynapseId allocate(NeuronId source, NeuronId destination);
    
    // Deallocate a synapse
    void deallocate(SynapseId id);
    
    // Get total count
    size_t size() const { return size_; }
    
    // Memory usage
    size_t memoryUsage() const;
    
    // Clear all synapses
    void clear();
    
private:
    // Simple synapse storage
    struct SynapseStorage {
        SynapseId id;
        NeuronId sourceNeuron;
        NeuronId destinationNeuron;
        SynapticWeight weight;
        Delay delay;
        SynapseType type;
        std::vector<Timestamp> preSpikeHistory;
        std::vector<Timestamp> postSpikeHistory;
        PlasticityFlags plasticityFlags;
        float shortTermDepression;
        float shortTermFacilitation;
        float lastPreSpikeTime;
        float lastPostSpikeTime;
        float eligibilityTrace;
        float efficacy;
    };
    
    size_t size_;
    size_t capacity_;
    std::vector<std::unique_ptr<SynapseStorage>> synapses_;
    std::vector<SynapseIndex> freeList_;
    std::atomic<size_t> freeListTop_;
    
    // Add a new synapse to the pool
    void addSynapse(NeuronId source, NeuronId destination);
};

} // namespace nlm
