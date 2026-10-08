#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Forward declaration
class Neuron;

/**
 * @brief Synapse representing a connection between neurons
 * 
 * This class implements a biologically-inspired synaptic connection with
 * the following key features:
 * 
 * ## Core Properties:
 * - **Weight**: Synaptic strength (-1.0 to 1.0), negative for inhibitory
 * - **Delay**: Transmission delay (1-5 simulation steps, 1-5ms)
 * - **Type**: Excitatory, inhibitory, or modulatory
 * - **Plasticity**: Configurable Hebbian and STDP learning rules
 * 
 * ## Advanced Features:
 * - **Short-term plasticity**: Tsodyks-Markram model (facilitation + depression)
 * - **Eligibility traces**: For reward-modulated learning (TD learning)
 * - **Spike history tracking**: Pre- and post-synaptic spike times
 * - **Synaptic efficacy**: Use-dependent modulation of synaptic strength
 * - **Connection specificity**: Source and destination neuron identification
 * 
 * ## Synaptic Dynamics:
 * 1. **Transmission**: Spike triggers delayed event to deliver synaptic current
 * 2. **Short-term plasticity**: Activity-dependent changes in efficacy
 * 3. **Plasticity**: Long-term changes based on spike timing (STDP)
 * 4. **Modulation**: Neuromodulators influence eligibility traces and learning
 * 
 * The synapse operates as an event-driven system where spikes are queued
 * with delays, and synaptic input is delivered to postsynaptic neurons
 * only when the delayed event occurs.
 * 
 * @note Uses PIMPL pattern to hide implementation details and reduce compile
 *       times. All member functions are in Synapse.cpp.
 */
class Synapse {
public:
    /**
     * @brief Create synapse with connection specification
     * 
     * @param id Unique identifier for this synapse
     * @param source NeuronId of presynaptic (source) neuron
     * @param destination NeuronId of postsynaptic (target) neuron
     */
    Synapse(SynapseId id, NeuronId source, NeuronId destination);
    
    ~Synapse();
    
    /**
     * @brief Disable copying (PIMPL pattern)
     */
    Synapse(const Synapse&) = delete;
    Synapse& operator=(const Synapse&) = delete;
    
    /**
     * @brief Enable move operations (PIMPL pattern)
     */
    Synapse(Synapse&&) noexcept;
    Synapse& operator=(Synapse&&) noexcept;
    
    /**
     * @brief Get the synapse's unique identifier
     * 
     * @return SynapseId Unique identifier
     */
    SynapseId getId() const;
    
    /**
     * @brief Get the source (presynaptic) neuron identifier
     * 
     * @return NeuronId Source neuron identifier
     */
    NeuronId getSourceNeuron() const;
    
    /**
     * @brief Get the destination (postsynaptic) neuron identifier
     * 
     * @return NeuronId Destination neuron identifier
     */
    NeuronId getDestinationNeuron() const;
    
    /**
     * @brief Get the current synaptic weight
     * 
     * @return SynapticWeight Synaptic strength (-1.0 to 1.0)
     * @note Negative weights indicate inhibitory synapses
     */
    SynapticWeight getWeight() const;
    
    /**
     * @brief Set the synaptic weight
     * 
     * @param weight New synaptic strength (-1.0 to 1.0)
     */
    void setWeight(SynapticWeight weight);
    
    /**
     * @brief Add to the synaptic weight (for plasticity)
     * 
     * @param delta Change in synaptic weight
     * @note Weight is clamped to prevent instability
     */
    void addToWeight(SynapticWeight delta);
    
    /**
     * @brief Get the synaptic transmission delay
     * 
     * @return Delay Delay in simulation steps (typically 1-5 steps)
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
     * @return SynapseType Type classification
     */
    SynapseType getType() const;
    
    /**
     * @brief Set the synapse type
     * 
     * @param type New type classification
     */
    void setType(SynapseType type);
    
    /**
     * @brief Check if synapse is excitatory
     * 
     * @return bool True if weight > 0 (excitatory)
     */
    bool isExcitatory() const;
    
    /**
     * @brief Check if synapse is inhibitory
     * 
     * @return bool True if weight < 0 (inhibitory)
     */
    bool isInhibitory() const;
    
