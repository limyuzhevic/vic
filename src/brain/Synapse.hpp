#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Forward declaration
class Neuron;

/**
 * @brief Synapse representing a connection between neurons
 * 
 * @details Implements real synaptic transmission with delays and short-term plasticity.
 *          Synapses are the fundamental communication channels in the neural system,
 *          transmitting signals from pre-synaptic to post-synaptic neurons. Each synapse
 *          has unique properties including weight, delay, type, and plasticity rules.
 * 
 * @note Thread-unsafe access requires external synchronization
 * @warning Synapse IDs must be unique within their region
 * @ingroup Neuronal Dynamics
 */
class Synapse {
/**
     * @brief Constructor - Create synapse with neuron connections
     * 
     * @details Creates a new synapse connecting a source neuron to a destination neuron.
     *          The synapse is initialized with default parameters and ready for use
     *          in neural communication.
     * 
     * @param id Unique identifier for this synapse
     * @param source NeuronId of the pre-synaptic neuron (source)
     * @param destination NeuronId of the post-synaptic neuron (destination)
     * 
     * @throws std::bad_alloc if memory allocation fails
     * @note Synapse ID must be unique within its region
     * @warning Source and destination neurons must exist when synapse is activated
     * @thread_safety Thread-unsafe initialization
     * @post Synapse is created and ready for configuration
     * @ingroup Neuronal Dynamics
     */
    Synapse(SynapseId id, NeuronId source, NeuronId destination);

    /**
     * @brief Destructor - Clean up synapse resources
     * 
     * @details Releases all allocated memory and cleans up synapse state.
     *          Performs cleanup of spike history and internal implementation data.
     * 
     * @note All connections should be properly handled before synapse destruction
     * @thread_safety Thread-safe if no concurrent operations
     * @post Synapse resources are freed
     */
    ~Synapse();

    /**
     * @brief Copy constructor (deleted)
     * 
     * @details Copy construction is not supported to prevent accidental copying
     *          and maintain object ownership semantics.
     * 
     * @note Synapse contains non-copyable resources (pointer to implementation)
     * @warning Never use copy constructor - explicitly deleted
     */
    Synapse(const Synapse&) = delete;

    /**
     * @brief Copy assignment operator (deleted)
     * 
     * @details Copy assignment is not supported for the same reasons as copy constructor.
     * 
     * @note Prevents shallow copying of internal resources
     * @warning Never use copy assignment - explicitly deleted
     */
    Synapse& operator=(const Synapse&) = delete;

    /**
     * @brief Move constructor (noexcept)
     * 
     * @details Move constructor allows efficient transfer of synapse ownership
     *          by moving the internal pointer to implementation. The source synapse
     *          becomes invalid after move operation.
     * 
     * @param other Synapse to move from
     * 
     * @note Source synapse becomes unusable after move
     * @thread_safety Exception-safe
     * @post Source synapse is in valid but unspecified state
     * @ingroup Neuronal Dynamics
     */
    Synapse(Synapse&&) noexcept;

    /**
     * @brief Move assignment operator (noexcept)
     * 
     * @details Move assignment allows efficient transfer of synapse ownership,
     *          properly cleaning up existing resources before transferring.
     * 
     * @param other Synapse to move from
     * 
     * @return Reference to this synapse after assignment
     * @note Source synapse becomes unusable after move
     * @thread_safety Exception-safe, may throw
     * @post Source synapse is in valid but unspecified state
     * @ingroup Neuronal Dynamics
     */
    Synapse& operator=(Synapse&&) noexcept;

    /**
     * @brief Get synapse ID
     * 
     * @details Retrieves the unique identifier of this synapse, which is used
     *          for addressing this synapse in connections and state queries.
     * 
     * @return SynapseId Identifier of this synapse
     * 
     * @note ID is immutable after construction
     * @thread_safety Thread-safe - read-only access
     * @return Unique synapse identifier
     * @ingroup Neuronal Dynamics
     */
    SynapseId getId() const;

    /**
     * @brief Get source neuron ID
     * 
     * @details Retrieves the unique identifier of the pre-synaptic neuron
     *          that sends signals through this synapse.
     * 
     * @return NeuronId Identifier of source neuron
     * 
     * @note Identifies where this synapse originates
     * @thread_safety Thread-safe - read-only access
     * @return Source neuron ID
     * @ingroup Neuronal Dynamics
     */
    NeuronId getSourceNeuron() const;

    /**
     * @brief Get destination neuron ID
     * 
     * @details Retrieves the unique identifier of the post-synaptic neuron
     *          that receives signals through this synapse.
     * 
     * @return NeuronId Identifier of destination neuron
     * 
     * @note Identifies where this synapse terminates
     * @thread_safety Thread-safe - read-only access
     * @return Destination neuron ID
     * @ingroup Neuronal Dynamics
     */
    NeuronId getDestinationNeuron() const;

