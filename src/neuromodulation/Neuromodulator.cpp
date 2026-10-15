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
    // Phase 2: Implement real dopamine-modulated plasticity factor
    // Current implementation: Higher dopamine increases plasticity
    // TODO: Make this biologically realistic with different receptor types
    return 0.5f + 0.5f * pImpl->level;
}

void Dopamine::update(TimestepDuration dt) {
    // Phase 2: Implement real dopamine dynamics
    // Current implementation: Decay towards baseline
    // TODO: Implement burst signaling, phasic responses, receptor dynamics
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Dopamine::signalReward(float reward) {
    // Phase 2: Implement real reward signaling
    // Current implementation: Burst of dopamine on reward
    // TODO: Implement more complex reward prediction error signaling
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Phase 2: Implement reward prediction error signaling
    // Current implementation: Dopamine responds to prediction error
    // TODO: Implement real RPE dynamics with temporal difference learning
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate);
}

} // namespace nlm
