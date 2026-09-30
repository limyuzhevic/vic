#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

/**
 * @class STDP
 * @brief Spike-Timing-Dependent Plasticity (STDP) learning rule
 * @details Implements Hebbian learning that modifies synaptic strength
 * based on the precise timing relationship between pre- and post-synaptic spikes.
 * 
 * STDP is a biologically plausible learning rule that captures the essential
 * temporal dependency of synaptic plasticity observed in the brain.
 * 
 * Algorithm Explanation:
 * 1. **Problem**: How do synapses strengthen when pre-synaptic neuron fires before
 *    post-synaptic, and weaken when post fires before pre?
 * 2. **Mathematical formulation**: Δw = A+ * exp(-Δt/τ+) for Δt > 0
 * 3. **Biological basis**: NMDA receptor activation and calcium dynamics
 * 4. **Implementation**: Pairwise spike interaction with exponential windows
 * 
 * Design Rationale:
 * - Uses Pimpl idiom for implementation hiding and binary compatibility
 * - Event-driven spike propagation for efficiency with sparse activity
 * - Modular plasticity system allowing multiple concurrent rules
 * 
 * References:
 * - Abbott, L.F., & Hooper, S.L. (2004). The computational anatomy of
 *   synaptic plasticity. Science, 304(5679), 1806-1810.
 * - Bi, G.Q., & Poo, M.M. (2001). Synaptic modifications in neural
 *   development, learning and disease. Nature Reviews Neuroscience,
 *   2(4), 251-262.
 */

class STDP : public PlasticityRule {
public:
    /**
     * @brief Construct STDP plasticity rule with default parameters
     * @details Default parameters:
     *   - LTP weight (A+): 0.01 (10% potentiation per optimal spike pair)
     *   - LTD weight (A-): 0.012 (12% depression per optimal spike pair)
     *   - Time constant τ: 20ms (decay time of plasticity effect)
     *   - Min/max weight: [-1.0, 1.0] (normalized synaptic weights)
     */
    STDP();
    
    /**
     * @brief Destroy STDP plasticity rule
     */
    ~STDP() override;
    
    /**
     * @brief Update synaptic weights based on spike timing correlations
     * @param synapse Pointer to synapse to modify
     * @param preSpikes Timestamps of pre-synaptic neuron spikes
     * @param postSpikes Timestamps of post-synaptic neuron spikes
     * @param dt Simulation timestep duration
     * 
     * @details STDP Implementation:
     * For each pre-post spike pair with timing difference Δt = t_post - t_pre:
     * 
     * **Pre-before-post (Δt > 0)**: Potentiation (LTP)
     *   Δw = A+ * exp(-Δt / τ+)
     *   Example: If Δt = 10ms, τ+ = 20ms, A+ = 0.01
     *   Δw = 0.01 * exp(-10/20) = 0.01 * 0.6065 = 0.0061 (0.61% potentiation)
     * 
     * **Post-before-pre (Δt < 0)**: Depression (LTD)
     *   Δw = A- * exp(Δt / τ-)
     *   Example: If Δt = -10ms, τ- = 20ms, A- = 0.012
     *   Δw = -0.012 * exp(-10/20) = -0.0073 (0.73% depression)
     * 
     * **Simultaneous spikes (Δt ≈ 0)**: No change (rare in practice)
     * 
     * **Biological Inspiration**:
     * - NMDA receptor activation requires both presynaptic glutamate and
     *   postsynaptic depolarization (voltage-dependent Mg2+ block removal)
     * - Calcium influx through NMDA receptors triggers LTP when pre spikes first
     * - Backpropagating action potentials trigger LTD when post spikes first
     * - Reflects spike-timing-dependent calcium dynamics
     * 
     * **Implementation Notes**:
     * - O(n²) complexity where n is spike count (pairwise spike interactions)
     * - For sparse activity, use spike queues for efficient Δt computation
     * - Vectorized implementation for parallel spike pair processing
     * - eligibility trace for reward-modulated STDP integration
     * 
     * **Limitations**:
     * - Simplified pairwise rule (doesn't capture triplet interactions)
     * - Assumes single exponential window (real STDP has asymmetric windows)
     * - Doesn't account for synaptic eligibility traces or metaplasticity
     * - Neglects dendritic compartmentalization effects
     */
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
                 
    /**
     * @brief Apply direct weight change to synapse
     * @param synapse Pointer to synapse to modify
     * @param delta Weight change value (can be positive or negative)
     * 
     * @details Used for non-timing-dependent plasticity mechanisms
     * or when external learning signals override STDP.
     */
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    /**
     * @brief Get rule name identifier
     * @return String describing this plasticity rule
     */
    const char* getName() const override;
    
    /**
     * @brief Set long-term potentiation weight (A+)
     * @param weight Potentiation amplitude (typically 0.01-0.05)
     * 
     * @details A+ determines the maximum potentiation per spike pair.
     * Larger values produce stronger synaptic strengthening.
     */
    void setLTPWeight(float weight);    // Long-term potentiation weight
    
    /**
     * @brief Get long-term potentiation weight
     * @return Current LTP weight value
     */
    float getLTPWeight() const;
    
    /**
     * @brief Set long-term depression weight (A-)
     * @param weight Depression amplitude (typically 0.01-0.05)
     * 
     * @details A- determines the maximum depression per spike pair.
     * Often similar to A+ but may differ in biological systems.
     */
    void setLTDWeight(float weight);    // Long-term depression weight
    
    /**
     * @brief Get long-term depression weight
     * @return Current LTD weight value
     */
    float getLTDWeight() const;
    
    /**
     * @brief Set STDP time constant (τ)
     * @param tau Time constant in milliseconds (typically 10-50ms)
     * 
     * @details τ controls the width of the STDP time window.
     * Smaller τ means STDP effects decay faster with spike timing difference.
     */
    void setTimeConstant(float tau);    // STDP time constant (ms)
    
    /**
     * @brief Get STDP time constant
     * @return Current time constant value (ms)
     */
    float getTimeConstant() const;
    
    /**
     * @brief Configure all STDP parameters from configuration
     * @param ltpWeight Long-term potentiation weight
     * @param ltdWeight Long-term depression weight  
     * @param tau STDP time constant
     * 
     * @details Convenience method for batch parameter setting.
     * Validates all parameters before applying.
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
