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
    // Phase 2: Real dopamine-modulated plasticity factor
    // Dopaminergic modulation scales plasticity window
    float modulation = 0.5f + 0.5f * pImpl->level;  // Range: 0.5 - 1.0
    return modulation;
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics: exponential decay to baseline
    float decay = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
}

void Dopamine::signalReward(float reward) {
    // Real reward signaling: phasic burst proportional to reward prediction
    float burst = std::min(1.0f, reward * pImpl->releaseRate);
    pImpl->level = std::min(pImpl->peak, pImpl->level + burst);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Reward prediction error: negative for worse than expected, positive for better
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate);
}

} // namespace nlm
