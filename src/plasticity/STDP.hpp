#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

// Spike-Timing-Dependent Plasticity
// PLACEHOLDER - Phase 2 will implement real STDP

// STDP (Spike-Timing-Dependent Plasticity)
// Biologically inspired learning rule based on precise spike timing
// PLACEHOLDER - Phase 2 will implement real STDP

/**
 * @brief Spike-Timing-Dependent Plasticity (STDP) learning rule
 * 
 * STDP is one of the most biologically accurate synaptic learning rules,
 * where the sign and magnitude of synaptic weight changes depend on the
 * precise timing of pre- and post-synaptic spikes. This implements the
 * fundamental Hebbian principle with temporal precision.
 * 
 * @details STDP captures the essential biology of synaptic plasticity:
 * - Pre-before-post spikes → Long-Term Potentiation (LTP)
 * - Post-before-pre spikes → Long-Term Depression (LTD)
 * - The effect decays exponentially with time interval
 * - Asymmetric window typically ~20-50ms width
 * 
 * STDP is crucial for temporal sequence learning and forms the basis for
 * many neural network computations in biological systems. It enables the
 * network to learn causal relationships and temporal patterns.
 * 
 * @see RewardModulatedSTDP for reward-modulated variant
 * @see TripletSTDP for extended triplet-based model
 */
class STDP : public PlasticityRule {
public:
    /**
     * @brief Constructor - Creates STDP rule with default parameters
     * 
     * @details Initializes with typical biological values:
     * - ltpWeight: 0.01 (potentiation magnitude)
     * - ltdWeight: 0.012 (depression magnitude) 
     * - timeConstant: 20.0ms (STDP time window)
     * - minWeight: -1.0 (minimum synaptic weight)
     * - maxWeight: 1.0 (maximum synaptic weight)
     */
    STDP();
    
    /**
     * @brief Destructor
     */
    ~STDP() override;
    
    /**
     * @brief Update synaptic weights based on spike timing
     * 
     * @param synapse Pointer to the synapse being updated
     * @param preSpikes Vector of timestamps for pre-synaptic spikes
     * @param postSpikes Vector of timestamps for post-synaptic spikes  
     * @param dt Simulation timestep duration (ms)
     * 
     * @details Computes weight changes using the STDP rule:
     * For each pre-post spike pair with Δt = t_post - t_pre:
     * - If Δt > 0 (pre before post): LTP = A+ × exp(-Δt/τ+)
     * - If Δt < 0 (post before pre): LTD = A- × exp(Δt/τ-)
     * 
     * @param synapse Target synapse for weight modification
     * @param preSpikes List of pre-synaptic spike times in current window
     * @param postSpikes List of post-synaptic spike times in current window
     * @param dt Current simulation timestep for rate-based adjustments
     */
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    
    /**
     * @brief Apply computed weight change to synapse
     * 
     * @param synapse Target synapse for modification
     * @param delta Weight change amount to apply
     * 
     * @details Applies the computed weight change, respecting synapse weight
     * bounds and updating eligibility traces where appropriate.
     */
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    /**
     * @brief Get rule name identifier
     * 
     * @return const char* String identifier "STDP"
     */
    const char* getName() const override;
    
    /**
     * @brief Set long-term potentiation (LTP) weight parameter
     * 
     * @param weight A+ value for pre-before-post potentiation (default: 0.01)
     * 
     * @details LTP weight controls the magnitude of synaptic strengthening
     * when pre-synaptic spikes precede post-synaptic spikes. Typical biological
     * values range from 0.001 to 0.1, with 0.01 being a common baseline.
     */
    void setLTPWeight(float weight);    // Long-term potentiation weight
    
    /**
     * @brief Get long-term potentiation weight
     * 
     * @return float Current A+ parameter value
     */
    float getLTPWeight() const;
    
    /**
     * @brief Set long-term depression (LTD) weight parameter
     * 
     * @param weight A- value for post-before-pre depression (default: 0.012)
     * 
     * @details LTD weight controls the magnitude of synaptic weakening
     * when post-synaptic spikes precede pre-synaptic spikes. Typically slightly
     * larger than LTP to balance the asymmetry of the STDP window.
     */
    void setLTDWeight(float weight);    // Long-term depression weight
    
    /**
     * @brief Get long-term depression weight
     * 
     * @return float Current A- parameter value
     */
    float getLTDWeight() const;
    
    /**
     * @brief Set STDP time constant parameter
     * 
     * @param tau STDP time constant τ (default: 20.0ms)
     * 
     * @details Time constant controls the width of the STDP window.
     * Longer τ values make the rule more sensitive to spikes that occurred
     * further in the past. Typical biological values range from 10-50ms.
     */
    void setTimeConstant(float tau);    // STDP time constant (ms)
    
    /**
     * @brief Get STDP time constant
     * 
     * @return float Current τ parameter value in milliseconds
     */
    float getTimeConstant() const;
    
    /**
     * @brief Configure STDP parameters from configuration
     * 
     * @param ltpWeight LTP weight parameter
     * @param ltdWeight LTD weight parameter  
     * @param tau STDP time constant
     * 
     * @details Convenience method for configuring STDP from config file
     * or command-line arguments. Sets all three main parameters.
     */
    void configure(float ltpWeight, float ltdWeight, float tau);
    
    /**
     * @brief Get mathematical formulation of STDP rule
     * 
     * @return std::string Detailed explanation of STDP equations
     * 
     * @details Returns a string containing the complete mathematical formulation
     * of the STDP learning rule, including parameter definitions and biological
     * interpretation. Useful for documentation and debugging.
     */
    std::string getMathematicalFormulation() const;

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
