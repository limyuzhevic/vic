#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

/**
 * @class STDP
 * @brief Spike-Timing-Dependent Plasticity (STDP) implementation
 * @details Implements the classic STDP learning rule where synaptic strength
 *         depends on the precise timing between pre- and post-synaptic spikes:
 *         - Pre-before-Post (pre < post): Long-Term Potentiation (LTP)
 *         - Post-before-Pre (post < pre): Long-Term Depression (LTD)
 *         - No spike pair: No change
 * 
 * @note The STDP learning window typically has an asymmetric shape:
 *       - Positive weights for Δt < 0 (post before pre)
 *       - Negative weights for Δt > 0 (pre before post)
 *       - Exponential decay with time difference
 * 
 * @author NLM Research Team
 * @date Version 0.1.0
 * @ingroup plasticity
 * @implements spike_timing_dependent_plasticity
 * @extends PlasticityRule
 * 
 * @see TripletSTDP for extended STDP with presynaptic spike interactions
 * @see RewardModulatedSTDP for reward-modulated STDP variants
 */

class STDP : public PlasticityRule {
public:
    /**
     * @brief Construct a new STDP object
     * @details Creates STDP with default parameters:
     *          - LTP weight: 0.01
     *          - LTD weight: 0.012
     *          - Time constant: 20ms
     * 
     * @note Default parameters are based on biological observations:
     *       - LTP magnitude smaller than LTD
     *       - Time constant typically 10-30ms in biological neurons
     */
    STDP();
    
    /**
     * @brief Destroy the STDP object
     * @details Cleans up any allocated resources and resets internal state.
     */
    ~STDP() override;
    
    /**
     * @brief Update synaptic weights based on spike timing
     * @param synapse Pointer to the synapse being updated
     * @param preSpikes Vector of pre-synaptic spike timestamps
     * @param postSpikes Vector of post-synaptic spike timestamps
     * @param dt Simulation timestep duration
     * @details Implements the STDP learning rule by:
     *          - Calculating pairwise spike time differences (Δt = t_post - t_pre)
     *          - Applying LTP for Δt < 0 (post before pre)
     *          - Applying LTD for Δt > 0 (pre before post)
     *          - Summing weight changes from all spike pairs
     *          - Applying total weight change to synapse
     * 
     * @note The STDP update follows this mathematical formulation:
     *       w_new = w_old + Σ(η(Δt) * x_pre * x_post)
     *       where η(Δt) is the STDP learning function
     * 
     * @param synapse Synapse object to update
     * @param preSpikes Pre-synaptic spike history
     * @param postSpikes Post-synaptic spike history
     * @param dt Simulation timestep
     * @pre synapse must not be nullptr
     * @pre preSpikes and postSpikes must not be nullptr
     * @pre dt must be positive
     * @see applyWeightChange() for applying computed weight changes
     */
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    
    /**
     * @brief Apply weight change to synapse
     * @param synapse Pointer to the synapse to update
     * @param delta Weight change to apply
     * @details Applies the computed weight change to the synapse.
     *         This method is called by the STDP update method after
     *         computing the total weight change from spike timing.
     * 
     * @note Weight changes are bounded to prevent synapse weights
     *       from becoming too large or too small:
     *       - Maximum weight: 1.0 (normalized)
     *       - Minimum weight: -1.0 (normalized)
     * 
     * @param synapse Synapse to update
     * @param delta Weight change (can be positive or negative)
     * @pre synapse must not be nullptr
     * @see update() for computing weight changes from spike timing
     */
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    /**
     * @brief Get the name of this plasticity rule
     * @return const char* Name of the plasticity rule
     * @details Returns a string identifier for this plasticity rule.
     *         This is used for logging, debugging, and identification purposes.
     * 
     * @return Name of plasticity rule: "STDP"
     */
    const char* getName() const override;
    
    // ========== STDP PARAMETERS ==========
    
    /**
     * @brief Set long-term potentiation (LTP) weight
     * @param weight LTP weight (typically 0.01-0.05)
     * @details Sets the weight for LTP (potentiating synapses).
     *         LTP occurs when pre-synaptic spike precedes post-synaptic spike
     *         (Δt < 0), strengthening the synapse.
     * 
     * @note Biological STDP typically has:
     *       - LTP magnitude: 0.01-0.05 (1-5%)
     *       - LTD magnitude: 0.01-0.05 (1-5%)
     *       - LTD magnitude often slightly larger than LTP
     * 
     * @param weight LTP weight magnitude
     * @see getLTPWeight() for getting current value
     * @see configure() for setting multiple parameters at once
     */
    void setLTPWeight(float weight);    // Long-term potentiation weight
    /**
     * @brief Get the long-term potentiation (LTP) weight
     * @return float Current LTP weight
     * @see setLTPWeight() for setting value
     */
    float getLTPWeight() const;
    /**
     * @brief Set long-term depression (LTD) weight
     * @param weight LTD weight (typically 0.012-0.06)
     * @details Sets the weight for LTD (depressing synapses).
     *         LTD occurs when post-synaptic spike precedes pre-synaptic spike
     *         (Δt > 0), weakening the synapse.
     * 
     * @param weight LTD weight magnitude
     * @see getLTDWeight() for getting current value
     * @see configure() for setting multiple parameters at once
     */
    void setLTDWeight(float weight);    // Long-term depression weight
    /**
     * @brief Get the long-term depression (LTD) weight
     * @return float Current LTD weight
     * @see setLTDWeight() for setting value
     */
    float getLTDWeight() const;
    /**
     * @brief Set STDP time constant
     * @param tau Time constant (in milliseconds)
     * @details The time constant controls how quickly the STDP learning
     *         effect decays with spike time difference. Larger τ means the
     *         learning effect extends further in time.
     * 
     * @note The STDP learning function is typically:
     *       η(Δt) = A_plus * exp(-Δt/τ_plus) - A_minus * exp(-Δt/τ_minus)
     *       where τ_plus and τ_minus are separate time constants.
     * 
     * @param tau STDP time constant in milliseconds
     * @see getTimeConstant() for getting current value
     * @see configure() for setting multiple parameters at once
     */
    void setTimeConstant(float tau);    // STDP time constant (ms)
    /**
     * @brief Get the STDP time constant
     * @return float Current time constant (in ms)
     * @see setTimeConstant() for setting value
     */
    float getTimeConstant() const;
    
    /**
     * @brief Configure STDP parameters from configuration values
     * @param ltpWeight Long-term potentiation weight
     * @param ltdWeight Long-term depression weight
     * @param tau Time constant (ms)
     * @details Convenience method to set all STDP parameters at once.
     *         This is typically called during brain initialization to
     *         load STDP parameters from the configuration file.
     * 
     * @param ltpWeight LTP weight
     * @param ltdWeight LTD weight
     * @param tau Time constant
     * @see setLTPWeight() for individual parameter setting
     * @see setLTDWeight() for individual parameter setting
     * @see setTimeConstant() for individual parameter setting
     */
    void configure(float ltpWeight, float ltdWeight, float tau);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Reward-modulated STDP (R-STDP)
// PLACEHOLDER - Phase 2
class RewardModulatedSTDP : public PlasticityRule {
public:
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override {}
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override {}
    const char* getName() const override { return "R-STDP"; }
};

// Spike-timing dependent plasticity with triplet interactions
// PLACEHOLDER - Phase 2
class TripletSTDP : public PlasticityRule {
public:
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override {}
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override {}
    const char* getName() const override { return "TripletSTDP"; }
};

} // namespace nlm
