#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

/**
 * @brief Forward declarations for Synapse class dependencies
 * 
 * These classes are used in Synapse's interface but not fully included
 * to avoid circular dependencies and improve compilation performance.
 */
class Neuron;

/**
 * @brief Represents a connection between two neurons
 * 
 * Synapse implements synaptic transmission between neurons, including delays
 * for signal propagation and support for short-term plasticity mechanisms.
 * Synapses are the fundamental communication channels in spiking neural networks,
 * carrying information from presynaptic to postsynaptic neurons.
 * 
 * Key features:
 * - Directional connection (presynaptic → postsynaptic)
 * - Configurable delay for signal propagation
 * - Adjustable synaptic weight for connection strength
 * - Spike timing history for plasticity mechanisms
 * - Support for multiple plasticity rules
 * - Short-term plasticity and efficacy modulation
 * - Eligibility traces for reward-modulated learning
 * 
 * @note This is a placeholder implementation. Phase 2 will implement
 *       detailed synaptic dynamics including conductance-based models,
 *       specific neurotransmitter types, and more sophisticated plasticity
 *       mechanisms.
 * 
 * @see Neuron Presynaptic and postsynaptic neurons
 * @see PlasticityFlags Controls which plasticity rules are active
 * @see NeuralRegion Contains local synapses
 * @see Brain Top-level coordinator of neural networks
 * 
 * Example usage:
 * @code
 * Synapse synapse(0, 1, 2); // Synapse ID 0 from neuron 1 to neuron 2
 * synapse.setWeight(0.5f);
 * synapse.setDelay(2); // 2 timestep delay
 * synapse.recordPreSpike(currentTime);
 * synapse.recordPostSpike(currentTime);
 * @endcode
 */
class Synapse {
public:
    /**
     * @brief Constructor creating a synapse with source and destination
     * 
     * Creates a new synapse connection between two neurons. The synapse is
     * initialized with default parameters and in a ready state to transmit
     * signals. The IDs are used for identification and connectivity management.
     * 
     * @param id Unique identifier for this synapse (must be positive and unique)
     * @param source Neuron identifier for the presynaptic neuron
     * @param destination Neuron identifier for the postsynaptic neuron
     * @throws std::invalid_argument if IDs are invalid or already in use
     * 
     * @note Synapse IDs should be managed carefully to avoid conflicts and
     *       ensure proper connectivity in large networks.
     */
    Synapse(SynapseId id, NeuronId source, NeuronId destination);
    
    /**
     * @brief Destructor
     */
    ~Synapse();
    
    /**
     * @brief Copy constructor (deleted)
     * 
     * Synapse objects cannot be copied to prevent resource management issues
     * and maintain synapse integrity. Each synapse is a unique communication
     * channel that should be moved rather than copied.
     */
    Synapse(const Synapse&) = delete;
    
    /**
     * @brief Copy assignment operator (deleted)
     */
    Synapse& operator=(const Synapse&) = delete;
    
    /**
     * @brief Move constructor
     * 
     * Efficiently transfers ownership of internal resources from another Synapse
     * object. The source object becomes unusable after the move operation.
     * 
     * @param other The Synapse object to move from
     */
    Synapse(Synapse&&) noexcept;
    
    /**
     * @brief Move assignment operator
     * 
     * Efficiently transfers ownership of internal resources from another Synapse
     * object. The source object becomes unusable after the move operation.
     * 
     * @param other The Synapse object to move from
     * @return Reference to this Synapse object
     */
    Synapse& operator=(Synapse&&) noexcept;
    
    /**
     * @brief Get the unique identifier of this synapse
     * 
     * Returns the synapse ID that was specified during construction. This ID
     * is used for referencing the synapse in connectivity, statistics, and
     * debugging purposes.
     * 
     * @return SynapseId The unique identifier for this synapse
     */
    SynapseId getId() const;
    
    /**
     * @brief Get the source neuron identifier
     * 
     * Returns the neuron identifier of the presynaptic neuron (the neuron that
     * sends signals through this synapse). This defines the direction of the
     * connection.
     * 
     * @return NeuronId The identifier of the source neuron
     */
    NeuronId getSourceNeuron() const;
    
    /**
     * @brief Get the destination neuron identifier
     * 
     * Returns the neuron identifier of the postsynaptic neuron (the neuron that
     * receives signals through this synapse). This defines the direction of the
     * connection.
     * 
     * @return NeuronId The identifier of the destination neuron
     */
    NeuronId getDestinationNeuron() const;
    
