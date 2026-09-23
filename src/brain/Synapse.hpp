#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

/**
 * @class Synapse
 * @brief Connection between neurons implementing synaptic transmission
 * 
 * The Synapse class represents a biological synaptic connection between two neurons,
 * implementing the transmission of electrical signals from presynaptic to postsynaptic
 * neurons with realistic dynamics including transmission delays and short-term plasticity.
 * This is the fundamental communication mechanism in the neural network.
 * 
 * Key synaptic properties:
 * - **Transmission**: Signal propagation from source to destination neuron
 * - **Delay**: Temporal delay due to axonal conduction and synaptic transmission
 * - **Weight**: Synaptic strength determining signal amplitude (excitatory/inhibitory)
 * - **Type**: Functional classification (excitatory, inhibitory, modulatory)
 * - **Plasticity**: Activity-dependent weight modulation for learning
 * - **Short-term Plasticity**: Use-dependent changes in synaptic efficacy
 * - **Eligibility Traces**: Mechanism for reward-modulated learning
 * 
 * Synapses enable the coordinated activity of neural populations and support complex
 * information processing through spatiotemporal patterns of connectivity.
 * 
 * @warning Synaptic dynamics are critical for network behavior. Incorrect delay
 * settings or weight management can cause significant simulation artifacts.
 * 
 * @see Neuron Neuron class (pre-synaptic and post-synaptic neurons)
 * @see Brain Brain-level synapse management and population connectivity
 * @see STDP Spike-timing dependent plasticity for synapse modification
 * @see NeuralWorkingMemory Synapses for persistent activity
 * 
 * @par Example Usage:
 * @code
 * // Create synapse between two neurons
 * Synapse synapse(synapseId, sourceNeuronId, destinationNeuronId);
 * 
 * // Configure synaptic properties
 * synapse.setType(SynapseType::Excitatory);
 * synapse.setWeight(1.5f);  // Excitation
 * synapse.setDelay(2);      // 2 timestep delay
 * 
 * // Record spike activity for plasticity
 * synapse.recordPreSpike(preSpikeTime);
 * synapse.recordPostSpike(postSpikeTime);
 * 
 * // Simulate synaptic transmission
 * synapse.step(currentTime);
 * 
 * // Save synapse state for checkpointing
 * float weight = synapse.getWeight();
 * float efficacy = synapse.getEfficacy();
 * @endcode
 */
class Synapse {
public:
    /**
     * @brief Constructs a synapse connection between two neurons
     * 
     * Creates a new synaptic connection from a presynaptic to a postsynaptic neuron.
     * The synapse is initialized with default parameters and is ready for synaptic
     * transmission once configured. This represents the formation of a new neural
     * connection in the developing network.
     * 
     * @param id Unique identifier for this synapse (for debugging and state management)
     * @param source Source neuron that sends signals (presynaptic neuron)
     * @param destination Target neuron that receives signals (postsynaptic neuron)
     * @note The synapse does not take ownership of the neurons. The neurons must
     * remain valid throughout the synapse's lifetime. This allows synapses to be
     * created and destroyed dynamically while neurons persist.
     * 
     * @see SynapseId Type definition for synapse identifiers
     * @see NeuronId Type definition for neuron identifiers
     * @see getSourceNeuron() Access source neuron
     * @see getDestinationNeuron() Access target neuron
     * @see initializeRandom() Initialize synaptic parameters randomly
     * 
     * @par Implementation Notes:
     * @code
     * This constructor uses the pImpl pattern, delegating actual implementation
     * to a hidden implementation structure to optimize memory usage and improve
     * maintainability of the complex synaptic state.
     * @endcode
     */
    Synapse(SynapseId id, NeuronId source, NeuronId destination);
    
    ~Synapse();
    
    /**
     * @brief Destructor - cleans up synapse resources
     * 
     * Properly cleans up all synapse resources including spike history buffers,
     * eligibility traces, and implementation details. Ensures graceful
     * shutdown of the synaptic connection.
     * 
     * @note The pImpl pattern ensures proper cleanup of the implementation
     * structure without exposing internal details to the interface.
     */
    ~Synapse();
    
    // Disable copying, enable moving
    