    /**
     * @brief Get synaptic weight
     * 
     * @details Retrieves the current synaptic weight, which determines the
     *          strength of the connection. Positive weights represent excitatory
     *          connections, negative weights represent inhibitory connections.
     * 
     * @return SynapticWeight Current synaptic weight
     * 
     * @note Weight is continuously modified by plasticity rules
     * @thread_safety Thread-safe - read-only access
     * @return Synaptic weight
     * @ingroup Neuronal Dynamics
     */
    SynapticWeight getWeight() const;

    /**
     * @brief Set synaptic weight
     * 
     * @details Changes the synaptic weight, directly modifying connection strength.
     *          This bypasses the gradual plasticity learning process and is typically
     *          used for initialization or testing.
     * 
     * @param weight New synaptic weight (positive for excitatory, negative for inhibitory)
     * 
     * @throws std::runtime_error if synapse is in use
     * @note Plasticity rules are not applied when weight is set directly
     * @thread_safety Thread-unsafe if synapse is in simulation
     * @post Synaptic weight is updated
     * @ingroup Neuronal Dynamics
     */
    void setWeight(SynapticWeight weight);

    /**
     * @brief Add to synaptic weight
     * 
     * @details Incrementally changes synaptic weight by the specified delta.
     *          This simulates the effect of plasticity rules or external modulation.
     * 
     * @param delta Amount to change synaptic weight
     * 
     * @note Plasticity rules may add additional weight changes
     * @thread_safety Thread-unsafe if concurrent with other weight operations
     * @post Synaptic weight is changed by delta
     * @ingroup Neuronal Dynamics
     */
    void addToWeight(SynapticWeight delta);

    /**
     * @brief Get synaptic delay
     * 
     * @details Retrieves the transmission delay for this synapse in simulation steps.
     *          This determines when the post-synaptic effect occurs relative to the
     *          pre-synaptic spike.
     * 
     * @return Delay Transmission delay in simulation steps
     * 
     * @note Controls timing of signal transmission
     * @thread_safety Thread-safe - read-only access
     * @return Delay in steps
     * @ingroup Neuronal Dynamics
     */
    Delay getDelay() const;

    /**
     * @brief Set synaptic delay
     * 
     * @details Changes the transmission delay for this synapse. This affects
     *          the timing of signal transmission from pre-synaptic to post-synaptic
     *          neuron. Should be set before synapse is added to simulation.
     * 
     * @param delay New transmission delay in simulation steps
     * 
     * @throws std::runtime_error if synapse is in use
     * @note Delay affects spike timing-dependent plasticity
     * @thread_safety Thread-unsafe if synapse is in simulation
     * @post Delay is updated
     * @ingroup Neuronal Dynamics
     */
    void setDelay(Delay delay);

    /**
     * @brief Get synapse type
     * 
     * @details Retrieves the type classification of this synapse, which indicates
     *          its functional role in the neural network.
     * 
     * @return SynapseType Type of this synapse
     * 
     * @note Type affects synaptic properties and plasticity rules
     * @thread_safety Thread-safe - read-only access
     * @return Synapse type
     * @ingroup Neuronal Dynamics
     */
    SynapseType getType() const;

    /**
     * @brief Set synapse type
     * 
     * @details Changes the type classification of this synapse. This affects
     *          synaptic behavior, plasticity rules, and functional role in the network.
     * 
     * @param type New synapse type
     * 
     * @throws std::runtime_error if synapse is in use
     * @note Type affects plasticity and signal transmission properties
     * @thread_safety Thread-unsafe if synapse is in simulation
     * @post Synapse type is updated
     * @ingroup Neuronal Dynamics
     */
    void setType(SynapseType type);

    /**
     * @brief Check if synapse is excitatory
     * 
     * @details Determines if this synapse has an excitatory effect on the
     *          post-synaptic neuron based on its weight and type.
     * 
     * @return bool true if synapse is excitatory (positive weight)
     * 
     * @note Used to classify synaptic influence
     * @thread_safety Thread-safe - read-only access
     * @return Excitatory status
     * @ingroup Neuronal Dynamics
     */
    bool isExcitatory() const;

    /**
     * @brief Check if synapse is inhibitory
     * 
     * @details Determines if this synapse has an inhibitory effect on the
     *          post-synaptic neuron based on its weight and type.
     * 
     * @return bool true if synapse is inhibitory (negative weight)
     * 
     * @note Used to classify synaptic influence
     * @thread_safety Thread-safe - read-only access
     * @return Inhibitory status
     * @ingroup Neuronal Dynamics
     */
    bool isInhibitory() const;

