#pragma once

/**
 * SynapseMemoryPool - Specialized memory pool for synapse objects
 * 
 * Provides optimized memory allocation for Synapse objects with
 * cache-friendly data layout and tracking of synapse-specific metadata.
 * 
 * Features:
 * - Pre-allocated synapse objects
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
class Synapse;

/**
 * SynapseMemoryPool - Specialized pool for synapses with cache-aligned states
 */
class SynapseMemoryPool {
public:
    SynapseMemoryPool(size_t initialSynapses = 4096, size_t maxBlockSize = 8192);
    ~SynapseMemoryPool();
    
    // Disable copying
    SynapseMemoryPool(const SynapseMemoryPool&) = delete;
    SynapseMemoryPool& operator=(const SynapseMemoryPool&) = delete;
    
    // Allocate a synapse with its state
    SynapseId allocate(NeuronId source, NeuronId destination);
    
    // Deallocate a synapse
    void deallocate(SynapseId id);
    
    // Access synapse state directly (for SoA access)
    template<typename Member>
    Member* getState(SynapseIndex index);
    
    // Get source/destination neurons
    NeuronId getSource(SynapseIndex index) const;
    NeuronId getDestination(SynapseIndex index) const;
    
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
    
    // Free list for deallocated synapses
    std::vector<SynapseIndex> freeList_;
    std::atomic<size_t> freeListTop_;
    
    size_t initialSynapses_;
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
