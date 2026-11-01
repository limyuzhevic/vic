#pragma once

#include "../core/Types/Types.hpp"
#include <vector>

namespace nlm {

// Forward declaration
class Neuron;

// Synapse representing a connection between neurons
// Implements real synaptic transmission with delays and short-term plasticity

#pragma once

#include "../core/Types/Types.hpp"
#include <vector>

namespace nlm {

// Forward declaration
class Neuron;

// Synapse representing a connection between neurons
/**
 * @class Synapse
 * @brief Real synaptic transmission with delays and short-term plasticity
 * 
 * The Synapse class implements a biologically realistic synaptic connection
 * between neurons with several key features:
 * 
 * - **Real synaptic dynamics**: Tsodyks-Markram short-term plasticity model
 * - **Transmission delays**: Configurable delay in simulation steps
 * - **Spike-timing dependent plasticity (STDP) support**: Tracks pre- and post-
 *   spike times for STDP learning
 * - **Eligibility traces**: For reward-modulated learning mechanisms
 * - **Use-dependent modulation**: Efficacy changes based on firing patterns
 * - **Weighted connections**: Synaptic weight can be plastic and bounded
 * 
 * The synapse operates on incoming spikes from source neurons and generates
 * postsynaptic effects after the specified delay. It maintains state for
 * short-term plasticity and eligibility traces for various learning mechanisms.
 * 
 * @note Uses Pimpl pattern for implementation hiding
 * @note Thread-safe for individual synapse operations
 * @see Neuron, PlasticityRule, EligibilityTrace
 */
class Synapse {
public:
    /**
     * @brief Constructor
     * @param id Unique identifier for this synapse
     * @param source NeuronId Identifier of source neuron
     * @param destination NeuronId Identifier of destination neuron
     * @details Creates a new synaptic connection with specified endpoints.
     * Default type is SynapseType::Excitatory, weight 0.0, delay 1 step.
     */
    Synapse(SynapseId id, NeuronId source, NeuronId destination);
    
    /**
     * @brief Destructor
     * @details Cleans up synapse resources
     */
    ~Synapse();
    
    // Disable copying, enable moving
    Synapse(const Synapse&) = delete;
    Synapse& operator=(const Synapse&) = delete;
    Synapse(Synapse&&) noexcept;
    Synapse& operator=(Synapse&&) noexcept;
    
    /**
     * @brief Get synapse identifier
     * @return SynapseId Unique identifier for this synapse
     */
    SynapseId getId() const;
    
    /**
     * @brief Get source neuron identifier
     * @return NeuronId Identifier of neuron that sends spikes to this synapse
     */
    NeuronId getSourceNeuron() const;
    
    /**
     * @brief Get destination neuron identifier
     * @return NeuronId Identifier of neuron that receives spikes from this synapse
     */
    NeuronId getDestinationNeuron() const;
    
    /**
     * @brief Get current synaptic weight
     * @return SynapticWeight Current synaptic efficacy (typically -1.0 to 1.0)
     */
    SynapticWeight getWeight() const;
    
    /**
     * @brief Set synaptic weight
     * @param weight New synaptic weight
     * @note Weight is clamped to [MIN_WEIGHT, MAX_WEIGHT] range
     */
    void setWeight(SynapticWeight weight);
    
    /**
     * @brief Modify synaptic weight
     * @param delta Change in synaptic weight
     * @details Adds delta to current weight, with automatic clamping to
     * prevent numerical instability
     */
    void addToWeight(SynapticWeight delta);
    
    /**
     * @brief Get synaptic delay
     * @return Delay Synaptic delay in simulation steps
     */
    Delay getDelay() const;
    
    /**
     * @brief Set synaptic delay
     * @param delay New synaptic delay in simulation steps
     */
    void setDelay(Delay delay);
    
    /**
     * @brief Get synapse type
     * @return SynapseType Functional type of this synapse (Excitatory, Inhibitory, etc.)
     */
    SynapseType getType() const;
    
    /**
     * @brief Set synapse type
     * @param type New functional type for this synapse
     */
    void setType(SynapseType type);
    
    /**
     * @brief Check if synapse is excitatory
     * @return bool true if synapse type is excitatory
     */
    bool isExcitatory() const;
    
    /**
     * @brief Check if synapse is inhibitory
     * @return bool true if synapse type is inhibitory
     */
    bool isInhibitory() const;
    
