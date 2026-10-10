#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

/**
 * @brief Spike-Timing-Dependent Plasticity (STDP) implementation.
 * 
 * STDP is a fundamental learning rule in neuroscience that modifies synaptic
 * strength based on the relative timing of pre- and post-synaptic spikes.
 * This rule captures the biological phenomenon where the order and timing
 * of neural activity determine whether connections are strengthened or weakened.
 * 
 * STDP is considered the most biologically plausible learning rule because
 * it reflects actual synaptic mechanisms observed in experiments:
 * - NMDA receptor-dependent calcium signaling
 * - Backpropagating action potentials
 * - Short-term synaptic dynamics
 * 
 * The rule implements asymmetric learning windows:
 * - Pre-before-post spikes → potentiation (strengthening)
 * - Post-before-pre spikes → depression (weakening)
 * 
 * @note This is a placeholder implementation for Phase 2. The complete
 *       implementation will include eligibility traces, triplet interactions,
 *       and integration with neuromodulatory signals.
 * 
 * @see PlasticityRule for base class interface
 * @see HebbianRule for complementary Hebbian learning
 * @see RewardModulatedSTDP for reward-dependent variations
 * @see TripletSTDP for higher-order spike interaction rules
 * @see StructuralPlasticity for structural changes (new synapses, pruning)
 * 
 * @example
 * @code
 * // Create STDP learning rule
 * STDP stdp;
 * 
 * // Configure STDP parameters
 * stdp.configure(0.01f, 0.012f, 20.0f);  // (A+, A-, τ)
 * 
 * // Apply STDP based on spike timing
 * stdp.update(synapse, preSpikes, postSpikes, dt);
 * 
 * // Or configure via individual parameters
 * stdp.setLTPWeight(0.01f);    // A+ = 0.01 (potentiation amplitude)
 * stdp.setLTDWeight(0.012f);   // A- = 0.012 (depression amplitude)
 * stdp.setTimeConstant(20.0f); // τ = 20ms (time window)
 * @endcode
 */
class STDP : public PlasticityRule {
public:
    /**
     * @brief Constructs a STDP learning rule with default parameters.
     * 
     * Initializes the spike-timing-dependent plasticity system with
     * standard biological parameters that balance learning speed and stability.
     * 
     * Default parameters reflect typical values observed in experimental
     * studies of STDP in cortical and hippocampal neurons:
     * - A+ = 0.01: Potentiation amplitude for pre-before-post spikes
     * - A- = 0.012: Depression amplitude for post-before-pre spikes
     * - τ = 20ms: Time constant for exponential learning windows
     * - Range: [-1.0, 1.0] standard synaptic weight bounds
     */
    STDP();
    
    /**
     * @brief Destroys the STDP learning rule.
     * 
     * Cleans up internal resources and performs any necessary cleanup
     * before the STDP rule is destroyed. The default implementation
     * is sufficient as the pImpl unique_ptr handles resource management.
     */
    ~STDP() override;
    
