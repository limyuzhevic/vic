#pragma once

#include "../core/Types/Types.hpp"
#include "../core/Errors/Errors.hpp"
#include <vector>
#include <stdexcept>

namespace nlm {

// Forward declaration
class Neuron;

// Synapse representing a connection between neurons
// Implements real synaptic transmission with delays and short-term plasticity
class Synapse {
public:
    // Create synapse with IDs
    Synapse(SynapseId id, NeuronId source, NeuronId destination) throw(SynapseException);
    
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
    void setWeight(SynapticWeight weight) throw(SynapseException);
    void addToWeight(SynapticWeight delta) throw(SynapseException);
    
    // Delay (in simulation steps)
    Delay getDelay() const;
    void setDelay(Delay delay) throw(SynapseException);
    
    // Type
    SynapseType getType() const;
    void setType(SynapseType type) throw(SynapseException);
    
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
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) throw(SynapseException);
    
    // Eligibility trace (for reward-modulated learning)
    float getEligibilityTrace() const;
    void setEligibilityTrace(float trace) throw(SynapseException);
    void decayEligibilityTrace(float decayRate) throw(SynapseException);
    
    // Synaptic efficacy (use-dependent modulation)
    float getEfficacy() const;
    void setEfficacy(float efficacy) throw(SynapseException);
    
    // Update synapse for one simulation step
    // TODO PHASE 2: Implement real synaptic dynamics
    void step(Timestamp currentTime) throw(SynapseException);
    
    // Reset to initial state
    void reset() throw(SynapseException);
    
    // Initialize with random parameters
    void initializeRandom(class RandomGenerator& rng) throw(SynapseException);
    
    // Validation methods
    void validateState() const throw(SynapseException);
    void validateWeight(SynapticWeight weight) const throw(SynapseException);
    void validateDelay(Delay delay) const throw(SynapseException);
    void validateTimestamp(Timestamp timestamp) const throw(SynapseException);
    
private:
    struct Impl;
    Impl* pImpl;
    
    // Private helper methods
    static constexpr SynapticWeight MIN_WEIGHT = -1.0f;
    static constexpr SynapticWeight MAX_WEIGHT = 1.0f;
    static constexpr Delay MIN_DELAY = 0;
    static constexpr Delay MAX_DELAY = 10000;
};

} // namespace nlm
