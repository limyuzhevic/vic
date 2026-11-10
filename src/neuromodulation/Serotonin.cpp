#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Serotonin::Impl {
    class Brain* brain;
    float level;
    float moodLevel;
    float impulsivityLevel;
    float socialBehaviorLevel;
    float decayRate;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , moodLevel(1.0f)
        , impulsivityLevel(1.0f)
        , socialBehaviorLevel(1.0f)
        , decayRate(0.05f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

const char* Serotonin::getName() const {
    return "Serotonin";
}

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = level;
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin reduces impulsivity and may stabilize plasticity
    return std::max(0.5f, 1.0f - pImpl->level * 0.3f);
}

void Serotonin::update(TimestepDuration dt) {
    // Decay serotonin level
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::improveMood(float moodBoost) {
    // Improve mood and social behavior
    pImpl->moodLevel = std::min(1.0f, pImpl->moodLevel + moodBoost * 0.1f);
    pImpl->socialBehaviorLevel = std::min(1.0f, pImpl->socialBehaviorLevel + moodBoost * 0.5f);
    pImpl->level = std::min(2.0f, pImpl->level + moodBoost * 0.05f);
}

void Serotonin::reduceImpulsivity() {
    // Reduce impulsive behavior
    pImpl->impulsivityLevel = std::max(0.0f, pImpl->impulsivityLevel - 0.1f);
}

void Serotonin::enhanceSocialBehavior() {
    // Enhance social interactions
    pImpl->socialBehaviorLevel = std::min(1.0f, pImpl->socialBehaviorLevel + 0.1f);
}

} // namespace nlm