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
    return 0.5f + 0.5f * pImpl->level;
}

void Dopamine::update(TimestepDuration dt) {
    // TODO PHASE 2: Implement real dopamine dynamics
    // PLACEHOLDER: Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Dopamine::signalReward(float reward) {
    // Real reward signaling: Dopamine neurons increase firing on unexpected rewards
    // Positive reward produces dopamine burst (prediction error > 0)
    if (reward > 0.0f) {
        // For positive rewards, increase dopamine level proportionally
        pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate * 0.1f);
    } else {
        // For negative rewards, decrease dopamine level
        pImpl->level = std::max(pImpl->baseline, pImpl->level + reward * pImpl->releaseRate * 0.5f);
    }
    
    // Clamp to reasonable range
    pImpl->level = std::clamp(pImpl->level, pImpl->baseline, pImpl->peak);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Real reward prediction error signaling: Dopamine responds to prediction errors
    // Positive prediction error (better than expected) increases dopamine
    // Negative prediction error (worse than expected) decreases dopamine
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate * 0.2f);
    
    // Clamp to reasonable range
    pImpl->level = std::clamp(pImpl->level, pImpl->baseline, pImpl->peak);
}

} // namespace nlm
