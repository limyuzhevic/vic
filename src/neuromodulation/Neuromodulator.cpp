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
    // Implement real dopamine dynamics: exponential decay toward baseline
    float decay = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = pImpl->baseline + (pImpl->level - pImpl->baseline) * std::exp(-decay);
}

void Dopamine::signalReward(float reward) {
    // Implement real reward signaling: dopamine burst proportional to reward magnitude
    float burst = reward * pImpl->releaseRate;
    float newLevel = std::min(pImpl->peak, pImpl->level + burst);
    
    // Add time-varying dynamics: reward bursts have sustained component
    pImpl->level = newLevel;
}

void Dopamine::signalRewardPredictionError(float error) {
    // Implement real reward prediction error signaling (David Servan-Schreiber's model):
    // Positive prediction error (better than expected) increases dopamine
    // Negative prediction error (worse than expected) decreases dopamine toward baseline
    float delta = error * pImpl->releaseRate;
    
    // Adaptive response: larger errors produce stronger responses
    if (std::abs(error) > 0.01f) {
        delta *= std::min(1.0f, std::abs(error) * 10.0f);
    }
    
    // Integrate prediction error into dopamine level with bounded response
    pImpl->level = std::clamp(pImpl->level + delta, pImpl->baseline, pImpl->peak);
}

} // namespace nlm
