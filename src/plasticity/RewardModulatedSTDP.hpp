#pragma once

#include "STDP.hpp"

namespace nlm {

// Reward-Modulated STDP (R-STDP)
// Combines spike-timing dependence with reward modulation
// 
// Mathematical formulation:
// Δw = A+ * exp(-Δt / τ+) * F(δ)
// Δw = A- * exp(Δt / τ-) * F(δ)
// 
// Where F(δ) is a reward modulation function:
// F(δ) = 1 + β * δ for δ ≥ 0 (positive prediction error)
// F(δ) = 1 - β * |δ| for δ < 0 (negative prediction error)
// 
// δ = dopamine_signal * prediction_error
// β = reward sensitivity parameter

class RewardModulatedSTDP : public STDP {
public:
    RewardModulatedSTDP();
    ~RewardModulatedSTDP() override;
    
    using STDP::update;  // Inherit base class update with different signature
    
    // Override with reward-modulated version
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt,
                float rewardPredictionError = 0.0f) override;
    
    // Reward modulation function
    void setRewardSensitivity(float beta);  // β parameter
    float getRewardSensitivity() const;
    
    // Enable/disable reward modulation
    void enableRewardModulation(bool enable);
    bool isRewardModulationEnabled() const;
    
    // Get current modulation factor
    float getCurrentModulationFactor() const;
    
    // Set reward prediction error signal
    void setPredictionError(float error) { predictionError = error; }
    float getPredictionError() const { return predictionError; }

private:
    struct Impl;
    Impl* pImpl;
    
    // External reward prediction error signal
    float predictionError;
    bool rewardModulationEnabled;
};

} // namespace nlm
