#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Synapse representing a connection between neurons
// Implements real synaptic transmission with:
// - Synaptic delays (1-5 simulation steps)
// - Short-term plasticity (Tsodyks-Markram model)
// - Spike-timing-dependent plasticity (STDP)
// - Reward-modulated learning
// - Synaptic efficacy modulation
// - Biological weight bounds (-1.0 to 1.0)

class Synapse {
public:
    // Create synapse with unique identifiers
    // @param id: Unique synapse identifier
    // @param source: Source neuron identifier (pre-synaptic)
    // @param destination: Destination neuron identifier (post-synaptic)
    Synapse(SynapseId id, NeuronId source, NeuronId destination);
    
    // Destructor
    ~Synapse();
    
    // Disable copying, enable moving
    Synapse(const Synapse&) = delete;
    Synapse& operator=(const Synapse&) = delete;
    // Move constructor with exception safety
    Synapse(Synapse&&) noexcept;
    // Move assignment operator with self-assignment protection
    Synapse& operator=(Synapse&&) noexcept;
    
    // Identity and identification
    // @return SynapseId: Unique identifier of this synapse
    SynapseId getId() const;
    
    // Connection management
    // @return NeuronId: Source neuron identifier (pre-synaptic)
    NeuronId getSourceNeuron() const;
    // @return NeuronId: Destination neuron identifier (post-synaptic)
    NeuronId getDestinationNeuron() const;
    
    // Synaptic weight management
    // @return SynapticWeight: Current synaptic weight (-1.0 to 1.0)
    SynapticWeight getWeight() const;
    // Set absolute synaptic weight
    // @param weight: New synaptic weight (-1.0 to 1.0)
    void setWeight(SynapticWeight weight);
    // Modify synaptic weight by delta (preserves bounds)
    // @param delta: Weight change (positive for LTP, negative for LTD)
    void addToWeight(SynapticWeight delta);
    
    // Synaptic delay (transmission timing)
    // @return Delay: Synaptic delay in simulation steps (1-5 typical)
    Delay getDelay() const;
    // Set synaptic transmission delay
    // @param delay: Delay in simulation steps (minimum 1)
    void setDelay(Delay delay);
    
    // Synapse type and classification
    // @return SynapseType: Excitatory, inhibitory, or modulatory
    SynapseType getType() const;
    // Set synapse type
    // @param type: Synapse type to set
    void setType(SynapseType type);
    
    // Excitatory/Inhibitory classification
    // @return true if synapse releases excitatory neurotransmitters
    bool isExcitatory() const;
    // @return true if synapse releases inhibitory neurotransmitters
    bool isInhibitory() const;
    
    // Activity history for plasticity and learning
    // Record pre-synaptic spike for STDP
    // @param timestamp: Time when pre-synaptic neuron spiked
    void recordPreSpike(Timestamp timestamp);
    // Record post-synaptic spike for STDP
    // @param timestamp: Time when post-synaptic neuron spiked
    void recordPostSpike(Timestamp timestamp);
    // Get pre-synaptic spike history
    // @return Vector of pre-synaptic spike timestamps (most recent first)
    const std::vector<Timestamp>& getPreSpikeHistory() const;
    // Get post-synaptic spike history
    // @return Vector of post-synaptic spike timestamps (most recent first)
    const std::vector<Timestamp>& getPostSpikeHistory() const;
    // Clear spike history (used for synaptic reset)
    void clearHistory();
    
    // Plasticity rules configuration
    // Get current plasticity flags
    // @return Const reference to plasticity flags
    const PlasticityFlags& getPlasticityFlags() const;
    // Get non-const reference to plasticity flags
    // @return Non-const reference to plasticity flags
    PlasticityFlags& getPlasticityFlags();
    // Enable specific plasticity mechanisms
    // @param hebbian: Enable Hebbian learning (correlation-based)
    // @param stdp: Enable Spike-Timing-Dependent Plasticity
    // @param rewardModulated: Enable reward-modulated learning (dopamine)
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    // Eligibility trace (for reward-modulated learning)
    // @return float: Current eligibility trace value (decays over time)
    float getEligibilityTrace() const;
    // Set eligibility trace (used by reward-modulated learning)
    // @param trace: New eligibility trace value
    void setEligibilityTrace(float trace);
    // Decay eligibility trace (called each simulation step)
    // @param decayRate: Fraction to decay (typically 0.001 for fast decay)
    void decayEligibilityTrace(float decayRate);
    
    // Synaptic efficacy (use-dependent modulation)
    // @return float: Current synaptic efficacy (0.0 to 2.0)
    float getEfficacy() const;
    // Set synaptic efficacy (modulates weight changes)
    // @param efficacy: Efficacy factor (higher = stronger learning)
    void setEfficacy(float efficacy);
    
    // Update synapse for one simulation step
    // Implements synaptic dynamics:
    // - Short-term plasticity (depression/facilitation)
    // - Eligibility trace decay
    // - Efficacy modulation
    // - Weight bounds enforcement
    // @param currentTime: Current simulation time in ms
    void step(Timestamp currentTime);
    
    // Reset to initial state
    // Clears: weight, spike history, eligibility trace, efficacy
    void reset();
    
    // Initialize with random parameters
    // Sets biologically plausible initial conditions:
    // - Excitatory/inhibitory specific weights
    // - Random delays (1-5 steps)
    // - Initial short-term plasticity state
    // - Zero eligibility trace
    // @param rng: Random number generator for initialization
    void initializeRandom(class RandomGenerator& rng);
    
private:
    // PIMPL idiom for implementation hiding
    // Synaptic transmission and plasticity details are stored here
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm

} // namespace nlm
