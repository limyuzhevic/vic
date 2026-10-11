#include "Norepinephrine.hpp"
#include <algorithm>

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float arousalLevel;
    float sensoryEnhancement;
    float focusLevel;
    float decayRate;
    float releaseRate;
    
    Impl() : level(0.0f), baseline(0.0f), arousalLevel(1.0f), 
             sensoryEnhancement(1.0f), focusLevel(0.5f), 
             decayRate(0.05f), releaseRate(1.0f) {}
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
    // Norepinephrine increases plasticity for alertness and focus
    float arousalMod = 1.0f + pImpl->arousalLevel * 0.3f;
    float focusMod = 1.0f + pImpl->focusLevel * 0.2f;
    float sensoryMod = 1.0f + pImpl->sensoryEnhancement * 0.2f;
    return std::min(2.0f, arousalMod * focusMod * sensoryMod);
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::initialize(class Brain* brain) {
    (void)brain; // Not used currently
}

void Norepinephrine::reset() {
    pImpl->level = 0.0f;
}

void Norepinephrine::increaseArousal(float level) {
    pImpl->arousalLevel = std::clamp(level, 0.0f, 1.0f);
}

void Norepinephrine::enhanceSensoryProcessing(float enhancement) {
    pImpl->sensoryEnhancement = std::clamp(enhancement, 0.0f, 1.0f);
}

void Norepinephrine::setFocusLevel(float level) {
    pImpl->focusLevel = std::clamp(level, 0.0f, 1.0f);
}

} // namespace nlm