#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Forward declaration
class Neuron;

// Synapse representing a connection between neurons
// Implements real synaptic transmission with delays and short-term plasticity
// Uses pimpl idiom for efficient memory management and implementation hiding

class Synapse {
public:
// Create synapse with IDs
    // @param id Unique identifier for this synapse
    // @param source Source neuron identifier
    // @param destination Target neuron identifier
    Synapse(SynapseId id, NeuronId source, NeuronId destination);
    
    ~Synapse();
    
    // Disable copying, enable moving - synapses are lightweight objects
    // that can be moved but not copied to avoid double ownership of pImpl
    Synapse(const Synapse&) = delete;
    Synapse& operator=(const Synapse&) = delete;
    Synapse(Synapse&&) noexcept;
    Synapse& operator=(Synapse&&) noexcept;
    
    // Identity access
    // @return Unique identifier for this synapse
    SynapseId getId() const;
    
    // Connection information
    // @return Source neuron identifier
    NeuronId getSourceNeuron() const;
    // @return Target/destination neuron identifier
    NeuronId getDestinationNeuron() const;
    
    // Synaptic weight management
    // @return Current synaptic weight (-1.0 to 1.0, negative=inhibitory)
    SynapticWeight getWeight() const;
    // Set synaptic weight
    // @param weight New synaptic weight (clamped to [-1.0, 1.0])
    void setWeight(SynapticWeight weight);
    // Add delta to current weight
    // @param delta Change in synaptic weight
    void addToWeight(SynapticWeight delta);
    
    // Synaptic delay (in simulation steps)
    // @return Current delay in simulation steps
    Delay getDelay() const;
    // Set synaptic delay
    // @param delay Delay in simulation steps (typically 1-5ms at 1ms timestep)
    void setDelay(Delay delay);
    
    // Synapse classification
    // @return Synapse type (Excitatory, Inhibitory, Modulatory, etc.)
    SynapseType getType() const;
    // Set synapse type
    // @param type New synapse type classification
    void setType(SynapseType type);
    
    // Convenience methods for synapse type checking
    // @return true if synapse is excitatory
    bool isExcitatory() const;
    // @return true if synapse is inhibitory
    bool isInhibitory() const;
    
    // Activity history for plasticity
    // Record pre-synaptic spike occurrence
    // @param timestamp Simulation time when pre-synaptic neuron spiked
    void recordPreSpike(Timestamp timestamp);
    // Record post-synaptic spike occurrence
    // @param timestamp Simulation time when post-synaptic neuron spiked
    void recordPostSpike(Timestamp timestamp);
    // @return List of pre-synaptic spike timestamps for STDP calculations
    const std::vector<Timestamp>& getPreSpikeHistory() const;
    // @return List of post-synaptic spike timestamps for STDP calculations
    const std::vector<Timestamp>& getPostSpikeHistory() const;
    // Clear all spike history (used for short-term plasticity reset)
    void clearHistory();
    
    // Plasticity flags
    // @return Constant reference to plasticity flags for read-only access
    const PlasticityFlags& getPlasticityFlags() const;
    // @return Mutable reference to plasticity flags for advanced plasticity control
    PlasticityFlags& getPlasticityFlags();
    
    // Enable/disable specific plasticity rules
    // @param hebbian Enable Hebbian potentiation (careful with weight bounds)
    // @param stdp Enable spike-timing-dependent plasticity
    // @param rewardModulated Enable reward-modulated learning with eligibility traces
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    // Eligibility trace (for reward-modulated learning)
    // @return Current eligibility trace value (decays over time)
    float getEligibilityTrace() const;
    // Set eligibility trace (used when reward is received)
    // @param trace New eligibility trace value
    void setEligibilityTrace(float trace);
    // Decay eligibility trace exponentially (called automatically during step)
    // @param decayRate Decay rate per timestep (typically 0.001)
    void decayEligibilityTrace(float decayRate);
    
    // Synaptic efficacy (use-dependent modulation)
    // @return Current synaptic efficacy (0.0 to 2.0, scales weight)
    float getEfficacy() const;
    // Set synaptic efficacy (modulates synaptic strength independently of weight)
    // @param efficacy New efficacy value (clamped to [0.0, 2.0])
    void setEfficacy(float efficacy);
    
    // Update synapse for one simulation step
    // Implements short-term plasticity (Tsodyks-Markram model) and eligibility trace decay
    // @param currentTime Simulation time in seconds
    void step(Timestamp currentTime);
    
    // Reset to initial state
    void reset();
    
    // Initialize with biologically realistic random parameters
    // Based on synapse type and biological constraints
    // @param rng Random number generator for parameter sampling
    void initializeRandom(class RandomGenerator& rng);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
