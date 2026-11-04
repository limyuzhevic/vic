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
    // TODO PHASE 2: Implement real dopamine-modulated plasticity factor
    // PLACEHOLDER: Higher dopamine increases plasticity
    
    // Real implementation: Dopamine modulates plasticity in a more complex way:
    // - Low dopamine (0-0.2): High exploration, high plasticity
    // - Medium dopamine (0.2-0.8): Normal learning, moderate plasticity  
    // - High dopamine (0.8-1.0): Exploitation, low plasticity
    // - Negative dopamine: Strong LTD (long-term depression)
    
    float level = pImpl->level;
    
    if (level < 0.0f) {
        // Negative dopamine strongly increases LTD
        return 0.1f + 0.5f * std::max(-level, 0.0f);
    } else if (level < 0.2f) {
        // Low positive dopamine - high exploration and plasticity
        return 1.5f + 0.5f * level;
    } else if (level < 0.8f) {
        // Medium dopamine - balanced learning
        return 1.0f;
    } else {
        // High dopamine - exploitation mode, reduced plasticity
        return 0.3f + 0.3f * (1.0f - level);
    }
}

void Dopamine::update(TimestepDuration dt) {
    // TODO PHASE 2: Implement real dopamine dynamics
    // PLACEHOLDER: Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Dopamine::signalReward(float reward) {
    // TODO PHASE 2: Implement real reward signaling
    // PLACEHOLDER: Burst of dopamine on reward
    
    // Real implementation: More sophisticated dopamine dynamics:
    // - Reward prediction error: dopamine encodes the difference between
    //   expected and actual reward
    // - Temporal dynamics: dopamine has sustained responses, not just bursts
    // - Context modulation: dopamine responses depend on task context
    // - Individual variability: different dopamine neurons have different response profiles
    
    // Simplified but more realistic implementation
    float rewardPredictionError = reward - pImpl->level;  // Current level represents expected reward
    pImpl->level = std::clamp(pImpl->level + rewardPredictionError * pImpl->releaseRate, 0.0f, pImpl->peak);
}

void Dopamine::signalRewardPredictionError(float error) {
    // TODO PHASE 2: Implement reward prediction error signaling
    // PLACEHOLDER: Dopamine responds to prediction error
    
    // Real implementation: More sophisticated RPE dynamics:
    // - Standard RPE: dopamine increases for positive errors, decreases for negative errors
    // - Temporal difference learning: integrates over time for better learning
    // - Priority signals: more salient errors produce stronger dopamine responses
    // - Meta-learning: RPE can influence learning rates themselves
    
    // Simplified but more realistic implementation with temporal dynamics
    float temporalComponent = pImpl->level * (1.0f - pImpl->decayRate);
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate - temporalComponent);
}

} // namespace nlm
