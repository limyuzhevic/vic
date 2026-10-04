#pragma once

/**
 * @file Synapse.hpp
 * @brief Synapse interface - implements real synaptic transmission with delays and short-term plasticity
 * @author NLM Team
 * @date 2024
 * @version 1.0
 * @warning Phase 1 implementation. Full synaptic dynamics implementation planned for Phase 2.
 */

#include "../core/Types/Types.hpp"

namespace nlm {

/**
 * @namespace nlm
 * @brief Neural Machine Learning (NLM) - A spiking neural network framework for cognitive computing
 *
 * This namespace encompasses the entire NLM ecosystem, from low-level neuron dynamics to
 * high-level cognitive functions including memory, prediction, attention, and development.
 */

// Forward declaration
class Neuron;

/**
 * @struct Synapse
 * @brief Represents a connection between neurons with synaptic transmission
 * @details Implements real synaptic transmission with delays, short-term plasticity,
 *          eligibility traces for reward-modulated learning, and synaptic efficacy.
 *          Synapses are the fundamental communication channels in the neural network,
 *          carrying signals from presynaptic to postsynaptic neurons.
 *
 * Key features:
 * - Bidirectional spike recording for STDP (Spike-Timing-Dependent Plasticity)
 * - Short-term plasticity (facilitation and depression)
 * - Reward-modulated learning via eligibility traces
 * - Synaptic efficacy (use-dependent modulation)
 * - Weight constraints (-1.0 to 1.0) for stability
 * - Delay-based transmission for realistic timing
 *
 * Synapse dynamics are updated during each simulation step (Synapse::step()),
 * allowing for real-time synaptic transmission and plasticity.
 */

class Synapse {
public:
    /**
     * @brief Constructs a synapse with specified connection
     * @param id Unique identifier for the synapse
     * @param source Neuron ID of the presynaptic (source) neuron
     * @param destination Neuron ID of the postsynaptic (target) neuron
     * @details Creates a new synapse instance and initializes its internal state.
     *          The synapse is created in an uninitialized state with sensible defaults:
     *          - Weight: 0.0 (neutral)
     *          - Delay: 1 step (minimum realistic delay)
     *          - Type: Excitatory (default)
     *          - Eligibility trace: 0.0
     *          - Efficacy: 1.0
     *          - Short-term plasticity: Fully recovered
     * @pre The synapse ID should be unique within its region
     * @pre Source and destination neuron IDs should be valid and distinct
     */
    Synapse(SynapseId id, NeuronId source, NeuronId destination);
    
    /**
     * @brief Destructor cleans up synapse resources
     * @details Deletes the private implementation (pImpl) and cleans up
     *          any allocated resources. This ensures proper memory management.
     */
    ~Synapse();
    
    /**
     * @brief Disables copying (synapse is not copyable)
     */
    Synapse(const Synapse&) = delete;
    
    /**
     * @brief Disables copy assignment
     */
    Synapse& operator=(const Synapse&) = delete;
    
    /**
     * @brief Enables move construction (efficient transfer of ownership)
     */
    Synapse(Synapse&&) noexcept;
    
    /**
     * @brief Enables move assignment (efficient transfer of ownership)
     */
    Synapse& operator=(Synapse&&) noexcept;
    
    /**
     * @brief Gets the synapse's unique identifier
     * @return SynapseId The synapse's unique ID
     */
    SynapseId getId() const;
    
    /**
     * @brief Gets the source (presynaptic) neuron ID
     * @return NeuronId The ID of the presynaptic neuron
     */
    NeuronId getSourceNeuron() const;
    
    /**
     * @brief Gets the destination (postsynaptic) neuron ID
     * @return NeuronId The ID of the postsynaptic neuron
     */
    NeuronId getDestinationNeuron() const;
    
    /**
     * @brief Gets the current synaptic weight
     * @return SynapticWeight The current weight (-1.0 to 1.0)
     * @details Weight determines the sign and magnitude of synaptic influence:
     *          - Positive weight: Excitatory (depolarizing)
     *          - Negative weight: Inhibitory (hyperpolarizing)
     *          - Zero weight: Neutral (no effect)
     * @see setWeight()
     * @see addToWeight()
     */
    SynapticWeight getWeight() const;
    
    /**
     * @brief Sets the synaptic weight
     * @param weight New weight value (-1.0 to 1.0)
     * @details Updates the synapse's weight to the specified value.
     *          Weight is validated and clamped to reasonable bounds to prevent
     *          numerical instability or runaway excitation/inhibition.
     * @throws NeuralException if setting weight fails
     * @warning Extreme weights can cause network instability.
     */
    void setWeight(SynapticWeight weight);
    