    /**
     * @brief Record pre-synaptic spike
     * 
     * @details Records that the pre-synaptic neuron has spiked at the specified
     *          time. This spike history is used by plasticity rules to modify
     *          synaptic weight based on spike timing.
     * 
     * @param timestamp Time at which the pre-synaptic neuron spiked
     * 
     * @throws std::runtime_error if timestamp is invalid
     * @note Spike timing critical for STDP and other timing-based plasticity
     * @thread_safety Thread-unsafe if concurrent with spike recording
     * @post Spike timestamp is added to history
     * @ingroup Neuronal Dynamics
     */
    void recordPreSpike(Timestamp timestamp);

    /**
     * @brief Record post-synaptic spike
     * 
     * @details Records that the post-synaptic neuron has spiked at the specified
     *          time. This spike history is used by some plasticity rules to modify
     *          synaptic weight based on spike timing relationships.
     * 
     * @param timestamp Time at which the post-synaptic neuron spiked
     * 
     * @throws std::runtime_error if timestamp is invalid
     * @note Post-synaptic spike timing important for certain plasticity mechanisms
     * @thread_safety Thread-unsafe if concurrent with spike recording
     * @post Spike timestamp is added to history
     * @ingroup Neuronal Dynamics
     */
    void recordPostSpike(Timestamp timestamp);

    /**
     * @brief Get pre-synaptic spike history
     * 
     * @details Retrieves the history of pre-synaptic spikes, including timestamps
     *          and potentially other spike-related information. This is used by
     *          plasticity rules to compute weight changes based on spike timing.
     * 
     * @return const std::vector<Timestamp>& Reference to vector of pre-synaptic spike timestamps
     * 
     * @note History is maintained for plasticity computation
     * @thread_safety Thread-safe - read-only access
     * @return Spike history vector
     * @ingroup Neuronal Dynamics
     */
    const std::vector<Timestamp>& getPreSpikeHistory() const;

    /**
     * @brief Get post-synaptic spike history
     * 
     * @details Retrieves the history of post-synaptic spikes, including timestamps
     *          and potentially other spike-related information. This is used by
     *          plasticity rules to compute weight changes based on spike timing.
     * 
     * @return const std::vector<Timestamp>& Reference to vector of post-synaptic spike timestamps
     * 
     * @note History is maintained for plasticity computation
     * @thread_safety Thread-safe - read-only access
     * @return Spike history vector
     * @ingroup Neuronal Dynamics
     */
    const std::vector<Timestamp>& getPostSpikeHistory() const;

    /**
     * @brief Clear spike history
     * 
     * @details Clears both pre-synaptic and post-synaptic spike histories.
     *          This resets the spike timing information used by plasticity rules.
     * 
     * @post Spike histories are empty
     * @thread_safety Thread-unsafe if concurrent with spike recording
     * @ingroup Neuronal Dynamics
     */
    void clearHistory();

    /**
     * @brief Get plasticity flags
     * 
     * @details Retrieves the plasticity flags that determine which plasticity
     *          rules are active for this synapse (Hebbian, STDP, reward-modulated).
     * 
     * @return const PlasticityFlags& Const reference to plasticity flags
     * 
     * @note Flags control which plasticity mechanisms are applied
     * @thread_safety Thread-safe - read-only access
     * @return Plasticity flags
     * @ingroup Plasticity and Learning
     */
    const PlasticityFlags& getPlasticityFlags() const;

    /**
     * @brief Get plasticity flags (non-const)
     * 
     * @details Retrieves mutable reference to plasticity flags, allowing direct
     *          modification of which plasticity rules are active.
     * 
     * @return PlasticityFlags& Reference to plasticity flags
     * 
     * @note Use with caution as this bypasses normal plasticity rule selection
     * @thread_safety Thread-unsafe if concurrent with plasticity operations
     * @return Mutable plasticity flags reference
     * @ingroup Plasticity and Learning
     */
    PlasticityFlags& getPlasticityFlags();

