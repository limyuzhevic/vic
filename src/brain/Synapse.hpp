/*!
 * @file Synapse.hpp
 * @brief Synapse class representing a connection between neurons
 * @details Implements real synaptic transmission with delays and short-term plasticity.
 *          Each synapse connects one source neuron to one destination neuron and
 *          mediates signal transmission with synaptic weight and delay.
 * 
 * The Synapse class is fundamental to neural computation, enabling:
 * - Signal transmission between neurons
 * - Activity-dependent synaptic modification (plasticity)
 * - Temporal dynamics through synaptic delays
 * - Short-term plasticity effects (facilitation/depression)
 * - Eligibility traces for reward-modulated learning
 * 
 * @author NLM Project
 * @date Phase 2-6 Development
 * @version 6.0.0
 * @copyright MIT License
 * 
 * @see Neuron
 * @see Brain
 * @see PlasticityRule
 * @see STDP
 * @see Hebbian
 */

#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Forward declaration
class Neuron;

/**
 * @class Synapse
 * @brief Represents a synaptic connection between two neurons
 * @details Models biological synapses with the following features:
 *          - Fixed delay in signal transmission
 *          - Synaptic weight modulation
 *          - Spike-timing dependent plasticity (STDP)
 *          - Short-term plasticity (STP)
 *          - Reward-modulated plasticity
 *          - Directionality (source → destination)
 *          - Type classification (Excitatory, Inhibitory, etc.)
 * 
 * The synapse state evolves based on pre- and post-synaptic spike activity,
 * implementing various learning rules to adjust synaptic efficacy over time.
 * 
 * @see https://en.wikipedia.org/wiki/Synapse
 * @see https://en.wikipedia.org/wiki/Spike-timing-dependent_plasticity
 */
class Synapse {
public:
    /**
     * @brief Constructs a synapse with source and destination neuron IDs
     * @param id Unique identifier for this synapse
     * @param source Neuron from which signals originate
     * @param destination Neuron that receives signals
     * @details Creates a new synapse connection. The synapse is initially
     *          configured with default parameters and ready for simulation.
     */
    Synapse(SynapseId id, NeuronId source, NeuronId destination);

    /**
     * @brief Destructor cleans up synapse resources
     * @details Returns synapse to memory pool if using memory pool allocation,
     *          otherwise frees implementation object.
     */
    ~Synapse();
    
    /**
     * @brief Copy constructor is deleted (non-copyable)
     * @details Synaptic connections cannot be copied due to implementation
     *          complexity and potential for dangling references.
     */
    Synapse(const Synapse&) = delete;

    /**
     * @brief Copy assignment operator is deleted (non-assignable)
     * @details Maintains synaptic connection integrity and prevents unintended
     *          sharing of connection references.
     */
    Synapse& operator=(const Synapse&) = delete;

    /**
     * @brief Move constructor transfers ownership of implementation
     * @param other The synapse to move from (becomes invalid after move)
     * @details Efficiently transfers ownership without deep copying synaptic state.
     *          The source synapse becomes invalid and unusable.
     */
    Synapse(Synapse&& other) noexcept;

    /**
     * @brief Move assignment operator transfers ownership of implementation
     * @param other The synapse to move from (becomes invalid after move)
     * @return Reference to this synapse for chaining operations
     * @details Replaces current implementation and cleans up the old one.
     *          The source synapse becomes invalid.
     */
    Synapse& operator=(Synapse&& other) noexcept;

    // Identity management

    /**
     * @brief Gets the unique identifier of this synapse
     * @return SynapseId The synapse's unique identifier
     * @details The identifier remains constant throughout the synapse's lifetime
     *          and is used for referencing and debugging.
     */
    SynapseId getId() const;

    /**
     * @brief Gets the source neuron ID
     * @return NeuronId ID of the neuron from which signals originate
     * @details Determines the direction of signal flow and influences
     *          plasticity rule application.
     */
    NeuronId getSourceNeuron() const;

