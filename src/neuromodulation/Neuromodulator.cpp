#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

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
    // Fix: Implement real dopamine-modulated plasticity factor
    // Higher dopamine increases plasticity up to a maximum
    float plasticityFactor = 0.1f + 0.9f * pImpl->level;  // Range 0.1 to 1.0
    return plasticityFactor;
}

void Dopamine::update(TimestepDuration dt) {
    // Fix: Implement real dopamine dynamics with proper decay
    // Decay towards baseline over time
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Dopamine::signalReward(float reward) {
    // Fix: Implement real reward signaling with burst dynamics
    // Dopamine bursts on unexpected reward
    pImpl->peak = std::min(pImpl->peak + reward * 0.5f, 1.0f);  // Cap the peak
    pImpl->level = pImpl->peak;  // Set to peak for reinforcement
}

void Dopamine::signalRewardPredictionError(float error) {
    // Fix: Implement reward prediction error signaling
    // Dopamine responds to prediction error (unsigned magnitude)
    pImpl->level = std::max(0.0f, pImpl->level + std::abs(error) * pImpl->releaseRate);
}

} // namespace nlm