    /**
     * @brief Updates synaptic weights based on spike timing correlations.
     * 
     * Implements the core STDP learning rule by analyzing the timing
     * differences between pre- and post-synaptic spikes. The rule applies
     * asymmetric learning windows to determine appropriate weight changes.
     * 
     * Algorithm:
     * 1. For each pre-post spike pair with timing difference Δt = t_post - t_pre:
     *    a. If Δt > 0 (pre fires before post): apply potentiation
     *    b. If Δt < 0 (post fires before pre): apply depression
     * 2. Accumulate total weight change across all spike pairs
     * 3. Apply total change scaled by synaptic efficacy
     * 4. Update eligibility trace for reward-modulated learning
     * 
     * Mathematical Formulation:
     * For each pre-post spike pair with timing difference Δt = t_post - t_pre:
     * 
     * If Δt > 0 (pre before post): POTENTIATION
     *   Δw = A⁺ × exp(-Δt / τ⁺)
     *   
     * If Δt < 0 (post before pre): DEPRESSION  
     *   Δw = -A⁻ × exp(Δt / τ⁻)
     * 
     * Where:
     * - A⁺ = ltpWeight (potentiation amplitude)
     * - A⁻ = ltdWeight (depression amplitude)
     * - τ⁺ = τ⁻ = timeConstant (STDP time window)
     * - Δt in milliseconds
     * 
     * Biological Interpretation:
     * - Reflects NMDA receptor-mediated calcium signaling dynamics
     * - Pre-before-post activates NMDA receptors when postsynaptic spikes
     * - Post-before-pre causes backpropagating action potentials
     * - Captures temporal asymmetry observed in hippocampal experiments
     * 
     * @param synapse Pointer to the synapse to update
     * @param preSpikes Vector of pre-synaptic spike timestamps (chronological)
     * @param postSpikes Vector of post-synaptic spike timestamps (chronological)
     * @param dt Timestep duration for learning rate normalization
     * 
     * @note Spike timing precision: Timestamps use milliseconds
     * @note Time window: τ = 1-100ms (typical range)
     * @note Potentiation range: +A⁺ × [0,1] = [0,A⁺]
     * @note Depression range: -A⁻ × [0,1] = [-A⁻,0]
     * @note Eligibility traces updated for reward-modulated variants
     * 
     * @see applyWeightChange for direct weight modification
     * @see setLTPWeight for configuring potentiation parameters
     * @see setLTDWeight for configuring depression parameters
     * @see setTimeConstant for configuring time window
     * 
     * @example
     * @code
     * // Example spike timing analysis:
     * std::vector<Timestamp> preSpikes = {10.0, 20.5, 30.2, 40.8};
     * std::vector<Timestamp> postSpikes = {10.5, 20.8, 35.0, 45.2};
     * 
     * // Pre-before-post (Δt > 0): potentiation
     * // pre 10.0, post 10.5: Δt = 0.5ms, Δw = A⁺ × exp(-0.5/20) = 0.01 × 0.975
     * // pre 20.5, post 20.8: Δt = 0.3ms, Δw = A⁺ × exp(-0.3/20) = 0.01 × 0.986
     * 
     * // Post-before-pre (Δt < 0): depression
     * // pre 40.8, post 35.0: Δt = -5.8ms, Δw = -A⁻ × exp(-5.8/20) = -0.012 × 0.746
     * 
     * // Cumulative effect determines final weight change
     * @endcode
     */
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    
    /**
     * @brief Applies a direct weight change using STDP principles.
     * 
     * This method provides a direct interface for STDP weight modification,
     * bypassing spike timing analysis for performance or specific learning scenarios.
     * It ensures consistent weight update handling with biological constraints.
     * 
     * @param synapse Pointer to the synapse to modify
     * @param delta Weight change amount (positive for potentiation, negative for depression)
     * 
     * @note Applies STDP-specific bounds: weight clamped to [minWeight, maxWeight]
     * @note Handles both potentiation (positive delta) and depression (negative delta)
     * @note Includes synaptic efficacy scaling if available
     * 
     * @see update for spike-timing-dependent learning
     * @see setLTPWeight for potentiation parameter configuration
     * @see setLTDWeight for depression parameter configuration
     */
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    /**
     * @brief Returns the name of the STDP plasticity rule.
     * 
     * Provides a string identifier for the STDP learning algorithm,
     * useful for logging, debugging, and configuration purposes.
     * 
     * @return const char* String "STDP"
     * 
     * @note Consistent with class name for easy identification
     * @see isEnabled for learning permission status
     * @see configure for parameter configuration
     */
    const char* getName() const override;
    
    /**
     * @brief Configures long-term potentiation (LTP) parameters.
     * 
     * Sets the amplitude of synaptic strengthening when pre-synaptic spikes
     * occur before post-synaptic spikes. LTP represents the molecular basis
     * of memory formation and learning in biological systems.
     * 
     * @param weight Potentiation amplitude (A⁺) (0.0 to 1.0)
     * @note Represents maximum weight increase for ideal timing
     * @note Biologically: NMDA receptor binding strength
     * @note Default value: 0.01 (1% maximum potentiation per spike pair)
     * @see getLTPWeight for current LTP parameter
     * @see configure for batch parameter configuration
     */
    void setLTPWeight(float weight);
    
    /**
     * @brief Returns the current long-term potentiation (LTP) parameter.
     * 
     * Retrieves the potentiation amplitude used for synaptic strengthening
     * in STDP learning.
     * 
     * @return float Current LTP parameter (A⁺)
     * @note Default value: 0.01
     * @see setLTPWeight for LTP parameter configuration
     */
    float getLTPWeight() const;
    
