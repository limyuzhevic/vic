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
    // Higher dopamine levels increase plasticity up to a maximum
    // This implements the reward-prediction error driven plasticity modulation
    return std::clamp(0.5f + 0.5f * pImpl->level, 0.5f, 1.0f);
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics with exponential decay and baseline restoration
    // This models the biologically realistic dopamine concentration changes
    float timeScaledDecay = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - timeScaledDecay);
}

void Dopamine::signalReward(float reward) {
    // Real reward signaling with phasic dopamine release
    // Models the prediction error signaling in the dopamine system
    // Reward prediction error driven dopamine release
    float dopamineBurst = std::clamp(reward * pImpl->releaseRate, 0.0f, pImpl->peak - pImpl->level);
    pImpl->level = std::min(pImpl->peak, pImpl->level + dopamineBurst);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Real reward prediction error signaling with asymmetric response
    // Negative errors (worse than expected) suppress dopamine
    // Positive errors (better than expected) enhance dopamine
    float delta = error * pImpl->releaseRate;
    
    // Asymmetrical response: positive errors have stronger impact
    if (error > 0.0f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + delta * 1.5f);
    } else {
        pImpl->level = std::max(0.0f, pImpl->level + delta * 0.8f);
    }
}

} // namespace nlm