    /**
     * @brief Gets the destination neuron ID
     * @return NeuronId ID of the neuron that receives signals
     * @details Determines the target of signal transmission and how incoming
     *          signals affect the destination neuron's activity.
     */
    NeuronId getDestinationNeuron() const;

    // Weight management

    /**
     * @brief Gets the current synaptic weight
     * @return SynapticWeight Current synaptic weight (-1.0 to 1.0)
     * @details Weight determines the strength of connection:
     *          - Positive weight: Excitatory connection
     *          - Negative weight: Inhibitory connection
     *          - Weight magnitude: Connection strength
     */
    SynapticWeight getWeight() const;

    /**
     * @brief Sets the absolute synaptic weight
     * @param weight New synaptic weight (-1.0 to 1.0)
     * @details Directly modifies the synaptic connection strength. This is a
     *          low-level operation. Use plasticity rules for learning-based weight
     *          changes.
     */
    void setWeight(SynapticWeight weight);

    /**
     * @brief Increments synaptic weight by a delta value
     * @param delta Change in synaptic weight
     * @details Thread-safe addition operation used during plasticity computations.
     *          Positive delta increases excitatory effect, negative decreases.
     */
    void addToWeight(SynapticWeight delta);

    // Temporal properties

    /**
     * @brief Gets the synaptic delay in simulation steps
     * @return Delay Synaptic delay (typically 1-10 steps)
     * @details Delay represents biological transmission time between neurons.
     *          Longer delays affect spike-timing dependent plasticity and
     *          temporal coding.
     */
    Delay getDelay() const;

    /**
     * @brief Sets the synaptic delay
     * @param delay New synaptic delay in simulation steps
     * @warning Delays affect the temporal dynamics of neural computation.
     *          Changing delays may affect learning and behavior.
     */
    void setDelay(Delay delay);

    // Type classification

    /**
     * @brief Gets the synapse type classification
     * @return SynapseType The synapse type
     * @details Determines how the synapse affects the target neuron:
     *          - Excitatory: Increases postsynaptic firing probability
     *          - Inhibitory: Decreases postsynaptic firing probability
     *          - Modulatory: Triggers neuromodulator release
     *          - Electrical: Direct electrical coupling
     */
    SynapseType getType() const;

    /**
     * @brief Sets the synapse type
     * @param type New synapse type
     * @details Affects both signal transmission and plasticity rule application.
     */
    void setType(SynapseType type);

    /**
     * @brief Checks if synapse is excitatory
     * @return bool true if synapse type is Excitatory
     */
    bool isExcitatory() const;

    /**
     * @brief Checks if synapse is inhibitory
     * @return bool true if synapse type is Inhibitory
     */
    bool isInhibitory() const;

    // Spike history for plasticity

    /**
     * @brief Records a presynaptic spike event
     * @param timestamp Time when the spike occurred
     * @details STDP and other plasticity rules use this information to compute
     *          spike-timing dependent weight changes. Maintains history of recent
     *          presynaptic activity.
     */
    void recordPreSpike(Timestamp timestamp);

    /**
     * @brief Records a postsynaptic spike event
     * @param timestamp Time when the spike occurred
     * @details Required for STDP computations where postsynaptic spike timing
     *          relative to presynaptic spikes determines weight changes.
     */
    void recordPostSpike(Timestamp timestamp);

    /**
     * @brief Gets the history of presynaptic spikes
     * @return const std::vector<Timestamp>& Reference to pre-synaptic spike history
     * @details Used by plasticity rules to determine recent presynaptic activity
     *          patterns.
     */
    const std::vector<Timestamp>& getPreSpikeHistory() const;

    /**
     * @brief Gets the history of postsynaptic spikes
     * @return const std::vector<Timestamp>& Reference to post-synaptic spike history
     */
    const std::vector<Timestamp>& getPostSpikeHistory() const;

