#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

/**
 * @class Synapse
 * @brief Implements a synaptic connection between two neurons
 * 
 * The Synapse class represents a connection that transmits electrical or chemical
 * signals from a presynaptic neuron to a postsynaptic neuron. Each synapse has
 * dynamic properties including weight, type, delay, and plasticity mechanisms.
 * 
 * @details Synapse functionality includes:
 * - Synaptic transmission with configurable delays
 * - Dynamic weight modulation through learning rules
 * - Short-term plasticity mechanisms for activity-dependent modulation
 * - Spike history recording for STDP learning
 * - Eligibility traces for reward-modulated plasticity
 * - Different synapse types (excitatory, inhibitory, modulatory)
 * 
 * Synaptic transmission follows the equation:
 * Post-synaptic potential = Weight × Pre-synaptic spike × Transmissibility
 * 
 * The synapse maintains separate histories for pre- and post-synaptic spikes
 * to enable precise timing-based learning mechanisms.
 */
class Synapse {
public:
    /**
     * @brief Construct a synapse with unique identifiers
     * 
     * Creates a new synaptic connection with specified source and destination
     * neurons. Initial parameters are set to biologically plausible defaults.
     * 
     * @param id Unique identifier for this synapse
     * @param source NeuronId of presynaptic (source) neuron
     * @param destination NeuronId of postsynaptic (target) neuron
     */
    Synapse(SynapseId id, NeuronId source, NeuronId destination);
    
    /**
     * @brief Destructor releases synapse resources
     */
    ~Synapse();
    
    /**
     * @brief Disable copying (synapses must have unique IDs)
     */
    Synapse(const Synapse&) = delete;
    
    /**
     * @brief Disable copy assignment
     */
    Synapse& operator=(const Synapse&) = delete;
    
    /**
     * @brief Move constructor enables synapse transfer
     */
    Synapse(Synapse&&) noexcept;
    
    /**
     * @brief Move assignment operator enables synapse transfer
     */
    Synapse& operator=(Synapse&&) noexcept;
    
    /**
     * @brief Get the synapse's unique identifier
     * 
     * @return SynapseId Unique identifier for this synapse
     */
    SynapseId getId() const;
    
    /**
     * @brief Get the presynaptic (source) neuron ID
     * 
     * @return NeuronId ID of the source neuron
     */
    NeuronId getSourceNeuron() const;
    
    /**
     * @brief Get the postsynaptic (destination) neuron ID
     * 
     * @return NeuronId ID of the target neuron
     */
    NeuronId getDestinationNeuron() const;
    
    /**
     * @brief Get the current synaptic weight
     * 
     * @return SynapticWeight Current synaptic strength (0.0 to 1.0)
     */
    SynapticWeight getWeight() const;
    
    /**
     * @brief Set the synaptic weight
     * 
     * @param weight New synaptic weight (clamped to [0.0, 1.0])
     */
    void setWeight(SynapticWeight weight);
    
    /**
     * @brief Add delta to synaptic weight
     * 
     * @param delta Change in synaptic weight (can be positive or negative)
     */
    void addToWeight(SynapticWeight delta);
    
    /**
     * @brief Get the synaptic transmission delay
     * 
     * @return Delay Delay in simulation steps (default: 1)
     */
    Delay getDelay() const;
    
    /**
     * @brief Set the synaptic transmission delay
     * 
     * @param delay Delay in simulation steps
     */
    void setDelay(Delay delay);
    
    /**
     * @brief Get the synapse type
     * 
     * @return SynapseType Type classification (excitatory, inhibitory, etc.)
     */
    SynapseType getType() const;
    
    /**
     * @brief Set the synapse type
     * 
     * @param type New synapse type classification
     */
    void setType(SynapseType type);
    
    /**
     * @brief Check if synapse is excitatory
     * 
     * @return bool true if excitatory, false otherwise
     */
    bool isExcitatory() const;
    
    /**
     * @brief Check if synapse is inhibitory
     * 
     * @return bool true if inhibitory, false otherwise
     */
    bool isInhibitory() const;
    
    /**
     * @brief Record presynaptic (pre) spike for STDP learning
     * 
     * @param timestamp Time when presynaptic neuron spiked (ms)
     */
    void recordPreSpike(Timestamp timestamp);
    
    /**
     * @brief Record postsynaptic (post) spike for STDP learning
     * 
     * @param timestamp Time when postsynaptic neuron spiked (ms)
     */
    void recordPostSpike(Timestamp timestamp);
    
    /**
     * @brief Get presynaptic spike history for plasticity calculations
     * 
     * @return const std::vector<Timestamp>& Vector of presynaptic spike times
     */
    const std::vector<Timestamp>& getPreSpikeHistory() const;
    
    /**
     * @brief Get postsynaptic spike history for plasticity calculations
     * 
     * @return const std::vector<Timestamp>& Vector of postsynaptic spike times
     */
    const std::vector<Timestamp>& getPostSpikeHistory() const;
    
    /**
     * @brief Clear spike history records
     */
    void clearHistory();
    
    /**
     * @brief Get plasticity flags for learning rule activation
     * 
     * @return const PlasticityFlags& Reference to plasticity configuration
     */
    const PlasticityFlags& getPlasticityFlags() const;
    
    /**
     * @brief Get plasticity flags for learning rule activation (mutable)
     * 
     * @return PlasticityFlags& Reference to plasticity configuration
     */
    PlasticityFlags& getPlasticityFlags();
    
    /**
     * @brief Enable specific plasticity rules for this synapse
     * 
     * @param hebbian Enable Hebbian learning
     * @param stdp Enable spike-timing-dependent plasticity
     * @param rewardModulated Enable reward-modulated plasticity
     */
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    /**
     * @brief Get eligibility trace for reward-modulated learning
     * 
     * @return float Current eligibility trace value
     */
    float getEligibilityTrace() const;
    
    /**
     * @brief Set eligibility trace value
     * 
     * @param trace New eligibility trace value
     */
    void setEligibilityTrace(float trace);
    
    /**
     * @brief Decay eligibility trace by specified factor
     * 
     * @param decayRate Decay factor (0.0 to 1.0)
     */
    void decayEligibilityTrace(float decayRate);
    
    /**
     * @brief Get current synaptic efficacy (use-dependent modulation)
     * 
     * @return float Current efficacy factor
     */
    float getEfficacy() const;
    
    /**
     * @brief Set synaptic efficacy (use-dependent modulation)
     * 
     * @param efficacy New efficacy factor
     */
    void setEfficacy(float efficacy);
    
    /**
     * @brief Update synapse for one simulation step
     * 
     * Applies synaptic dynamics including transmission delay, short-term
     * plasticity, and any time-dependent changes.
     * 
     * @param currentTime Current simulation time in ms
     */
    void step(Timestamp currentTime);
    
    /**
     * @brief Reset synapse to initial state
     * 
     * Restores default parameters and clears all history.
     */
    void reset();
    
    /**
     * @brief Initialize synapse with random biological parameters
     * 
     * Sets initial weight, delay, and other parameters using random values
     * drawn from biologically plausible distributions.
     * 
     * @param rng Random number generator instance
     */
    void initializeRandom(class RandomGenerator& rng);
    
private:
    /**
     * @brief Pimpl idiom implementation details
     * 
     * Forward declaration to Synapse implementation for reduced compile times
     * and encapsulation of implementation details.
     */
    struct Impl;
    Impl* pImpl;  // Pointer to implementation
} __attribute__((aligned(64)));  // Cache line alignment for performance

} // namespace nlm