    /**
     * @brief Configures long-term depression (LTD) parameters.
     * 
     * Sets the amplitude of synaptic weakening when post-synaptic spikes
     * occur before pre-synaptic spikes. LTD provides homeostatic regulation
     * and prevents runaway excitation in neural networks.
     * 
     * @param weight Depression amplitude (A⁻) (0.0 to 1.0)
     * @note Represents maximum weight decrease for ideal timing
     * @note Biologically: Low calcium concentration signaling
     * @note Default value: 0.012 (1.2% maximum depression per spike pair)
     * @see getLTDWeight for current LTD parameter
     * @see configure for batch parameter configuration
     */
    void setLTDWeight(float weight);
    
    /**
     * @brief Returns the current long-term depression (LTD) parameter.
     * 
     * Retrieves the depression amplitude used for synaptic weakening
     * in STDP learning.
     * 
     * @return float Current LTD parameter (A⁻)
     * @note Default value: 0.012
     * @see setLTDWeight for LTD parameter configuration
     */
    float getLTDWeight() const;
    
    /**
     * @brief Configures the STDP time constant (learning window).
     * 
     * Sets the exponential decay time constant for both potentiation and
     * depression learning windows. This parameter controls how quickly
     * the influence of spike timing decays over time.
     * 
     * @param tau Time constant (τ) for exponential window (ms)
     * @note Controls the width of the STDP learning window
     * @note Higher values: Longer time influence, more integration
     * @note Lower values: Shorter time influence, more temporal specificity
     * @note Range: 1-100ms (biologically plausible range)
     * @note Default value: 20.0ms
     * @see getTimeConstant for current time constant
     * @see configure for batch parameter configuration
     */
    void setTimeConstant(float tau);
    
    /**
     * @brief Returns the current STDP time constant.
     * 
     * Retrieves the exponential decay time constant used for both
     * potentiation and depression learning windows.
     * 
     * @return float Current time constant (τ) in milliseconds
     * @note Default value: 20.0ms
     * @see setTimeConstant for time constant configuration
     */
    float getTimeConstant() const;
    
    /**
     * @brief Batch configuration for STDP parameters.
     * 
     * Configures multiple STDP parameters in a single call for convenience
     * and efficiency. This ensures consistent parameter sets and reduces
     * the number of function calls needed for setup.
     * 
     * @param ltpWeight Potentiation amplitude (A⁺)
     * @param ltdWeight Depression amplitude (A⁻)
     * @param tau Time constant for exponential window (ms)
     * 
     * @note Uses individual setter methods internally for validation
     * @note Ensures parameter consistency across all STDP functions
     * @note Useful for loading STDP parameters from configuration files
     * 
     * @see setLTPWeight for individual LTP configuration
     * @see setLTDWeight for individual LTD configuration
     * @see setTimeConstant for individual time constant configuration
     */
    void configure(float ltpWeight, float ltdWeight, float tau);
    
private:
    /**
     * @brief Internal implementation structure for STDP.
     * 
     * Encapsulates runtime parameters and state for spike-timing-dependent
     * plasticity. Includes learning rule parameters and synaptic weight bounds.
     * 
     * @see STDP for public interface
     * @see update for learning rule implementation
     * @see applyWeightChange for weight update implementation
     */
    struct Impl;
    Impl* pImpl;
};

/**
 * @brief Reward-modulated STDP (R-STDP) implementation.
 * 
 * Extends STDP with reward/punishment signals that modulate synaptic learning.
 * This rule implements reinforcement learning where the success or failure
 * of actions influences synaptic changes, creating a bridge between unsupervised
 * (STDP) and supervised (reward) learning.
 * 
 * Key Features:
 * - STDP provides baseline learning rule
 * - Reward signals modulate learning strength
 * - Punishment reduces or reverses learning
 * - Enables combination of temporal and outcome-based information
 * 
 * Mathematical Model:
 * Δw = R × STDP(Δt)
 * where:
 * - R is reward signal (+1 for reward, -1 for punishment, 0 for neutral)
 * - STDP(Δt) is the standard STDP weight change for spike timing Δt
 * 
 * Biological Plausibility:
 * - Reflects dopaminergic modulation of synaptic plasticity
 * - Dopamine signals gate STDP-based learning
 * * - Implements reward prediction error signals
 * 
 * @note This is a placeholder implementation for Phase 2
 * @note The implementation is simplified and may not reflect biological accuracy
 * @note R-STDP provides a foundation for value-based learning systems
 * 
 * @see PlasticityRule for base class interface
 * @see STDP for core STDP learning rule
 * @see TripletSTDP for higher-order spike interaction rules
 */