    /**
     * @brief Clears all spike history
     * @details Typically called at the start of a new simulation episode to
     *          reset plasticity state.
     */
    void clearHistory();

    // Plasticity management

    /**
     * @brief Gets the plasticity flags for this synapse
     * @return const PlasticityFlags& Current plasticity configuration
     * @details Indicates which plasticity mechanisms are enabled for this synapse.
     *          Controls whether STDP, Hebbian, or reward-modulated learning applies.
     */
    const PlasticityFlags& getPlasticityFlags() const;

    /**
     * @brief Gets the plasticity flags for modification
     * @return PlasticityFlags& Reference to plasticity flags
     * @warning Modifying plasticity flags directly is not recommended. Use dedicated
     *          enablePlasticity methods instead.
     */
    PlasticityFlags& getPlasticityFlags();

    /**
     * @brief Enables or disables specific plasticity mechanisms
     * @param hebbian Enable Hebbian plasticity
     * @param stdp Enable STDP
     * @param rewardModulated Enable reward-modulated plasticity
     * @details Configures the synapse's learning capabilities. Different mechanisms
     *          can be enabled/disabled independently for fine-grained control.
     */
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);

    // Reward-modulated learning

    /**
     * @brief Gets the current eligibility trace value
     * @return float Eligibility trace for reward-modulated learning
     * @details Tracks recent activity that contributes to future reward-modulated
     *          weight changes. Decays over time based on a decay rate.
     */
    float getEligibilityTrace() const;

    /**
     * @brief Sets the eligibility trace value
     * @param trace New eligibility trace value
     */
    void setEligibilityTrace(float trace);

    /**
     * @brief Decays the eligibility trace
     * @param decayRate Decay factor (typically 0.0-1.0 per timestep)
     * @details Implements the temporal decay of eligibility traces, which is
     *          crucial for reward-modulated learning where rewards arrive later
     *          than the actions they reinforce.
     */
    void decayEligibilityTrace(float decayRate);

    // Synaptic efficacy

    /**
     * @brief Gets the current synaptic efficacy
     * @return float Synaptic efficacy (use-dependent modulation)
     * @details Represents the current strength of the connection independent of
     *          weight. Can be modulated by short-term plasticity and other mechanisms.
     */
    float getEfficacy() const;

    /**
     * @brief Sets the synaptic efficacy
     * @param efficacy New efficacy value
     */
    void setEfficacy(float efficacy);

    // Simulation operations

    /**
     * @brief Updates synapse for one simulation step
     * @param currentTime Current simulation time
     * @warning TODO PHASE 2: Implement real synaptic dynamics
     * @details Currently a placeholder. Will implement:
     *          - Short-term depression/facilitation
     *          - Synaptic conductance dynamics
     *          - Neuromodulator interactions
     */
    void step(Timestamp currentTime);

    /**
     * @brief Resets synapse to initial state
     * @details Clears spike history, resets eligibility traces, and restores
     *          baseline efficacy. Called at the start of new episodes.
     */
    void reset();

    /**
     * @brief Initializes synapse with random parameters
     * @param rng Random number generator for stochastic initialization
     * @details Sets initial weight, delay, and other parameters randomly
     *          according to biological distributions.
     */
    void initializeRandom(class RandomGenerator& rng);

    // Debugging and monitoring

    /**
     * @brief Logs synapse status for debugging and monitoring
     * @details Outputs synaptic connection information including weight, delay,
     *          spike history, and plasticity state to the logging system.
     */
    void logStatus() const;

private:
    /**
     * @brief pImpl class for synapse implementation details
     * @details Contains all synapse implementation details including:
     *          - Connection state (weight, delay, type)
     *          - Spike history buffers
     *          - Plasticity state
     *          - Short-term plasticity parameters
     *          - Eligibility traces
     *          - Synaptic efficacy
     * 
     * The pImpl pattern provides encapsulation and thread-safe access.
     */
    struct Impl;
    Impl* pImpl;
};
