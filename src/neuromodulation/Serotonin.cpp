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
    float impulsivityLevel;
    float socialBehaviorLevel;
    
    Impl() 
        : level(0.0f)
        , baseline(0.05f)
        , peak(1.0f)
        , decayRate(0.02f)
        , releaseRate(1.0f)
        , moodLevel(0.0f)
        , impulsivityLevel(0.0f)
        , socialBehaviorLevel(0.0f) {}
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
    // Serotonin modulates mood, impulsivity, and social behavior
    float baseFactor = 0.5f + 0.5f * pImpl->level;
    
    // Apply mood, impulsivity, and social effects
    float totalFactor = baseFactor * (1.0f + pImpl->moodLevel + pImpl->impulsivityLevel + pImpl->socialBehaviorLevel);
    return std::clamp(totalFactor, 0.2f, 2.0f);
}

void Serotonin::update(TimestepDuration dt) {
    // Serotonin dynamics
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Modulate mood, impulsivity, and social behavior based on level
    pImpl->moodLevel = std::min(1.0f, pImpl->level * 1.5f);  // Mood improves with 5-HT
    pImpl->impulsivityLevel = std::max(0.0f, (pImpl->level - 0.7f) * 2.0f);  // Impulsivity increases when 5-HT is high
    pImpl->socialBehaviorLevel = std::min(1.0f, pImpl->level * 1.2f);  // Social behavior improves with 5-HT
}

void Serotonin::signalMood(float moodStrength) {
    // 5-HT increases with positive mood
    pImpl->level = std::min(pImpl->peak, pImpl->level + moodStrength * pImpl->releaseRate);
    NLM_LOG_INFO("Serotonin: Mood signal increased level to " + std::to_string(pImpl->level));
}

void Serotonin::signalImpulsivity(float impulsivityStrength) {
    // 5-HT increases with impulsivity (negative feedback)
    pImpl->level = std::min(pImpl->peak, pImpl->level + impulsivityStrength * pImpl->releaseRate * 0.8f);
    NLM_LOG_INFO("Serotonin: Impulsivity signal increased level to " + std::to_string(pImpl->level));
}

void Serotonin::signalSocialBehavior(float socialStrength) {
    // 5-HT increases with social behavior
    pImpl->level = std::min(pImpl->peak, pImpl->level + socialStrength * pImpl->releaseRate * 1.2f);
    NLM_LOG_INFO("Serotonin: Social behavior signal increased level to " + std::to_string(pImpl->level));
}

} // namespace nlm
