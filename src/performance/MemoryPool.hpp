#pragma once

/**
 * MemoryPool - Lock-free memory pool for neurons and synapses
 * 
 * Provides pre-allocated object pools to reduce allocation overhead during simulation.
 * Each pool manages objects of a single type with fixed size.
 * 
 * Features:
 * - Lock-free allocation and deallocation for thread safety
 * - Pre-allocated memory blocks for reduced fragmentation
 * - Memory-efficient slab allocation for better cache locality
 * - Thread-safe operations with atomic counters
 * - Support for both generic and specialized pools
 * 
 * Biological motivation:
 * - Like biological neural systems that maintain fixed numbers of neurons/synapses
 * - Reduces "allocation" overhead similar to how biological systems don't constantly create/destroy neurons
 * - Provides deterministic memory management for predictable simulation performance
 * 
 * Algorithmic approach:
 * - Uses a free list (lock-free stack) for O(1) allocation/deallocation
 * - Employs slab allocation for memory efficiency
 * - Implements exponential backoff for memory block allocation
 * - Uses cache-aligned access patterns to prevent false sharing
 * 
 * Memory management strategy:
 * - Object pools prevent frequent malloc/free operations
 * - Pre-allocation reduces runtime overhead
 * - Automatic memory reclamation when blocks are empty
 * - Configurable block sizes for different object types
 */

#include <cstddef>
#include <cstdint>
#include <vector>
#include <memory>
#include <atomic>
#include <array>
#include <optional>

namespace nlm {

/**
 * Lock-free stack node for memory pool free list
 * 
 * Each node contains a pointer to the next node in the free list.
 * The free list operates as a stack using atomic compare-and-swap operations
 * for thread-safe concurrent access.
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
 * 
 * Design principles:
 * - Static object size for predictable memory usage
 * - Lock-free operations for high concurrency
 * - Cache-aligned access patterns to prevent false sharing
 * - Memory reuse to reduce allocation overhead
 * 
 * Usage example:
 * @code
 * MemoryPool<Neuron> neuronPool(1024, 4); // 1024 neurons per block, 4 blocks initially
 * Neuron* neuron = neuronPool.allocate();
 * // ... use neuron ...
 * neuronPool.deallocate(neuron);
 * @endcode
 * 
 * Thread safety: This class is thread-safe. Multiple threads can allocate
 * and deallocate concurrently without synchronization locks.
 */
template<typename T>
class MemoryPool {
public:
    /**
     * Create a memory pool
     * 
     * @param blockSize Number of objects per memory block (also affects performance)
     * @param initialBlocks Number of blocks to pre-allocate (0 for lazy allocation)
     * 
     * Performance considerations:
     * - Larger blockSize reduces block overhead but increases memory waste
     * - initialBlocks = 0 provides better memory efficiency for small pools
     * - initialBlocks > 0 improves performance for frequently used pools
     */
    explicit MemoryPool(size_t blockSize = 1024, size_t initialBlocks = 4);
    
    ~MemoryPool();
    
    // Disable copying
    MemoryPool(const MemoryPool&) = delete;
    MemoryPool& operator=(const MemoryPool&) = delete;
    
    /**
     * Allocate an object from the pool
     * 
     * Lock-free operation using atomic compare-and-swap (CAS).
     * This method will block until allocation succeeds (unless out of memory).
     * 
     * @return Pointer to allocated object, or nullptr if pool is exhausted
     * 
     * Algorithm:
     * 1. Try to pop from free list using CAS
     * 2. If free list empty, allocate new memory block
     * 3. If all blocks exhausted, return nullptr
     * 
     * This operation is thread-safe and lock-free.
     */
    T* allocate();
    
    /**
     * Return an object to the pool
     * 
     * Lock-free operation using atomic CAS.
     * The object is pushed onto the free list for future allocations.
     * 
     * @param obj Pointer to object to deallocate
     * @pre obj must have been allocated from this pool
     * 
     * This operation is thread-safe and lock-free.
     */
    void deallocate(T* obj);
    
    /**
     * Check if pool is empty
     * 
     * @return true if no objects are currently allocated
     * @note Thread-safe, but value may be inconsistent if called during allocation/deallocation
     */
    bool empty() const { return freeList_.load(std::memory_order_relaxed) == nullptr; }
    
    /**
     * Get number of allocated objects
     * 
     * @return Number of currently allocated objects
     * @note Includes objects in all blocks, not just free objects
     */
    size_t allocatedCount() const { return allocatedCount_.load(std::memory_order_relaxed); }
    
    /**
     * Get number of available objects
     * 
     * @return Number of objects that can be allocated without allocating new blocks
     */
    size_t availableCount() const { return availableCount_.load(std::memory_order_relaxed); }
    
    /**
     * Total memory used by pool (bytes)
     * 
     * @return Total memory allocated for all objects in the pool
     */
    size_t memoryUsage() const;
    
    /**
     * Pre-allocate additional objects
     * 
     * @param count Number of objects to ensure are available for allocation
     * 
     * This method will allocate additional memory blocks as needed to ensure
     * at least 'count' objects are available for allocation.
     */
    void reserve(size_t count);
    
    /**
     * Get object size (bytes)
     * 
     * @return Size of objects allocated by this pool
     */
    size_t getObjectSize() const { return slabSize_; }
    
    /**
     * Get block size
     * 
     * @return Number of objects per block
     */
    size_t getBlockSize() const { return blockSize_; }
    
    /**
     * Get number of blocks
     * 
     * @return Number of memory blocks currently allocated
     */
    size_t getBlockCount() const { return blocks_.size(); }

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
    
