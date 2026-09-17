#pragma once

#include "PlasticityRule.hpp"
#include "../core/Types/Types.hpp"

namespace nlm {

// Reward-modulated STDP (R-STDP) implementation
// Integrates dopamine signaling with spike-timing dependent plasticity
// Implements temporal difference learning with synaptic eligibility traces
// 
// Biological basis:
// - VTA dopamine neurons signal reward prediction error (RPE)
// - DA modulates STDP efficacy via D1 and D2 receptor pathways
// - Positive RPE (better than expected): enhances LTP, reduces LTD
// - Negative RPE (worse than expected): suppresses LTP, enhances LTD
// - Zero RPE (expected): baseline STDP unchanged
// 
// Mathematical formulation:
// Δw(t) = φ(r(t)) × [A+ × exp(-(t_post - t_pre)/τ+) - A- × exp(-(t_pre - t_post)/τ-)]
// 
// Where:
//   φ(r(t)) is the reward modulation function (dopamine signal)
//   A+ = rewardBaseline + rewardModGain × dopamineLevel (potentiation)
//   A- = rewardBaseline + rewardModGain × dopamineLevel × α (depression)
//   τ+, τ- = STDP time constants (typically 10-30ms for potentiation, 50-100ms for depression)

class RewardModulatedSTDP : public PlasticityRule {
public:
    // Constructor with configuration
    explicit RewardModulatedSTDP(const STDPConfig& config = STDPConfig());
    ~RewardModulatedSTDP() override;
    
    // Update synaptic weights based on spike timing with reward modulation
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    
    // Apply weight change with neuromodulation effects
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    // Get rule name
    const char* getName() const override { return "R-STDP"; }
    
    // Reward modulation parameters
    void setRewardBaseline(float reward);
    void setRewardModGain(float gain);
    float getRewardBaseline() const;
    float getRewardModGain() const;
    
    // Compute reward modulation factor
    float computeRewardModulation(float dopamine, float predictionError, float dt);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm