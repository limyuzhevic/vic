#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Social/mood state
    float socialCohesion;
    float moodState;
    float impulsivity;
    
    Impl() 
        : level(0.0f)
        , baseline(0.0f)
        , peak(1.0f)
        , decayRate(0.05f)
        , releaseRate(1.0f)
        , socialCohesion(0.0f)
        , moodState(0.0f)
        , impulsivity(0.0f) {}
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
    // Serotonin modulates plasticity for mood-related learning
    return 0.7f + 0.3f * pImpl->level;
}

void Serotonin::update(TimestepDuration dt) {
    // Real 5-HT dynamics
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update mood components
    pImpl->moodState = std::max(0.0f, pImpl->moodState - pImpl->decayRate * static_cast<float>(dt));
    pImpl->impulsivity = std::max(0.0f, pImpl->impulsivity - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::signalSocialMood(float mood) {
    // Signal social mood
    pImpl->moodState = std::min(pImpl->peak, pImpl->moodState + mood);
    pImpl->level = std::min(pImpl->peak, pImpl->level + mood * 0.3f);
    
    // Reduce impulsivity with higher mood
    pImpl->impulsivity = std::max(0.0f, pImpl->impulsivity - mood * 0.5f);
}

} // namespace nlm
