#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Serotonin::Impl {
    class Brain* brain;
    float level;
    float moodLevel;
    float impulsivityLevel;
    float socialBehaviorLevel;
    float baseline;
    float decayRate;
    float moodBoostRate;
    float impulseControlRate;
    float sociabilityRate;
    
    Impl()
        : brain(nullptr)
        , level(0.0f)
        , moodLevel(0.5f)
        , impulsivityLevel(0.5f)
        , socialBehaviorLevel(0.5f)
        , baseline(0.1f)
        , decayRate(0.05f)
        , moodBoostRate(0.5f)
        , impulseControlRate(0.5f)
        , sociabilityRate(0.5f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

void Serotonin::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Serotonin system initialized");
}

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Smooth mood and related states based on level
    pImpl->moodLevel = 0.3f + 0.7f * pImpl->level;
    pImpl->impulsivityLevel = 0.7f - 0.5f * pImpl->level; // Inverse: higher serotonin reduces impulsivity
    pImpl->socialBehaviorLevel = 0.4f + 0.6f * pImpl->level;
}

void Serotonin::enhanceMood(float positivity) {
    pImpl->moodLevel = std::min(1.0f, pImpl->moodLevel + positivity * pImpl->moodBoostRate);
    pImpl->level = std::min(1.0f, pImpl->level + positivity * 0.2f);
}

void Serotonin::reduceImpulsivity(float control) {
    pImpl->impulsivityLevel = std::max(0.0f, pImpl->impulsivityLevel - control * pImpl->impulseControlRate);
    pImpl->level = std::min(1.0f, pImpl->level + control * 0.15f);
}

void Serotonin::modulateSocialBehavior(float sociability) {
    pImpl->socialBehaviorLevel = std::clamp(sociability, 0.0f, 1.0f);
    pImpl->level = std::min(1.0f, pImpl->level + sociability * 0.25f);
}

float Serotonin::getMoodLevel() const {
    return pImpl->moodLevel;
}

float Serotonin::getImpulsivityLevel() const {
    return pImpl->impulsivityLevel;
}

float Serotonin::getSocialBehaviorLevel() const {
    return pImpl->socialBehaviorLevel;
}

void Serotonin::reset() {
    pImpl->level = 0.0f;
    pImpl->moodLevel = 0.5f;
    pImpl->impulsivityLevel = 0.5f;
    pImpl->socialBehaviorLevel = 0.5f;
}

} // namespace nlm