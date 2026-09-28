#pragma once

#include "../core/Types/Types.hpp"
#include <vector>

namespace nlm {

/**
 * @brief Real synaptic dynamics with delays and short-term plasticity
 * 
 * Implements biological synaptic transmission with:
 * - Fixed transmission delays (1-5ms typical)
 * - Short-term plasticity (facilitation and depression)
 * - Spike-timing-dependent plasticity (STDP) support
 * - Reward-modulated plasticity support
 * - Hebbian learning support
 * - Synaptic efficacy use-dependent modulation
 * 
 * @note Synapse is bidirectional: records both pre- and post-synaptic spikes
 *       for STDP and other timing-dependent plasticity rules.
 * @note Synaptic weight bounds prevent numerical instability (-1.0 to +1.0).
 * @note Eligibility traces enable delayed reward-modulated learning.
 */
class Synapse {
public:
    /**
     * @brief Construct synapse with unique identifier
     * 
     * @param id Unique synapse identifier (assigned by brain region)
     * @param source Neuron that emits spikes (presynaptic)
     * @param destination Neuron that receives spikes (postsynaptic)
     * 
     * @note Memory ownership managed through pImpl idiom for efficiency.
     *       Copies and moves are efficient due to pointer semantics.
     */
    Synapse(SynapseId id, NeuronId source, NeuronId destination);
    
    /**
     * @brief Destroy synapse and clean up implementation
     */
    ~Synapse();
    
    // Disable copying for safety (handles complex internal state)
    Synapse(const Synapse&) = delete;
    Synapse& operator=(const Synapse&) = delete;
    
    // Enable moving for efficiency
    Synapse(Synapse&& other) noexcept;
    Synapse& operator=(Synapse&& other) noexcept;

    /**
     * @brief Get synapse identifier
     * 
     * @return Unique synapse ID for mapping and debugging
     */
    SynapseId getId() const;
    
    /**
     * @brief Get source neuron identifier
     * 
     * @return Presynaptic neuron ID
     */
    NeuronId getSourceNeuron() const;
    
    /**
     * @brief Get destination neuron identifier
     * 
     * @return Postsynaptic neuron ID
     */
    NeuronId getDestinationNeuron() const;

    /**
     * @brief Get current synaptic weight
     * 
     * @return Synaptic weight (-1.0 to +1.0 typical)
     * 
     * @note Weight determines synaptic strength and sign:
     *       - Positive: Excitatory (depolarizing)
     *       - Negative: Inhibitory (hyperpolarizing)
     *       - Zero: No effect
     */
    SynapticWeight getWeight() const;
    
    /**
     * @brief Set absolute synaptic weight
     * 
     * @param weight New synaptic weight
     * 
     * @note Weight is clamped to valid range [-1.0, 1.0] to prevent instability.
     */
    void setWeight(SynapticWeight weight);
    
    /**
     * @brief Modify synaptic weight by delta
     * 
     * @param delta Weight change amount
     * 
     * @note Weight change is clamped to prevent numerical overflow.
     *       Used by plasticity rules (STDP, Hebbian, reward-modulated).
     */
    void addToWeight(SynapticWeight delta);

    /**
     * @brief Get transmission delay in simulation steps
     * 
     * @return Delay steps (1-5 typical for biological synapses)
     * 
     * @note Delay represents time from presynaptic spike to
     *       postsynaptic potential arrival.
     */
    Delay getDelay() const;
    
    /**
     * @brief Set transmission delay
     * 
     * @param delay New delay in simulation steps
     * 
     * @note Longer delays allow temporal filtering and phase relationships.
     */
    void setDelay(Delay delay);

    /**
     * @brief Get synaptic type
     * 
     * @return Synapse type (Excitatory, Inhibitory, or Other)
     */
    SynapseType getType() const;
    
    /**
     * @brief Set synaptic type
     * 
     * @param type New synapse type
     * 
     * @note Type influences default weight sign and plasticity rules.
     */
    void setType(SynapseType type);

    /**
     * @brief Check if synapse is excitatory
     * 
     * @return True if synaptic weight positive or type is Excitatory
     */
    bool isExcitatory() const;
    
    /**
     * @brief Check if synapse is inhibitory
     * 
     * @return True if synaptic weight negative or type is Inhibitory
     */
    bool isInhibitory() const;

    // ----- Activity Recording for Plasticity -----
    /**
     * @brief Record presynaptic spike for plasticity
     * 
     * @param timestamp Time when presynaptic neuron spiked
     * 
     * @note Called by source neuron when it fires.
     *       Spike history used by STDP and other timing-dependent rules.
     */
    void recordPreSpike(Timestamp timestamp);
    
    /**
     * @brief Record postsynaptic spike for plasticity
     * 
     * @param timestamp Time when postsynaptic neuron spiked
     * 
     * @note Called by destination neuron when it fires.
     *       Enables STDP that depends on both pre- and post-synaptic timing.
     */
    void recordPostSpike(Timestamp timestamp);
    
