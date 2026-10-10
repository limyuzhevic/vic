#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Forward declaration
class Neuron;

/**
 * @brief Synapse representing a connection between neurons
 * 
 * A synapse implements the fundamental communication mechanism between neurons
 * in the NLM brain. It handles synaptic transmission with delays, short-term
 * plasticity, and eligibility traces for learning. Synapses are the basic
 * units of information flow in the neural network.
 * 
 * Key features:
 * - Bidirectional connectivity (source → destination)
 * - Transmission delay for realistic spike timing
 * - Synaptic weight modulation for strength control
 * - Activity history for plasticity computations
 * - Plasticity rule application (STDP, Hebbian, reward-modulated)
 * - Eligibility trace management for reward-modulated learning
 * - Efficacy modulation for use-dependent plasticity
 * 
 * Synaptic transmission follows a two-stage process:
 * 1. Spike generation at source neuron
 * 2. Delayed transmission to destination neuron via synapse
 * 3. Post-synaptic potential generation in destination neuron
 * 
 * The synapse model combines biological realism with computational efficiency,
 * supporting both event-driven spike transmission and continuous synaptic dynamics.
 */
class Synapse {
public:
    /**
     * @brief Construct a new synapse with specified connection parameters
     * 
     * @param id Unique identifier for this synapse
     * @param source Source neuron identifier
     * @param destination Target neuron identifier
     * 
     * @note The synapse is initialized with default parameters:
     * - Weight: 0.0 (neutral)
     * - Delay: 1 simulation step (default transmission delay)
     * - Type: Excitatory (default)
     * - Plasticity flags: All false (disabled)
     */
    Synapse(SynapseId id, NeuronId source, NeuronId destination);
    
    /**
     * @brief Destroy the Synapse instance
     * 
     * @note Ensures proper cleanup of synapse state and resources.
     *       The synapse should be in a clean state (typically reset) before
     *       destruction to avoid memory leaks.
     */
    ~Synapse();
    
    // Disable copying, enable moving for efficient resource management
    Synapse(const Synapse&) = delete;
    Synapse& operator=(const Synapse&) = delete;
    Synapse(Synapse&&) noexcept;
    Synapse& operator=(Synapse&&) noexcept;
    
    /**
     * @brief Get the synapse's unique identifier
     * 
     * @return SynapseId The synapse's identifier
     * 
     * @note Used for system-wide operations, connectivity queries,
     *       and synapse management functions.
     */
    SynapseId getId() const;
    
    /**
     * @brief Get the source neuron identifier
     * 
     * @return NeuronId Source neuron identifier
     * 
     * @note The neuron that generates spikes transmitted through this synapse.
     */
    NeuronId getSourceNeuron() const;
    
    /**
     * @brief Get the destination neuron identifier
     * 
     * @return NeuronId Target neuron identifier
     * 
     * @note The neuron that receives spikes transmitted through this synapse.
     */
    NeuronId getDestinationNeuron() const;
    
    // -------------------------------------------------------------------------
    // SYNAPTIC WEIGHT MANAGEMENT
    // -------------------------------------------------------------------------
    
    /**
     * @brief Get the current synaptic weight
     * 
     * @return SynapticWeight Current synaptic weight (-1.0 to 1.0 typical range)
     * 
     * @note Synaptic weight determines the strength and sign (excitatory/inhibitory)
     *       of the connection. Positive weights are typically excitatory, negative
     *       weights are inhibitory. Weight magnitude affects postsynaptic potential amplitude.
     */
    SynapticWeight getWeight() const;
    
    /**
     * @brief Set the synaptic weight directly
     * 
     * @param weight New synaptic weight (-1.0 to 1.0 typical range)
     * 
     * @note This method directly modifies the synaptic weight, useful for
     *       controlled testing, initialization, or manual weight adjustment.
     *       For learning through plasticity rules, use plasticity mechanisms instead.
     */
    void setWeight(SynapticWeight weight);
    
    /**
     * @brief Add a delta to the synaptic weight (plasticity operation)
     * 
     * @param delta Weight change amount (-1.0 to 1.0 typical range)
     * 
     * @note This is the primary method for synaptic weight modification through
     *       plasticity rules (STDP, Hebbian, reward-modulated). Plasticity rules
     *       call this method to implement learning based on neural activity.
     */
    void addToWeight(SynapticWeight delta);
    
    // -------------------------------------------------------------------------
    // TRANSMISSION DELAY MANAGEMENT
    // -------------------------------------------------------------------------
    
