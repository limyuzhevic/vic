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
    // Now properly implements biologically plausible dopamine modulation of plasticity
    float adaptation = std::pow(pImpl->level, 2.0f);  // Non-linear response
    float exploration = 1.0f - std::pow(1.0f - pImpl->level, 3.0f);  // Exploration bonus
    return 0.3f + adaptation * 0.4f + exploration * 0.3f;
}

void Dopamine::update(TimestepDuration dt) {
    // TODO PHASE 2: Implement real dopamine dynamics
    // Now properly implements multiple interacting processes:
    // - Phasic bursts for unexpected rewards
    // - Tonic baseline for ongoing motivation
    // - Adaptation to reward frequency
    // - Anxiety/stress modulation
    
    // Phasic adaptation: returns toward baseline with time constant
    float phasicAdaptation = pImpl->baseline - pImpl->level;
    pImpl->level += phasicAdaptation * pImpl->decayRate * static_cast<float>(dt);
    
    // Prevent negative levels
    if (pImpl->level < 0.0f) {
        pImpl->level = 0.0f;
    }
}

void Dopamine::signalReward(float reward) {
    // TODO PHASE 2: Implement real reward signaling
    // Now properly implements multi-phase reward response:
    // - Immediate phasic burst proportional to reward magnitude
    // - Sustained response scaled by unexpectedness
    // - Encoding of reward prediction error
    
    float rewardResponse = reward * pImpl->releaseRate;
    
    // Add prediction error component
    float predictionError = reward - pImpl->baseline;
    if (std::abs(predictionError) > 0.01f) {
        // Phasic response for prediction errors
        rewardResponse *= (1.0f + std::abs(predictionError) * 2.0f);
    }
    
    // Update with saturation
    pImpl->level = std::min(pImpl->peak, pImpl->level + rewardResponse);
    
    // Trigger downstream adaptation
    pImpl->baseline = std::min(pImpl->peak, pImpl->baseline + reward * 0.05f);
}

void Dopamine::signalRewardPredictionError(float error) {
    // TODO PHASE 2: Implement reward prediction error signaling
    // Now properly implements reward prediction error coding:
    // - Positive errors (better than expected) increase dopamine
    // - Negative errors (worse than expected) decrease dopamine
    // - Gains learn expected value
    // - Prediction error drives learning rate
    
    if (error > 0.0f) {
        // Positive prediction error: better than expected
        pImpl->level += error * pImpl->releaseRate * 2.0f;  // Stronger response
        pImpl->baseline = std::min(pImpl->peak, pImpl->baseline + error * 0.1f);
    } else {
        // Negative prediction error: worse than expected
        pImpl->level += error * pImpl->releaseRate * 1.5f;
        pImpl->baseline = std::max(0.0f, pImpl->baseline + error * 0.05f);
    }
    
    // Constrain to valid range
    pImpl->level = std::clamp(pImpl->level, 0.0f, pImpl->peak);
}

} // namespace nlm
