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
    float moodLevel;
    float impulsivity;
    float socialBehavior;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.08f), releaseRate(0.6f),
             moodLevel(1.0f), impulsivity(1.0f), socialBehavior(1.0f) {}
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
    // Serotonin stabilizes neural circuits and reduces learning rate
    return 1.0f - pImpl->level * 0.4f;
}

void Serotonin::update(TimestepDuration dt) {
    // Faster decay than DA or NE
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::signalMoodChange(float moodDelta) {
    // Serotonin signals mood changes and social evaluation
    if (std::abs(moodDelta) > 0.0f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + moodDelta * pImpl->releaseRate);
    }
}

void Serotonin::signalSocialFeedback(float socialValue) {
    // Social reward/punishment affects serotonin
    pImpl->level = std::min(pImpl->peak, pImpl->level + socialValue * pImpl->releaseRate * 0.8f);
}

float Serotonin::getMoodLevel() const {
    return pImpl->moodLevel + pImpl->level;
}

float Serotonin::getImpulseControl() const {
    // High serotonin = better impulse control
    return 1.0f - pImpl->level * 0.7f;
}

float Serotonin::getSocialBehaviorStrength() const {
    // High serotonin = more social behavior
    return 1.0f + pImpl->level * 0.5f;
}

} // namespace nlm