    /**
     * @brief Get the synaptic transmission delay
     * 
     * @return Delay Transmission delay in simulation steps
     * 
     * @note Delay affects spike timing and is crucial for realistic neural dynamics,
     *       especially for spike-timing-dependent plasticity (STDP). Longer delays
     *       can create more complex temporal patterns of activity.
     */
    Delay getDelay() const;
    
    /**
     * @brief Set the synaptic transmission delay
     * 
     * @param delay New transmission delay in simulation steps
     * 
     * @note Changing delay affects spike timing and can alter network dynamics.
     *       Important for modeling different types of synaptic connections
     *       (fast vs. slow transmission).
     */
    void setDelay(Delay delay);
    
    // -------------------------------------------------------------------------
    // SYNAPSE TYPE AND FUNCTION
    // -------------------------------------------------------------------------
    
    /**
     * @brief Get the synapse type (Excitatory, Inhibitory, Modulatory, etc.)
     * 
     * @return SynapseType The synapse's functional type
     * 
     * @note Synapse type determines the qualitative effect on postsynaptic neurons
     *       and influences plasticity rule eligibility. Type is typically set during
     *       initialization and may be changed based on developmental or learning processes.
     */
    SynapseType getType() const;
    
    /**
     * @brief Set the synapse type
     * 
     * @param type New synapse type
     * 
     * @note Changing synapse type affects both its functional properties
     *       and which plasticity rules can be applied to it.
     */
    void setType(SynapseType type);
    
    /**
     * @brief Check if the synapse is excitatory
     * 
     * @return bool true if synapse is excitatory, false otherwise
     * 
     * @note This method provides a quick check for synaptic sign without
     *       accessing the full type. Can be used for optimized processing.
     */
    bool isExcitatory() const;
    
    /**
     * @brief Check if the synapse is inhibitory
     * 
     * @return bool true if synapse is inhibitory, false otherwise
     * 
     * @note This method provides a quick check for synaptic sign without
     *       accessing the full type. Can be used for optimized processing.
     */
    bool isInhibitory() const;
    
    // -------------------------------------------------------------------------
    // ACTIVITY HISTORY AND PLASTICITY
    // -------------------------------------------------------------------------
    
    /**
     * @brief Record a pre-synaptic spike event
     * 
     * @param timestamp Timestamp when the pre-synaptic neuron spiked
     * 
     * @note Pre-synaptic spike times are essential for spike-timing-dependent
     *       plasticity (STDP) and other timing-based learning mechanisms. This method
     *       records the timing of spikes arriving at the synapse from the source neuron.
     */
    void recordPreSpike(Timestamp timestamp);
    
    /**
     * @brief Record a post-synaptic spike event
     * 
     * @param timestamp Timestamp when the post-synaptic neuron spiked
     * 
     * @note Post-synaptic spike times are used for anti-Hebbian plasticity,
     *       spike-timing-dependent plasticity, and other learning rules that depend
     *       on the relationship between pre- and post-synaptic activity.
     */
    void recordPostSpike(Timestamp timestamp);
    
    /**
     * @brief Get the pre-synaptic spike history
     * 
     * @return const std::vector<Timestamp>& Vector of pre-synaptic spike timestamps
     * 
     * @note Used by plasticity rules (STDP, Hebbian, etc.) to compute timing
     *       relationships between pre- and post-synaptic activity for weight updates.
     */
    const std::vector<Timestamp>& getPreSpikeHistory() const;
    
    /**
     * @brief Get the post-synaptic spike history
     * 
     * @return const std::vector<Timestamp>& Vector of post-synaptic spike timestamps
     * 
     * @note Used by plasticity rules that depend on the precise timing of
     *       postsynaptic activity relative to presynaptic inputs.
     */
    const std::vector<Timestamp>& getPostSpikeHistory() const;
    
    /**
     * @brief Clear all spike history records
     * 
     * @note Resets spike timing records. Called during reset() operations
     *       and when plasticity eligibility needs to be reset for new learning episodes.
     */
    void clearHistory();
    
    // -------------------------------------------------------------------------
    // PLASTICITY FLAG MANAGEMENT
    // -------------------------------------------------------------------------
    
    /**
     * @brief Get the synapse's plasticity flags
     * 
     * @return const PlasticityFlags& Reference to plasticity flags
     * 
     * @note Plasticity flags determine which plasticity rules can be applied
     *       to this synapse. Typically set by the brain system based on neuron
     *       type, developmental stage, and learning context.
     */
    const PlasticityFlags& getPlasticityFlags() const;
    
