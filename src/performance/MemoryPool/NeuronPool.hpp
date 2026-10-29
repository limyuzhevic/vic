// NeuronPool - Specialized memory pool for neurons with cache-aligned states
// 
// Provides pre-allocated neuron objects with SoA (Structure of Arrays) layout
// for improved cache locality and SIMD optimization.
// 
// Features:
// - Cache-aligned memory layout
// - SoA storage for efficient SIMD operations
// - Pre-allocated blocks for reduced allocation overhead
// - Lock-free allocation/deallocation for thread safety
// - Direct access to neuron state members for hot path optimization
// 
// Biological motivation:
// - Neurons maintain fixed numbers in many biological systems
// - Similar to how biological neurons have consistent state structures
// - Cache-friendly access patterns mimic neural connectivity

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
 * NeuronPool - Specialized pool for neurons with cache-aligned states
 * 
 * Provides efficient storage and access to neuron objects using SoA (Structure of Arrays)
 * layout for optimal cache locality and SIMD optimization.
 */
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
    
    // Get neuron by index (for iteration)
    NeuronId getNeuronId(NeuronIndex index) const;
    
    // Reset pool (clear all neurons)
    void clear();
    
private:
    static constexpr size_t CACHE_LINE = 64;
    
    struct alignas(CACHE_LINE) alignas(sizeof(void*)) NeuronBlock {
        // SoA storage for neuron states - all members are vectors for cache efficiency
        std::vector<float> membranePotential;
        std::vector<float> restingPotential;
        std::vector<float> threshold;
        std::vector<float> resetPotential;
        std::vector<float> leakConductance;
        std::vector<float> synapseConductance;
        std::vector<float> firingRate;
        std::vector<uint8_t> firingState;  // FiringState enum (1 byte)
        std::vector<uint32_t> refractoryRemaining;
        std::vector<uint32_t> refractoryPeriod;
        std::vector<float> adaptationVariable;
        std::vector<float> lastSpikeTime;
        std::vector<uint64_t> neuronType;  // NeuronType enum (uint64_t for alignment)
        std::vector<uint64_t> regionId;
        std::vector<uint64_t> populationId;
        
        // Per-neuron current injection (not in NeuronState, but needed for hot path)
        std::vector<float> synapticInput;
        
        NeuronBlock() : membranePotential(0), restingPotential(0),
                       threshold(0), resetPotential(0), leakConductance(0),
                       synapseConductance(0), firingRate(0), firingState(0),
                       refractoryRemaining(0), refractoryPeriod(0), adaptationVariable(0),
                       lastSpikeTime(0), neuronType(0), regionId(0), populationId(0),
                       synapticInput(0) {}
    };
    
    // Memory management
    size_t size_;
    size_t capacity_;
    std::vector<std::unique_ptr<NeuronBlock>> blocks_;
    
    // Free list for deallocated neurons
    std::vector<NeuronIndex> freeList_;
    std::atomic<size_t> freeListTop_;
    
    // Current block and index for allocation
    std::atomic<NeuronBlock*> currentBlock_;
    std::atomic<size_t> currentIndex_;
    
    // Statistics
    std::atomic<size_t> allocatedCount_;
}; 

} // namespace nlm