    /**
     * @brief Get the synaptic weight
     * 
     * Returns the current weight of this synapse, which determines the strength
     * of the connection. The interpretation of the weight depends on the synapse
     * type: positive values typically indicate excitatory connections, negative
     * values inhibitory connections.
     * 
     * @return SynapticWeight Current synaptic weight
     * 
     * @note Weight values are typically normalized between -1.0 and 1.0,
     *       but can be any value depending on the model and scale used.
     */
    SynapticWeight getWeight() const;
    
    /**
     * @brief Set the synaptic weight
     * 
     * Changes the synaptic weight, which determines the strength of the
     * connection. This is the primary way to control synaptic strength and
     * is used by plasticity mechanisms.
     * 
     * @param weight The new synaptic weight
     * 
     * @note Weight interpretation depends on synapse type and model.
     *       Positive weights typically indicate excitatory connections,
     *       negative weights inhibitory connections.
     */
    void setWeight(SynapticWeight weight);
    
    /**
     * @brief Add to synaptic weight
     * 
     * Modifies the synaptic weight by adding the specified delta value. This
     * is used by plasticity mechanisms to gradually change synaptic strength
     * based on activity and learning rules.
     * 
     * @param delta The amount to add to the synaptic weight
     */
    void addToWeight(SynapticWeight delta);
    
    /**
     * @brief Get the synaptic delay
     * 
     * Returns the current delay of this synapse, which represents the number
     * of timesteps required for signals to propagate from the presynaptic
     * to the postsynaptic neuron. This introduces temporal dynamics into the
     * network.
     * 
     * @return Delay The synaptic delay in timesteps
     */
    Delay getDelay() const;
    
    /**
     * @brief Set the synaptic delay
     * 
     * Changes the synaptic delay, which affects the timing of signal
     * transmission. Delays are used to implement realistic propagation times
     * and temporal dynamics in the network.
     * 
     * @param delay The new synaptic delay (in timesteps)
     */
    void setDelay(Delay delay);
    
    /**
     * @brief Get the synapse type
     * 
     * Returns the type of this synapse, which determines its characteristics
     * and behavior. Synapse types include excitatory, inhibitory, and modulatory
     * connections, each with different properties.
     * 
     * @return SynapseType The synapse type (Excitatory, Inhibitory, Modulatory)
     */
    SynapseType getType() const;
    
    /**
     * @brief Set the synapse type
     * 
     * Changes the synapse type, which affects its properties and behavior.
     * This can be useful for changing the functional role of a synapse
     * during development or learning.
     * 
     * @param type The new synapse type
     */
    void setType(SynapseType type);
    
    /**
     * @brief Check if the synapse is excitatory
     * 
     * Determines whether this synapse is excitatory, meaning it increases the
     * likelihood of firing in the postsynaptic neuron when activated.
     * 
     * @return bool true if synapse is excitatory, false otherwise
     */
    bool isExcitatory() const;
    
    /**
     * @brief Check if the synapse is inhibitory
     * 
     * Determines whether this synapse is inhibitory, meaning it decreases the
     * likelihood of firing in the postsynaptic neuron when activated.
     * 
     * @return bool true if synapse is inhibitory, false otherwise
     */
    bool isInhibitory() const;
    
    /**
     * @brief Record a presynaptic spike
     * 
     * Records the timestamp of a spike in the presynaptic neuron. This is
     * essential for spike-timing-dependent plasticity (STDP) and other
     * timing-dependent computations that depend on precise spike timing.
     * 
     * @param timestamp The simulation time when the presynaptic spike occurred
     */
    void recordPreSpike(Timestamp timestamp);
    
    /**
     * @brief Record a postsynaptic spike
     * 
     * Records the timestamp of a spike in the postsynaptic neuron. This is
     * used by certain plasticity mechanisms and for analysis of firing patterns.
     * 
     * @param timestamp The simulation time when the postsynaptic spike occurred
     */
    void recordPostSpike(Timestamp timestamp);
    
    /**
     * @brief Get the presynaptic spike history
     * 
     * Returns a reference to the vector containing timestamps of all spikes
     * in the presynaptic neuron. This is used for spike-timing-dependent
     * plasticity and other timing-dependent computations.
     * 
     * @return const std::vector<Timestamp>& Reference to the presynaptic spike history
     */
    const std::vector<Timestamp>& getPreSpikeHistory() const;
    
    /**
     * @brief Get the postsynaptic spike history
     * 
     * Returns a reference to the vector containing timestamps of all spikes
     * in the postsynaptic neuron. This is used for certain plasticity mechanisms
     * and analysis of firing patterns.
     * 
     * @return const std::vector<Timestamp>& Reference to the postsynaptic spike history
     */
    const std::vector<Timestamp>& getPostSpikeHistory() const;
    