    /**
     * @brief Record presynaptic spike
     * @param timestamp Simulation time when presynaptic neuron spiked
     * @details Updates spike history and triggers short-term plasticity dynamics.
     * Called by source neuron when it spikes.
     */
    void recordPreSpike(Timestamp timestamp);
    
    /**
     * @brief Record postsynaptic spike
     * @param timestamp Simulation time when postsynaptic neuron spiked
     * @details Updates spike history for STDP calculations.
     * Called by destination neuron when it spikes.
     */
    void recordPostSpike(Timestamp timestamp);
    
    /**
     * @brief Get presynaptic spike history
     * @return const std::vector<Timestamp>& Vector of presynaptic spike times
     * @see recordPreSpike, clearHistory
     */
    const std::vector<Timestamp>& getPreSpikeHistory() const;
    
    /**
     * @brief Get postsynaptic spike history
     * @return const std::vector<Timestamp>& Vector of postsynaptic spike times
     */
    const std::vector<Timestamp>& getPostSpikeHistory() const;
    
    /**
     * @brief Clear spike history
     * @details Removes all spike records. Used during checkpointing or reset
     * to save memory while preserving weight and other long-term state.
     */
    void clearHistory();
    
    /**
     * @brief Set presynaptic spike history
     * @param history New presynaptic spike history vector
     * @note Maintains maximum history size (MAX_SPIKE_HISTORY) to prevent memory blow-up
     */
    void setPreSpikeHistory(const std::vector<Timestamp>& history);
    
    /**
     * @brief Set postsynaptic spike history
     * @param history New postsynaptic spike history vector
     * @note Maintains maximum history size (MAX_SPIKE_HISTORY) to prevent memory blow-up
     */
    void setPostSpikeHistory(const std::vector<Timestamp>& history);
    
    /**
     * @brief Get plasticity flags
     * @return const PlasticityFlags& Current plasticity configuration
     */
    const PlasticityFlags& getPlasticityFlags() const;
    
    /**
     * @brief Get mutable plasticity flags
     * @return PlasticityFlags& Mutable plasticity configuration
     */
    PlasticityFlags& getPlasticityFlags();
    
    /**
     * @brief Enable plasticity mechanisms
     * @param hebbian Enable Hebbian plasticity rules
     * @param stdp Enable Spike-Timing-Dependent Plasticity
     * @param rewardModulated Enable reward-modulated plasticity
     */
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    /**
     * @brief Get eligibility trace value
     * @return float Current eligibility trace value
     * @see setEligibilityTrace, decayEligibilityTrace
     */
    float getEligibilityTrace() const;
    
    /**
     * @brief Set eligibility trace
     * @param trace New eligibility trace value
     */
    void setEligibilityTrace(float trace);
    
    /**
     * @brief Decay eligibility trace
     * @param decayRate Decay rate per timestep (typically 0.001-0.01)
     * @details Exponential decay of eligibility trace for reward-modulated learning
     */
    void decayEligibilityTrace(float decayRate);
    
    /**
     * @brief Get synaptic efficacy
     * @return float Current efficacy (use-dependent modulation factor)
     * @see setEfficacy
     */
    float getEfficacy() const;
    
    /**
     * @brief Set synaptic efficacy
     * @param efficacy New efficacy value (typically 0.0-2.0)
     * @note Efficacy is clamped to [0.0, 2.0] range
     */
    void setEfficacy(float efficacy);
    
    /**
     * @brief Perform synaptic dynamics update
     * @details Updates synaptic state for one simulation step, including:
     * - Short-term plasticity decay (facilitation and depression)
     * - Eligibility trace decay
     * - Weight bounds enforcement
     * 
     * @param currentTime Current simulation time in milliseconds
     */
    void step(Timestamp currentTime);
    
    /**
     * @brief Reset synaptic state
     * @details Resets short-term state (spike histories, eligibility traces, efficacy)
     * but preserves synaptic weight as it's considered long-term learned state.
     * Called at the start of new episodes to prevent carryover effects.
     */
    void reset();
    
    /**
     * @brief Initialize synaptic parameters
     * @param rng Random number generator
     * @details Sets up initial values based on synapse type:
     * - Excitatory: Small positive weights (0.1-0.4)
     * - Inhibitory: Small negative weights (-0.1 to -0.4)  
     * - Other types: Small random weights (-0.1 to 0.1)
     * 
     * Also sets random delay (1-5 steps) and short-term plasticity state.
     */
    void initializeRandom(class RandomGenerator& rng);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm

} // namespace nlm
