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
 * 
 */

#include <cstddef>
#include <cstdint>
#include <vector>
#include <memory>
#include <atomic>
#include <array>
#include <optional>
#include <mutex>

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
// Forward declaration for NeuronPool and SynapsePool to avoid circular includes
class NeuronPool;
class SynapsePool;

/**
 * MemoryPool - Generic lock-free memory pool for fixed-size objects
 * 
 * Template parameter T specifies the object type to allocate.
 * All objects have the same fixed size determined at construction.
 * 
 * Features:
 * - Lock-free allocation and deallocation
 * - Pre-allocated memory blocks
 * - Memory-efficient slab allocation
 * - Thread-safe operations
 */
template<typename T>
class MemoryPool {
public:
    /**
     * Create a memory pool
     * @param blockSize Number of objects per memory block
     * @param initialBlocks Number of blocks to pre-allocate
     */
    explicit MemoryPool(size_t blockSize = 1024, size_t initialBlocks = 4);
    
    ~MemoryPool();
    
    // Disable copying
    MemoryPool(const MemoryPool&) = delete;
    MemoryPool& operator=(const MemoryPool&) = delete;
    
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

/**
 * NeuronPool - Specialized memory pool for neurons with cache-aligned SoA storage
 * 
 * Provides efficient memory layout for neural computation where all neurons
 * share the same state structure (Structure-of-Arrays), which is much more
 * efficient for SIMD operations and batch processing.
 * 
 * Key optimizations:
 * - Cache line aligned storage (64 bytes) to prevent false sharing
 * - Structure-of-Arrays layout for better vectorization
 * - Lock-free operations for high concurrency
 * - Pre-allocated memory blocks to reduce fragmentation
 */
class NeuronPool {
public:
    /**
     * Create a neuron pool
     * @param initialNeurons Number of neurons to pre-allocate
     */
    explicit NeuronPool(size_t initialNeurons = 1024);
    
    ~NeuronPool();
    
    // Disable copying
    NeuronPool(const NeuronPool&) = delete;
    NeuronPool& operator=(const NeuronPool&) = delete;
    
    /**
     * Allocate a neuron from the pool
     * Returns a NeuronId that can be used to access the neuron in the SoA arrays
     */
    NeuronId allocate();
    
    /**
     * Deallocate a neuron back to the pool
     */
    void deallocate(NeuronId id);
    
    /**
     * Access neuron state directly via index (for SoA access)
     * @param index NeuronIndex (index into the SoA arrays)
     * @return Pointer to the neuron state at the given index
     */
    template<typename Member>
    Member* getState(NeuronIndex index);
    
    /**
     * Get neuron state as structured access
     * @param index NeuronIndex
     * @return Reference to the neuron state structure
     */
    NeuronState& getNeuronState(NeuronIndex index);
    const NeuronState& getNeuronState(NeuronIndex index) const;
    
    /**
     * Get total number of neurons in the pool
     */
    size_t size() const { return size_; }
    
    /**
     * Get number of allocated neurons
     */
    size_t allocatedCount() const { return allocatedCount_.load(std::memory_order_relaxed); }
    
    /**
     * Get number of available slots
     */
    size_t availableCount() const { return availableCount_.load(std::memory_order_relaxed); }
    
    /**
     * Total memory usage of the pool (bytes)
     */
    size_t memoryUsage() const;
    
    /**
     * Pre-allocate neurons
     */
    void reserve(size_t count);
    
    /**
     * Get NeuronIndex from NeuronId
     * @param id NeuronId
     * @return Corresponding index into SoA arrays
     */
    NeuronIndex getIndex(NeuronId id) const;
    
    /**
     * Get NeuronId from NeuronIndex
     */
    NeuronId getId(NeuronIndex index) const;
    
    /**
     * Check if pool is empty
     */
    bool empty() const { return freeList_.load(std::memory_order_relaxed) == nullptr; }
    
    /**
     * Reset all neurons to initial state
     */
    void resetAllNeurons();
    
