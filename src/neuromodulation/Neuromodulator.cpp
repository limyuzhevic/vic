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
    // Real dopamine-modulated plasticity: Higher dopamine increases plasticity
    // Dopamine signals prediction errors and reward, scaling learning rates accordingly
    return 0.5f + 0.5f * pImpl->level;
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics: Decay towards baseline, with bursts on reward signals
    // Models biological dopamine neuron dynamics with phasic and tonic components
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Dopamine::signalReward(float reward) {
    // Real reward signaling: Phasic dopamine burst proportional to reward prediction error
    // Models midbrain dopamine neuron responses to unexpected rewards
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Real reward prediction error signaling: Dopamine responds to prediction errors
    // Implements Rescorla-Wagner style prediction error signaling
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate);
}

} // namespace nlm
