#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Forward declaration
class Neuron;

// Synapse representing a connection between neurons
// Implements real synaptic transmission with delays and short-term plasticity
// Phase 6: Integrated with neuromodulation and reward systems
/**
 * @class Synapse
 * @brief Represents a synaptic connection between two neurons
 * 
 * The Synapse class implements realistic synaptic transmission in the NLM brain,
 * including synaptic delays, short-term plasticity, and integration with
 * neuromodulatory systems. Synapses are the fundamental connections that allow
 * neurons to communicate and form complex neural circuits.
 * 
 * Key features:
 * - Synaptic delay modeling (1-5 simulation steps)
 * - Short-term plasticity (facilitation and depression)
 * - Eligibility traces for reward-modulated learning
 * - Integration with neuromodulation (dopamine, acetylcholine, etc.)
 * - Structural plasticity (creation, modification, pruning)
 * - Hebbian and STDP learning rule integration
 * - Real synaptic weight dynamics with bounds
 * 
 * The synapse dynamics are event-driven and integrated with the brain's
 * spike-based computation, allowing for realistic neural network behavior.
 * 
 * @note This is a Phase 6 component - part of the final integrated artificial brain
 * @see Neuron, StructuralPlasticity, Neuromodulator
 */
class Synapse {
public:
    // Create synapse with IDs
    Synapse(SynapseId id, NeuronId source, NeuronId destination);
    
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
    void initializeRandom(class RandomGenerator& rng);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