class RewardModulatedSTDP : public PlasticityRule {
public:
    /**
     * @brief Constructs a RewardModulatedSTDP learning rule.
     * 
     * Initializes the reward-modulated STDP system with default parameters
     * for reinforcement learning. The rule is ready to process spike timing
     * and apply reward-based modulation.
     */
    RewardModulatedSTDP();
    
    /**
     * @brief Updates synaptic weights using reward-modulated STDP.
     * 
     * This method is currently a placeholder implementation. In a complete
     * implementation, it would apply STDP learning rules modulated by
     * reward signals that influence synaptic change magnitude and direction.
     * 
     * @param synapse Pointer to the synapse to update
     * @param preSpikes Vector of pre-synaptic spike timestamps
     * @param postSpikes Vector of post-synaptic spike timestamps
     * @param dt Timestep duration for learning rate normalization
     */
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    
    /**
     * @brief Applies a direct weight change using reward-modulated STDP.
     * 
     * This method is currently a placeholder implementation. In a complete
     * implementation, it would apply weight changes modulated by reward signals.
     * 
     * @param synapse Pointer to the synapse to modify
     * @param delta Weight change amount
     */
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    /**
     * @brief Returns the name of the reward-modulated STDP plasticity rule.
     * 
     * @return const char* String "R-STDP"
     */
    const char* getName() const override { return "R-STDP"; }
};

/**
 * @brief Spike-timing dependent plasticity with triplet interactions.
 * 
 * Extends STDP to account for higher-order spike interactions involving
 * triplets of spikes (pre-post-post, pre-pre-post, post-pre-pre). This
 * rule captures more complex temporal patterns of neural activity that
 * simple pairwise STDP cannot represent.
 * 
 * Biological Motivation:
 * - Reflects synaptic dynamics with multiple calcium sources
 * - Captures interactions between NMDA and AMPA receptors
 * - Models short-term synaptic facilitation and depression
 * - Provides more realistic learning dynamics
 * 
 * Algorithm:
 * 1. Process standard STDP pairwise interactions
 * 2. Add triplet interaction terms for:
 *    - Pre-before-post-post (postsynaptic calcium influx)
 *    - Pre-pre-post (presynaptic facilitation)
 *    - Post-pre-pre (backpropagating potentials)
 * 3. Combine all contributions for final weight change
 * 
 * Mathematical Model (simplified):
 * Δw = STDP_pairwise + STDP_triplet
 * 
 * Where triplet terms depend on recent spike history and synaptic
 * short-term dynamics. This provides more accurate predictions of
 * experimental STDP data with complex spike patterns.
 * 
 * @note This is a placeholder implementation for Phase 2
 * @note The implementation is simplified and may not reflect biological accuracy
 * @note Triplet STDP provides more realistic learning dynamics
 * 
 * @see PlasticityRule for base class interface
 * @see STDP for core pairwise STDP learning rule
 * @see RewardModulatedSTDP for reward-dependent variations
 */
class TripletSTDP : public PlasticityRule {
public:
    /**
     * @brief Constructs a TripletSTDP learning rule.
     * 
     * Initializes the triplet-based spike-timing-dependent plasticity
     * system with default parameters for higher-order spike interaction learning.
     */
    TripletSTDP();
    
    /**
     * @brief Updates synaptic weights using triplet STDP principles.
     * 
     * This method is currently a placeholder implementation. In a complete
     * implementation, it would apply STDP rules with triplet interaction
     * terms that capture higher-order spike timing patterns.
     * 
     * @param synapse Pointer to the synapse to update
     * @param preSpikes Vector of pre-synaptic spike timestamps
     * @param postSpikes Vector of post-synaptic spike timestamps
     * @param dt Timestep duration for learning rate normalization
     */
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    
    /**
     * @brief Applies a direct weight change using triplet STDP principles.
     * 
     * This method is currently a placeholder implementation. In a complete
     * implementation, it would apply weight changes with triplet interaction terms.
     * 
     * @param synapse Pointer to the synapse to modify
     * @param delta Weight change amount
     */
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    /**
     * @brief Returns the name of the triplet STDP plasticity rule.
     * 
     * @return const char* String "TripletSTDP"
     */
    const char* getName() const override { return "TripletSTDP"; }
};

} // namespace nlm
