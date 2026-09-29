#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Synapse class representing a synaptic connection between neurons
// Implements real synaptic transmission with delays and short-term plasticity (STP)
// This class models biological synapses including weight dynamics, spike-timing
// dependent plasticity (STDP), and use-dependent efficacy modulation

class Synapse {
public:
    // Create synapse with unique identifier and connection
    // source: presynaptic neuron ID that sends the signal
    // destination: postsynaptic neuron ID that receives the signal
    Synapse(SynapseId id, NeuronId source, NeuronId destination);
    
    ~Synapse();
    
    // Disable copying for memory safety, enable moving semantics
    Synapse(const Synapse&) = delete;
    Synapse& operator=(const Synapse&) = delete;
    Synapse(Synapse&&) noexcept;
    Synapse& operator=(Synapse&&) noexcept;
    
    // Identity management
    // Returns unique synapse identifier
    SynapseId getId() const;
    
    // Connection specification
    // Returns presynaptic neuron that generates the signal
    NeuronId getSourceNeuron() const;
    // Returns postsynaptic neuron that receives the signal
    NeuronId getDestinationNeuron() const;
    
    // Synaptic weight management
    // Returns current synaptic weight (-1.0 to 1.0, negative= inhibitory)
    SynapticWeight getWeight() const;
    // Sets absolute synaptic weight
    void setWeight(SynapticWeight weight);
    // Adds delta to current weight (preserves sign)
    void addToWeight(SynapticWeight delta);
    
    // Synaptic transmission timing
    // Returns delay in simulation steps (typical: 1-5 steps = 1-5ms)
    Delay getDelay() const;
    // Sets synaptic transmission delay
    void setDelay(Delay delay);
    
    // Synapse type classification
    // Returns synapse type determining signaling characteristics
    SynapseType getType() const;
    // Sets synapse type (Excitatory, Inhibitory, Modulatory)
    void setType(SynapseType type);
    
    // Functional classification
    // Returns true if synapse is excitatory (depolarizing)
    bool isExcitatory() const;
    // Returns true if synapse is inhibitory (hyperpolarizing)
    bool isInhibitory() const;
    
    // Activity history for spike-timing-dependent plasticity (STDP)
    // Records presynaptic spike for STDP eligibility calculation
    void recordPreSpike(Timestamp timestamp);
    // Records postsynaptic spike for STDP eligibility calculation
    void recordPostSpike(Timestamp timestamp);
    // Returns presynaptic spike history (last ~100 spikes)
    const std::vector<Timestamp>& getPreSpikeHistory() const;
    // Returns postsynaptic spike history (last ~100 spikes)
    const std::vector<Timestamp>& getPostSpikeHistory() const;
    // Clears all spike history (resets STDP eligibility)
    void clearHistory();
    
    // Plasticity configuration
    // Returns current plasticity flags for modification rules
    const PlasticityFlags& getPlasticityFlags() const;
    // Returns mutable plasticity flags for modification
    PlasticityFlags& getPlasticityFlags();
    
    // Enable/disable plasticity mechanisms
    // hebbian: enables activity-dependent strengthening
    // stdp: enables spike-timing-dependent plasticity
    // rewardModulated: enables reward-modulated plasticity
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    // Reward-modulated learning (eligibility traces)
    // Returns current eligibility trace value (accumulates for reward modulation)
    float getEligibilityTrace() const;
    // Sets absolute eligibility trace value
    void setEligibilityTrace(float trace);
    // Decays eligibility trace based on time constant
    void decayEligibilityTrace(float decayRate);
    
    // Synaptic efficacy (use-dependent modulation)
    // Returns current synaptic efficacy (0.0 to 2.0, scales weight changes)
    float getEfficacy() const;
    // Sets synaptic efficacy (affects learning strength)
    void setEfficacy(float efficacy);
    
    // Update synapse for one simulation step
    // Implements complete synaptic dynamics including:
    // 1. Short-term plasticity (STP) state decay
    // 2. Eligibility trace decay
    // 3. Efficacy updates based on usage
    // 4. Weight bounds clamping
    void step(Timestamp currentTime);
    
    // Reset synapse to initial state
    // Clears all activity history and resets parameters
    void reset();
    
    // Initialize synapse with biologically realistic random parameters
    // Sets random weight, delay, and plasticity state based on synapse type
    void initializeRandom(class RandomGenerator& rng);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
