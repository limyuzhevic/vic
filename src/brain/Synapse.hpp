#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Forward declaration
class Neuron;

/**
 * Synapse representing a connection between neurons
 * Implements real synaptic transmission with delays and short-term plasticity
 */

/**
 * The Synapse class represents a synaptic connection between two neurons.
 * 
 * Synapses are the fundamental communication channels in the neural system,
 * transmitting signals from pre-synaptic to post-synaptic neurons with
 * configurable delays, weights, and plasticity properties.
 * 
 * Key features:
 * - Configurable transmission delays (in simulation steps)
 * - Variable synaptic weight (excitatory or inhibitory)
 * - Spike-timing-dependent plasticity (STDP) support
 * - Hebbian learning support
 * - Reward-modulated plasticity support
 * - Short-term synaptic efficacy modulation
 * - Activity history recording for learning
 * 
 * Each synapse maintains internal state including pre- and post-synaptic
 * spike history, eligibility traces, and current efficacy for dynamic learning.
 */
class Synapse {
public:
    /**
     * Constructor for Synapse
     * 
     * @param id Unique identifier for this synapse
     * @param source Source neuron ID
     * @param destination Destination neuron ID
     */
    Synapse(SynapseId id, NeuronId source, NeuronId destination);
    
    /**
     * Destructor
     */
    ~Synapse();
    
    /**
     * Disable copying
     */
    Synapse(const Synapse&) = delete;
    
    /**
     * Disable copy assignment
     */
    Synapse& operator=(const Synapse&) = delete;
    
    /**
     * Move constructor
     * 
     * @param other Rvalue reference to another Synapse object
     */
    Synapse(Synapse&&) noexcept;
    
    /**
     * Move assignment operator
     * 
     * @param other Rvalue reference to another Synapse object
     * @return Reference to this Synapse object
     */
    Synapse& operator=(Synapse&&) noexcept;
    
    /**
     * Get synapse identifier
     * 
     * @return Unique synapse ID
     */
    SynapseId getId() const;
    
    /**
     * Get source neuron identifier
     * 
     * @return ID of pre-synaptic neuron
     */
    NeuronId getSourceNeuron() const;
    
    /**
     * Get destination neuron identifier
     * 
     * @return ID of post-synaptic neuron
     */
    NeuronId getDestinationNeuron() const;
    
    /**
     * Get synaptic weight
     * 
     * @return Synaptic weight (positive for excitatory, negative for inhibitory)
     */
    SynapticWeight getWeight() const;
    
    /**
     * Set synaptic weight
     * 
     * @param weight New synaptic weight
     */
    void setWeight(SynapticWeight weight);
    
    /**
     * Add delta to synaptic weight
     * 
     * @param delta Change in synaptic weight
     */
    void addToWeight(SynapticWeight delta);
    
    /**
     * Get transmission delay
     * 
     * @return Delay in simulation steps
     */
    Delay getDelay() const;
    
    /**
     * Set transmission delay
     * 
     * @param delay Delay in simulation steps
     */
    void setDelay(Delay delay);
    
    /**
     * Get synapse type
     * 
     * @return Synapse type (Excitatory, Inhibitory, Modulatory)
     */
    SynapseType getType() const;
    
    /**
     * Set synapse type
     * 
     * @param type New synapse type
     */
    void setType(SynapseType type);
    
    /**
     * Check if synapse is excitatory
     * 
     * @return True if synapse releases excitatory neurotransmitters
     */
    bool isExcitatory() const;
    
    /**
     * Check if synapse is inhibitory
     * 
     * @return True if synapse releases inhibitory neurotransmitters
     */
    bool isInhibitory() const;
    
    /**
     * Record pre-synaptic spike
     * 
     * Essential for spike-timing-dependent plasticity (STDP) timing calculations.
     * 
     * @param timestamp Time when pre-synaptic neuron spiked
     */
    void recordPreSpike(Timestamp timestamp);
    
    /**
     * Record post-synaptic spike
     * 
     * Essential for spike-timing-dependent plasticity (STDP) timing calculations.
     * 
     * @param timestamp Time when post-synaptic neuron spiked
     */
    void recordPostSpike(Timestamp timestamp);
    
    /**
     * Get pre-synaptic spike history
     * 
     * @return Constant reference to vector of pre-synaptic spike timestamps
     */
    const std::vector<Timestamp>& getPreSpikeHistory() const;
    
    /**
     * Get post-synaptic spike history
     * 
     * @return Constant reference to vector of post-synaptic spike timestamps
     */
    const std::vector<Timestamp>& getPostSpikeHistory() const;
    
    /**
     * Clear spike history
     */
    void clearHistory();
    
    /**
     * Get plasticity flags (constant)
     * 
     * @return Constant reference to plasticity flags
     */
    const PlasticityFlags& getPlasticityFlags() const;
    
    /**
     * Get plasticity flags (mutable)
     * 
     * @return Mutable reference to plasticity flags
     */
    PlasticityFlags& getPlasticityFlags();
    
    /**
     * Configure plasticity rules for this synapse
     * 
     * @param hebbian Enable Hebbian plasticity
     * @param stdp Enable spike-timing-dependent plasticity
     * @param rewardModulated Enable reward-modulated plasticity
     */
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    /**
     * Get eligibility trace
     * 
     * @return Current eligibility trace value (used for reward-modulated learning)
     */
    float getEligibilityTrace() const;
    
    /**
     * Set eligibility trace
     * 
     * @param trace New eligibility trace value
     */
    void setEligibilityTrace(float trace);
    
    /**
     * Decay eligibility trace
     * 
     * @param decayRate Rate at which eligibility trace decays (0.0-1.0)
     */
    void decayEligibilityTrace(float decayRate);
    
    /**
     * Get synaptic efficacy
     * 
     * @return Current synaptic efficacy (use-dependent modulation factor)
     */
    float getEfficacy() const;
    
    /**
     * Set synaptic efficacy
     * 
     * @param efficacy New synaptic efficacy
     */
    void setEfficacy(float efficacy);
    
    /**
     * Execute one simulation step
     * 
     * Updates synaptic state for one timestep, including delay countdown,
     * efficacy modulation, and eligibility trace decay.
     * 
     * @param currentTime Current simulation time in seconds
     */
    void step(Timestamp currentTime);
    
    /**
     * Reset synapse to initial state
     * 
     * Clears activity history, resets eligibility trace, and resets efficacy.
     */
    void reset();
    
    /**
     * Initialize synapse with random parameters
     * 
     * Sets initial weight, delay, and type to random biologically plausible values.
     * 
     * @param rng Reference to random number generator
     */
    void initializeRandom(class RandomGenerator& rng);
    
private:
    /**
     * Pimpl pattern implementation details
     */
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
