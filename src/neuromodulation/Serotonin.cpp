#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float moodStrength;
    float socialLevel;
    float wellBeing;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.08f),
             moodStrength(1.0f), socialLevel(0.5f), wellBeing(0.5f) {}
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
    // Serotonin promotes stability and social behavior
    return 0.3f + 0.7f * pImpl->level;
}

void Serotonin::update(TimestepDuration dt) {
    // Serotonin dynamics: gradual decay, promotes emotional stability
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    pImpl->wellBeing = std::min(1.0f, pImpl->wellBeing + (pImpl->level - pImpl->wellBeing) * 0.05f);
}

void Serotonin::signalMood(float moodSignal) {
    // Positive experiences trigger serotonin release for improved mood
    pImpl->level = std::min(pImpl->peak, pImpl->level + moodSignal * pImpl->moodStrength);
}

void Serotonin::modulateSocialBehavior(float socialLevel) {
    // Modulate social behavior engagement
    pImpl->socialLevel = std::clamp(socialLevel, 0.0f, 1.0f);
}

float Serotonin::getWellBeing() const {
    return pImpl->wellBeing;
}

} // namespace nlm