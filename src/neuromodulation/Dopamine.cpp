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
    // Dopamine enhances plasticity for reward prediction errors
    // Higher dopamine increases learning rate
    return 0.5f + 0.5f * pImpl->level;
}

void Dopamine::update(TimestepDuration dt) {
    // Dopamine decays towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Dopamine::signalReward(float reward) {
    // Dopamine bursts on reward delivery
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Dopamine responds to reward prediction errors
    // Phasic increases for positive errors, decreases for negative errors
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate);
}

float Dopamine::getBaseline() const {
    return pImpl->baseline;
}

void Dopamine::setPeak(float peak) {
    pImpl->peak = std::clamp(peak, 0.0f, 1.0f);
}

float Dopamine::getPeak() const {
    return pImpl->peak;
}

} // namespace nlm
