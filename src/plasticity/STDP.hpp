#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

/**
 * @class STDP
 * @brief Spike-Timing-Dependent Plasticity implementation
 * 
 * STDP is a learning rule that modifies synaptic strength based on the precise
 * timing relationship between pre- and post-synaptic spikes. This rule implements
 * the classic Hebbian learning principle with timing-dependent modulation,
 * where synaptic strengthening (long-term potentiation - LTP) occurs when
 * pre-synaptic spikes precede post-synaptic spikes, and synaptic weakening
 * (long-term depression - LTD) occurs when post-synaptic spikes precede
 * pre-synaptic spikes.
 * 
 * The STDP mechanism is fundamental to many neural learning processes and is
 * considered one of the most biologically plausible learning rules. It captures
 * the essence of temporal causality in neural networks and is widely used in
 * spiking neural network models.
 * 
 * @note This class uses the PImpl idiom to hide implementation details and
 *       efficiently manage the internal synaptic weight update algorithms.
 * @note Current implementation is a placeholder for Phase 2 development.
 * @note Thread safety is ensured for all public methods.
 * 
 * @see PlasticityRule
 * @see Hebbian
 * @see RewardModulatedSTDP
 * @see TripletSTDP
 * @see HebbianRule
 * 
 * @ingroup plasticity
 */

class STDP : public PlasticityRule {
public:
    /**
     * @brief Constructs an STDP instance with default parameters
     * 
     * Creates an STDP plasticity rule with default parameters for spike-timing
     * dependent plasticity. The default parameters provide a balanced initial
     * condition for synaptic learning and have been chosen based on typical
     * biological observations.
     * 
     * @note The actual implementation details are hidden by the PImpl idiom.
     *       All parameter settings are forwarded to the implementation class.
     * 
     * @see configure
     * @see setLTPWeight
     * @see setLTDWeight
     * @see setTimeConstant
     */
    STDP();
    
    /**
     * @brief Virtual destructor
     * 
     * Cleans up STDP resources and performs finalization of synaptic weight
     * update algorithms. Due to the PImpl idiom, the destructor is defined
     * in the implementation file.
     * 
     * @note Properly cleans up any allocated resources used by the internal
     *       synaptic weight update computations.
     */
    ~STDP() override;
    
    /**
     * @brief Update synaptic weights based on spike timing differences
     * 
     * Implements the core STDP algorithm by computing weight changes based on
     * the timing relationship between pre-synaptic and post-synaptic spikes.
     * This method computes the weight modification according to the STDP rule
     * using the provided spike time data and applies it through applyWeightChange.
     * 
     * @param synapse Pointer to the synapse whose weight will be modified.
     *               Must be a valid synapse pointer within the neural network.
     * @param preSpikes Vector of pre-synaptic spike times in milliseconds.
     *                 Contains timestamps of all spikes that occurred at the
     *                 pre-synaptic neuron since the last plasticity update.
     *                 Sorted in chronological order.
     * @param postSpikes Vector of post-synaptic spike times in milliseconds.
     *                 Contains timestamps of all spikes that occurred at the
     *                 post-synaptic neuron since the last plasticity update.
     *                 Sorted in chronological order.
     * @param dt Simulation timestep duration in milliseconds. Used for
     *           rate-based STDP variants and temporal derivative calculations
     *           in continuous-time implementations.
     * 
     * @note The STDP algorithm computes weight changes by comparing each
     *       pre-synaptic spike with each post-synaptic spike and applying
     *       the appropriate LTP or LTD based on their temporal relationship.
     * @note Implementation complexity is O(n×m) where n and m are the numbers
     *       of pre- and post-synaptic spikes respectively.
     * @note This method is thread-safe and can be called concurrently for
     *       different synapses.
     * 
     * @see applyWeightChange
     * @see getName
     * @see setTimeConstant
     * @see configure
     */
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    
    /**
     * @brief Apply a direct weight change to a synapse
     * 
     * Applies a weight change delta directly to the specified synapse,
     * bypassing the spike timing computation. This method is used internally
     * by the update() method after computing the STDP weight modification.
     * It can also be used for external weight manipulation.
     * 
     * @param synapse Pointer to the synapse to modify. Must be a valid
     *               synapse pointer within the neural network.
     * @param delta Weight change amount in synaptic weight units. Positive
     *             values potentiate the synapse (LTP), negative values depress
     *             it (LTD). The delta represents the net weight change from
     *             the STDP computation.
     * 
     * @note The implementation ensures numerical stability by clipping
     *       delta values to prevent excessive weight changes and maintaining
     *       synaptic weight bounds.
     * @note This method should respect the synaptic weight limits (typically
     *       -1.0 to +1.0 or 0.0 to +1.0 depending on the neural model).
     * 
     * @see update
     */
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    /**
     * @brief Get the human-readable name of the STDP rule
     * 
     * Returns a descriptive string identifying this as a Spike-Timing-Dependent
     * Plasticity rule. This is useful for configuration, logging, and debugging
     * purposes.
     * 
     * @return Const char pointer to the string "STDP". This is a static
     *         string literal that should not be modified by callers.
     * 
     * @note The name uniquely identifies this plasticity rule among different
     *       STDP variants (e.g., "R-STDP", "TripletSTDP").
     * 
     * @see update
     * @see applyWeightChange
     */
    const char* getName() const override;
    
