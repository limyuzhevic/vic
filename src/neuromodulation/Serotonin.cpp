#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct Serotonin::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float moodLevel;
    float impulsivityModulator;
    float socialBehaviorModulator;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.1f)
        , peak(1.0f)
        , decayRate(0.04f)
        , releaseRate(0.7f)
        , moodLevel(0.0f)
        , impulsivityModulator(0.0f)
        , socialBehaviorModulator(0.0f) {}
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
    // Serotonin has complex effects on plasticity
    // Optimal serotonin promotes stable learning and memory consolidation
    // Low serotonin increases plasticity but may reduce stability
    return 1.0f - 0.5f * std::abs(pImpl->level - 0.3f);
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::signalMood(float mood) {
    // Mood affects serotonin level
    pImpl->moodLevel = std::clamp(mood, 0.0f, 1.0f);
    
    // Update serotonin based on mood
    if (mood > pImpl->level) {
        setLevel(std::min(pImpl->peak, pImpl->level + mood * pImpl->releaseRate * 0.6f));
    }
}

void Serotonin::signalSocialBehavior(float socialCohesion) {
    // Social behavior increases serotonin
    pImpl->socialBehaviorModulator = std::clamp(socialCohesion, 0.0f, 1.0f);
    
    if (socialCohesion > 0.0f) {
        setLevel(std::min(pImpl->peak, pImpl->level + socialCohesion * pImpl->releaseRate * 0.3f));
    }
}

float Serotonin::getMoodLevel() const {
    return pImpl->moodLevel;
}

float Serotonin::getImpulsivityModulation() const {
    // Serotonin reduces impulsivity
    return 1.0f - pImpl->level * 0.5f;
}

float Serotonin::getSocialBehaviorModulation() const {
    return pImpl->socialBehaviorModulator;
}

void Serotonin::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Serotonin (mood/social behavior) system initialized");
}

} // namespace nlm