    /**
     * Get cache-friendly access to neuron arrays
     * Returns pointers to SoA arrays for direct memory access
     */
    const std::vector<float>* getMembranePotentials() const { return membranePotentials_.data(); }
    const std::vector<float>* getRestingPotentials() const { return restingPotentials_.data(); }
    const std::vector<float>* getThresholds() const { return thresholds_.data(); }
    const std::vector<float>* getResetPotentials() const { return resetPotentials_.data(); }
    const std::vector<float>* getLeakConductances() const { return leakConductances_.data(); }
    const std::vector<float>* getSynapticConductances() const { return synapticConductances_.data(); }
    const std::vector<float>* getFiringRates() const { return firingRates_.data(); }
    const std::vector<FiringState>* getFiringStates() const { return firingStates_.data(); }
    const std::vector<uint32_t>* getRefractoryRemainings() const { return refractoryRemainings_.data(); }
    const std::vector<uint32_t>* getRefractoryPeriods() const { return refractoryPeriods_.data(); }
    const std::vector<float>* getAdaptationVariables() const { return adaptationVariables_.data(); }
    const std::vector<float>* getLastSpikeTimes() const { return lastSpikeTimes_.data(); }
    
    /**
     * Get total number of available NeuronIndices
     */
    size_t capacity() const { return capacity_; }
    
private:
    static constexpr size_t CACHE_LINE = 64;
    
    /**
     * Neuron block with cache line alignment for SoA storage
     * All neurons in this block share the same state structure layout
     */
    struct alignas(CACHE_LINE) alignas(sizeof(void*)) NeuronBlock {
        // SoA storage for neuron states - all arrays have the same length
        std::vector<float> membranePotentials;
        std::vector<float> restingPotentials;
        std::vector<float> thresholds;
        std::vector<float> resetPotentials;
        std::vector<float> leakConductances;
        std::vector<float> synapticConductances;
        std::vector<float> firingRates;
        std::vector<FiringState> firingStates;
        std::vector<uint32_t> refractoryRemainings;
        std::vector<uint32_t> refractoryPeriods;
        std::vector<float> adaptationVariables;
        std::vector<float> lastSpikeTimes;
        std::vector<uint64_t> neuronType;  // NeuronType enum
        std::vector<uint64_t> regionId;
        std::vector<uint64_t> populationId;
        
        // Per-neuron current injection (not in NeuronState, but needed for hot path)
        std::vector<float> synapticInput;
        
        // For fast index-to-address translation
        std::vector<uint64_t> indexToId;
        
        NeuronBlock() {}
        
        // Initialize block with given capacity
        void initialize(size_t capacity);
    };
    
    /**
     * Lock-free free list for available neurons
     */
    std::atomic<NeuronId*> freeList_;
    
    /**
     * Vector of neuron blocks for scalable memory management
     */
    std::vector<std::unique_ptr<NeuronBlock>> blocks_;
    
    /**
     * Current block we're allocating from (for fast allocation)
     */
    std::atomic<NeuronBlock*> currentBlock_;
    
    /**
     * Current index in current block for allocation
     */
    std::atomic<size_t> currentIndex_;
    
    /**
     * Total number of neurons allocated
     */
    size_t size_;
    
    /**
     * Total capacity of the pool
     */
    size_t capacity_;
    
    /**
     * Statistics
     */
    std::atomic<size_t> allocatedCount_;
    std::atomic<size_t> availableCount_;
    
    /**
     * For thread-safe access to blocks
     */
    mutable std::mutex blocksMutex_;
    
    /**
     * Mapping from NeuronId to NeuronIndex (for fast lookup)
     */
    std::vector<NeuronIndex> idToIndex_;
    
    /**
     * Mapping from NeuronIndex to NeuronId (for fast lookup)
     */
    std::vector<NeuronId> indexToId_;
    
    /**
     * Free list for deallocated neurons (optimization for temporary reuse)
     */
    std::vector<NeuronIndex> freeListIndex_;
    std::atomic<size_t> freeListTop_;
    
    /**
     * Add a new block to the pool
     */
    void addBlock();
    
    /**
     * Initialize all neurons in a block
     */
    void initializeBlock(NeuronBlock* block);
    
    /**
     * Cleanup a deallocated neuron
     */
    void cleanupNeuron(NeuronBlock* block, NeuronIndex index);
    
