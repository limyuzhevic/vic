#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f) {}
};

Dopamine::Dopamine() : pImpl(new Impl) {}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "DA";
}

float Dopamine::getLevel() const {
    return pImpl->level;
}

void Dopamine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Dopamine::getPlasticityFactor() const {
    // Phase 6: Real dopamine-modulated plasticity factor
    // Dopamine affects both learning rate and the balance between LTP/LTD
    // Higher dopamine increases plasticity up to a point, then decreases it
    // to prevent runaway synaptic changes
    
    float level = pImpl->level;
    
    // Convert to plasticity factor: 0.0 (no plasticity) to 2.0 (high plasticity)
    // Low dopamine (<0.2) reduces plasticity to stabilize learned behavior
    // Moderate dopamine (0.2-0.6) increases plasticity for learning
    // High dopamine (>0.6) reduces plasticity to prevent overfitting
    float plasticityFactor = 1.0f;
    
    if (level < 0.2f) {
        plasticityFactor = 0.2f + (level / 0.2f) * 0.8f;  // 0.2 to 1.0
    } else if (level < 0.6f) {
        plasticityFactor = 1.0f + (level - 0.2f) / 0.4f * 0.8f;  // 1.0 to 1.8
    } else {
        plasticityFactor = 1.8f - (level - 0.6f) / 0.4f * 0.8f;  // 1.8 to 1.0
    }
    
    return plasticityFactor;
}

void Dopamine::update(TimestepDuration dt) {
    // Phase 6: Real dopamine dynamics with decay and smoothing
    // Dopamine levels decay over time when not actively reinforced
    // and respond to reward prediction errors
    
    // Exponential decay towards baseline
    float timeFactor = 1.0f - std::exp(-pImpl->decayRate * static_cast<float>(dt));
    pImpl->level = pImpl->baseline + (pImpl->level - pImpl->baseline) * (1.0f - timeFactor);
}

void Dopamine::signalReward(float reward) {
    // Phase 6: Real reward signaling with burst response
    // Positive rewards cause dopamine burst, negative rewards cause dip
    // Response is scaled by reward magnitude and saturates at peak level
    
    // Reward burst: reward strength increases dopamine level
    float rewardImpact = std::clamp(reward / 10.0f, 0.0f, 1.0f);  // Normalize reward to [0,1]
    pImpl->level = std::min(pImpl->peak, pImpl->level + rewardImpact * pImpl->releaseRate);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Phase 6: Real reward prediction error signaling
    // Positive prediction error (better than expected) increases dopamine
    // Negative prediction error (worse than expected) decreases dopamine
    // This drives learning based on prediction errors
    
    // Prediction error impact: error in [-2,2] range maps to reasonable dopamine changes
    float errorImpact = std::clamp(error / 2.0f, -1.0f, 1.0f);
    
    if (errorImpact > 0) {
        // Positive prediction error: burst of dopamine
        pImpl->level = std::min(pImpl->peak, pImpl->level + errorImpact * pImpl->releaseRate);
    } else {
        // Negative prediction error: dip in dopamine
        pImpl->level = std::max(pImpl->baseline, pImpl->level + errorImpact * pImpl->releaseRate);
    }
}

} // namespace nlm