    /**
     * @brief Disable copying - Synapse contains non-copyable resources
     * 
     * Synapses contain complex internal structures including spike history
     * buffers, eligibility traces, and implementation pointers that cannot be
     * safely copied. Copying would result in double-free or resource corruption.
     * Use move semantics instead.
     * 
     * @param other Source synapse to copy from (parameter kept for syntax)
     * @warning This constructor is deleted and will cause compilation errors
     * if attempted to be used. Use the move constructor instead.
     * 
     * @see Synapse(Synapse&&) Move constructor for safe transfer
     * @see operator=(Synapse&&) Move assignment operator
     */
    Synapse(const Synapse&) = delete;
    
    /**
     * @brief Disable assignment - Synapse contains non-copyable resources
     * 
     * Synapses contain non-shareable resources that cannot be safely assigned.
     * This operation would lead to resource corruption or double-free.
     * 
     * @param other Source synapse to assign from (parameter kept for syntax)
     * @return Reference to this synapse (parameter kept for syntax)
     * @warning This operator is deleted and will cause compilation errors
     * if attempted to be used.
     * 
     * @see Synapse& operator=(Synapse&&) Move assignment for safe transfer
     */
    Synapse& operator=(const Synapse&) = delete;
    
    /**
     * @brief Move constructor - efficiently transfers synapse ownership
     * 
     * This constructor allows efficient transfer of synapse ownership, moving
     * the implementation pointer and all associated resources (spike history,
     * eligibility traces, etc.) from the source synapse to this synapse.
     * The source synapse is left in a valid but unspecified state after the move.
     * 
     * @param other Synapse to move from (invalidated after move)
     * @warning After moving from a Synapse object, the source synapse should not
     * be used as it no longer owns its resources.
     * 
     * @see Synapse(const Synapse&) Deleted copy constructor
     * @see operator=(Synapse&&) Move assignment operator
     */
    Synapse(Synapse&&) noexcept;
    
    /**
     * @brief Move assignment operator - efficiently transfers synapse ownership
     * 
     * Transfers ownership of the implementation structure and all associated
     * resources from the source synapse to this synapse. Properly cleans up
     * existing resources before assignment. This operation has strong exception
     * safety guarantees.
     * 
     * @param other Synapse to move from
     * @return Reference to this synapse (allows chaining)
     * @warning After assignment, the source synapse should not be used.
     * 
     * @see Synapse& operator=(const Synapse&) Deleted copy assignment operator
     */
    Synapse& operator=(Synapse&&) noexcept;
    
    /**
     * @brief Gets the unique identifier of this synapse
     * 
     * Returns the synapse's unique identifier, which can be used for addressing
     * this synapse in the brain, debugging, or state management.
     * 
     * @return SynapseId Unique identifier for this synapse
     * @note The synapse ID is set at construction and remains constant throughout
     * the synapse's lifetime, providing stable addressing for state management.
     * 
     * @see SynapseId Type definition for synapse identifiers
     * @see setWeight() Set synaptic weight for plasticity
     * @see getEfficacy() Get current synaptic efficacy
     */
    SynapseId getId() const;
    
    /**
     * @brief Gets the source neuron (presynaptic neuron)
     * 
     * Returns the neuron that sends signals through this synapse. The source neuron
     * generates action potentials that are transmitted to the target neuron.
     * 
     * @return NeuronId ID of the source neuron (presynaptic)
     * @note This neuron is the origin of all synaptic signals transmitted through
     * this synapse. The synapse's activity depends on the spiking behavior of this neuron.
     * 
     * @see getDestinationNeuron() Get target neuron (postsynaptic)
     * @see getSourceNeuron() Get source neuron
     * @see getWeight() Get synaptic weight for transmission strength
     * @see recordPreSpike() Record spikes from this neuron for plasticity
     */
    NeuronId getSourceNeuron() const;
    
    /**
     * @brief Gets the target neuron (postsynaptic neuron)
     * 
     * Returns the neuron that receives signals through this synapse. The target neuron
     * is affected by action potentials transmitted from the source neuron.
     * 
     * @return NeuronId ID of the target neuron (postsynaptic)
     * @note This neuron receives and integrates synaptic inputs from the source neuron.
     * The synapse's contribution to this neuron's membrane potential depends on its
     * weight and efficacy parameters.
     * 
     * @see getSourceNeuron() Get source neuron (presynaptic)
     * @see getWeight() Get synaptic weight
     * @see receiveExcitatoryInput() Post-synaptic effect on target neuron
     */
    NeuronId getDestinationNeuron() const;
    