    /**
     * @brief Set the long-term potentiation (LTP) weight factor
     * 
     * Configures the weight magnitude for synaptic strengthening when
     * pre-synaptic spikes precede post-synaptic spikes by a sufficient
     * time interval. This parameter controls the amount of potentiation
     * per spike pair.
     * 
     * @param weight The LTP weight factor. Must be positive (0.0 < weight).
     *              Typical biological values range from 0.01 to 0.1. Larger
     *              values produce stronger potentiation effects.
     * 
     * @note The LTP weight is multiplied by an exponential decay function
     *       of the spike time difference to produce the final weight change.
     * @note This parameter should be set during rule configuration, typically
     *       through the configure() method.
     * 
     * @see getLTPWeight
     * @see setLTDWeight
     * @see configure
     */
    void setLTPWeight(float weight);    // Long-term potentiation weight
    
    /**
     * @brief Get the current long-term potentiation (LTP) weight factor
     * 
     * Returns the current LTP weight factor that determines the strength
     * of synaptic potentiation in the STDP rule.
     * 
     * @return Current LTP weight factor (positive float value).
     * 
     * @see setLTPWeight
     */
    float getLTPWeight() const;
    
    /**
     * @brief Set the long-term depression (LTD) weight factor
     * 
     * Configures the weight magnitude for synaptic weakening when
     * post-synaptic spikes precede pre-synaptic spikes by a sufficient
     * time interval. This parameter controls the amount of depression
     * per spike pair.
     * 
     * @param weight The LTD weight factor. Must be positive (0.0 < weight).
     *              Typical biological values range from 0.01 to 0.1. Larger
     *              values produce stronger depression effects.
     * 
     * @note The LTD weight is multiplied by an exponential decay function
     *       of the spike time difference to produce the final weight change.
     * @note This parameter should be set during rule configuration, typically
     *       through the configure() method.
     * 
     * @see getLTDWeight
     * @see setLTPWeight
     * @see configure
     */
    void setLTDWeight(float weight);    // Long-term depression weight
    
    /**
     * @brief Get the current long-term depression (LTD) weight factor
     * 
     * Returns the current LTD weight factor that determines the strength
     * of synaptic depression in the STDP rule.
     * 
     * @return Current LTD weight factor (positive float value).
     * 
     * @see setLTDWeight
     */
    float getLTDWeight() const;
    
