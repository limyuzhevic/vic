#pragma once

#include "../core/Types/Types.hpp"
#include <memory>

namespace nlm {

// Memory pool integration constants
constexpr size_t DEFAULT_NEURON_POOL_SIZE = 1024;
constexpr size_t DEFAULT_SYNAPSE_POOL_SIZE = 4096;
constexpr bool DEFAULT_USE_MEMORY_POOLS = true;

// NeuronPool - Specialized pool for neurons with cache-aligned states
class NeuronPool {
public:
    NeuronPool(size_t initialCapacity = DEFAULT_NEURON_POOL_SIZE);
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
    
    // Initialize the pool with a given capacity
    void reserve(size_t capacity);
    
    // Check if using memory pool
    bool isEnabled() const { return useMemoryPool_; }
    
    // Enable/disable memory pool
    void setEnabled(bool enabled) { useMemoryPool_ = enabled; }
    
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
    
    // Enable/disable memory pool
    bool useMemoryPool_;
};

// SynapsePool - Specialized pool for synapses
class SynapsePool {
public:
    SynapsePool(size_t initialCapacity = DEFAULT_SYNAPSE_POOL_SIZE);
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
    
    // Initialize the pool with a given capacity
    void reserve(size_t capacity);
    
    // Check if using memory pool
    bool isEnabled() const { return useMemoryPool_; }
    
    // Enable/disable memory pool
    void setEnabled(bool enabled) { useMemoryPool_ = enabled; }
    
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
    
    // Enable/disable memory pool
    bool useMemoryPool_;
};

// Helper function to get NeuronPool from SynapsePool
inline NeuronPool& getNeuronPool(SynapsePool& pool) {
    return *reinterpret_cast<NeuronPool*>(&pool);
}

inline const NeuronPool& getNeuronPool(const SynapsePool& pool) {
    return *reinterpret_cast<const NeuronPool*>(&pool);
}

// Memory pool configuration structure
struct MemoryPoolConfig {
    bool enabled = DEFAULT_USE_MEMORY_POOLS;
    size_t neuronPoolSize = DEFAULT_NEURON_POOL_SIZE;
    size_t synapsePoolSize = DEFAULT_SYNAPSE_POOL_SIZE;
    
    // Load configuration from Config object
    static MemoryPoolConfig fromConfig(const Config& config);
    
    // Apply configuration to Brain
    void applyToBrain(Brain& brain) const;
};

} // namespace nlm
