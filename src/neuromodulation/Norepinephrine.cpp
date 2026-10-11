#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Arousal and vigilance modulation
    float arousalStrength;
    float signalDetectionStrength;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f),
             arousalStrength(1.0f), signalDetectionStrength(1.0f) {}
};

Norepinephrine::Norepinephrine() : pImpl(std::make_unique<Impl>()) {}

Norepinephrine::~Norepinephrine() = default;

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Norepinephrine::getPlasticityFactor() const {
    return 0.3f + 0.7f * pImpl->level;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::increaseArousal(float arousalStrength) {
    pImpl->arousalStrength = arousalStrength;
}

void Norepinephrine::enhanceSignalDetection(float enhancement) {
    pImpl->signalDetectionStrength = enhancement;
}

} // namespace nlm