#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float arousalLevel;  // Modulates arousal and vigilance
    float signalGain;    // Signal detection sensitivity
    
    Impl() 
        : level(0.0f), baseline(0.0f), peak(1.0f),
          decayRate(0.15f), releaseRate(1.2f), 
          arousalLevel(1.0f), signalGain(1.0f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

const char* Norepinephrine::getName() const {
    return "NE";
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, pImpl->peak);
}

float Norepinephrine::getPlasticityFactor() const {
    // Norepinephrine enhances signal detection and arousal
    // Higher NE increases plasticity for attention-related processes
    return 0.4f + 0.6f * pImpl->level * pImpl->signalGain;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Real norepinephrine dynamics with faster decay than dopamine
    float dtFloat = static_cast<float>(dt);
    pImpl->level = pImpl->baseline + (pImpl->level - pImpl->baseline) * std::exp(-pImpl->decayRate * dtFloat);
}

void Norepinephrine::increaseArousal(float levelIncrease) {
    // Increase overall arousal and alertness
    pImpl->level = std::min(pImpl->peak, pImpl->level + levelIncrease);
}

void Norepinephrine::enhanceSignalDetection(float gain) {
    // Improve signal-to-noise ratio and detect weak signals
    pImpl->signalGain = std::clamp(gain, 0.5f, 2.0f);
}

} // namespace nlm