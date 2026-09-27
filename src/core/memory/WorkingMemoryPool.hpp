#pragma once

/**
 * WorkingMemoryPool - Memory pool for working memory traces
 * 
 * Provides optimized memory allocation for working memory trace objects
 * used for storing active neural representations and cognitive states.
 * 
 * Features:
 * - Pre-allocated working memory traces
 * - Cache-aligned storage
 * - Thread-safe operations
 * - Memory tracking and statistics
 * - Automatic block management
 */

#include <cstddef>
#include <cstdint>
#include <vector>
#include <mutex>
#include <atomic>
#include <memory>

namespace nlm {

/**
 * WorkingMemoryPool - Memory pool for working memory traces
 */
class WorkingMemoryPool {
public:
    WorkingMemoryPool(size_t initialCapacity = 512, size_t maxBlockSize = 2048);
    ~WorkingMemoryPool();
    
    // Disable copying
    WorkingMemoryPool(const WorkingMemoryPool&) = delete;
    WorkingMemoryPool& operator=(const WorkingMemoryPool&) = delete;
    
    /**
     * Working memory trace structure
     */
    struct MemoryTrace {
        NeuronId neuronId;
        float activation;
        float importance;
        float recency;
        uint32_t timestamp;
        bool isActive;
        
        MemoryTrace()
            : neuronId(0), activation(0.0f), importance(0.0f), 
              recency(0.0f), timestamp(0), isActive(false) {}
    };
    
    /**
     * Allocate a memory trace entry
     */
    MemoryTrace* allocateTrace();
    
    /**
     * Deallocate a memory trace entry
     */
    void deallocateTrace(MemoryTrace* trace);
    
    /**
     * Get number of allocated traces
     */
    size_t allocatedCount() const { return allocatedCount_; }
    
    /**
     * Get memory usage in bytes
     */
    size_t memoryUsage() const;
    
    /**
     * Get total objects allocated
     */
    size_t totalObjectsAllocated() const { return totalAllocated_; }
    
    /**
     * Reserve additional traces
     */
    void reserve(size_t traces);
    
    /**
     * Clear pool (keep memory for reuse)
     */
    void clear();
    
    /**
     * Get pool statistics
     */
    struct Statistics {
        size_t allocatedCount;
        size_t memoryUsage;
        size_t totalAllocated;
        float utilization;
    };
    
    Statistics getStatistics() const;
    
private:
    static constexpr size_t CACHE_LINE = 64;
    
    /**
     * Memory block for working memory traces
     */
    struct alignas(CACHE_LINE) MemoryBlock {
        // SoA storage for memory traces
        std::vector<NeuronId> neuronIds;
        std::vector<float> activations;
        std::vector<float> importances;
        std::vector<float> recencies;
        std::vector<uint32_t> timestamps;
        std::vector<bool> isActives;
        
        MemoryBlock() {}
    };
    
    // Memory blocks
    std::vector<std::unique_ptr<MemoryBlock>> blocks_;
    
    // Free list for deallocated traces
    std::vector<MemoryTrace*> freeList_;
    std::atomic<size_t> freeListTop_;
    
    size_t maxBlockSize_;
    size_t totalAllocated_;
    std::atomic<size_t> allocatedCount_;
    
    std::mutex mutex_;
};

} // namespace nlm