    /**
     * @brief Adds a delta to the synaptic weight
     * @param delta Weight change value
     * @details Increments or decrements the current weight by the specified delta.
     *          This is used for online weight updates during learning.
     * @note Weight is automatically clamped to [-1.0, 1.0] after modification.
     */
    void addToWeight(SynapticWeight delta);
    
    /**
     * @brief Gets the synaptic delay
     * @return Delay Synaptic delay in simulation steps
     * @details Delay represents the time between presynaptic spike and
     *          postsynaptic response. Typical values range from 1-5 steps
     *          (1-5 ms at 1ms timestep).
     * @see setDelay()
     */
    Delay getDelay() const;
    
    /**
     * @brief Sets the synaptic delay
     * @param delay New delay value in simulation steps
     * @details Updates the synapse's transmission delay. Delay affects the
     *          timing of synaptic effects and is critical for realistic neural dynamics.
     * @note Longer delays can affect network stability and require larger buffers.
     */
    void setDelay(Delay delay);
    
    /**
     * @brief Gets the synapse type
     * @return SynapseType The synapse type (Excitatory, Inhibitory, etc.)
     * @details Synapse type indicates the functional role and effect on the
     *          postsynaptic neuron. Can be overridden by weight.
     * @see setType()
     * @see isExcitatory()
     * @see isInhibitory()
     */
    SynapseType getType() const;
    
    /**
     * @brief Sets the synapse type
     * @param type The new synapse type
     * @details Updates the synapse's type. Note that synapse type and weight
     *          can be inconsistent (e.g., inhibitory synapse with positive weight).
     *          The type is primarily used for network initialization and
     *          certain plasticity rules.
     */
    void setType(SynapseType type);
    
    /**
     * @brief Checks if the synapse is excitatory
     * @return bool true if synapse type is Excitatory
     * @see getType()
     * @see isInhibitory()
     */
    bool isExcitatory() const;
    
    /**
     * @brief Checks if the synapse is inhibitory
     * @return bool true if synapse type is Inhibitory
     * @see getType()
     * @see isExcitatory()
     */
    bool isInhibitory() const;
    
    /**
     * @brief Records a presynaptic spike for plasticity
     * @param timestamp Time when the presynaptic neuron spiked (seconds)
     * @details Records spike timing information in the synapse's pre-spike history.
     *          This data is used by STDP algorithms to determine synaptic eligibility
     *          and weight updates based on spike timing relationships.
     * @throws NeuralException if recording spike fails
     * @note Maintains a sliding window of recent spikes for efficiency.
     * @see getPreSpikeHistory()
     * @see recordPostSpike()
     * @see clearHistory()
     */
    void recordPreSpike(Timestamp timestamp);
    
    /**
     * @brief Records a postsynaptic spike for plasticity
     * @param timestamp Time when the postsynaptic neuron spiked (seconds)
     * @details Records spike timing information in the synapse's post-spike history.
     *          Together with pre-spike history, this enables STDP learning rules.
     * @throws NeuralException if recording spike fails
     * @note Maintains a sliding window of recent spikes for efficiency.
     * @see getPostSpikeHistory()
     * @see recordPreSpike()
     * @see clearHistory()
     */
    void recordPostSpike(Timestamp timestamp);
    
    /**
     * @brief Gets the presynaptic spike history
     * @return const std::vector<Timestamp>& Reference to the pre-spike history vector
     * @details Returns the list of timestamps when the presynaptic neuron spiked.
     *          This history is used by STDP algorithms to compute eligibility traces
     *          and determine synaptic weight updates.
     * @note The history is a sliding window with maximum size MAX_SPIKE_HISTORY (100).
     * @see recordPreSpike()
     * @see clearHistory()
     */
    const std::vector<Timestamp>& getPreSpikeHistory() const;
    
    /**
     * @brief Gets the postsynaptic spike history
     * @return const std::vector<Timestamp>& Reference to the post-spike history vector
     * @details Returns the list of timestamps when the postsynaptic neuron spiked.
     *          This history, combined with pre-spike history, enables STDP learning.
     * @note The history is a sliding window with maximum size MAX_SPIKE_HISTORY (100).
     * @see recordPostSpike()
     * @see clearHistory()
     */
    const std::vector<Timestamp>& getPostSpikeHistory() const;
    
    /**
     * @brief Clears both pre- and post-spike history
     * @details Removes all recorded spike timestamps. Useful when resetting
     *          synaptic plasticity or clearing learning history.
     * @note This also resets the synaptic eligibility trace to zero.
     * @see recordPreSpike()
     * @see recordPostSpike()
     */
    void clearHistory();
    