    /**
     * Initialize global mapping tables
     */
    void initializeMappings();
    
    /**
     * Rebuild mapping tables (when blocks change)
     */
    void rebuildMappings();
};

/**
 * SynapsePool - Specialized memory pool for synapses with SoA storage
 * 
 * Provides efficient memory layout for synaptic connections where all synapses
 * share the same state structure (Structure-of-Arrays), which is much more
 * efficient for batch processing and synaptic weight updates.
 * 
 * Key optimizations:
 * - Cache line aligned storage (64 bytes) to prevent false sharing
 * - Structure-of-Arrays layout for better vectorization
 * - Lock-free operations for high concurrency
 * - Pre-allocated memory blocks to reduce fragmentation
 */
class SynapsePool {
public:
    /**
     * Create a synapse pool
     * @param initialSynapses Number of synapses to pre-allocate
     */
    explicit SynapsePool(size_t initialSynapses = 1024);
    
    ~SynapsePool();
    
    // Disable copying
    SynapsePool(const SynapsePool&) = delete;
    SynapsePool& operator=(const SynapsePool&) = delete;
    
    /**
     * Allocate a synapse from the pool
     * @param source Source neuron ID
     * @param destination Destination neuron ID
     * @return SynapseId identifying the allocated synapse
     */
    SynapseId allocate(NeuronId source, NeuronId destination);
    
    /**
     * Deallocate a synapse back to the pool
     */
    void deallocate(SynapseId id);
    
    /**
     * Access synapse state directly via index (for SoA access)
     * @param index SynapseIndex (index into the SoA arrays)
     * @return Pointer to the synapse state at the given index
     */
    template<typename Member>
    Member* getState(SynapseIndex index);
    
    /**
     * Get synapse state as structured access
     * @param index SynapseIndex
     * @return Reference to the synapse state structure
     */
    SynapseState& getSynapseState(SynapseIndex index);
    const SynapseState& getSynapseState(SynapseIndex index) const;
    
    /**
     * Get source neuron ID for a synapse
     */
    NeuronId getSource(SynapseIndex index) const;
    
    /**
     * Get destination neuron ID for a synapse
     */
    NeuronId getDestination(SynapseIndex index) const;
    
    /**
     * Get synaptic weight
     */
    SynapticWeight getWeight(SynapseIndex index) const;
    
    /**
     * Set synaptic weight
     */
    void setWeight(SynapseIndex index, SynapticWeight weight);
    
    /**
     * Get total number of synapses in the pool
     */
    size_t size() const { return size_; }
    
    /**
     * Get number of allocated synapses
     */
    size_t allocatedCount() const { return allocatedCount_.load(std::memory_order_relaxed); }
    
    /**
     * Get number of available slots
     */
    size_t availableCount() const { return availableCount_.load(std::memory_order_relaxed); }
    
    /**
     * Total memory usage of the pool (bytes)
     */
    size_t memoryUsage() const;
    
    /**
     * Pre-allocate synapses
     */
    void reserve(size_t count);
    
    /**
     * Get SynapseIndex from SynapseId
     * @param id SynapseId
     * @return Corresponding index into SoA arrays
     */
    SynapseIndex getIndex(SynapseId id) const;
    
    /**
     * Get SynapseId from SynapseIndex
     */
    SynapseId getId(SynapseIndex index) const;
    
    /**
     * Check if pool is empty
     */
    bool empty() const { return freeList_.load(std::memory_order_relaxed) == nullptr; }
    
    /**
     * Reset all synapses to initial state
     */
    void resetAllSynapses();
    