    /**
     * @brief Gets the current synaptic weight
     * 
     * Returns the current synaptic weight, which determines the strength and sign
     * (excitatory/inhibitory) of the connection between the source and target neurons.
     * This is the primary parameter controlling synaptic transmission.
     * 
     * @return SynapticWeight Current synaptic weight
     * @note Positive weights represent excitatory synapses, negative weights represent
     * inhibitory synapses, and zero weight represents no connection (or neutral connection).
     * Weight magnitude determines signal transmission strength.
     * 
     * @see setWeight() Set synaptic weight
     * @see addToWeight() Modify synaptic weight (for plasticity)
     * @see getEfficacy() Get current synaptic efficacy
     * @see isExcitatory() Check if synapse is excitatory
     * @see isInhibitory() Check if synapse is inhibitory
     * 
     * @warning Large absolute weights can cause numerical instability in neural simulations.
     * Consider implementing weight normalization or clipping in simulation control.
     */
    SynapticWeight getWeight() const;
    
    /**
     * @brief Sets the synaptic weight
     * 
     * Sets the synaptic weight, determining the strength and sign of the connection
     * between neurons. This is a key parameter for controlling synaptic transmission
     * and network dynamics.
     * 
     * @param weight New synaptic weight
     * @note Valid ranges depend on the specific neural model and simulation. Typical
     * values range from -1.0 (strong inhibition) to +1.0 (strong excitation), but
     * simulation-specific constraints may require different ranges.
     * 
     * @see getWeight() Get current synaptic weight
     * @see addToWeight() Modify synaptic weight incrementally
     * @see initializeRandom() Initialize with random weight
     * 
     * @warning Setting inappropriate weight values can cause simulation instability
     * or biologically unrealistic dynamics. Consider using plasticity rules for
     * gradual weight changes during learning.
     */
    void setWeight(SynapticWeight weight);
    
    /**
     * @brief Modifies synaptic weight incrementally
     * 
     * Increments the synaptic weight by a delta value, implementing incremental
     * changes such as those produced by plasticity rules (STDP, Hebbian learning).
     * This is the preferred method for implementing learning and adaptation.
     * 
     * @param delta Change to add to current weight
     * @note Positive delta increases weight (potentiates synapse),
     * negative delta decreases weight (depresses synapse).
     * 
     * @see setWeight() Set weight directly
     * @see getWeight() Get current weight
     * @see STDP Plasticity rule for weight changes based on spike timing
     * @see enablePlasticity() Enable plasticity rules for this synapse
     * 
     * @warning The delta value should be within reasonable bounds to maintain
     * numerical stability. Plasticity rules typically provide appropriate delta
     * values based on neural activity patterns.
     */
    void addToWeight(SynapticWeight delta);
    
    /**
     * @brief Gets the transmission delay of this synapse
     * 
     * Returns the transmission delay in simulation timesteps, representing the time
     * required for signals to propagate from the source to target neuron. This models
     * axonal conduction delay and synaptic transmission time.
     * 
     * @return Delay Transmission delay in timesteps
     * @note Typical delays range from 0 to several timesteps. Delays are crucial for
     * temporal coding and can significantly affect network dynamics and information
     * processing capabilities.
     * 
     * @see setDelay() Set transmission delay
     * @see step() Current step affects delay timing
     * @see getSourceNeuron() Get source neuron
     * 
     * @warning Delay settings must be consistent with simulation timestep size.
     * Large delays can cause performance issues in large networks but may be necessary
     * for realistic temporal dynamics.
     */
    Delay getDelay() const;
    
    /**
     * @brief Sets the transmission delay
     * 
     * Sets the transmission delay in simulation timesteps for this synapse connection.
     * This parameter controls the temporal latency of signal transmission between
     * source and target neurons.
     * 
     * @param delay New transmission delay in timesteps
     * @note Typical values range from 0 (instantaneous transmission) to several timesteps
     * (representing biological transmission delays). Zero delay may be appropriate for
     * abstract models, while multi-step delays are needed for realistic temporal dynamics.
     * 
     * @see getDelay() Get current transmission delay
     * @see step() Update current step position
     * @note Delay values should be optimized based on the specific neural model
     * and simulation requirements.
     */
    void setDelay(Delay delay);
    