    /**
     * @brief Clear spike history
     * 
     * Removes all spike timestamps from both presynaptic and postsynaptic
     * histories. This is called at the end of each timestep to maintain
     * a rolling window of recent spikes for STDP and other timing-dependent
     * computations.
     */
    void clearHistory();
    
    /**
     * @brief Get plasticity flags
     * 
     * Returns a reference to the plasticity flags that determine which plasticity
     * mechanisms are active for this synapse. This controls how the synapse
     * strength can change based on activity.
     * 
     * @return const PlasticityFlags& Reference to the plasticity flags
     */
    const PlasticityFlags& getPlasticityFlags() const;
    
    /**
     * @brief Get mutable plasticity flags
     * 
     * Returns a mutable reference to the plasticity flags for this synapse.
     * This is used by the plasticity engine to modify synapse plasticity properties.
     * 
     * @return PlasticityFlags& Reference to the plasticity flags
     */
    PlasticityFlags& getPlasticityFlags();
    
    /**
     * @brief Enable plasticity mechanisms
     * 
     * Enables specified plasticity mechanisms for this synapse, allowing its
     * strength to be modified based on activity. This is used to control
     * which plasticity rules are active.
     * 
     * @param hebbian Enable Hebbian plasticity
     * @param stdp Enable spike-timing-dependent plasticity
     * @param rewardModulated Enable reward-modulated plasticity
     */
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    /**
     * @brief Get the eligibility trace
     * 
     * Returns the current eligibility trace, which is used in reward-modulated
     * learning to track recent activity that could be reinforced by future rewards.
     * The eligibility trace decays over time and is multiplied by reward signals
     * to determine synaptic weight changes.
     * 
     * @return float Current eligibility trace value
     */
    float getEligibilityTrace() const;
    
    /**
     * @brief Set the eligibility trace
     * 
     * Sets the eligibility trace to the specified value. The eligibility trace
     * is used in reward-modulated learning to track recent activity that could
     * be reinforced by future rewards.
     * 
     * @param trace The new eligibility trace value
     */
    void setEligibilityTrace(float trace);
    
    /**
     * @brief Decay the eligibility trace
     * 
     * Decays the eligibility trace by multiplying it by an exponential decay
     * factor. This simulates the natural decay of synaptic eligibility for
     * reward-modulated learning over time.
     * 
     * @param decayRate The decay rate (should be between 0.0 and 1.0)
     */
    void decayEligibilityTrace(float decayRate);
    
    /**
     * @brief Get the synaptic efficacy
     * 
     * Returns the current synaptic efficacy, which represents the use-dependent
     * modulation of synaptic strength. Efficacy changes based on recent
     * activity patterns and can interact with plasticity to determine the
     * final synaptic weight.
     * 
     * @return float Current synaptic efficacy
     */
    float getEfficacy() const;
    
    /**
     * @brief Set the synaptic efficacy
     * 
     * Sets the synaptic efficacy to the specified value. The efficacy is
     * use-dependent and can be modulated by recent activity patterns.
     * 
     * @param efficacy The new synaptic efficacy
     */
    void setEfficacy(float efficacy);
    
    /**
     * @brief Update synapse for one simulation step
     * 
     * Advances the synapse by one timestep using the current simulation time.
     * This is the primary update function that handles signal propagation,
     * eligibility trace decay, and other time-dependent processes.
     * 
     * @param currentTime The current simulation time in seconds
     * 
     * @note This function implements synaptic transmission dynamics including
     *       delay management, eligibility trace updates, and potential signal
     *       delivery to the postsynaptic neuron.
     */
    void step(Timestamp currentTime);
    
    /**
     * @brief Reset synapse to initial state
     * 
     * Resets the synapse to its initial state as defined during construction.
     * This includes resetting spike history, eligibility trace, and other
     * internal state variables. This is useful for running multiple simulation
     * trials with the same initial conditions.
     */
    void reset();
    
    /**
     * @brief Initialize synapse with random parameters
     * 
     * Sets up the synapse with random parameters such as weight, delay, and
     * other properties. This is typically called once at the beginning of
     * the simulation to establish a diverse initial connectivity pattern.
     * 
     * @param rng Reference to the random number generator to use for initialization
     */
    void initializeRandom(class RandomGenerator& rng);
    
private:
    /**
     * @brief Internal implementation details (Pimpl idiom)
     * 
     * Encapsulates the internal implementation of the Synapse class,
     * allowing for better compilation times and hiding implementation details
     * from the header file.
     */
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm