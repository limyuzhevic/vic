#pragma once

#include "../core/Types/Types.hpp"
#include "../performance/MemoryPool.hpp"
#include <vector>

namespace nlm {

// Forward declaration
class Neuron;
class RandomGenerator;

// Synapse representing a connection between neurons
// Implements real synaptic transmission with delays and short-term plasticity

class Synapse {
public:
    // Create synapse using memory pool
    static Synapse* createSynapse(SynapseId id, NeuronId source, NeuronId destination);
    
    // Destroy synapse
    static void destroySynapse(Synapse* synapse);
    
    ~Synapse();
    
    // Disable copying, enable moving
    Synapse(const Synapse&) = delete;
    Synapse& operator=(const Synapse&) = delete;
    Synapse(Synapse&&) noexcept;
    Synapse& operator=(Synapse&&) noexcept;
    
    // Identity
    SynapseId getId() const;
    
    // Connection
    NeuronId getSourceNeuron() const;
    NeuronId getDestinationNeuron() const;
    
    // Weight
    SynapticWeight getWeight() const;
    void setWeight(SynapticWeight weight);
    void addToWeight(SynapticWeight delta);
    
    // Delay (in simulation steps)
    Delay getDelay() const;
    void setDelay(Delay delay);
    
    // Type
    SynapseType getType() const;
    void setType(SynapseType type);
    
    // Excitatory/Inhibitory
    bool isExcitatory() const;
    bool isInhibitory() const;
    
    // Activity history for plasticity
    void recordPreSpike(Timestamp timestamp);
    void recordPostSpike(Timestamp timestamp);
    const std::vector<Timestamp>& getPreSpikeHistory() const;
    const std::vector<Timestamp>& getPostSpikeHistory() const;
    void clearHistory();
    
    // Plasticity flags
    const PlasticityFlags& getPlasticityFlags() const;
    PlasticityFlags& getPlasticityFlags();
    
    // Enable/disable plasticity rules
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    // Eligibility trace (for reward-modulated learning)
    float getEligibilityTrace() const;
    void setEligibilityTrace(float trace);
    void decayEligibilityTrace(float decayRate);
    
    // Synaptic efficacy (use-dependent modulation)
    float getEfficacy() const;
    void setEfficacy(float efficacy);
    
    // Update synapse for one simulation step
    // TODO PHASE 2: Implement real synaptic dynamics
    void step(Timestamp currentTime);
    
    // Reset to initial state
    void reset();
    
    // Initialize with random parameters
    void initializeRandom(RandomGenerator& rng);
    
private:
    struct Impl;
    Impl* pImpl;
    
    // Friend class for memory management
    friend class SynapsePoolManager;
};

/**
 * Synapse memory pool manager
 */
class SynapsePoolManager {
public:
    static SynapsePoolManager& getInstance() {
        static SynapsePoolManager instance;
        return instance;
    }
    
    Synapse* createSynapse(SynapseId id, NeuronId source, NeuronId destination) {
        Synapse* synapse = pool_.allocate();
        if (synapse) {
            // Initialize synapse state
            new (&synapse->pImpl) Synapse::Impl();
            // Set initial values
            synapse->pImpl->id = id;
            synapse->pImpl->sourceNeuron = source;
            synapse->pImpl->destinationNeuron = destination;
            synapse->pImpl->weight = 0.0f;
            synapse->pImpl->delay = 1;
            synapse->pImpl->type = SynapseType::Excitatory;
            synapse->pImpl->eligibilityTrace = 0.0f;
            synapse->pImpl->efficacy = 1.0f;
            synapse->pImpl->shortTermDepression = 1.0f;
            synapse->pImpl->shortTermFacilitation = 0.0f;
            synapse->pImpl->lastPreSpikeTime = -1.0f;
            synapse->pImpl->lastPostSpikeTime = -1.0f;
        }
        return synapse;
    }
    
    void destroySynapse(Synapse* synapse) {
        if (synapse) {
            synapse->~Synapse();
            pool_.deallocate(synapse);
        }
    }
    
    void reserveSynapses(size_t count) {
        pool_.reserve(count);
    }
    
    size_t getAllocatedCount() const {
        return pool_.allocatedCount();
    }
    
    size_t getAvailableCount() const {
        return pool_.availableCount();
    }
    
    size_t getMemoryUsage() const {
        return pool_.memoryUsage();
    }

private:
    SynapsePoolManager() = default;
    ~SynapsePoolManager() = default;
    
    // Disable copying
    SynapsePoolManager(const SynapsePoolManager&) = delete;
    SynapsePoolManager& operator=(const SynapsePoolManager&) = delete;
    
    MemoryPool<Synapse> pool_;
};

} // namespace nlm