    /**
     * @brief Get presynaptic spike history
     * 
     * @return Vector of recent presynaptic spike timestamps
     * 
     * @note History limited to MAX_SPIKE_HISTORY to prevent unbounded growth.
     *       Used by STDP to calculate precise spike timing differences.
     */
    const std::vector<Timestamp>& getPreSpikeHistory() const;
    
    /**
     * @brief Get postsynaptic spike history
     * 
     * @return Vector of recent postsynaptic spike timestamps
     */
    const std::vector<Timestamp>& getPostSpikeHistory() const;
    
    /**
     * @brief Clear spike history for both pre- and post-synaptic events
     * 
     * @note Called during reset or when history needs to be cleared.
     *       Important for preventing spurious plasticity effects.
     */
    void clearHistory();

    // ----- Plasticity Configuration -----
    /**
     * @brief Get plasticity flags determining active learning mechanisms
     * 
     * @return Const reference to plasticity flags structure
     * 
     * @note Flags control which plasticity mechanisms are active:
     *       - Hebbian: Activity-dependent strengthening ( BCM theory)
     *       - STDP: Spike-timing-dependent plasticity (correct timing)
     *       - Reward-modulated: Dopamine-gated learning (TD learning)
     */
    const PlasticityFlags& getPlasticityFlags() const;
    
    /**
     * @brief Get mutable plasticity flags for modification
     * 
     * @return Mutable reference to plasticity flags structure
     */
    PlasticityFlags& getPlasticityFlags();
    
    /**
     * @brief Enable/disable plasticity learning mechanisms
     * 
     * @param hebbian Enable Hebbian learning
     * @param stdp Enable STDP learning
     * @param rewardModulated Enable reward-modulated learning
     * 
     * @note Plasticity mechanisms determine how synapses adapt based on activity
     *       and neuromodulation signals from brain regions.
     */
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);

    // ----- Reward-Modulated Learning -----
    /**
     * @brief Get eligibility trace for delayed reward-modulated learning
     * 
     * @return Eligibility trace value (-1.0 to +1.0)
     * 
     * @note Eligibility traces enable learning that depends on temporal
     *       difference between action and reward (temporal difference learning).
     */
    float getEligibilityTrace() const;
    
    /**
     * @brief Set eligibility trace value
     * 
     * @param trace New eligibility trace value
     */
    void setEligibilityTrace(float trace);
    
    /**
     * @brief Decay eligibility trace exponentially
     * 
     * @param decayRate Decay factor per timestep (0.0-1.0)
     * 
     * @note Eligibility traces decay when no spike activity occurs,
     *       ensuring temporal specificity of learning.
     */
    void decayEligibilityTrace(float decayRate);

    // ----- Synaptic Efficacy Modulation -----
    /**
     * @brief Get synaptic efficacy (use-dependent modulation factor)
     * 
     * @return Efficacy multiplier (0.0 to 2.0)
     * 
     * @note Efficacy modulates synaptic weight based on recent usage.
     *       High efficacy means stronger influence per unit weight.
     */
    float getEfficacy() const;
    
    /**
     * @brief Set synaptic efficacy
     * 
     * @param efficacy New efficacy value (clamped to [0.0, 2.0])
     */
    void setEfficacy(float efficacy);

    // ----- Simulation Update -----
    /**
     * @brief Perform one simulation step with synaptic dynamics
     * 
     * @param currentTime Current simulation time in ms
     * 
     * @note Updates:
     *       - Short-term plasticity (facilitation and depression)
     *       - Eligibility trace decay
     *       - Synaptic efficacy modulation
     *       - Weight bounds clamping
     * 
     * @note Short-term plasticity follows Tsodyks-Markram model with:
     *       - Recovery from depression (slow variable)
     *       - Facilitation (fast variable)
     *       - Utilization based on presynaptic activity
     */
    void step(Timestamp currentTime);

    // ----- Reset and Initialization -----
    /**
     * @brief Reset synapse to initial state
     * 
     * @note Clears weight, spike history, eligibility trace, and plasticity state.
     *       Called when synapse is removed or brain resets.
     */
    void reset();
    
    /**
     * @brief Initialize synapse with random biological parameters
     * 
     * @param rng Random number generator reference
     * 
     * @note Sets biologically realistic parameters based on synapse type:
     *       - Excitatory: small positive weights (0.1-0.4), moderate efficacy
     *       - Inhibitory: negative weights (-0.1 to -0.4)
     *       - Other types: small random weights (-0.1 to 0.1)
     *       - Random delay: 1-5 steps (1-5ms)
     */
    void initializeRandom(class RandomGenerator& rng);

private:
    /**
     * @brief Implementation details (pointer-to-implementation idiom)
     * 
     * @note Enables efficient copying/moving and hides implementation complexity.
     *       All state variables stored in implementation struct for optimization.
     */
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
