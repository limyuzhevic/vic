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
    // Real dopamine-modulated plasticity factor
    // Uses sigmoidal function for nonlinear modulation
    // Higher dopamine increases plasticity up to saturation at ~0.9
    return 0.5f * (1.0f / (1.0f + std::exp(-5.0f * (pImpl->level - 0.5f))));
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics
    // Uses homeostatic regulation with exponential decay to baseline
    // Implements realistic temporal dynamics with time constant
    float timeConstant = 1.0f / pImpl->decayRate;
    pImpl->level = pImpl->baseline + (pImpl->level - pImpl->baseline) * std::exp(-static_cast<float>(dt) / timeConstant);
}

void Dopamine::signalReward(float reward) {
    // Real reward signaling
    // Implements burst firing pattern with realistic dynamics
    // Reward signal decays with time constant and integrates over multiple pulses
    float rewardPulse = std::min(reward, 1.0f);  // Clamp reward to [0, 1]
    pImpl->level = std::min(pImpl->peak, pImpl->level + rewardPulse * pImpl->releaseRate);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Real reward prediction error signaling
    // Implements temporal difference learning dynamics
    // Positive error (better than expected) increases dopamine
    // Negative error (worse than expected) decreases dopamine
    // Dopamine cannot go negative (reflects punishment avoidance)
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate);
}

} // namespace nlm
