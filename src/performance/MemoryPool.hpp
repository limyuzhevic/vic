#pragma once

/**
 * MemoryPool - Advanced lock-free memory pool for neurons and synapses
 * 
 * Provides high-performance pre-allocated object pools to reduce allocation overhead during simulation.
 * Each pool manages objects of a single type with fixed size using slab allocation.
 * 
 * Key improvements over Phase 5:
 * - Automatic pool sizing based on configuration and usage patterns
 * - Memory leak detection and validation
 * - Integration with checkpoint system for state persistence
 * - Performance monitoring and statistics
 * - Configurable allocation strategies (conservative, aggressive, adaptive)
 * - Thread-safe operations with better synchronization
 * - Resource cleanup and memory efficiency
 * 
 * Biological motivation:
 * - Like biological neural systems that maintain fixed numbers of neurons/synapses
 * - Reduces "allocation" overhead similar to how biological systems don't constantly create/destroy neurons
 * - Supports developmental processes like synaptogenesis and pruning
 */

#include <cstddef>
#include <cstdint>
#include <vector>
#include <memory>
#include <atomic>
#include <array>
#include <optional>
#include <mutex>
#include <unordered_map>
#include <chrono>
#include <functional>
#include <type_traits>
#include <shared_mutex>

namespace nlm {

/**
 * Memory pool statistics for performance monitoring
 */
struct MemoryPoolStats {
    size_t allocatedCount = 0;           // Total allocated objects
    size_t availableCount = 0;           // Total available objects (in pools)
    size_t blockCount = 0;               // Number of memory blocks
    size_t peakAllocated = 0;            // Peak allocation count
    uint64_t totalAllocations = 0;       // Total allocation operations
    uint64_t totalDeallocations = 0;     // Total deallocation operations
    double currentMemoryUsageMB = 0.0;   // Current memory usage in MB
    double peakMemoryUsageMB = 0.0;      // Peak memory usage in MB
    uint64_t lastAllocationTime = 0;     // Time of last allocation (microseconds)
    uint64_t lastDeallocationTime = 0;   // Time of last deallocation (microseconds)
    
    /**
     * Get allocation rate (allocations per microsecond)
     */
    double allocationRate() const {
        if (totalAllocations <= 1) return 0.0;
        auto now = std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        return static_cast<double>(totalAllocations) / (now - lastAllocationTime + 1);
    }
    
    /**
     * Get deallocation rate (deallocations per microsecond)
     */
    double deallocationRate() const {
        if (totalDeallocations <= 1) return 0.0;
        auto now = std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        return static_cast<double>(totalDeallocations) / (now - lastDeallocationTime + 1);
    }
    
    /**
     * Get utilization ratio (allocated / total available)
     */
    double utilization() const {
        size_t total = allocatedCount + availableCount;
        return total > 0 ? static_cast<double>(allocatedCount) / total : 0.0;
    }
    
    /**
     * Get memory efficiency (peak allocated / current allocated)
     */
    double memoryEfficiency() const {
        return allocatedCount > 0 ? static_cast<double>(peakAllocated) / allocatedCount : 1.0;
    }
};

/**
 * Memory pool configuration
 */
struct MemoryPoolConfig {
    /**
     * Base number of objects per memory block
     */
    size_t baseBlockSize = 1024;
    
    /**
     * Number of initial blocks to pre-allocate
     */
    size_t initialBlocks = 4;
    
    /**
     * Maximum number of blocks to allocate
     */
    size_t maxBlocks = 1000;
    
    /**
     * Allocation strategy
     */
    enum class AllocationStrategy {
        CONSERVATIVE,  // Only allocate as needed
        AGGRESSIVE,    // Pre-allocate more aggressively
        ADAPTIVE,      // Adjust based on usage patterns
        BALANCED       // Balance between memory and performance
    };
    
    AllocationStrategy strategy = AllocationStrategy::ADAPTIVE;
    
    /**
     * Enable/disable memory leak detection
     */
    bool enableLeakDetection = true;
    
