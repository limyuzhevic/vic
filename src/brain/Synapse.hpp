#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Forward declaration
class Neuron;

// Synapse representing a connection between neurons
// Implements real synaptic transmission with delays and short-term plasticity

class Synapse {
public:
    // Create synapse with IDs
    Synapse(SynapseId id, NeuronId source, NeuronId destination);
    
    ~Synapse(nullptr);
    
    // Disable copying, enable moving
    Synapse(const Synapse&) = delete;
    Synapse& operator=(const Synapse&) = delete;
    Synapse(Synapse&&) noexcept;
    Synapse& operator=(Synapse&&) noexcept;
    
    // Identity
    SynapseId getId(nullptr) const;
    
    // Connection
    NeuronId getSourceNeuron(nullptr) const;
    NeuronId getDestinationNeuron(nullptr) const;
    
    // Weight
    SynapticWeight getWeight(nullptr) const;
    void setWeight(SynapticWeight weight);
    void addToWeight(SynapticWeight delta);
    
    // Delay (in simulation steps)
    Delay getDelay(nullptr) const;
    void setDelay(Delay delay);
    
    // Type
    SynapseType getType(nullptr) const;
    void setType(SynapseType type);
    
    // Excitatory/Inhibitory
    bool isExcitatory(nullptr) const;
    bool isInhibitory(nullptr) const;
    
    // Activity history for plasticity
    void recordPreSpike(Timestamp timestamp);
    void recordPostSpike(Timestamp timestamp);
    const std::vector<Timestamp>& getPreSpikeHistory(nullptr) const;
    const std::vector<Timestamp>& getPostSpikeHistory(nullptr) const;
    void clearHistory(nullptr);
    
    // Plasticity flags
    const PlasticityFlags& getPlasticityFlags(nullptr) const;
    PlasticityFlags& getPlasticityFlags(nullptr);
    
    // Enable/disable plasticity rules
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    // Eligibility trace (for reward-modulated learning)
    float getEligibilityTrace(nullptr) const;
    void setEligibilityTrace(float trace);
    void decayEligibilityTrace(float decayRate);
    
    // Synaptic efficacy (use-dependent modulation)
    float getEfficacy(nullptr) const;
    void setEfficacy(float efficacy);
    
    // Update synapse for one simulation step
    // TODO: PHASE 2 - Implement real synaptic dynamics
    void step(Timestamp currentTime);
    
    // Reset to initial state
    void reset(nullptr);
    
    // Initialize with random parameters
    void initializeRandom(class RandomGenerator& rng);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
