#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

/**
 * @brief Spike-Timing-Dependent Plasticity (STDP)
 * 
 * STDP is a fundamental learning mechanism in which the timing relationship between
 * pre- and post-synaptic spikes determines whether synaptic strength is increased
 * (potentiation) or decreased (depression). This Hebbian-like learning rule implements
 * the biological principle that "neurons that fire together wire together, but neurons
 * that fire out of sync decay apart."
 * 
 * Key features:
 * - Temporal precision: Learning depends on precise spike timing
 * - Asymmetric Hebbian: Pre-before-post potentiates, post-before-pre depresses
 * - Asymmetric anti-Hebbian: Implements competition and stabilization
 * - Spike-timing window: Defines temporal integration window for learning
 * - Biological plausibility: Matches experimental observations of synaptic learning
 * - Integration with network dynamics: Responds to actual neural activity patterns
 * 
 * Computational model:
 * - Pre-synaptic spike before post-synaptic spike → LTP (Long-Term Potentiation)
 * - Post-synaptic spike before pre-synaptic spike → LTD (Long-Term Depression)
 * - Learning decays over time, creating temporal memory
 * - Multiple spike interactions can combine for complex learning patterns
 * 
 * Biological inspiration:
 * - Spike-timing-dependent synaptic plasticity in biological neurons
 * - Paired and triplet spike models from experimental data
 * - Neural development and critical period learning
 * - Memory formation through spike timing relationships
 * 
 * Phase 2 TODO: Implement real STDP with:
 * - Paired and triplet spike interaction models
 * - Biological parameter tuning (tau values, learning rates)
 * - Integration with eligibility traces for reward-modulated variants
 * - Computational efficiency for large-scale networks
 */
class STDP : public PlasticityRule {
public:
    /**
     * @brief Construct a new STDP instance
     * 
     * @note Initializes STDP with default parameters for LTP and LTD weights,
     *       time constants, and learning rates. These parameters can be
     *       configured through set methods after construction.
     */
    STDP();
    
    /**
     * @brief Destroy the STDP instance
     * 
     * @note Ensures proper cleanup of STDP-specific state and resources.
     */
    ~STDP() override;
    
    // -------------------------------------------------------------------------
    // ABSTRACT VIRTUAL METHOD IMPLEMENTATIONS
    // -------------------------------------------------------------------------
    
    /**
     * @brief Update synaptic weights based on spike timing relationships
     * 
     * @param synapse The synaptic connection to update
     * @param preSpikes Vector of pre-synaptic spike timestamps
     * @param postSpikes Vector of post-synaptic spike timestamps
     * @param dt Timestep duration for the current update (seconds)
     * 
     * @note This method implements the core STDP computation, analyzing the
     *       timing relationships between pre- and post-synaptic spikes and
     *       computing appropriate weight changes based on the STDP rule.
     *       
     * @note The method processes all combinations of pre- and post-synaptic
     *       spikes within the STDP learning window, summing contributions
     *       from individual spike pairs to determine total weight change.
     *       
     * @note Phase 2: Real implementation will include:
     * - Paired spike interaction models (single spike pairs)
     * - Triplet spike interaction models (multiple spike interactions)
     * - Asymmetric LTP/LTD weight computations
     * - Biological parameter application (tau values, learning rates)
     */
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    
    /**
     * @brief Apply a STDP weight change to the synapse
     * 
     * @param synapse The synaptic connection to modify
     * @param delta The weight change amount (positive for potentiation, negative for depression)
     * 
     * @note This method applies the computed STDP weight change, taking into
     *       account the synapse's current weight, plasticity flags, and
     *       rule-specific constraints.
     */
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    /**
     * @brief Get the rule name
     * 
     * @return const char* Always returns "STDP"
     */
    const char* getName() const override;
    
    // -------------------------------------------------------------------------
    // STDP-SPECIFIC PARAMETERS AND METHODS
    // -------------------------------------------------------------------------
    
