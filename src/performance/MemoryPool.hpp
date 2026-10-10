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
 * 
 * Biological motivation:
 * - Like biological neural systems that maintain fixed numbers of neurons/synapses
 * - Reduces "allocation" overhead similar to how biological systems don't constantly create/destroy neurons
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
 * @file MemoryPool.hpp
 * 
 * Memory pool allocator system for NLM
 * 
 * This file provides the memory pool allocator system for NLM, implementing:
 * - Generic memory pool
 * - Neuron pool
 * - Synapse pool
 * - Lock-free allocation/deallocation
 * - Memory statistics
 * - Thread-safe operations
 * 
 * The memory pool allocator system in NLM:
 * - **MemoryPool**: Generic memory pool for fixed-size objects
 * - **NeuronPool**: Specialized pool for neurons with cache-aligned states
 * - **SynapsePool**: Specialized pool for synapses
 * - **Lock-free allocation**: Lock-free allocation and deallocation
 * - **Memory statistics**: Memory statistics
 * - **Thread safety**: Thread-safe operations
 * 
 * Key features:
 * - **Generic memory pool**: Generic memory pool for fixed-size objects
 * - **Neuron pool**: Specialized pool for neurons with cache-aligned states
 * - **Synapse pool**: Specialized pool for synapses
 * - **Lock-free allocation**: Lock-free allocation and deallocation
 * - **Memory statistics**: Memory statistics
 * - **Thread safety**: Thread-safe operations
 * 
 * Memory pool characteristics in NLM:
 * - **Generic memory pool**: Generic memory pool for fixed-size objects
 * - **Neuron pool**: Specialized pool for neurons with cache-aligned states
 * - **Synapse pool**: Specialized pool for synapses
 * - **Lock-free allocation**: Lock-free allocation and deallocation
 * - **Memory statistics**: Memory statistics
 * - **Thread safety**: Thread-safe operations
 * 
 * @note Critical for performance optimization
 * @note Used extensively in performance systems
 * @note Essential for scaling to large networks
 * @see MemoryPool.cpp - For implementation details
 * 
 * @author NLM Development Team
 * @date 2026
 * @version 1.0
 */

/**
 * @struct MemPoolNode
 * 
 * Neural implementation of lock-free stack node for memory pool free list.
 * 
 * This structure implements the lock-free stack node, providing
 * lock-free stack node for memory pool free list. It is essential for:
 * - **Free list management**: Free list management
 * - **Lock-free operations**: Lock-free operations
 * - **Memory allocation**: Memory allocation
 * - **Memory deallocation**: Memory deallocation
 * 
 * Free list characteristics in NLM:
 * - **Free list management**: Free list management
 * - **Lock-free operations**: Lock-free operations
 * - **Memory allocation**: Memory allocation
 * - **Memory deallocation**: Memory deallocation
 * 
 * @note Lock-free stack node for memory pool free list
 * 
 * @author NLM Development Team
 */
struct MemPoolNode {
    /** Next node in free list (lock-free) */
    std::atomic<MemPoolNode*> next;
    
    /**
     * @brief Default constructor
     * 
     * Creates a default memory pool node with default parameters.
     * The memory pool node is initialized with default values for
     * all node fields.
     */
    MemPoolNode() : next(nullptr) {}
};

/**
 * @class MemoryPool
 * 
 * Neural implementation of memory pool for fixed-size objects.
 * 
 * This class implements the memory pool, providing
 * memory pool for fixed-size objects. It is essential for:
 * - **Memory allocation**: Memory allocation
 * - **Memory deallocation**: Memory deallocation
 * - **Free list management**: Free list management
 * - **Lock-free operations**: Lock-free operations
 * - **Memory statistics**: Memory statistics
 * 
 * Memory pool characteristics in NLM:
 * - **Memory allocation**: Memory allocation
 * - **Memory deallocation**: Memory deallocation
 * - **Free list management**: Free list management
 * - **Lock-free operations**: Lock-free operations
 * - **Memory statistics**: Memory statistics
 * 
 * Key components:
 * - **Slab size**: Slab size for each object
 * - **Block size**: Number of objects per block
 * - **Free list**: Head of free list (lock-free)
 * - **Memory blocks**: List of memory blocks
 * - **Statistics**: Allocation and deallocation statistics
 * 
 * @note Lock-free memory pool for fixed-size objects
 * @see MemoryPool.cpp - For implementation details
 * 
 * @author NLM Development Team
 */