    /**
     * @brief Record a presynaptic spike time
     * 
     * Used by STDP and short-term plasticity rules.
     * Stores spike times for up to MAX_SPIKE_HISTORY events.
     * 
     * @param timestamp Time at which presynaptic neuron spiked
     */
    void recordPreSpike(Timestamp timestamp);
    
    /**
     * @brief Record a postsynaptic spike time
     * 
     * Used by STDP for spike-timing dependent plasticity.
     * Stores spike times for up to MAX_SPIKE_HISTORY events.
     * 
     * @param timestamp Time at which postsynaptic neuron spiked
     */
    void recordPostSpike(Timestamp timestamp);
    
    /**
     * @brief Get all presynaptic spike times
     * 
     * @return const std::vector<Timestamp>& Vector of spike times
     */
    const std::vector<Timestamp>& getPreSpikeHistory() const;
    
    /**
     * @brief Get all postsynaptic spike times
     * 
     * @return const std::vector<Timestamp>& Vector of spike times
     */
    const std::vector<Timestamp>& getPostSpikeHistory() const;
    
    /**
     * @brief Clear spike history
     * 
     * Used periodically to prevent memory growth and focus on recent activity.
     */
    void clearHistory();
    
    /**
     * @brief Get the plasticity flags for this synapse
     * 
     * @return const PlasticityFlags& Plasticity configuration
     */
    const PlasticityFlags& getPlasticityFlags() const;
    
    /**
     * @brief Get mutable reference to plasticity flags
     * 
     * @return PlasticityFlags& Mutable plasticity configuration
     */
    PlasticityFlags& getPlasticityFlags();
    
    /**
     * @brief Configure which plasticity rules apply to this synapse
     * 
     * @param hebbian Enable Hebbian learning (activity-dependent strengthening)
     * @param stdp Enable spike-timing dependent plasticity (STDP)
     * @param rewardModulated Enable reward-modulated learning (TD learning)
     */
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    /**
     * @brief Get the eligibility trace value
     * 
     * @return float Current eligibility trace (0.0 to 1.0+)
     * @note Used for reward-modulated learning to bridge rewards with actions
     */
    float getEligibilityTrace() const;
    
    /**
     * @brief Set the eligibility trace value
     * 
     * @param trace New eligibility trace value
     */
    void setEligibilityTrace(float trace);
    
    /**
     * @brief Decay the eligibility trace
     * 
     * @param decayRate Decay factor per timestep (typically 0.001)
     * @note Called automatically during synaptic step()
     */
    void decayEligibilityTrace(float decayRate);
    
    /**
     * @brief Get the synaptic efficacy (use-dependent modulation)
     * 
     * @return float Synaptic efficacy (0.0 to 2.0)
     * @note Represents short-term changes in synaptic strength due to recent activity
     */
    float getEfficacy() const;
    
    /**
     * @brief Set the synaptic efficacy
     * 
     * @param efficacy New efficacy value (clamped to 0.0-2.0)
     */
    void setEfficacy(float efficacy);
    
    /**
     * @brief Update synapse for one simulation step
     * 
     * This function implements real synaptic dynamics including:
     * 1. **Short-term plasticity decay**: Tsodyks-Markram model recovery
     * 2. **Eligibility trace decay**: For reward-modulated learning
     * 3. **Efficacy updates**: Based on recent activity patterns
     * 4. **Weight bounds clamping**: Prevent runaway excitation
     * 
     * @param currentTime Current simulation time in ms
     * @note This is called automatically during the brain simulation loop
     */
    void step(Timestamp currentTime);
    
    /**
     * @brief Reset synapse to initial state (for restarting simulation)
     */
    void reset();
    
    /**
     * @brief Initialize synapse with biologically realistic parameters
     * 
     * Sets realistic synaptic parameters based on synapse type:
     * - Excitatory: Small positive weights (0.1-0.4)
     * - Inhibitory: Negative weights (-0.1 to -0.4)
     * - Other types: Small random weights (-0.1 to 0.1)
     * 
     * Also initializes short-term plasticity state, eligibility traces,
     * and spike history.
     * 
     * @param rng Random number generator for parameter sampling
     */
    void initializeRandom(RandomGenerator& rng);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
