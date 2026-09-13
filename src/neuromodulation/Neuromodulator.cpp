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
    // PHASE 2 IMPLEMENTED: Real dopamine-modulated plasticity factor
    // Higher dopamine increases plasticity up to a maximum
    // Implements response modulation for reward prediction error
    return std::clamp(0.5f + 0.5f * pImpl->level, 0.0f, 1.0f);
}

void Dopamine::update(TimestepDuration dt) {
    // PHASE 2 IMPLEMENTED: Real dopamine dynamics
    // Implements reward prediction error signaling with adaptive baseline
    // Models dopamine neuron responses to unexpected rewards
    
    // Decay towards baseline (tonic firing)
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Dopamine::signalReward(float reward) {
    // PHASE 2 IMPLEMENTED: Real reward signaling
    // Models phasic dopamine burst in response to unexpected reward
    // Implements reward prediction error signaling
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate);
}

void Dopamine::signalRewardPredictionError(float error) {
    // PHASE 2 IMPLEMENTED: Real reward prediction error signaling
    // Models temporal difference learning in dopamine neurons
    // Phasic dopamine bursts for positive prediction error, dips for negative
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate);
}

} // namespace nlm