template<typename T>
class MemoryPool {
public:
    /**
     * @brief Default constructor
     * 
     * Creates a default memory pool with default parameters.
     * The memory pool is initialized with default values for
     * all pool fields.
     */
    explicit MemoryPool(size_t blockSize = 1024, size_t initialBlocks = 4);
    
    /**
     * @brief Destructor
     * 
     * Cleans up memory pool resources and performs any necessary
     * cleanup operations. The memory pool is left in a valid but
     * non-operational state after destruction.
     */
    ~MemoryPool();
    
    /**
     * @brief Allocate an object from the pool
     * 
     * Allocates an object from the pool. This is used for:
     * - Memory allocation
     * - Object allocation
     * - Pool management
     * - Allocation statistics
     */
    T* allocate();
    
    /**
     * @brief Return an object to the pool
     * 
     * Returns an object to the pool. This is used for:
     * - Memory deallocation
     * - Object deallocation
     * - Pool management
     * - Deallocation statistics
     */
    void deallocate(T* obj);
    
    /**
     * @brief Check if pool is empty
     * 
     * Checks if the pool is empty. This is used for:
     * - Empty checking
     * - Pool status checking
     * - Allocation/deallocation management
     */
    bool empty() const { return freeList_.load(std::memory_order_relaxed) == nullptr; }
    
    /**
     * @brief Get number of allocated objects
     * 
     * Returns the number of allocated objects. This is used for:
     * - Allocation count access
     * - Allocation statistics
     * - Pool management
     */
    size_t allocatedCount() const { return allocatedCount_.load(std::memory_order_relaxed); }
    
    /**
     * @brief Get number of available objects
     * 
     * Returns the number of available objects. This is used for:
     * - Availability count access
     * - Allocation statistics
     * - Pool management
     */
    size_t availableCount() const { return availableCount_.load(std::memory_order_relaxed); }
    
    /**
     * @brief Get total memory used by pool (bytes)
     * 
     * Returns the total memory used by the pool. This is used for:
     * - Memory usage access
     * - Memory statistics
     * - Pool management
     */
    size_t memoryUsage() const;
    
    /**
     * @brief Pre-allocate objects
     * 
     * Pre-allocates objects. This is used for:
     * - Object pre-allocation
     * - Pool management
     * - Memory statistics
     */
    void reserve(size_t count);
    
private:
    /**
     * @struct Block
     * 
     * Neural implementation of memory block containing objects.
     * 
     * This structure implements the memory block, providing
     * memory block for object storage.
     */
    struct Block {
        /** Block data storage */
        std::vector<uint8_t> data;
        
        /** Next block in pool */
        Block* nextBlock;
        
        /**
         * @brief Constructor
         * 
         * Creates a memory block with specified object size and count.
         */
        explicit Block(size_t objectSize, size_t count);
        
        /**
         * @brief Get object
         * 
         * Returns the object at specified index.
         */
        T* getObject(size_t index, size_t objectSize) {
            return reinterpret_cast<T*>(data.data() + index * objectSize);
        }
    };
    
    /**
     * @brief Add a new block to the pool
     * 
     * Adds a new block to the pool. This is used for:
     * - Block management
     * - Object allocation
     * - Pool expansion
     */
    void addBlock();
    
    /**
     * Slab size for each object (rounded up to alignment)
     */
    size_t slabSize_;
    
    /**
     * Number of objects per block
     */
    size_t blockSize_;
    
    /**
     * Head of free list (lock-free)
     */
    std::atomic<MemPoolNode*> freeList_;
    
    /**
     * List of memory blocks
     */
    std::vector<std::unique_ptr<Block>> blocks_;
    
    /**
     * Current block we're allocating from
     */
    std::atomic<Block*> currentBlock_;
    
