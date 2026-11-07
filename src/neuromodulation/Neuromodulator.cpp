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
    if (reward == 0.0f) return;  // No-op for zero reward
    
    // Implement real reward signaling
    // Dopamine release is proportional to reward prediction error
    // For now, use a simplified model
    float rewardMagnitude = std::abs(reward);
    float rewardSign = reward > 0.0f ? 1.0f : -1.0f;
    
    // Faster rise for larger rewards, but saturate at peak level
    float levelChange = rewardMagnitude * pImpl->releaseRate;
    pImpl->level = std::min(pImpl->peak, pImpl->level + levelChange * rewardSign);
    
    // Decay baseline toward current level for the next prediction
    if (pImpl->level < pImpl->baseline * 0.5f) {
        pImpl->baseline = pImpl->level * 0.9f + pImpl->baseline * 0.1f;
    }
}

void Dopamine::signalRewardPredictionError(float error) {
    // TODO PHASE 2: Implement reward prediction error signaling
    // PLACEHOLDER: Dopamine responds to prediction error
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate);
}

} // namespace nlm
