#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float rewardHistory;
    float socialSignal;
    float impulsivity;
    
    Impl() : level(0.5f), baseline(0.1f), peak(1.0f), decayRate(0.06f), releaseRate(1.5f),
             rewardHistory(0.0f), socialSignal(0.0f), impulsivity(0.5f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin modulates plasticity based on mood and reward
    float rewardFactor = pImpl->rewardHistory * 0.4f;
    float socialFactor = pImpl->socialSignal * 0.3f;
    float impulsivityFactor = (1.0f - pImpl->impulsivity) * 0.2f;
    
    // High serotonin reduces impulsivity but enhances stable learning
    return 0.5f + 0.3f * pImpl->level + rewardFactor - impulsivityFactor;
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Decay reward history and social signals
    pImpl->rewardHistory *= 0.92f;
    pImpl->socialSignal *= 0.93f;
}

void Serotonin::signalMood(float moodSignal) {
    // Mood signals affect serotonin level
    pImpl->level = std::min(pImpl->peak, pImpl->level + moodSignal * pImpl->releaseRate);
    pImpl->rewardHistory += moodSignal * 0.6f;
}

} // namespace nlm
