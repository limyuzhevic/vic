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
    // Higher dopamine increases plasticity up to a maximum
    return 0.5f + 0.5f * pImpl->level;
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics: exponential decay toward baseline
    float decay = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
}

void Dopamine::signalReward(float reward) {
    // Real reward signaling: dopamine burst proportional to reward prediction error
    // Normalize reward to reasonable range
    float normalizedReward = std::clamp(reward / 10.0f, -1.0f, 1.0f);
    
    // Burst of dopamine: positive reward increases level, negative reward decreases it
    float burst = normalizedReward * pImpl->releaseRate;
    pImpl->level = std::min(pImpl->peak, pImpl->level + burst);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Reward prediction error signaling: dopamine responds to prediction errors
    // Positive error (better than expected) increases dopamine
    // Negative error (worse than expected) decreases dopamine
    float pError = std::clamp(error / 10.0f, -1.0f, 1.0f);
    
    float effect = pError * pImpl->releaseRate;
    pImpl->level = std::max(0.0f, pImpl->level + effect);
}

} // namespace nlm
