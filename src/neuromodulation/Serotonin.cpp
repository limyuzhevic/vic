#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float plasticityModulator;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f), plasticityModulator(1.0f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

const char* Serotonin::getName() const {
    return "5-HT";
}

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin modulates plasticity - decreases learning rate
    // Higher serotonin reduces plasticity (stabilizes memory)
    return 1.0f - pImpl->level * 0.5f;
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::signalReward(float reward) {
    // Serotonin responds to reward prediction error (negative for unexpected reward)
    // Reduces level on positive reward prediction error
    pImpl->level = std::max(0.0f, pImpl->level - reward * pImpl->releaseRate * 0.5f);
}

void Serotonin::signalRewardPredictionError(float error) {
    // Serotonin responds to reward prediction error
    // Increases level on negative prediction error (unexpected bad outcome)
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate * 0.5f);
}

void Serotonin::signalAversion(float stimulus) {
    // Serotonin responds to aversive stimuli
    // Increases level for aversive events
    pImpl->level = std::min(pImpl->peak, pImpl->level + stimulus * pImpl->releaseRate);
}

} // namespace nlm