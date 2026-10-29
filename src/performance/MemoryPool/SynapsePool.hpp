// SynapsePool - Specialized memory pool for synapses
// 
// Provides pre-allocated synapse objects with SoA (Structure of Arrays) layout
// optimized for efficient access patterns during synaptic transmission.
// 
// Features:
// - Cache-aligned memory layout
// - SoA storage for efficient access to synapse properties
// - Pre-allocated blocks for reduced allocation overhead
// - Lock-free allocation/deallocation for thread safety
// - Direct access to synaptic state for hot path optimization
// 
// Biological motivation:
// - Synapses maintain relatively stable numbers in neural circuits
// - Similar to biological synaptic structures that have consistent properties
// - Cache-friendly access patterns match synaptic transmission patterns

#pragma once

#include "MemoryPoolBase.hpp"
#include "../core/Types/Types.hpp"
#include <cstddef>
#include <cstdint>
#include <vector>
#include <memory>
#include <atomic>

namespace nlm {

/**
 * SynapsePool - Specialized pool for synapses with SoA layout
 * 
 * Provides efficient storage and access to synapse objects using SoA layout
 * for optimal cache locality and performance during synaptic transmission.
 */
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
    
    // Get total count
    size_t size() const { return size_; }
    
    // Memory usage
    size_t memoryUsage() const;
    
    // Get synapse by index (for iteration)
    SynapseId getSynapseId(SynapseIndex index) const;
    
    // Reset pool (clear all synapses)
    void clear();
    
    // Validate synapse indices
    bool isValid(SynapseIndex index) const { return index < size_; }
    
private:
    static constexpr size_t CACHE_LINE = 64;
    
    struct alignas(CACHE_LINE) SynapseBlock {
        // SoA storage for synapse states - all members are vectors for cache efficiency
        std::vector<uint64_t> sourceNeuron;  // Source neuron ID (uint64_t for alignment)
        std::vector<uint64_t> destinationNeuron;  // Destination neuron ID (uint64_t for alignment)
        std::vector<float> weight;
        std::vector<uint32_t> delay;
        std::vector<uint8_t> synapseType;  // SynapseType enum (1 byte)
        std::vector<uint8_t> plasticityFlags;  // Packed flags (1 byte)
        
        // Short-term plasticity state
        std::vector<float> shortTermDepression;
        std::vector<float> shortTermFacilitation;
        std::vector<float> lastPreSpikeTime;
        std::vector<float> lastPostSpikeTime;
        
        // Eligibility trace for learning
        std::vector<float> eligibilityTrace;
        
        // Synaptic efficacy
        std::vector<float> efficacy;
        
        SynapseBlock() : sourceNeuron(0), destinationNeuron(0), weight(0),
                        delay(0), synapseType(0), plasticityFlags(0),
                        shortTermDepression(0), shortTermFacilitation(0),
                        lastPreSpikeTime(0), lastPostSpikeTime(0),
                        eligibilityTrace(0), efficacy(0) {}
    };
    
    // Memory management
    size_t size_;
    size_t capacity_;
    std::vector<std::unique_ptr<SynapseBlock>> blocks_;
    std::vector<SynapseIndex> freeList_;
    std::atomic<size_t> freeListTop_;
    
    // Current block and index for allocation
    std::atomic<SynapseBlock*> currentBlock_;
    std::atomic<size_t> currentIndex_;
    
    // Statistics
    std::atomic<size_t> allocatedCount_;
    
    // Helper functions
    void addBlock();
    SynapseId createSynapseId(SynapseIndex index) const;
}; 

} // namespace nlm