    /**
     * @brief Gets a reference to the synapse's plasticity flags
     * @return PlasticityFlags& Reference to the synapse's plasticity flags
     * @note This provides direct access to the synapse's plasticity configuration.
     *       Flags control which plasticity rules are active for this synapse.
     * @see getPlasticityFlags() const
     * @see PlasticityFlags struct
     */
    PlasticityFlags& getPlasticityFlags();
    
    /**
     * @brief Gets the synaptic plasticity flags (const version)
     * @return const PlasticityFlags& Const reference to the synapse's plasticity flags
     * @see getPlasticityFlags()
     */
    const PlasticityFlags& getPlasticityFlags() const;
    
    /**
     * @brief Enables specific plasticity rules for this synapse
     * @param hebbian Whether to enable Hebbian plasticity
     * @param stdp Whether to enable STDP plasticity
     * @param rewardModulated Whether to enable reward-modulated plasticity
     * @throws NeuralException if enabling plasticity fails
     * @details Configures which synaptic plasticity rules are active for this synapse.
     *          Different plasticity rules can be enabled simultaneously, leading to
     *          complex interaction effects in synaptic weight dynamics.
     * @note Plasticity flags are inherited by the synapse's private implementation.
     */
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    /**
     * @brief Gets the current eligibility trace
     * @return float Eligibility trace value (-1.0 to 1.0)
     * @details The eligibility trace represents the synapse's readiness to undergo
     *          weight changes based on reward signals. It accumulates during learning
     *          and decays over time.
     * @see setEligibilityTrace()
     * @see decayEligibilityTrace()
     */
    float getEligibilityTrace() const;
    
    /**
     * @brief Sets the eligibility trace value
     * @param trace New eligibility trace value (-1.0 to 1.0)
     * @details Updates the synapse's eligibility trace. This trace is used in
     *          reward-modulated learning (e.g., policy gradient methods).
     * @throws NeuralException if setting eligibility trace fails
     * @warning Values outside [-1.0, 1.0] are clamped.
     */
    void setEligibilityTrace(float trace);
    
    /**
     * @brief Decays the eligibility trace
     * @param decayRate Decay rate (0.0 to 1.0)
     * @details Reduces the eligibility trace value by multiplying with
     *          (1 - decayRate). This models the decay of synaptic eligibility
     *          over time when no reward is received.
     * @note Fast decay rates are typically used (e.g., 0.001).
     */
    void decayEligibilityTrace(float decayRate);
    
    /**
     * @brief Gets the synaptic efficacy
     * @return float Synaptic efficacy value (0.0 to 2.0)
     * @details Synaptic efficacy represents use-dependent modulation of synaptic
     *          strength. It increases with presynaptic activity and can enhance
     *          or depress synaptic transmission.
 * @see setEfficacy()
     */
    float getEfficacy() const;
    
    /**
     * @brief Sets the synaptic efficacy
     * @param efficacy New efficacy value (0.0 to 2.0)
     * @details Updates the synapse's efficacy. Efficacy modulates how effectively
     *          the synapse can transmit signals based on usage history.
 * @throws NeuralException if setting efficacy fails
 * @warning Values outside [0.0, 2.0] are clamped.
     */
    void setEfficacy(float efficacy);
    
    /**
     * @brief Updates the synapse for one simulation step
     * @param currentTime The current simulation time (seconds)
     * @details Updates synaptic dynamics including:
     *          - Short-term plasticity (facilitation and depression)
     *          - Eligibility trace decay
     *          - Weight bounds enforcement
     *          - Synaptic efficacy dynamics
     * @throws NeuralException if synapse update fails
     * @note This is called during each simulation step by NeuralRegion::step().
     */
    void step(Timestamp currentTime);
    
    /**
     * @brief Resets the synapse to its initial state
     * @details Resets all synaptic state variables:
     *          - Weight to 0.0
     *          - Spike history to empty
     *          - Eligibility trace to 0.0
     *          - Efficacy to 1.0
     * @throws NeuralException if synapse reset fails
     */
    void reset();
    
    /**
     * @brief Initializes the synapse with random parameters
     * @param rng Reference to random number generator
     * @details Performs random initialization based on synapse type:
     *          - Excitatory synapses: small positive weights (0.1-0.4)
     *          - Inhibitory synapses: negative weights (-0.1 to -0.4)
     *          - Other types: small random weights (-0.1 to 0.1)
     * @throws NeuralException if random initialization fails
     * @note Used during network initialization to create realistic connectivity.
     */
    void initializeRandom(class RandomGenerator& rng);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
