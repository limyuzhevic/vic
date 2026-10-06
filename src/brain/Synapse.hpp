#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Forward declaration
class Neuron;
#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Forward declaration
class Neuron;

/**
 * @brief Synapse representing a connection between neurons
 * 
 * The Synapse class implements real synaptic transmission with:
 * - Delay-based spike propagation (1-5ms typical)
 * - Synaptic weight dynamics (plasticity)
 * - Short-term plasticity (facilitation/depression)
 * - Spike history for STDP learning
 * - Eligibility traces for reward-modulated learning
 * - Synaptic efficacy (use-dependent modulation)
 * 
 * Synapses are directed connections from source to destination neurons,
 * implementing the fundamental connectivity of neural networks.
 * 
 * @note Uses PIMPL idiom for efficient memory usage
 * @see Synapse.cpp for implementation details
 * @see Neuron.hpp for connected neurons
 */
class Synapse {
public:
    /**
     * @brief Creates a new synapse with specified connection
     * 
     * @param id Unique identifier for the synapse
     * @param source Neuron that sends spikes through this synapse
     * @param destination Neuron that receives spikes through this synapse
     */
    Synapse(SynapseId id, NeuronId source, NeuronId destination);
    
    /**
     * @brief Destructor
     */
    ~Synapse();
    
    /**
     * @brief Disable copying (synapses are not copyable)
     */
    Synapse(const Synapse&) = delete;
    Synapse& operator=(const Synapse&) = delete;
    
    /**
     * @brief Enable moving (synapses can be moved efficiently)
     */
    Synapse(Synapse&&) noexcept;
    Synapse& operator=(Synapse&&) noexcept;
    
    /**
     * @brief Get the synapse's unique identifier
     * 
     * @return SynapseId The synapse's ID
     */
    SynapseId getId() const;
    
    /**
     * @brief Get the source neuron (presynaptic)
     * 
     * @return NeuronId ID of neuron that sends spikes
     */
    NeuronId getSourceNeuron() const;
    
    /**
     * @brief Get the destination neuron (postsynaptic)
     * 
     * @return NeuronId ID of neuron that receives spikes
     */
    NeuronId getDestinationNeuron() const;
    
    /**
     * @brief Get the current synaptic weight
     * 
     * @return SynapticWeight Current weight (-1.0 to 1.0, typically 0.1-0.4)
     */
    SynapticWeight getWeight() const;
    
    /**
     * @brief Set the synaptic weight
     * 
     * @param weight New synaptic weight (clamped to [-1.0, 1.0])
     */
    void setWeight(SynapticWeight weight);
    
    /**
     * @brief Add to synaptic weight (used by plasticity rules)
     * 
     * @param delta Weight change (positive = potentiation, negative = depression)
     */
    void addToWeight(SynapticWeight delta);
    
    /**
     * @brief Get the synaptic delay in simulation steps
     * 
     * @return Delay Delay in steps (1-5 typical)
     */
    Delay getDelay() const;
    
    /**
     * @brief Set the synaptic delay
     * 
     * @param delay New delay in simulation steps
     */
    void setDelay(Delay delay);
    
    /**
     * @brief Get the synapse type
     * 
     * @return SynapseType Type of synaptic transmission
     */
    SynapseType getType() const;
    
    /**
     * @brief Set the synapse type
     * 
     * @param type New synapse type
     */
    void setType(SynapseType type);
    
    /**
     * @brief Check if synapse is excitatory
     * 
     * @return bool True if synapse type is Excitatory
     */
    bool isExcitatory() const;
    
    /**
     * @brief Check if synapse is inhibitory
     * 
     * @return bool True if synapse type is Inhibitory
     */
    bool isInhibitory() const;
    
    /**
     * @brief Record presynaptic spike for STDP
     * 
     * @param timestamp Time when presynaptic neuron spiked
     */
    void recordPreSpike(Timestamp timestamp);
    
    /**
     * @brief Record postsynaptic spike for STDP
     * 
     * @param timestamp Time when postsynaptic neuron spiked
     */
    void recordPostSpike(Timestamp timestamp);
    
    /**
     * @brief Get recent presynaptic spike history
     * 
     * @return const std::vector<Timestamp>& Vector of presynaptic spike times
     */
    const std::vector<Timestamp>& getPreSpikeHistory() const;
    
    /**
     * @brief Get recent postsynaptic spike history
     * 
     * @return const std::vector<Timestamp>& Vector of postsynaptic spike times
     */
    const std::vector<Timestamp>& getPostSpikeHistory() const;
    
    /**
     * @brief Clear spike history (used at simulation start)
     */
    void clearHistory();
    
    /**
     * @brief Get plasticity configuration flags
     * 
     * @return const PlasticityFlags& Reference to plasticity flags
     */
    const PlasticityFlags& getPlasticityFlags() const;
    
    /**
     * @brief Get mutable plasticity configuration flags
     * 
     * @return PlasticityFlags& Reference to plasticity flags (may modify)
     */
    PlasticityFlags& getPlasticityFlags();
    
    /**
     * @brief Enable/disable plasticity rules
     * 
     * @param hebbian Enable Hebbian (rate-based) plasticity
     * @param stdp Enable Spike-Timing-Dependent Plasticity
     * @param rewardModulated Enable reward-modulated plasticity
     */
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    /**
     * @brief Get eligibility trace for reward-modulated learning
     * 
     * @return float Current eligibility trace value (-1.0 to 1.0)
     */
    float getEligibilityTrace() const;
    
    /**
     * @brief Set eligibility trace
     * 
     * @param trace New eligibility trace value
     */
    void setEligibilityTrace(float trace);
    
    /**
     * @brief Decay eligibility trace over time
     * 
     * @param decayRate Fraction to decay per timestep (0.0 to 1.0)
     */
    void decayEligibilityTrace(float decayRate);
    
    /**
     * @brief Get synaptic efficacy (use-dependent modulation)
     * 
     * @return float Synaptic efficacy (0.0 to 2.0)
     */
    float getEfficacy() const;
    
    /**
     * @brief Set synaptic efficacy
     * 
     * @param efficacy New efficacy value (clamped to [0.0, 2.0])
     */
    void setEfficacy(float efficacy);
    
    /**
     * @brief Update synapse for one simulation step
     * 
     * Implements real synaptic dynamics including:
     * - Short-term plasticity state decay (Tsodyks-Markram model)
     * - Eligibility trace decay for reward-modulated learning
     * - Synaptic weight clamping to prevent instability
     * 
     * @param currentTime Current simulation time (s)
     */
    void step(Timestamp currentTime);
    
    /**
     * @brief Reset synapse to initial state
     */
    void reset();
    
    /**
     * @brief Initialize synapse with random biologically plausible parameters
     * 
     * @param rng Random number generator for reproducible initialization
     */
    void initializeRandom(class RandomGenerator& rng);
    
private:
    /**
     * @brief PIMPL idiom implementation details
     * 
     * Pointer-to-implementation pattern for efficient compilation
     */
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
