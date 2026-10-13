#include "Serotonin.hpp"
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
    float impulseControl;
    float socialStrength;
    float socialLearningRate;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), 
             decayRate(0.1f), releaseRate(1.0f),
             moodLevel(0.5f), impulseControl(0.5f), socialStrength(0.5f), socialLearningRate(0.1f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Serotonin::getPlasticityFactor() const {
    // 5-HT modulates plasticity: high levels reduce impulsivity, enhance social learning
    return 0.7f + 0.3f * pImpl->socialLearningRate;
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline with mood regulation
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update mood and impulse control based on level
    pImpl->moodLevel = 0.3f + 0.7f * pImpl->level;
    pImpl->impulseControl = 0.5f + 0.5f * pImpl->level;
}

void Serotonin::regulateMood(float moodLevel) {
    pImpl->moodLevel = std::clamp(moodLevel, 0.0f, 1.0f);
}

void Serotonin::controlImpulses(float controlStrength) {
    pImpl->impulseControl = std::clamp(controlStrength, 0.0f, 1.0f);
}

void Serotonin::modulateSocialBehavior(float socialStrength) {
    pImpl->socialStrength = std::clamp(socialStrength, 0.0f, 1.0f);
}

void Serotonin::setSocialLearningRate(float rate) {
    pImpl->socialLearningRate = std::clamp(rate, 0.0f, 0.5f);
}

} // namespace nlm