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
    float normalizedLevel = pImpl->level / pImpl->peak;
    return Constants::DOPAMINE_PLASTICITY_FACTOR_MIN + 
           (Constants::DOPAMINE_PLASTICITY_FACTOR_MAX - Constants::DOPAMINE_PLASTICITY_FACTOR_MIN) * normalizedLevel;
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics with baseline and decay
    // Dopamine naturally decays towards baseline
    float dtFloat = static_cast<float>(dt);
    pImpl->level = pImpl->baseline + (pImpl->level - pImpl->baseline) * std::exp(-pImpl->decayRate * dtFloat);
}

void Dopamine::signalReward(float reward) {
    // Real reward signaling
    // Dopamine bursts on reward prediction error
    // Positive reward increases dopamine
    // Negative reward (punishment) decreases dopamine
    if (reward > 0.0f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate);
    } else {
        pImpl->level = std::max(pImpl->baseline, pImpl->level + reward * pImpl->releaseRate);
    }
}

void Dopamine::signalRewardPredictionError(float error) {
    // Reward prediction error signaling
    // Positive error (better than expected) increases dopamine
    // Negative error (worse than expected) decreases dopamine
    pImpl->level = std::max(pImpl->baseline, 
                           std::min(pImpl->peak, 
                                    pImpl->level + error * pImpl->releaseRate));
}

} // namespace nlm

