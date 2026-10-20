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
    float arousalLevel;
    float locusCoeruleusInput;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.03f), releaseRate(1.2f),
             arousalLevel(1.0f), locusCoeruleusInput(0.0f) {}
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
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Norepinephrine::getPlasticityFactor() const {
    // NE modulates arousal-dependent plasticity
    float arousalModulation = 1.0f + pImpl->level * 0.5f;  // Enhanced arousal increases plasticity
    return arousalModulation;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay with slower rate than DA
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::signalArousal(float arousal) {
    // NE signals changes in arousal and alertness
    if (arousal > 0.0f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + arousal * pImpl->releaseRate);
    }
}

void Norepinephrine::signalStress(float stressLevel) {
    // Stress increases NE release (fight-or-flight response)
    pImpl->level = std::min(pImpl->peak, pImpl->level + stressLevel * pImpl->releaseRate * 1.5f);
}

float Norepinephrine::getArousalLevel() const {
    return pImpl->arousalLevel + pImpl->level;
}

float Norepinephrine::getFocusWidth() const {
    // High NE = focused attention, low NE = broad attention
    return 1.0f / (1.0f + pImpl->level * 3.0f);
}

} // namespace nlm