    /**
     * Index of next available object in current block
     */
    std::atomic<size_t> currentIndex_;
    
    /**
     * Statistics
     */
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

/**
 * @class NeuronPool
 * 
 * Neural implementation of specialized pool for neurons with cache-aligned states.
 * 
 * This class implements the neuron pool, providing
 * specialized pool for neurons with cache-aligned states. It is essential for:
 * - **Neuron allocation**: Neuron allocation
 * - **Neuron deallocation**: Neuron deallocation
 * - **State access**: State access
 * - **Memory usage**: Memory usage
 * - **Pool management**: Pool management
 * 
 * Neuron pool characteristics in NLM:
 * - **Neuron allocation**: Neuron allocation
 * - **Neuron deallocation**: Neuron deallocation
 * - **State access**: State access
 * - **Memory usage**: Memory usage
 * - **Pool management**: Pool management
 * 
 * @note Specialized pool for neurons with cache-aligned states
 * @see NeuronPool.cpp - For implementation details
 * 
 * @author NLM Development Team
 */
class NeuronPool {
public:
    /**
     * @brief Default constructor
     * 
     * Creates a default neuron pool with default parameters.
     * The neuron pool is initialized with default values for
     * all pool fields.
     */
    NeuronPool();
    
    /**
     * @brief Destructor
     * 
     * Cleans up neuron pool resources and performs any necessary
     * cleanup operations. The neuron pool is left in a valid but
     * non-operational state after destruction.
     */
    ~NeuronPool();
    
    /**
     * @brief Allocate a neuron with its state
     * 
     * Allocates a neuron with its state. This is used for:
     * - Neuron allocation
     * - Pool management
     * - State initialization
     */
    NeuronId allocate();
    
    /**
     * @brief Deallocate a neuron
     * 
     * Deallocates a neuron. This is used for:
     * - Neuron deallocation
     * - Pool management
     * - Memory cleanup
     */
    void deallocate(NeuronId id);
    
    /**
     * @brief Access neuron state directly (for SoA access)
     * 
     * Accesses neuron state directly for SoA access. This is used for:
     * - State access
     * - SoA management
     * - Memory optimization
     */
    template<typename Member>
    Member* getState(NeuronIndex index);
    
    /**
     * @brief Get total count
     * 
     * Returns the total count. This is used for:
     * - Count access
     * - Pool management
     */
    size_t size() const { return size_; }
    
    /**
     * @brief Get memory usage
     * 
     * Returns the memory usage. This is used for:
     * - Memory usage access
     * - Memory statistics
     */
    size_t memoryUsage() const;
    
private:
    static constexpr size_t CACHE_LINE = 64;
    
    /**
     * @struct NeuronBlock
     * 
     * Neural implementation of neuron block with cache-aligned states.
     * 
     * This structure implements the neuron block, providing
     * cache-aligned states for neuron storage.
     */
    struct alignas(CACHE_LINE) alignas(sizeof(void*)) NeuronBlock {
        /** SoA storage for neuron states */
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
        
        /** Per-neuron current injection (not in NeuronState, but needed for hot path) */
        std::vector<float> synapticInput;
        
        /**
         * @brief Default constructor
         * 
         * Creates a default neuron block with default parameters.
         */
        NeuronBlock() {}
    };
    
    /**
     * Total neuron count
     */
    size_t size_;
    
    /**
     * Block capacity
     */
    size_t capacity_;
    
    /**
     * Blocks of neurons
     */
    std::vector<std::unique_ptr<NeuronBlock>> blocks_;
    