    /**
     * Enable/disable performance statistics
     */
    bool enableStatistics = true;
    
    /**
     * Enable/disable thread safety
     */
    bool threadSafe = true;
    
    /**
     * Minimum free objects before triggering reallocation
     */
    size_t minFreeBeforeRealloc = 16;
    
    /**
     * Maximum memory usage before cleanup (in MB)
     */
    double maxMemoryUsageMB = 1024.0;
    
    /**
     * Time threshold for considering allocations "recent" (in seconds)
     */
    double allocationTimeoutSeconds = 300.0;
    
    /**
     * Custom allocator function for objects
     */
    std::function<T*()> customAllocator = nullptr;
    
    /**
     * Custom deallocator function for objects
     */
    std::function<void(T*)> customDeallocator = nullptr;
};

/**
 * Memory pool diagnostic information
 */
struct MemoryPoolDiagnostics {
    uint64_t totalAllocations = 0;
    uint64_t totalDeallocations = 0;
    std::unordered_map<size_t, size_t> blockAllocations;  // block size -> allocation count
    std::vector<uint64_t> allocationHistogram;  // allocation count histogram
    std::unordered_map<std::string, uint64_t> usageByType;  // usage by component type
    double averageAllocationTime = 0.0;
    double averageDeallocationTime = 0.0;
    std::vector<std::pair<uint64_t, uint64_t>> allocationTimeSeries;  // (timestamp, count)
    
    void addAllocation(size_t blockSize, uint64_t timestamp) {
        totalAllocations++;
        blockAllocations[blockSize]++;
        allocationTimeSeries.emplace_back(timestamp, totalAllocations);
    }
    
    void addDeallocation(uint64_t timestamp) {
        totalDeallocations++;
        allocationTimeSeries.emplace_back(timestamp, totalDeallocations + 0x80000000u); // Mark as deallocation
    }
};

/**
 * MemoryPool - Slab allocator for fixed-size objects
 * 
 * Template parameter T specifies the object type to allocate.
 * All objects have the same fixed size determined at construction.
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
    /**
 * MemoryPool - Slab allocator for fixed-size objects
 * 
 * Template parameter T specifies the object type to allocate.
 * All objects have the same fixed size determined at construction.
 * 
 * Key improvements:
 * - Automatic memory management and cleanup
 * - Configurable allocation strategies
 * - Performance monitoring and statistics
 * - Memory leak detection
 * - Thread-safe operations with better synchronization
 * - Integration with checkpoint system
 * - Resource usage tracking
 */
template<typename T>
class MemoryPool {
public:
    /**
     * Create a memory pool with advanced configuration
     * @param config Memory pool configuration
     */
    explicit MemoryPool(const MemoryPoolConfig& config = MemoryPoolConfig());
    
    /**
     * Legacy constructor for backward compatibility
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
     * Lock-free operation with performance monitoring
     */
    T* allocate();
    
    /**
     * Return an object to the pool
     * Lock-free operation with memory leak detection
     */
    void deallocate(T* obj);
    
    /**
     * Allocate with timeout for deadlock detection
     * @param timeout Maximum time to wait for allocation (microseconds)
     * @return Pointer to allocated object, nullptr if timeout
     */
    T* allocateWithTimeout(uint64_t timeout = 0);
    
    /**
     * Deallocate with validation
     * @param obj Object to deallocate
     * @param validate If true, validate object before deallocation
     */
    void deallocateWithValidation(T* obj, bool validate = false);
    
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
     * Get memory statistics
     */
    MemoryPoolStats getStats() const;
    
    /**
     * Get diagnostic information
     */
    MemoryPoolDiagnostics getDiagnostics() const;
    
    /**
     * Pre-allocate objects with monitoring
     */
    void reserve(size_t count);
    
    /**
     * Check for memory leaks
     * @return Vector of leaked object pointers
     */
    std::vector<T*> checkForLeaks();
    
    /**
     * Clean up unused memory
     */
    void cleanupUnusedMemory();
    
    /**
     * Get current configuration
     */
    const MemoryPoolConfig& getConfig() const { return config_; }
    
