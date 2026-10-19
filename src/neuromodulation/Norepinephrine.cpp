#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float arousalThreshold;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f), arousalThreshold(0.3f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() {
    delete pImpl;
}

const char* Norepinephrine::getName() const {
    return "NE";
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Norepinephrine::getPlasticityFactor() const {
    // NE modulates arousal - higher NE increases plasticity for novel/rewarding events
    // But excessive NE can impair learning (inverted U relationship)
    float arousal = pImpl->level;
    if (arousal < pImpl->arousalThreshold) {
        // Below threshold: arousal enhances plasticity
        return 0.5f + 0.5f * arousal;
    } else {
        // Above threshold: too much arousal impairs learning
        return std::max(0.1f, 1.0f - (arousal - pImpl->arousalThreshold) * 0.5f);
    }
}

void Norepinephrine::update(TimestepDuration dt) {
    // NE decay and environmental influence
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::signalArousal(float arousalStrength) {
    // NE release for arousal and vigilance
    pImpl->level = std::min(pImpl->peak, pImpl->level + arousalStrength * pImpl->releaseRate);
}

void Norepinephrine::signalStress(float stressLevel) {
    // NE release under stress
    pImpl->level = std::min(pImpl->peak, pImpl->level + stressLevel * pImpl->releaseRate * 0.8f);
}

void Norepinephrine::signalReward(float reward) {
    // NE response to unexpected reward (prediction error)
    if (reward > 0.5f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + (reward - 0.5f) * pImpl->releaseRate * 0.6f);
    }
}

} // namespace nlm
