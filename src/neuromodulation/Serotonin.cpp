#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float mood;
    float sociality;
    float impulsivity;
    
    Impl() 
        : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.05f), releaseRate(1.0f)
        , mood(0.0f), sociality(0.0f), impulsivity(0.5f) {}
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
    // Serotonin modulates mood and social behavior
    // Higher serotonin (serotonergic activity) is associated with improved mood
    // and reduced impulsivity, which can enhance learning
    return 0.5f + 0.3f * pImpl->level;
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    // Mood stabilizes over time
    pImpl->mood = 0.95f * pImpl->mood + 0.05f * pImpl->level;
}

void Serotonin::signalMood(float happiness) {
    // Increase serotonin level for positive experiences
    pImpl->level = std::min(pImpl->peak, pImpl->level + happiness * pImpl->releaseRate);
    pImpl->mood = std::clamp(pImpl->mood + happiness * 0.1f, 0.0f, 1.0f);
}

void Serotonin::modulateSocialBehavior(float sociality) {
    // Enhance social behavior and reduce aggression
    pImpl->sociality = std::clamp(pImpl->sociality + sociality * 0.1f, 0.0f, 1.0f);
    // Reduce impulsivity with higher sociality
    pImpl->impulsivity = 0.8f - 0.5f * pImpl->sociality;
    pImpl->impulsivity = std::clamp(pImpl->impulsivity, 0.1f, 1.0f);
}

float Serotonin::getImpulsivity() const {
    return pImpl->impulsivity;
}

} // namespace nlm