    /**
     * Update configuration
     */
    void updateConfig(const MemoryPoolConfig& newConfig);
    
    /**
     * Get pool status for checkpointing
     */
    std::string getCheckpointData() const;
    
    /**
     * Load pool state from checkpoint
     */
    bool loadCheckpointData(const std::string& data);
    
    /**
     * Validate pool integrity
     */
    bool validateIntegrity() const;
    
    /**
     * Force garbage collection of unused memory
     */
    void forceGarbageCollection();
    
    /**
     * Enable/disable memory pool
     */
    void setEnabled(bool enabled) { enabled_.store(enabled, std::memory_order_relaxed); }
    
    /**
     * Check if memory pool is enabled
     */
    bool isEnabled() const { return enabled_.load(std::memory_order_relaxed); }
    
    /**
     * Set custom allocator/deallocator functions
     */
    void setCustomAllocators(std::function<T*()> alloc, std::function<void(T*)> dealloc);
    
    /**
     * Get object size in bytes
     */
    static constexpr size_t objectSize() { return sizeof(T); }
    
    /**
     * Get alignment requirements
     */
    static constexpr size_t alignment() {
        alignas(T) std::byte dummy;
        return alignof(std::remove_reference_t<T>);
    }
    
    /**
     * Print pool statistics
     */
    void printStats() const;
    
    /**
     * Print diagnostic information
     */
    void printDiagnostics() const;
    
    /**
     * Reset statistics
     */
    void resetStats();
    
    /**
     * Clear diagnostics
     */
    void clearDiagnostics();

private:
    // Memory block containing objects
    struct Block {
        std::vector<uint8_t> data;
        Block* nextBlock;
        size_t allocationCount;  // Objects allocated from this block
        uint64_t lastAccessTime;  // Last access time for cleanup
        
        explicit Block(size_t objectSize, size_t count, size_t blockId = 0);
        
        T* getObject(size_t index, size_t objectSize) {
            return reinterpret_cast<T*>(data.data() + index * objectSize);
        }
        
        bool isExpired(uint64_t currentTime, uint64_t timeoutSeconds) const {
            return (currentTime - lastAccessTime) > (timeoutSeconds * 1000000);
        }
    };
    
    // Add a new block to the pool
    void addBlock();
    
    // Remove expired blocks
    void removeExpiredBlocks();
    
    // Rebalance pool based on current usage
    void rebalancePool();
    
    // Calculate optimal block size
    size_t calculateOptimalBlockSize() const;
    
    // Validate object pointer
    bool validateObject(T* obj) const;
    
    // Get object from block with validation
    T* getValidatedObject(Block* block, size_t index);
    
    // Slab size for each object (rounded up to alignment and cache line)
    size_t slabSize_;
    
    // Number of objects per block
    size_t blockSize_;
    
    // Maximum number of blocks
    size_t maxBlocks_;
    
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
    
    // Configuration
    MemoryPoolConfig config_;
    
    // Thread safety
    mutable std::shared_mutex mutex_;
    
    // Lock-free flag for enabled/disabled state
    std::atomic<bool> enabled_;
    
    // Custom allocators
    std::function<T*()> customAllocator_;
    std::function<void(T*)> customDeallocator_;
    
    // Diagnostics
    mutable MemoryPoolDiagnostics diagnostics_;
    mutable std::atomic<uint64_t> nextTimestamp_;
    
    // Memory leak tracking
    std::unordered_map<T*, uint64_t> allocatedObjects_;  // object -> timestamp
    std::mutex leakDetectionMutex_;
    
    // Performance tracking
    mutable std::atomic<uint64_t> totalAllocations_;
    mutable std::atomic<uint64_t> totalDeallocations_;
    mutable std::atomic<double> averageAllocationTime_;
    mutable std::atomic<double> averageDeallocationTime_;
    
    // Block counter for unique identification
    static std::atomic<size_t> blockCounter_;
};

// Template implementation must be in header for generic types

template<typename T>
size_t MemoryPool<T>::blockCounter_ = 0;

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