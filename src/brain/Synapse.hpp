#pragma once

#include "../core/Types/Types.hpp"
#include <vector>

namespace nlm {

// Forward declaration
class Neuron;

// Synapse representing a connection between neurons
// Implements real synaptic transmission with delays and short-term plasticity
// 
// This is the fundamental communication mechanism in the NLM brain. Synapses
// transmit spikes from pre-synaptic to post-synaptic neurons with realistic
// dynamics including synaptic delays, short-term plasticity, and eligibility traces
// for learning.
// 
// Synapse State:
// - Weight: Synaptic efficacy (learning parameter)
// - Delay: Transmission delay in simulation steps
// - Type: Excitatory or inhibitory (functional type)
// - Plasticity flags: Which plasticity rules are enabled
// - Eligibility trace: For reward-modulated learning
// - Efficacy: Use-dependent modulation for short-term plasticity
// - Short-term depression/facilitation: Dynamic synaptic modifications
// - Spike histories: For STDP and other spike timing-dependent processes
// 
// Key Features:
// - Realistic synaptic transmission with biophysical constraints
// - Multiple plasticity mechanisms (STDP, Hebbian, neuromodulated)
// - Short-term plasticity (STP) for dynamic gain control
// - Eligibility traces for credit assignment in reinforcement learning
// - Delayed spike propagation for realistic neural timing
// - Integration with brain-level learning and development systems
// 
// The synapse model supports:
// - Spike-timing dependent plasticity (STDP) for temporal learning
// - Reward-modulated plasticity for reinforcement learning
// - Hebbian co-activation learning for pattern association
// - Structural plasticity for connection creation/removal
// - Short-term dynamics for burst detection and filtering
// - Online learning during ongoing neural activity

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
    // Phase 5 checkpoint support: restore histories recorded by
    // recordPre/PostSpike (capped at the same MAX_SPIKE_HISTORY).
    void setPreSpikeHistory(const std::vector<Timestamp>& history);
    void setPostSpikeHistory(const std::vector<Timestamp>& history);
    
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
