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
    // Real dopamine-modulated plasticity factor: 
    // Baseline plasticity is 0.2, scaled by dopamine level to range 0.2-1.0
    return 0.2f + 0.8f * pImpl->level;
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics with exponential decay
    float dtFloat = static_cast<float>(dt);
    pImpl->level = pImpl->baseline + (pImpl->level - pImpl->baseline) * std::exp(-pImpl->decayRate * dtFloat);
}

void Dopamine::signalReward(float reward) {
    // Real reward signaling with phasic response
    // Pulse magnitude depends on reward magnitude
    float pulse = std::clamp(reward * pImpl->releaseRate, -pImpl->peak, pImpl->peak);
    pImpl->level = pImpl->baseline + (pImpl->level - pImpl->baseline) * 0.2f + pulse * 0.8f;
}

void Dopamine::signalRewardPredictionError(float error) {
    // Real reward prediction error signaling: predicts reward - expected reward
    // Positive error (better than expected) increases dopamine, negative decreases it
    float pulse = error * pImpl->releaseRate;
    pImpl->level = pImpl->baseline + (pImpl->level - pImpl->baseline) * 0.3f + pulse * 0.7f;
    // Keep within reasonable bounds
    pImpl->level = std::clamp(pImpl->level, 0.0f, pImpl->peak);
}

} // namespace nlm
