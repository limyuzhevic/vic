#pragma once

/**
 * @file Synapse.hpp
 * @brief Synaptic connection implementation between neurons
 * @author NLM Project
 * @date Phase 3
 * 
 * This file defines the Synapse class, which represents a synaptic connection
 * between two neurons in the NLM computational neuroscience platform. Synapses
 * implement real synaptic transmission with delays, short-term plasticity, and
 * plasticity rule application.
 * 
 * Key features:
 * - Directed connections from source to destination neurons
 * - Synaptic weight modulation for strength adjustment
 * - Spike timing-dependent plasticity (STDP)
 * - Reward-modulated learning via eligibility traces
 * - Short-term plasticity mechanisms ( Facilitation, Depression)
 * 
 * The synapse implementation includes:
 * - Transmission delay between pre- and post-synaptic neurons
 * - Spike history recording for plasticity computations
 * - Eligibility trace management for reward-modulated learning
 * - Synaptic efficacy tracking for use-dependent modulation
 * 
 * @note This class uses the PImpl idiom to minimize compilation dependencies
 *       and improve encapsulation of implementation details.
 */

/**
 * @brief Forward declaration to avoid circular dependencies
 */
class Neuron;

/**
 * @class Synapse
 * @brief Synaptic connection between neurons with plasticity support
 * 
 * The Synapse class implements a directed synaptic connection from a source
 * neuron to a destination neuron. It provides the fundamental mechanism for
 * neural communication in the NLM platform, including spike transmission,
 * weight modulation, and learning mechanisms.
 * 
 * The synapse supports multiple plasticity mechanisms:
 * - Hebbian learning ("fire together, wire together")
 * - Spike-timing-dependent plasticity (STDP)
 * - Reward-modulated learning
 * 
 * Each synapse maintains:
 * - Connection identity (source/destination neurons)
 * - Weight for transmission strength
 * - Transmission delay for timing accuracy
 * - Spike history for plasticity computations
 * - Plasticity flags for rule activation
 * - Eligibility trace for reward-modulated learning
 * 
 * @note The implementation follows the standard synaptic model used in
 *       computational neuroscience, with extensions for more complex dynamics.
 * 
 * @see Neuron - the pre- and post-synaptic neurons
 * @see PlasticityRule - for synaptic plasticity mechanisms
 * @see NeuronState - for postsynaptic neuron state changes
 */
class Synapse {
public:
    /**
     * @brief Constructor creates a new synaptic connection
     * 
     * @param id Unique identifier for this synapse
     * @param source Neuron ID of the presynaptic neuron
     * @param destination Neuron ID of the postsynaptic neuron
     * 
     * @throws std::bad_alloc if memory allocation fails
     * 
     * @note Synapse IDs are used for graph operations and connectivity tracing.
     *       Source and destination IDs must be valid neuron IDs in the system.
     */
    Synapse(SynapseId id, NeuronId source, NeuronId destination);

    /**
     * @brief Destructor cleans up synapse resources
     */
    ~Synapse();

    /**
     * @brief Copy constructor is deleted to prevent shallow copying
     * 
     * @warning Synapses are not copyable due to internal PImpl structure.
     *          Use move operations instead.
     */
    Synapse(const Synapse&) = delete;

    /**
     * @brief Copy assignment operator is deleted to prevent shallow copying
     * 
     * @warning Synapses are not copyable due to internal PImpl structure.
     *          Use move operations instead.
     */
    Synapse& operator=(const Synapse&) = delete;

    /**
     * @brief Move constructor efficiently transfers ownership
     * 
     * @param other Synapse to move from
     * @note After moving, the source synapse is in a valid but unspecified state.
     */
    Synapse(Synapse&&) noexcept;

    /**
     * @brief Move assignment operator efficiently transfers ownership
     * 
     * @param other Synapse to move from
     * @return Reference to this synapse
     * @note After moving, the source synapse is in a valid but unspecified state.
     */
    Synapse& operator=(Synapse&&) noexcept;

    /**
     * @brief Get the synapse's unique identifier
     * 
     * @return Synapse ID
     * @note Synapse IDs are used for graph operations and connectivity tracing.
     */
    SynapseId getId() const;

    /**
     * @brief Get source neuron ID (presynaptic)
     * 
     * @return Neuron ID of the presynaptic neuron
     * @note This neuron provides the spike train that triggers transmission.
     */
    NeuronId getSourceNeuron() const;

    /**
     * @brief Get destination neuron ID (postsynaptic)
     * 
     * @return Neuron ID of the postsynaptic neuron
     * @note This neuron receives the transmitted spike.
     */
    NeuronId getDestinationNeuron() const;

    /**
     * @brief Get synaptic weight
     * 
     * @return Synaptic weight
     * @note Weight range: typically [-1.0, 1.0] where positive = excitatory,
     *       negative = inhibitory. Weight = 0.0 means no connection.
     */
    SynapticWeight getWeight() const;

    /**
     * @brief Set synaptic weight
     * 
     * @param weight New synaptic weight
     * @param clampToRange If true, weight is clamped to valid range
     * @note Weight clamping prevents numerical instability.
     */
    void setWeight(SynapticWeight weight, bool clampToRange = true);

    /**
     * @brief Add delta to synaptic weight
     * 
     * @param delta Weight change amount
     * @see PlasticityRule for weight modification mechanisms
     */
    void addToWeight(SynapticWeight delta);

