#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Serotonin::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float moodEffect;
    float impulsivityMod;
    float socialBehaviorMod;
    float decayRate;
    float tonicLevel;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.0f)
        , moodEffect(0.0f)
        , impulsivityMod(1.0f)
        , socialBehaviorMod(0.5f)
        , decayRate(0.05f)
        , tonicLevel(0.2f) {}
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
    // Serotonin modulates mood and impulsivity
    // Affects learning strategy and behavioral flexibility
    float impulsivity = pImpl->level * (2.0f - pImpl->impulsivityMod);
    float stability = 0.5f + 0.5f * (1.0f - pImpl->impulsivityMod);
    return 0.3f * stability + 0.7f * impulsivity;
}

void Serotonin::update(TimestepDuration dt) {
    // Serotonin dynamics: oscillates around baseline
    float oscillation = std::sin(pImpl->level * 6.28f);  // ~1 Hz oscillation
    pImpl->level = pImpl->baseline + oscillation * pImpl->tonicLevel;
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
}

void Serotonin::signalMood(float mood) {
    // Signal mood change (positive emotions, well-being)
    if (mood > 0.0f) {
        pImpl->level = std::min(1.0f, pImpl->level + mood * 0.3f);
    } else {
        pImpl->level = std::max(0.0f, pImpl->level + mood * 0.2f);
    }
}

void Serotonin::signalSocial(float socialStrength) {
    // Signal social behavior modulation
    pImpl->level = std::min(1.0f, pImpl->level + socialStrength * pImpl->socialBehaviorMod);
}

float Serotonin::getMoodEffect() const {
    return pImpl->moodEffect;
}

float Serotonin::getImpulsivityMod() const {
    return pImpl->impulsivityMod;
}

float Serotonin::getSocialBehaviorMod() const {
    return pImpl->socialBehaviorMod;
}

void Serotonin::setImpulsivityMod(float mod) {
    pImpl->impulsivityMod = std::clamp(mod, 0.1f, 2.0f);
}

void Serotonin::setSocialBehaviorMod(float mod) {
    pImpl->socialBehaviorMod = std::clamp(mod, 0.1f, 2.0f);
}

} // namespace nlm