    /**
     * @brief Gets the synapse type classification
     * 
     * Returns the functional type of this synapse, which determines its computational
     * role and expected sign of influence on the target neuron. Synapse types categorize
     * connections into functional groups.
     * 
     * @return SynapseType The functional type of this synapse
     * @note Synapse types include Excitatory, Inhibitory, Modulatory, etc. Each type
     * has different computational properties and may have different default parameters.
     * 
     * @see SynapseType Enumerated synapse types
     * @see setType() Set synapse type
     * @see isExcitatory() Check if excitatory
     * @see isInhibitory() Check if inhibitory
     */
    SynapseType getType() const;
    
    /**
     * @brief Sets the synapse type classification
     * 
     * Sets the functional type of this synapse, affecting its computational role
     * and expected influence on the target neuron. Type changes can have significant
     * effects on network dynamics.
     * 
     * @param type New functional type for this synapse
     * @note Valid types include Excitatory, Inhibitory, Modulatory, etc. The available
     * types depend on the brain configuration and network architecture.
     * 
     * @see getType() Get current synapse type
     * @see SynapseType Enumerated synapse types
     * @see isExcitatory() Check if excitatory
     * @see isInhibitory() Check if inhibitory
     */
    void setType(SynapseType type);
    
    /**
     * @brief Checks if this synapse is excitatory
     * 
     * Returns true if this synapse is classified as excitatory, meaning it has a
     * positive weight and tends to depolarize the target neuron (increase firing likelihood).
     * 
     * @return true if synapse is excitatory, false otherwise
     * @note Excitatory synapses facilitate neural firing and promote network activity.
     * They are typically associated with positive synaptic weights.
     * 
     * @see getType() Get synapse type
     * @see isInhibitory() Check if inhibitory
     * @see getWeight() Get synaptic weight
     * @see step() Apply synaptic influence during simulation
     */
    bool isExcitatory() const;
    
    /**
     * @brief Checks if this synapse is inhibitory
     * 
     * Returns true if this synapse is classified as inhibitory, meaning it has a
     * negative weight and tends to hyperpolarize the target neuron (decrease firing likelihood).
     * 
     * @return true if synapse is inhibitory, false otherwise
     * @note Inhibitory synapses suppress neural firing and dampen network activity.
     * They are typically associated with negative synaptic weights.
     * 
     * @see getType() Get synapse type
     * @see isExcitatory() Check if excitatory
     * @see getWeight() Get synaptic weight
     */
    bool isInhibitory() const;
    
    /**
     * @brief Records a presynaptic spike for plasticity computation
     * 
     * Records the timestamp of a spike from the source neuron (presynaptic spike),
     * which is used by plasticity rules (especially STDP) to compute weight changes.
     * This creates the temporal information needed for spike-timing dependent plasticity.
     * 
     * @param timestamp Time of the presynaptic spike
     * @note Presynaptic spikes are essential for STDP and other timing-dependent plasticity
     * rules that modulate synaptic strength based on the precise timing between
     * presynaptic and postsynaptic spikes.
     * 
     * @see recordPostSpike() Record postsynaptic spikes
     * @see getPreSpikeHistory() Get presynaptic spike history
     * @see getPostSpikeHistory() Get postsynaptic spike history
     * @see STDP Plasticity rule implementation
     * @see enablePlasticity() Enable STDP for this synapse
     */
    void recordPreSpike(Timestamp timestamp);
    
    /**
     * @brief Records a postsynaptic spike for plasticity computation
     * 
     * Records the timestamp of a spike from the target neuron (postsynaptic spike),
     * which is used by plasticity rules (especially STDP) to compute weight changes.
     * This completes the pair with presynaptic spikes for timing-dependent plasticity.
     * 
     * @param timestamp Time of the postsynaptic spike
     * @note Postsynaptic spikes work in conjunction with presynaptic spikes to
     * determine the direction and magnitude of synaptic weight changes. The relative
     * timing between pre- and postsynaptic spikes determines whether the synapse
     * strengthens (LTP) or weakens (LTD).
     * 
     * @see recordPreSpike() Record presynaptic spikes
     * @see getPreSpikeHistory() Get presynaptic spike history
     * @see getPostSpikeHistory() Get postsynaptic spike history
     * @see STDP Plasticity rule implementation
     * @see enablePlasticity() Enable STDP for this synapse
     * 
     * @warning Recording both pre- and postsynaptic spikes is required for STDP
     * to function correctly. Missing either type of spike will prevent proper
     * plasticity computation.
     */
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