    /**
     * @brief Set the long-term potentiation (LTP) weight
     * 
     * @param weight LTP weight (typically positive, 0.0 to 1.0)
     * 
     * @note LTP weight determines the magnitude of synaptic strengthening
     *       when pre-synaptic spikes precede post-synaptic spikes by
     *       appropriate timing relationships.
     */
    void setLTPWeight(float weight);    ///< Long-term potentiation weight
    
    /**
     * @brief Get the current long-term potentiation (LTP) weight
     * 
     * @return float Current LTP weight
     * 
     * @note This parameter controls the strength of synaptic potentiation
     *       in the STDP rule. It can be adjusted to tune learning behavior.
     */
    float getLTPWeight() const;
    
    /**
     * @brief Set the long-term depression (LTD) weight
     * 
     * @param weight LTD weight (typically negative or small positive)
     * 
     * @note LTD weight determines the magnitude of synaptic depression
     *       when post-synaptic spikes precede pre-synaptic spikes.
     */
    void setLTDWeight(float weight);    ///< Long-term depression weight
    
    /**
     * @brief Get the current long-term depression (LTD) weight
     * 
     * @return float Current LTD weight
     * 
     * @note This parameter controls the strength of synaptic depression
     *       in the STDP rule. It influences the balance between potentiation
     *       and depression.
     */
    float getLTDWeight() const;
    
    /**
     * @brief Set the STDP time constant (tau)
     * 
     * @param tau Time constant (in seconds)
     * 
     * @note The time constant determines the temporal window over which
     *       spike timing relationships affect synaptic learning. Larger values
     *       create broader learning windows, smaller values create more precise
     *       timing requirements.
     */
    void setTimeConstant(float tau);    ///< STDP time constant (ms)
    
    /**
     * @brief Get the current STDP time constant
     * 
     * @return float Current time constant (seconds)
     * 
     * @note This parameter defines the temporal integration window for
     *       spike-timing-dependent learning. It affects the precision and
     *       range of STDP effects.
     */
    float getTimeConstant() const;
    
    /**
     * @brief Configure STDP parameters with all key values
     * 
     * @param ltpWeight Long-term potentiation weight
     * @param ltdWeight Long-term depression weight
     * @param tau Time constant (seconds)
     * 
     * @note Convenience method to set all STDP parameters in one call,
     *       useful for initialization or rule reconfiguration.
     */
    void configure(float ltpWeight, float ltdWeight, float tau);
    
private:
    struct Impl;
    Impl* pImpl;  ///< Private implementation for PIMPL pattern containing STDP state and parameters
};

/**
 * @brief Reward-Modulated STDP (R-STDP)
 * 
 * R-STDP extends the basic STDP rule by incorporating reward signals and
 * eligibility traces. This rule implements reinforcement learning where
 * spike-timing relationships prime synapses for reward-modulated changes,
 * allowing delayed rewards to modulate synaptic learning based on recent
 * activity patterns.
 * 
 * Key features:
 * - STDP with reward modulation: Combines timing-based learning with reinforcement
 * - Eligibility traces: Bridge temporal gap between action and reward
 * - Credit assignment: Assign rewards to causally contributing synapses
 * - Dopamine integration: Uses neuromodulatory signals for learning control
 * - Temporal credit assignment: Handle delayed reward effects
 * 
 * Computational model:
 * - Pre-post spike timing creates eligibility traces
 * - Reward signals reset and scale eligibility traces
 * - Trace values determine synaptic weight changes
 * - Enables delayed reward learning in temporal sequences
 * 
 * Biological inspiration:
 * - Dopamine-dependent synaptic plasticity
 * - Reward prediction error signaling
 * - Temporal difference learning in biological systems
 * - Credit assignment mechanisms in reinforcement learning
 * 
 * Phase 2 TODO: Implement real R-STDP with:
 * - Eligibility trace dynamics
 * - Reward-modulated weight update rules
 * - Dopamine integration and signaling
 * - Credit assignment algorithms
 */
class RewardModulatedSTDP : public PlasticityRule {
public:
    /**
     * @brief Construct a new RewardModulatedSTDP instance
     * 
     * @note Initializes R-STDP with default parameters for eligibility trace
     *       decay, reward modulation strength, and learning rates.
     */
    RewardModulatedSTDP();
    
