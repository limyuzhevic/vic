#pragma once

/**
 * PlasticityPool - Memory pool for plasticity state structures
 * 
 * Provides optimized memory allocation for plasticity state objects
 * used for storing synaptic plasticity information and eligibility traces.
 * 
 * Features:
 * - Pre-allocated plasticity states
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
 * PlasticityPool - Memory pool for plasticity state structures
 */
class PlasticityPool {
public:
    PlasticityPool(size_t initialSize = 2048, size_t maxBlockSize = 4096);
    ~PlasticityPool();
    
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
    
    /**
     * Allocate a plasticity state
     */
    PlasticityState* allocateState();
    
    /**
     * Deallocate a plasticity state
     */
    void deallocateState(PlasticityState* state);
    
    /**
     * Get number of allocated states
     */
    size_t allocatedCount() const { return allocatedCount_; }
    
    /**
     * Get memory usage in bytes
     */
    size_t memoryUsage() const;
    
    /**
     * Reserve additional states
     */
    void reserve(size_t states);
    
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
     * Memory block for plasticity states
     */
    struct alignas(CACHE_LINE) PlasticityBlock {
        // SoA storage for plasticity states
        std::vector<float> weights;
        std::vector<float> eligibilityTraces;
        std::vector<float> preSpikeTimes;
        std::vector<float> postSpikeTimes;
        std::vector<float> learningRates;
        std::vector<float> neuromodulationFactors;
        
        PlasticityBlock() {}
    };
    
    // Memory blocks
    std::vector<std::unique_ptr<PlasticityBlock>> blocks_;
    
    // Free list for deallocated states
    std::vector<PlasticityState*> freeList_;
    std::atomic<size_t> freeListTop_;
    
    size_t maxBlockSize_;
    size_t totalAllocated_;
    std::atomic<size_t> allocatedCount_;
    
    std::mutex mutex_;
};

} // namespace nlm