    /**
     * Get cache-friendly access to synapse arrays
     * Returns pointers to SoA arrays for direct memory access
     */
    const std::vector<uint64_t>* getSourceNeurons() const { return sourceNeurons_.data(); }
    const std::vector<uint64_t>* getDestinationNeurons() const { return destinationNeurons_.data(); }
    const std::vector<float>* getWeights() const { return weights_.data(); }
    const std::vector<uint32_t>* getDelays() const { return delays_.data(); }
    const std::vector<uint8_t>* getSynapseTypes() const { return synapseTypes_.data(); }
    const std::vector<uint8_t>* getPlasticityFlags() const { return plasticityFlags_.data(); }
    const std::vector<float>* getShortTermDepression() const { return shortTermDepression_.data(); }
    const std::vector<float>* getShortTermFacilitation() const { return shortTermFacilitation_.data(); }
    const std::vector<float>* getLastPreSpikeTimes() const { return lastPreSpikeTimes_.data(); }
    const std::vector<float>* getLastPostSpikeTimes() const { return lastPostSpikeTimes_.data(); }
    const std::vector<float>* getEligibilityTraces() const { return eligibilityTraces_.data(); }
    const std::vector<float>* getEfficacies() const { return efficacies_.data(); }
    
    /**
     * Get total number of available SynapseIndices
     */
    size_t capacity() const { return capacity_; }
    
private:
    static constexpr size_t CACHE_LINE = 64;
    
    /**
     * Synapse block with cache line alignment for SoA storage
     * All synapses in this block share the same state structure layout
     */
    struct alignas(CACHE_LINE) alignas(sizeof(void*)) SynapseBlock {
        // SoA storage for synapse states - all arrays have the same length
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
        
        // Synaptic efficacy (use-dependent modulation)
        std::vector<float> efficacy;
        
        // For fast index-to-address translation
        std::vector<uint64_t> indexToId;
        
        SynapseBlock() {}
        
        // Initialize block with given capacity
        void initialize(size_t capacity);
    };
    
    /**
     * Lock-free free list for available synapses
     */
    std::atomic<SynapseId*> freeList_;
    
    /**
     * Vector of synapse blocks for scalable memory management
     */
    std::vector<std::unique_ptr<SynapseBlock>> blocks_;
    
    /**
     * Current block we're allocating from (for fast allocation)
     */
    std::atomic<SynapseBlock*> currentBlock_;
    
    /**
     * Current index in current block for allocation
     */
    std::atomic<size_t> currentIndex_;
    
    /**
     * Total number of synapses allocated
     */
    size_t size_;
    
    /**
     * Total capacity of the pool
     */
    size_t capacity_;
    
    /**
     * Statistics
     */
    std::atomic<size_t> allocatedCount_;
    std::atomic<size_t> availableCount_;
    
    /**
     * For thread-safe access to blocks
     */
    mutable std::mutex blocksMutex_;
    
    /**
     * Mapping from SynapseId to SynapseIndex (for fast lookup)
     */
    std::vector<SynapseIndex> idToIndex_;
    
    /**
     * Mapping from SynapseIndex to SynapseId (for fast lookup)
     */
    std::vector<SynapseId> indexToId_;
    
    /**
     * Free list for deallocated synapses (optimization for temporary reuse)
     */
    std::vector<SynapseIndex> freeListIndex_;
    std::atomic<size_t> freeListTop_;
    
    /**
     * Add a new block to the pool
     */
    void addBlock();
    
    /**
     * Initialize all synapses in a block
     */
    void initializeBlock(SynapseBlock* block);
    
    /**
     * Cleanup a deallocated synapse
     */
    void cleanupSynapse(SynapseBlock* block, SynapseIndex index);
    
    /**
     * Initialize global mapping tables
     */
    void initializeMappings();
    
    /**
     * Rebuild mapping tables (when blocks change)
     */
    void rebuildMappings();
};

// Global pool instances (managed by Brain)
extern NeuronPool gNeuronPool;
extern SynapsePool gSynapsePool;

// Function to initialize global pools
void initializeGlobalPools(size_t neuronCount = 1024, size_t synapseCount = 1024);

// Function to reset global pools
void resetGlobalPools();

// Function to check if MemoryPool is enabled
bool isMemoryPoolEnabled();

// Legacy allocation function (for backward compatibility)
Neuron* legacyAllocateNeuron();
void legacyDeallocateNeuron(Neuron* neuron);

Synapse* legacyAllocateSynapse();
void legacyDeallocateSynapse(Synapse* synapse);

// Migration helper: switch from std::make_unique to MemoryPool
void enableMemoryPoolMigration(bool enable = true);

} // namespace nlm

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