    /**
     * @brief Set the STDP time constant (tau parameter)
     * 
     * Configures the time constant that controls the decay of the STDP
     * effect with increasing spike time difference. This parameter determines
     * how quickly the weight change magnitude decays as the temporal distance
     * between spikes increases.
     * 
     * @param tau The STDP time constant in milliseconds. Typical biological
     *           values range from 10ms to 100ms. Smaller values produce sharper
     *           temporal windows, larger values produce broader windows.
     * 
     * @note The time constant tau appears in the exponential decay function
     *       that weights the contribution of spike pairs based on their temporal
     *       separation: Δw ∝ exp(-|Δt|/τ).
     * @note This parameter is crucial for determining the temporal specificity
     *       of the STDP rule.
     * 
     * @see getTimeConstant
     * @see configure
     */
    void setTimeConstant(float tau);    // STDP time constant (ms)
    
    /**
     * @brief Get the current STDP time constant (tau parameter)
     * 
     * Returns the current STDP time constant that controls the temporal
     * decay of spike-timing dependent plasticity effects.
     * 
     * @return Current time constant in milliseconds (positive float value).
     * 
     * @see setTimeConstant
     */
    float getTimeConstant() const;
    
    /**
     * @brief Configure all STDP parameters in a single call
     * 
     * Convenience method to set all STDP parameters (LTP weight, LTD weight,
     * and time constant) simultaneously. This ensures consistent parameter
     * configuration and reduces the number of separate setter calls.
     * 
     * @param ltpWeight Long-term potentiation weight factor. Must be positive.
     * @param ltdWeight Long-term depression weight factor. Must be positive.
     * @param tau STDP time constant in milliseconds. Must be positive.
     * 
     * @note All parameters must have valid values (positive floats) before
     *       the configuration is applied.
     * @note This method is thread-safe and provides atomic configuration of
     *       all parameters.
     * 
     * @see setLTPWeight
     * @see setLTDWeight
     * @see setTimeConstant
     */
    void configure(float ltpWeight, float ltdWeight, float tau);
    
private:
    /**
     * @brief PImpl implementation structure for STDP
     * 
     * Internal implementation details for STDP are hidden by the PImpl idiom.
     * This structure contains the actual algorithmic implementation of the
     * spike-timing dependent plasticity rule.
     */
    struct Impl;
    
    /**
     * @brief Pointer to the STDP implementation
     * 
     * Forward declaration of the implementation pointer used by the PImpl idiom
     * to hide the concrete implementation of STDP from the header file.
     * This improves compilation times and encapsulation.
     */
    Impl* pImpl;
};

/**
 * @class RewardModulatedSTDP
 * @brief STDP with reward modulation (R-STDP)
 * 
 * Reward-Modulated STDP extends the standard STDP rule by incorporating
 * reward prediction error signals, making the learning rule more adaptive
 * to environmental outcomes. This rule combines spike-timing information
 * with neuromodulatory signals to guide synaptic plasticity based on both
 * temporal relationships and reward feedback.
 * 
 * R-STDP is particularly useful for reinforcement learning scenarios where
 * synaptic modifications should be influenced by reward prediction errors,
 * allowing the neural system to learn more efficiently from environmental
 * outcomes.
 * 
 * @note This class is a placeholder implementation for Phase 2 development.
 * @note The reward modulation typically involves dopamine or other
 *       neuromodulatory signals that scale the STDP weight updates.
 * 
 * @see STDP
 * @see Dopamine
 * @see PlasticityRule
 * 
 * @ingroup plasticity
 */
class RewardModulatedSTDP : public PlasticityRule {
public:
    /**
     * @brief Default constructor
     * 
     * Creates an R-STDP plasticity rule with default parameters. The actual
     * implementation details are hidden by the PImpl idiom.
     */
    RewardModulatedSTDP() = default;
    
    /**
     * @brief Virtual destructor
     * 
     * Cleans up R-STDP resources and performs finalization of reward-modulated
     * synaptic weight update algorithms.
     */
    ~RewardModulatedSTDP() override = default;
    
