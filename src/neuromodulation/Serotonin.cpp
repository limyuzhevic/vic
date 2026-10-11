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
    
    // Mood level
    float moodLevel;
    
    // Impulse control
    float impulseControl;
    
    // Social behavior
    float socialBehavior;
    
    // Impulsivity
    float impulsivity;
    
    Impl() 
        : level(0.0f)
        , baseline(0.0f)
        , peak(1.0f)
        , decayRate(0.1f)
        , releaseRate(1.0f)
        , moodLevel(0.5f)
        , impulseControl(0.5f)
        , socialBehavior(0.5f)
        , impulsivity(0.5f) {}
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
    // Serotonin effects: modulates mood, affect, and impulse control
    // Higher serotonin generally stabilizes learning
    float moodEffect = pImpl->moodLevel * 0.4f;
    float impulseEffect = (1.0f - pImpl->impulseControl) * 0.3f;
    float socialEffect = pImpl->socialBehavior * 0.3f;
    
    // Low serotonin can lead to unstable plasticity (impulsivity)
    if (pImpl->impulseControl < 0.3f) {
        return 0.5f + moodEffect + socialEffect - 0.4f * (1.0f - pImpl->impulseControl);
    }
    
    return 0.5f + moodEffect + impulseEffect + socialEffect;
}

void Serotonin::update(TimestepDuration dt) {
    // Serotonin dynamics: decay towards baseline, respond to rewards/social interactions
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update derived effects
    pImpl->impulseControl = std::clamp(pImpl->level * 0.7f + 0.3f * pImpl->impulseControl, 0.0f, 1.0f);
    pImpl->socialBehavior = std::clamp(pImpl->level * 0.6f + 0.4f * pImpl->socialBehavior, 0.0f, 1.0f);
}

float Serotonin::getMoodLevel() const {
    return pImpl->moodLevel;
}

void Serotonin::setMoodLevel(float mood) {
    pImpl->moodLevel = std::clamp(mood, 0.0f, 1.0f);
}

float Serotonin::getImpulseControl() const {
    return pImpl->impulseControl;
}

void Serotonin::setImpulseControl(float control) {
    pImpl->impulseControl = std::clamp(control, 0.0f, 1.0f);
}

float Serotonin::getSocialBehavior() const {
    return pImpl->socialBehavior;
}

void Serotonin::setSocialBehavior(float social) {
    pImpl->socialBehavior = std::clamp(social, 0.0f, 1.0f);
}

} // namespace nlm
