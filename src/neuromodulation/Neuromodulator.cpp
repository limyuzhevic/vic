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
    // Phase 6: Simplified plasticity factor
    // Current implementation: Linear mapping from dopamine level
    // TODO FUTURE PHASE: Implement real dopamine-modulated plasticity factor with sigmoidal response
    return 0.5f + 0.5f * pImpl->level;
}

void Dopamine::update(TimestepDuration dt) {
    // Phase 6: Simplified dopamine dynamics
    // Current implementation: Exponential decay towards baseline
    // TODO FUTURE PHASE: Implement real dopamine dynamics with receptor kinetics
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Dopamine::signalReward(float reward) {
    // Phase 6: Simplified reward signaling
    // Current implementation: Impulse response to reward
    // TODO FUTURE PHASE: Implement real reward signaling with phasic/tonic dynamics
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Phase 6: Simplified prediction error signaling
    // Current implementation: Response to prediction error
    // TODO FUTURE PHASE: Implement real reward prediction error signaling with temporal difference learning
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate);
}

} // namespace nlm