    /**
     * @brief Update synaptic weights with reward modulation
     * 
     * Placeholder implementation for reward-modulated STDP. This method
     * should compute weight changes based on spike timing differences,
     * scaled by reward prediction error signals from neuromodulatory systems.
     * 
     * @param synapse Pointer to the synapse whose weight will be modified.
     * @param preSpikes Vector of pre-synaptic spike times in milliseconds.
     * @param postSpikes Vector of post-synaptic spike times in milliseconds.
     * @param dt Simulation timestep duration in milliseconds.
     * 
     * @note The actual implementation will incorporate reward prediction errors
     *       from neuromodulatory systems (e.g., dopamine) to scale the STDP
     *       updates dynamically.
     * @note This method is a placeholder and will be fully implemented in
     *       Phase 2.
     */
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override {}
    
    /**
     * @brief Apply weight change with reward modulation
     * 
     * Placeholder implementation for applying reward-modulated weight changes.
     * This method should incorporate reward signals into the weight update.
     * 
     * @param synapse Pointer to the synapse to modify.
     * @param delta Weight change amount, potentially scaled by reward signals.
     * 
     * @note The actual implementation will scale delta by reward prediction errors
     *       to modulate learning strength based on environmental outcomes.
     */
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override {}
    
    /**
     * @brief Get the human-readable name of the R-STDP rule
     * 
     * Returns a descriptive string identifying this as a Reward-Modulated
     * STDP rule. This is useful for configuration, logging, and debugging.
     * 
     * @return Const char pointer to the string "R-STDP".
     * 
     * @see update
     * @see applyWeightChange
     */
    const char* getName() const override { return "R-STDP"; }
};

/**
 * @class TripletSTDP
 * @brief STDP with triplet interactions
 * 
 * Triplet STDP extends the standard STDP rule to include interactions between
 * spike triplets rather than just pairs. This rule accounts for the effects of
 * three spikes on synaptic weight changes, capturing more complex spike timing
 * patterns that are observed in biological neurons.
 * 
 * Triplet STDP is particularly important for modeling synaptic dynamics where
 * recent spikes have longer-lasting effects on synaptic strength. It provides
 * a more accurate representation of synaptic behavior in networks with bursty
 * spiking patterns.
 * 
 * @note This class is a placeholder implementation for Phase 2 development.
 * @note Triplet interactions are important for understanding synaptic learning
 *       in real neural circuits where neurons often fire in bursts.
 * 
 * @see STDP
 * @see PlasticityRule
 * 
 * @ingroup plasticity
 */
class TripletSTDP : public PlasticityRule {
public:
    /**
     * @brief Default constructor
     * 
     * Creates a Triplet STDP plasticity rule with default parameters. The actual
     * implementation details are hidden by the PImpl idiom.
     */
    TripletSTDP() = default;
    
    /**
     * @brief Virtual destructor
     * 
     * Cleans up Triplet STDP resources and performs finalization of triplet
     * synaptic weight update algorithms.
     */
    ~TripletSTDP() override = default;
    
    /**
     * @brief Update synaptic weights considering triplet interactions
     * 
     * Placeholder implementation for triplet-based STDP. This method should
     * compute weight changes based on interactions between spike triplets,
     * including contributions from pre-post-pre and post-pre-post patterns.
     * 
     * @param synapse Pointer to the synapse whose weight will be modified.
     * @param preSpikes Vector of pre-synaptic spike times in milliseconds.
     * @param postSpikes Vector of post-synaptic spike times in milliseconds.
     * @param dt Simulation timestep duration in milliseconds.
     * 
     * @note The actual implementation will account for spike triplet interactions,
     *       which are important for modeling synaptic dynamics in bursty firing patterns.
     */
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override {}
    
    /**
     * @brief Apply triplet interaction weight change
     * 
     * Placeholder implementation for applying triplet interaction weight changes.
     * 
     * @param synapse Pointer to the synapse to modify.
     * @param delta Weight change amount, potentially including triplet interaction terms.
     */
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override {}
    
    /**
     * @brief Get the human-readable name of the Triplet STDP rule
     * 
     * Returns a descriptive string identifying this as a Triplet STDP rule.
     * 
     * @return Const char pointer to the string "TripletSTDP".
     * 
     * @see update
     * @see applyWeightChange
     */
    const char* getName() const override { return "TripletSTDP"; }
};

} // namespace nlm