    /**
     * @brief Get synaptic delay in simulation steps
     * 
     * @return Delay (number of simulation steps)
     * @note Delay represents time between pre-synaptic spike and
     *       post-synaptic effect. 0 = instantaneous transmission.
     */
    Delay getDelay() const;

    /**
     * @brief Set synaptic delay
     * 
     * @param delay New delay in simulation steps
     * @param clampToRange If true, delay is clamped to valid range
     */
    void setDelay(Delay delay, bool clampToRange = true);

    /**
     * @brief Get synapse type
     * 
     * @return Synapse type
     * @note Type affects connection rules and plasticity eligibility.
     */
    SynapseType getType() const;

    /**
     * @brief Set synapse type
     * 
     * @param type New synapse type
     */
    void setType(SynapseType type);

    /**
     * @brief Check if synapse is excitatory
     * 
     * @return true if synapse is excitatory (positive weight)
     */
    bool isExcitatory() const;

    /**
     * @brief Check if synapse is inhibitory
     * 
     * @return true if synapse is inhibitory (negative weight)
     */
    bool isInhibitory() const;

    /**
     * @brief Record pre-synaptic spike for plasticity computation
     * 
     * @param timestamp Simulation time step of the spike
     * @note This spike will be used for STDP and other spike-timing-dependent
     *       plasticity rules. The timestamp is stored for later computation.
     */
    void recordPreSpike(Timestamp timestamp);

    /**
     * @brief Record post-synaptic spike for plasticity computation
     * 
     * @param timestamp Simulation time step of the spike
     * @note This spike will be used for STDP rules that depend on post-synaptic
     *       activity, such as anti-Hebbian plasticity.
     */
    void recordPostSpike(Timestamp timestamp);

    /**
     * @brief Get pre-synaptic spike history
     * 
     * @return Constant reference to timestamp vector
     * @note Spike history is used for STDP and other timing-dependent rules.
     */
    const std::vector<Timestamp>& getPreSpikeHistory() const;

    /**
     * @brief Get post-synaptic spike history
     * 
     * @return Constant reference to timestamp vector
     * @note Spike history is used for plasticity rules that depend on
     *       post-synaptic timing.
     */
    const std::vector<Timestamp>& getPostSpikeHistory() const;

    /**
     * @brief Clear all spike history
     * 
     * @note This is used to reset the synapse for testing or when changing
     *       connection patterns.
     */
    void clearHistory();

    /**
     * @brief Get plasticity flags for this synapse
     * 
     * @return Constant reference to plasticity flags
     * @note Flags determine which plasticity rules are active for this synapse.
     */
    const PlasticityFlags& getPlasticityFlags() const;

    /**
     * @brief Get mutable access to plasticity flags
     * 
     * @return Reference to plasticity flags
     * @warning Direct flag modification should be avoided except for
     *          specialized simulation code.
     */
    PlasticityFlags& getPlasticityFlags();

    /**
     * @brief Enable/disable specific plasticity rules
     * 
     * @param hebbian Enable Hebbian plasticity
     * @param stdp Enable STDP plasticity
     * @param rewardModulated Enable reward-modulated plasticity
     * @note Rules can be enabled/disabled independently for different synapses.
     */
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);

    /**
     * @brief Get eligibility trace value
     * 
     * @return Eligibility trace value
     * @note Eligibility trace represents accumulated weight changes that
     *       will be applied when reward is received (TD learning).
     */
    float getEligibilityTrace() const;

    /**
     * @brief Set eligibility trace value
     * 
     * @param trace New trace value
     */
    void setEligibilityTrace(float trace);

    /**
     * @brief Decay eligibility trace by factor
     * 
     * @param decayRate Decay factor (typically 0.0-1.0)
     * @note Eligibility traces decay over time, implementing temporal
     *       difference learning in a simple form.
     */
    void decayEligibilityTrace(float decayRate);

    /**
     * @brief Get synaptic efficacy
     * 
     * @return Synaptic efficacy value
     * @note Efficacy represents use-dependent modulation of synaptic strength,
     *       typically implemented through short-term plasticity mechanisms.
     */
    float getEfficacy() const;

    /**
     * @brief Set synaptic efficacy
     * 
     * @param efficacy New efficacy value
     */
    void setEfficacy(float efficacy);

    /**
     * @brief Advance synapse by one simulation step
     * 
     * This method implements synaptic dynamics for one time step:
     * - Apply eligibility trace decay
     * - Update short-term plasticity (facilitation/depression)
     * - Process spike transmission (with delays)
     * - Apply weight changes from plasticity rules
     * 
     * @param currentTime Current simulation time step
     * @note This is the core synapse simulation method.
     */
    void step(Timestamp currentTime);

    /**
     * @brief Reset synapse to initial state
     * 
     * @note This clears spike history, resets eligibility traces, and
     *       restores original weight and state variables.
     */
    void reset();

    /**
     * @brief Initialize synapse with random parameters
     * 
     * @param rng Random number generator
     * @note This populates synapse-specific parameters with random values
     *       for use in network initialization.
     */
    void initializeRandom(class RandomGenerator& rng);

protected:
    /**
     * @brief Protected constructor to enforce ID requirements
     * 
     * @param id Unique identifier for this synapse
     * @param source Presynaptic neuron ID
     * @param destination Postsynaptic neuron ID
     * @param initialize Automatically initialize synapse state
     */
    Synapse(SynapseId id, NeuronId source, NeuronId destination, bool initialize);

private:
    /**
     * @brief Private implementation details (PImpl idiom)
     */
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