    /**
     * @brief Get mutable reference to the synapse's plasticity flags
     * 
     * @return PlasticityFlags& Mutable reference to plasticity flags
     * 
     * @note Allows direct modification of plasticity eligibility and rule activation.
     */
    PlasticityFlags& getPlasticityFlags();
    
    /**
     * @brief Enable/disable specific plasticity rules for this synapse
     * 
     * @param hebbian Enable Hebbian plasticity
     * @param stdp Enable spike-timing-dependent plasticity  
     * @param rewardModulated Enable reward-modulated plasticity
     * 
     * @note Controls which learning mechanisms can modify synaptic weights
     *       based on neural activity and external signals. Typically managed
     *       by the brain system based on developmental and learning context.
     */
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    // -------------------------------------------------------------------------
    // ELIGIBILITY TRACE MANAGEMENT
    // -------------------------------------------------------------------------
    
    /**
     * @brief Get the synapse's eligibility trace for reward-modulated learning
     * 
     * @return float Current eligibility trace value (typically 0.0 to 1.0)
     * 
     * @note Eligibility traces bridge the gap between synaptic activity and
     *       delayed reward signals in reward-modulated learning. They accumulate
     *       pre-synaptic activity and decay over time, allowing delayed rewards
     *       to modulate previously eligible synapses.
     */
    float getEligibilityTrace() const;
    
    /**
     * @brief Set the synapse's eligibility trace
     * 
     * @param trace New eligibility trace value
     * 
     * @note Direct manipulation of eligibility traces is typically done by
     *       learning algorithms. Normal operation uses plasticity rules to
     *       update eligibility based on neural activity.
     */
    void setEligibilityTrace(float trace);
    
    /**
     * @brief Decay the synapse's eligibility trace
     * 
     * @param decayRate Decay factor (0.0 to 1.0, where 1.0 = no decay)
     * 
     * @note Eligibility traces naturally decay over time to limit the temporal
     *       window during which rewards can modulate synaptic learning. This
     *       method implements the decay component of the eligibility trace dynamics.
     */
    void decayEligibilityTrace(float decayRate);
    
    // -------------------------------------------------------------------------
    // SYNAPTIC EFFICACY MANAGEMENT
    // -------------------------------------------------------------------------
    
    /**
     * @brief Get the synapse's current efficacy
     * 
     * @return float Current synaptic efficacy (typically 0.0 to 1.0)
     * 
     * @note Synaptic efficacy represents use-dependent modulation of synaptic strength.
     *       It can be influenced by recent activity patterns, neuromodulators, and
     *       homeostatic mechanisms. Efficacy differs from weight in that it
     *       represents a state that affects weight changes rather than direct effect.
     */
    float getEfficacy() const;
    
    /**
     * @brief Set the synapse's efficacy
     * 
     * @param efficacy New efficacy value (typically 0.0 to 1.0)
     * 
     * @note Efficacy represents a state variable that influences how effectively
     *       plasticity rules modify synaptic weight. High efficacy means the synapse
     *       is more responsive to learning signals.
     */
    void setEfficacy(float efficacy);
    
    // -------------------------------------------------------------------------
    // SIMULATION OPERATIONS
    // -------------------------------------------------------------------------
    
    /**
     * @brief Update synapse for one simulation step
     * 
     * @param currentTime Current simulation time (seconds)
     * 
     * @note This method implements synaptic dynamics for the current step,
     *       including:
     *       - Efficacy trace decay
     *       - Post-synaptic potential computation
     *       - Eligibility trace updates
     *       - Use-dependent modulation
     *       
     * @note Currently a placeholder for Phase 3 implementation
     * @see step() for the general simulation step in the main brain loop
     */
    void step(Timestamp currentTime);
    
    /**
     * @brief Reset synapse to initial state
     * 
     * @note Clears all state variables including spike history, eligibility traces,
     *       and efficacy. Resets synaptic weight to its initial value. Used to prepare
     *       synapses for new simulation runs or learning episodes.
     */
    void reset();
    
    /**
     * @brief Initialize synapse with random parameters
     * 
     * @param rng Reference to random number generator
     * 
     * @note Sets synaptic parameters (weight, delay, type, etc.) using random
     *       values from the provided generator. Used during brain initialization
     *       to create diverse connectivity patterns and initial network dynamics.
     */
    void initializeRandom(class RandomGenerator& rng);
    
private:
    struct Impl;
    Impl* pImpl;  ///< Private implementation for PIMPL pattern
};

} // namespace nlm