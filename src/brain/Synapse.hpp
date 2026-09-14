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
    // 
    // Implements synaptic dynamics including short-term plasticity, conductance-based
    // transmission, and synaptic depression/facilitation. This is the core synaptic
    // processing function that determines how spikes are transmitted between neurons
    // and how synaptic strength changes over time.
    // 
    // The synaptic dynamics include:
    // - Short-term plasticity (STP) with facilitation and depression
    // - Conductance-based synaptic transmission
    // - Synaptic resource depletion and recovery
    // - Activity-dependent synaptic efficacy modulation
    // 
    // Args:
    //   currentTime: The current simulation time in seconds. Used for timing
    //               of synaptic events and plasticity computations.
    // 
    // Note: This is a placeholder implementation. In a full implementation,
    // this would handle real synaptic dynamics including:
    // - Alpha-function conductance changes
    // - Synaptic resource variables
    // - Short-term depression/facilitation
    // - Activity-dependent efficacy
    // - Spike timing dependent synaptic changes

    void step(Timestamp currentTime);

    // Reset to initial state
    // 
    // Resets the synaptic state to initial conditions, including:
    // - Resetting synaptic resources
    // - Restoring baseline efficacy
    // - Clearing short-term plasticity variables
    // - Restoring initial conductance state
    // 
    // This is typically called at the beginning of simulation or when
    // the synapse needs to be reset to a clean state.
    void reset();

    // Initialize with random parameters
    // 
    // Randomizes synaptic parameters including:
    // - Initial weight (based on distribution parameters)
    // - Delay (uniform random distribution)
    // - Synaptic type (excitatory/inhibitory)
    // - Plasticity flags
    // - Short-term plasticity variables
    // 
    // Args:
    //   rng: Random number generator for stochastic initialization
    // 
    // Used during brain initialization to create diverse synaptic connectivity
    // with biologically plausible parameter distributions.
    void initializeRandom(class RandomGenerator& rng);

private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