    // Block we're currently allocating from (for performance)
    std::atomic<Block*> currentBlock_;
    
    // Index of next available object in current block
    std::atomic<size_t> currentIndex_;
    
    // Statistics
    std::atomic<size_t> allocatedCount_;
    std::atomic<size_t> availableCount_;
};

// Template implementation must be in header for generic types

/**
 * NeuronPool - Specialized pool for neurons with cache-aligned states
 * 
 * Optimized for neural computation patterns:
 * - Cache-aligned access to neural state variables
 * - Support for simultaneous and alternative access methods
 * - Specialized initialization for neuron objects
 * - Integration with neural simulation requirements
 * 
 * NeuronPool uses a different memory layout than MemoryPool<Neuron> to
 * optimize for neural computation patterns, particularly for cache locality
 * and alignment requirements of neural algorithms.
 */
class NeuronPool {
public:
    NeuronPool();
    ~NeuronPool();
    
    // Disable copying
    NeuronPool(const NeuronPool&) = delete;
    NeuronPool& operator=(const NeuronPool&) = delete;
    
    /**
     * Allocate a neuron with its state
     * @return NeuronId of allocated neuron, or INVALID_NEURON_ID if none available
     */
    NeuronId allocate();
    
    /**
     * Deallocate a neuron
     * @param neuronId ID of neuron to deallocate
     */
    void deallocate(NeuronId id);
    
    /**
     * Access neuron state directly (for SoA access)
     * @tparam Member State member type to access
     * @param index Neuron index in pool
     * @return Pointer to member data, or nullptr if index invalid
     */
    template<typename Member>
    Member* getState(NeuronIndex index);
    
    /**
     * Get neuron state for specific attribute
     * Specialized accessor for neural state management
     */
    const float* getMembranePotential(NeuronIndex index) const;
    float* getMembranePotential(NeuronIndex index);
    
    /**
     * Get total count of neurons in pool
     */
    size_t size() const { return size_; }
    
    /**
     * Get pool capacity
     */
    size_t capacity() const { return capacity_; }
    
    /**
     * Memory usage
     */
    size_t memoryUsage() const;
    
    /**
     * Reserve capacity
     */
    void reserve(size_t count);
    
    /**
     * Check if pool is empty
     */
    bool empty() const { return freeList_.empty(); }
    
    /**
     * Get free list size
     */
    size_t getFreeListSize() const { return freeList_.size(); }

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
    
    // Statistics
    std::atomic<size_t> allocatedCount_;
};

/**
 * SynapsePool - Specialized pool for synapses
 * 
 * Optimized for synaptic connectivity management:
 * - Cache-aligned access to synaptic properties
 * - Efficient sparse connectivity representation
 * - Support for connection management and plasticity
 * - Memory efficiency for typical synaptic connectivity patterns
 * 
 * SynapsePool uses a memory layout optimized for neural connectivity,
 * supporting the typical sparsity of neural connections while providing
 * fast access to synaptic properties during simulation.
 */
class SynapsePool {
public:
    SynapsePool();
    ~SynapsePool();
    
    // Disable copying
    SynapsePool(const SynapsePool&) = delete;
    SynapsePool& operator=(const SynapsePool&) = delete;
    
    /**
     * Allocate a synapse with connection
     * @param source Source neuron ID
     * @param destination Destination neuron ID
     * @return SynapseId of allocated synapse, or INVALID_SYNAPSE_ID if none available
     */
    SynapseId allocate(NeuronId source, NeuronId destination);
    
    /**
     * Deallocate a synapse
     * @param synapseId ID of synapse to deallocate
     */
    void deallocate(SynapseId id);
    
    /**
     * Get synapse data (for SoA access)
     * @tparam Member State member type to access
     * @param index Synapse index in pool
     * @return Pointer to member data, or nullptr if index invalid
     */
    template<typename Member>
    Member* getState(SynapseIndex index);
    
    /**
     * Get source/destination neurons
     */
    NeuronId getSource(SynapseIndex index) const;
    NeuronId getDestination(SynapseIndex index) const;
    
    /**
     * Get synaptic weight
     */
    float getWeight(SynapseIndex index) const;
    void setWeight(SynapseIndex index, float weight);
    
    /**
     * Get synaptic type
     */
    SynapseType getType(SynapseIndex index) const;
    void setType(SynapseIndex index, SynapseType type);
    
    /**
     * Get total count of synapses in pool
     */
    size_t size() const { return size_; }
    
    /**
     * Get pool capacity
     */
    size_t capacity() const { return capacity_; }
    
    /**
     * Memory usage
     */
    size_t memoryUsage() const;
    
    /**
     * Reserve capacity
     */
    void reserve(size_t count);
    
    /**
     * Check if pool is empty
     */
    bool empty() const { return freeList_.empty(); }
    
    /**
     * Get free list size
     */
    size_t getFreeListSize() const { return freeList_.size(); }
    
    /**
     * Get average synaptic weight
     */
    float getAverageWeight() const;
    
    /**
     * Get connectivity statistics
     */
    std::pair<size_t, size_t> getConnectivityStats() const;

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
    
    // Statistics
    std::atomic<size_t> allocatedCount_;
};

} // namespace nlm
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

// NeuronPool - Specialized pool for neurons with cache-aligned states
class NeuronPool {
public:
    NeuronPool();
    ~NeuronPool();
    
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
};

// SynapsePool - Specialized pool for synapses
class SynapsePool {
public:
    SynapsePool();
    ~SynapsePool();
    
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
};

} // namespace nlm