    /**
     * @brief Enable/disable plasticity rules
     * 
     * @details Activates or deactivates specific plasticity rules for this synapse,
     *          controlling which learning mechanisms will modify synaptic weight.
     * 
     * @param hebbian Enable Hebbian plasticity
     * @param stdp Enable STDP plasticity
     * @param rewardModulated Enable reward-modulated plasticity
     * 
     * @throws std::runtime_error if synapse is in use
     * @note Plasticity rules determine how and when synaptic weight changes
     * @thread_safety Thread-unsafe if synapse is in simulation
     * @post Plasticity rules are configured
     * @ingroup Plasticity and Learning
     */
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);

    /**
     * @brief Get eligibility trace
     * 
     * @details Retrieves the eligibility trace value, which is used by reward-modulated
     *          plasticity rules to determine how recent spikes contribute to learning.
     *          Eligibility traces decay over time and are modified by spikes.
     * 
     * @return float Current eligibility trace value
     * 
     * @note Used in reward-modulated learning algorithms
     * @thread_safety Thread-safe - read-only access
     * @return Eligibility trace value
     * @ingroup Plasticity and Learning
     */
    float getEligibilityTrace() const;

    /**
     * @brief Set eligibility trace
     * 
     * @details Sets the eligibility trace value directly. This bypasses the
     *          automatic accumulation from spike events and is typically used
     *          for initialization or testing.
     * 
     * @param trace New eligibility trace value
     * 
     * @throws std::runtime_error if synapse is in use
     * @note Eligibility traces modulate reward-modulated plasticity
     * @thread_safety Thread-unsafe if synapse is in simulation
     * @post Eligibility trace is set directly
     * @ingroup Plasticity and Learning
     */
    void setEligibilityTrace(float trace);

    /**
     * @brief Decay eligibility trace
     * 
     * @details Decays the eligibility trace by the specified factor, simulating
     *          the natural decay of synaptic eligibility over time in the absence
     *          of reward signals.
     * 
     * @param decayRate Decay factor (0.0 to 1.0, where 1.0 means complete decay)
     * 
     * @throws std::runtime_error if decay rate is invalid
     * @note Eligibility traces naturally decay without reward signals
     * @thread_safety Thread-unsafe if concurrent with other trace operations
     * @post Eligibility trace is decayed
     * @ingroup Plasticity and Learning
     */
    void decayEligibilityTrace(float decayRate);

    /**
     * @brief Get synaptic efficacy
     * 
     * @details Retrieves the synaptic efficacy value, which represents the
     *          use-dependent modulation of synaptic strength. This can account
     *          for short-term plasticity effects like depression and potentiation.
     * 
     * @return float Current synaptic efficacy
     * 
     * @note Efficacy modulates synaptic transmission independently of weight
     * @thread_safety Thread-safe - read-only access
     * @return Synaptic efficacy value
     * @ingroup Neuronal Dynamics
     */
    float getEfficacy() const;

    /**
     * @brief Set synaptic efficacy
     * 
     * @details Sets the synaptic efficacy value directly. Efficacy represents
     *          activity-dependent modulation of synaptic strength that operates
     *          independently of the long-term synaptic weight.
     * 
     * @param efficacy New synaptic efficacy value
     * 
     * @throws std::runtime_error if synapse is in use
     * @note Efficacy is separate from synaptic weight for short-term effects
     * @thread_safety Thread-unsafe if synapse is in simulation
     * @post Synaptic efficacy is set directly
     * @ingroup Neuronal Dynamics
     */
    void setEfficacy(float efficacy);

    /**
     * @brief Update synapse for one simulation step
     * 
     * @details Advances the synapse state for one simulation timestep, updating
     *          internal dynamics such as short-term plasticity, eligibility traces,
     *          and other use-dependent mechanisms.
     * 
     * @param currentTime Current simulation time in milliseconds
     * 
     * @throws std::runtime_error if update fails
     * @note This is called automatically during brain simulation
     * @thread_safety Thread-unsafe if concurrent with other synapse operations
     * @post Synapse internal state is updated
     * @ingroup Neuronal Dynamics
     */
    void step(Timestamp currentTime);

    /**
     * @brief Reset to initial state
     * 
     * @details Resets the synapse to its initial state, clearing spike history,
     *          resetting eligibility trace to zero, and restoring default efficacy.
     *          This is typically used at the start of a new simulation episode.
     * 
     * @throws std::runtime_error if reset fails
     * @note This does not reset synaptic weight (long-term memory)
     * @thread_safety Thread-unsafe if synapse is in simulation
     * * @post Short-term state is reset
     * @ingroup Neuronal Dynamics
     */
    void reset();

    /**
     * @brief Initialize with random parameters
     * 
     * @details Initializes synapse parameters with biologically plausible random values.
     *          This includes setting random weight, delay, and efficacy values based on
     *          statistical distributions typical for neural systems.
     * 
     * @param rng Reference to random number generator for sampling parameters
     * 
     * @throws std::runtime_error if initialization fails
     * @note Should be called before synapse is added to simulation
     * @thread_safety Thread-unsafe before simulation
     * @post Synapse parameters are initialized with random values
     * @ingroup Neuronal Dynamics
     */
    void initializeRandom(class RandomGenerator& rng);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
