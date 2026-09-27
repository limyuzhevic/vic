#pragma once

/**
 * NeuronMemoryPool - Specialized memory pool for neuron objects
 * 
 * Provides optimized memory allocation for Neuron objects with
 * cache-friendly data layout and tracking of neuron-specific metadata.
 * 
 * Features:
 * - Pre-allocated neuron objects
 * - Cache-aligned storage for better performance
 * - Thread-safe allocation/deallocation
 * - Memory statistics tracking
 * - Automatic block management
 */

#include <cstddef>
#include <cstdint>
#include <vector>
#include <mutex>
#include <atomic>
#include <memory>

namespace nlm {

// Forward declaration
class Neuron;

/**
 * NeuronMemoryPool - Specialized pool for neurons with cache-aligned states
 */
class NeuronMemoryPool {
public:
    NeuronMemoryPool(size_t initialNeurons = 1024, size_t maxBlockSize = 4096);
    ~NeuronMemoryPool();
    
    // Disable copying
    NeuronMemoryPool(const NeuronMemoryPool&) = delete;
    NeuronMemoryPool& operator=(const NeuronMemoryPool&) = delete;
    
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
    
    // Pool management
    void expandBlock();
    size_t calculateBlockSize();
    
    // Statistics and diagnostics
    float getCacheEfficiency() const;
    void cleanup();
    
private:
    static constexpr size_t CACHE_LINE = 64;
    
    struct alignas(CACHE_LINE) NeuronBlock {
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
    
    size_t initialNeurons_;
    size_t maxBlockSize_;
    size_t totalAllocated_;
    size_t blocksAllocated_;
    std::atomic<size_t> allocatedCount_;
    
    // Statistics
    std::atomic<size_t> cacheHits_;
    std::atomic<size_t> cacheMisses_;
    
    std::mutex mutex_;
};

} // namespace nlm