// Implementation of Serotonin neuromodulator
#include "Serotonin.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Serotonin::Impl {
    float level;                     // Current 5-HT concentration
    float baseline;                  // Baseline level
    float peak;                      // Peak level
    float decayRate;                 // Rate of decay
    float releaseRate;               // Rate of release
    float moodFactor;                // Effect on mood/impulsivity
    float socialFactor;              // Effect on social behavior
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.03f), releaseRate(0.6f),
             moodFactor(1.0f), socialFactor(1.0f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() {
    delete pImpl;
}

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
    // Serotonin affects mood and impulsivity
    // Higher serotonin = more stable mood = more consistent learning
    float moodFactor = 1.0f - 0.3f * pImpl->level;  // Higher 5-HT = less impulsivity
    float socialFactor = 0.7f + 0.6f * pImpl->level;
    return moodFactor * socialFactor;
}

void Serotonin::update(TimestepDuration dt) {
    // Serotonin has slower dynamics
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::signalReward(float intensity) {
    // Positive reward increases serotonin
    pImpl->level = std::min(pImpl->peak, pImpl->level + intensity * pImpl->releaseRate);
}

void Serotonin::signalSocialEvent(float value) {
    // Social interactions boost serotonin
    pImpl->level = std::min(pImpl->peak, pImpl->level + value * pImpl->socialFactor * 0.5f);
}

} // namespace nlm