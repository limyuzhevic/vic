// Norepinephrine: Arousal and vigilance
// TODO PHASE 2

#include "Norepinephrine.hpp"
#include <algorithm>

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float arousalLevel;
    float vigilance;
    float baseline;
    float decayRate;
    float releaseRate;
    
    Impl() : level(0.0f), arousalLevel(1.2f), vigilance(1.8f), 
             baseline(0.0f), decayRate(0.08f), releaseRate(1.0f) {}
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
    // Norepinephrine increases arousal and vigilance
    // Higher NE enhances focus and alertness
    return 1.0f + pImpl->level * (pImpl->arousalLevel - 1.0f) +
           0.2f * pImpl->level * (pImpl->vigilance - 1.0f);
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::signalArousal(float strength) {
    // Boost arousal when experiencing stimulating events
    pImpl->level = std::min(pImpl->level + strength * pImpl->releaseRate, 1.0f);
}

void Norepinephrine::increaseVigilance(float increase) {
    // Increase vigilance/focus
    pImpl->level = std::min(pImpl->level + increase * pImpl->vigilance * pImpl->releaseRate, 1.0f);
}

} // namespace nlm