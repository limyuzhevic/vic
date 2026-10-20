#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

/**
 * @class STDP
 * @brief Spike-Timing-Dependent Plasticity implementation for synaptic learning
 * @details STDP is a Hebbian learning rule where synaptic strength depends on the
 * precise timing between pre- and post-synaptic spikes. This implementation
 * provides the biological mechanism for learning associations through spike timing.
 * 
 * STDP Mechanism:
 * - Pre-before-post spikes: Long-Term Potentiation (LTP) - strengthen synapse
 * - Post-before-pre spikes: Long-Term Depression (LTD) - weaken synapse
 * - Temporal window: ~20-50ms for biological plausibility
 * - Synaptic weight bounded: prevents runaway excitation
 * 
 * Learning Dynamics:
 * - Asymmetric weight changes: LTP > LTD typically
 * - Activity-dependent plasticity: strength depends on spike history
 * - Normalization: maintains overall network stability
 * - Pair-based implementation: processes spike pairs individually
 * 
 * Thread Safety: Not thread-safe for concurrent synaptic updates. STDP updates
 * should be synchronized with neural simulation to prevent race conditions.
 * 
 * Performance: O(m*n) for update where m = pre-synaptic spikes, n = post-synaptic spikes.
 * Optimized for sparse spike patterns typical in biological neurons.
 * 
 * Biological Plausibility:
 * - Implements classic STDP observed in cortical synapses
 * - Time constants match experimental data (~10-100ms)
 * - Weight changes follow sigmoidal curves
 * - Supports synaptic tagging and capture
 * 
 * @code
 * // Configure STDP parameters
 * STDP stdp;
 * stdp.setLTPWeight(0.05f);    // Max potentiation (5% weight increase)
 * stdp.setLTDWeight(0.1f);     // Max depression (10% weight decrease)
 * stdp.setTimeConstant(20.0f); // 20ms time constant
 * 
 * // Process spikes during simulation step
 * vector<Timestamp> preSpikes = {0.1, 0.3, 0.7};
 * vector<Timestamp> postSpikes = {0.25, 0.5};
 * stdp.update(synapse, preSpikes, postSpikes, dt);
 * 
 * // Apply resulting weight change
 * stdp.applyWeightChange(synapse, delta);
 * 
 * // Continuous learning
 * // Each spike pair contributes small weight changes
 * // Over time, synapses strengthen for consistent pre-post patterns
 * @endcode
 * 
 * References:
 * - Bi, G.Q. & Poo, M.M. (1998). Synaptic modifications in cultured neurons:
 *   dependence on spike timing and membrane potential. Journal of Neurobiology.
 * - Song, S.Y., Miller, K.D. & Abbott, L.F. (2000). Competitive learning in
 *   layered neural networks. Physical Review E.
 * 
 * @see PlasticityRule
 * @see Synapse
 * @see Hebbian
 * @see Brain
 * @see SimulationClock
 */
class STDP : public PlasticityRule {
public:
    STDP();
    ~STDP() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // STDP parameters
    void setLTPWeight(float weight);    // Long-term potentiation weight
    float getLTPWeight() const;
    void setLTDWeight(float weight);    // Long-term depression weight
    float getLTDWeight() const;
    void setTimeConstant(float tau);    // STDP time constant (ms)
    float getTimeConstant() const;
    
    // Update parameters from config
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