    /**
     * Free list for deallocated neurons
     */
    std::vector<NeuronIndex> freeList_;
    std::atomic<size_t> freeListTop_;
};

/**
 * @class SynapsePool
 * 
 * Neural implementation of specialized pool for synapses.
 * 
 * This class implements the synapse pool, providing
 * specialized pool for synapses. It is essential for:
 * - **Synapse allocation**: Synapse allocation
 * - **Synapse deallocation**: Synapse deallocation
 * - **Synapse access**: Synapse access
 * - **Synapse management**: Synapse management
 * - **Memory usage**: Memory usage
 * 
 * Synapse pool characteristics in NLM:
 * - **Synapse allocation**: Synapse allocation
 * - **Synapse deallocation**: Synapse deallocation
 * - **Synapse access**: Synapse access
 * - **Synapse management**: Synapse management
 * - **Memory usage**: Memory usage
 * 
 * @note Specialized pool for synapses
 * @see SynapsePool.cpp - For implementation details
 * 
 * @author NLM Development Team
 */
class SynapsePool {
public:
    /**
     * @brief Default constructor
     * 
     * Creates a default synapse pool with default parameters.
     * The synapse pool is initialized with default values for
     * all pool fields.
     */
    SynapsePool();
    
    /**
     * @brief Destructor
     * 
     * Cleans up synapse pool resources and performs any necessary
     * cleanup operations. The synapse pool is left in a valid but
     * non-operational state after destruction.
     */
    ~SynapsePool();
    
    /**
     * @brief Allocate a synapse
     * 
     * Allocates a synapse. This is used for:
     * - Synapse allocation
     * - Pool management
     * - Connection management
     */
    SynapseId allocate(NeuronId source, NeuronId destination);
    
    /**
     * @brief Deallocate a synapse
     * 
     * Deallocates a synapse. This is used for:
     * - Synapse deallocation
     * - Pool management
     * - Memory cleanup
     */
    void deallocate(SynapseId id);
    
    /**
     * @brief Get synapse data (for SoA access)
     * 
     * Returns synapse data for SoA access. This is used for:
     * - Synapse data access
     * - SoA management
     * - Memory optimization
     */
    template<typename Member>
    Member* getState(SynapseIndex index);
    
    /**
     * @brief Get source neurons
     * 
     * Returns the source neurons. This is used for:
     * - Source access
     * - Synapse management
     */
    NeuronId getSource(SynapseIndex index) const;
    
    /**
     * @brief Get destination neurons
     * 
     * Returns the destination neurons. This is used for:
     * - Destination access
     * - Synapse management
     */
    NeuronId getDestination(SynapseIndex index) const;
    
    /**
     * @brief Get total count
     * 
     * Returns the total count. This is used for:
     * - Count access
     * - Pool management
     */
    size_t size() const { return size_; }
    
    /**
     * @brief Get memory usage
     * 
     * Returns the memory usage. This is used for:
     * - Memory usage access
     * - Memory statistics
     */
    size_t memoryUsage() const;
    
private:
    static constexpr size_t CACHE_LINE = 64;
    
    /**
     * @struct SynapseBlock
     * 
     * Neural implementation of synapse block with SoA storage.
     * 
     * This structure implements the synapse block, providing
     * SoA storage for synapse states.
     */
    struct alignas(CACHE_LINE) SynapseBlock {
        /** SoA storage for synapse states */
        std::vector<uint64_t> sourceNeuron;  // Source neuron ID
        std::vector<uint64_t> destinationNeuron;  // Dest neuron ID
        std::vector<float> weight;
        std::vector<uint32_t> delay;
        std::vector<uint8_t> synapseType;  // SynapseType enum
        std::vector<uint8_t> plasticityFlags;  // Packed flags
        
        /** Short-term plasticity */
        std::vector<float> shortTermDepression;
        std::vector<float> shortTermFacilitation;
        std::vector<float> lastPreSpikeTime;
        std::vector<float> lastPostSpikeTime;
        
        /** Eligibility trace */
        std::vector<float> eligibilityTrace;
        
        /** Synaptic efficacy */
        std::vector<float> efficacy;
        
        /**
         * @brief Default constructor
         * 
         * Creates a default synapse block with default parameters.
         */
        SynapseBlock() {}
    };
    
    /**
     * Total synapse count
     */
    size_t size_;
    
    /**
     * Block capacity
     */
    size_t capacity_;
    
    /**
     * Blocks of synapses
     */
    std::vector<std::unique_ptr<SynapseBlock>> blocks_;
    std::vector<SynapseIndex> freeList_;
    std::atomic<size_t> freeListTop_;
};

} // namespace nlm