    /**
     * @brief Destroy the RewardModulatedSTDP instance
     */
    ~RewardModulatedSTDP() override;
    
    // -------------------------------------------------------------------------
    // ABSTRACT VIRTUAL METHOD IMPLEMENTATIONS
    // -------------------------------------------------------------------------
    
    /**
     * @brief Update synaptic weights based on reward-modulated STDP
     * 
     * @param synapse The synaptic connection to update
     * @param preSpikes Vector of pre-synaptic spike timestamps
     * @param postSpikes Vector of post-synaptic spike timestamps
     * @param dt Timestep duration for the current update (seconds)
     * 
     * @note This method implements R-STDP computation, analyzing spike timing
     *       relationships and maintaining eligibility traces for reward-modulated
     *       weight changes. The actual weight change depends on subsequent reward signals.
     */
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    
    /**
     * @brief Apply a reward-modulated weight change
     * 
     * @param synapse The synaptic connection to modify
     * @param delta The weight change amount (scaled by reward signal)
     * 
     * @note This method applies the reward-modulated weight change, which
     *       incorporates the effects of reward signals on synaptic learning.
     */
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    /**
     * @brief Get the rule name
     * 
     * @return const char* Always returns "R-STDP"
     */
    const char* getName() const override;
};

/**
 * @brief Spike-timing dependent plasticity with triplet interactions
 * 
 * Triplet STDP extends the basic STDP rule by including interactions between
 * multiple spikes (triplets) rather than just pairs. This model accounts for
 * the biological observation that synaptic plasticity depends not only on
 * individual spike pairs but also on the number and timing of nearby spikes
 * in the pre- and post-synaptic neurons.
 * 
 * Key features:
 * - Triplet interaction: Considers multiple spike patterns, not just pairs
 * - Biological accuracy: Matches experimental data better than pair-only models
 * - Computational complexity: More accurate but computationally intensive
 * - Neural plausibility: Reflects biological synaptic plasticity rules
 * - Learning dynamics: Captures complex temporal learning patterns
 * 
 * Biological inspiration:
 * - Paired and triplet spike models from experimental data
 * - Calcium dynamics and synaptic plasticity interactions
 * - Multi-spike interaction effects in biological learning
 * - Learning rules that depend on spike train statistics
 * 
 * Phase 2 TODO: Implement real Triplet STDP with:
 * - Triplet spike interaction algorithms
 * - Calcium-based learning dynamics
 * - Paired and triplet weight computations
 * - Integration with existing STDP infrastructure
 */
class TripletSTDP : public PlasticityRule {
public:
    /**
     * @brief Construct a new TripletSTDP instance
     * 
     * @note Initializes Triplet STDP with default parameters for triplet
     *       interaction models, calcium dynamics, and learning rates.
     */
    TripletSTDP();
    
    /**
     * @brief Destroy the TripletSTDP instance
     */
    ~TripletSTDP() override;
    
    // -------------------------------------------------------------------------
    // ABSTRACT VIRTUAL METHOD IMPLEMENTATIONS
    // -------------------------------------------------------------------------
    
    /**
     * @brief Update synaptic weights based on triplet spike interactions
     * 
     * @param synapse The synaptic connection to update
     * @param preSpikes Vector of pre-synaptic spike timestamps
     * @param postSpikes Vector of post-synaptic spike timestamps
     * @param dt Timestep duration for the current update (seconds)
     * 
     * @note This method implements triplet STDP computation, analyzing the
     *       relationships between multiple pre- and post-synaptic spikes
     *       rather than just individual spike pairs. The method considers
     *       triplet interactions involving 1-3 spikes from each neuron.
     */
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    
    /**
     * @brief Apply a triplet STDP weight change
     * 
     * @param synapse The synaptic connection to modify
     * @param delta The weight change amount
     * 
     * @note This method applies the triplet STDP weight change, which
     *       incorporates the effects of multiple spike interactions.
     */
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    /**
     * @brief Get the rule name
     * 
     * @return const char* Always returns "TripletSTDP"
     */
    const char* getName() const override;
};

} // namespace nlm