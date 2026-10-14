#include "Serotonin.hpp"
#include <algorithm>

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baseline;
    float moodStrength;
    float socialBehaviorStrength;
    float impulseControlStrength;
    float decayRate;
    float releaseRate;
    
    Impl() : level(0.0f), baseline(0.0f), moodStrength(1.0f), 
             socialBehaviorStrength(1.0f), impulseControlStrength(1.0f), 
             decayRate(0.1f), releaseRate(1.0f) {}
};

Serotonin::Serotonin() : pImpl(std::make_unique<Impl>()) {}

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
    // Serotonin modulates mood and social behavior, affecting learning
    return 1.0f + 0.2f * pImpl->moodStrength + 0.3f * pImpl->socialBehaviorStrength;
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::improveMood(float strength) {
    pImpl->moodStrength = std::min(1.0f, pImpl->moodStrength + strength);
}

void Serotonin::facilitateSocialBehavior(float strength) {
    pImpl->socialBehaviorStrength = std::min(1.0f, pImpl->socialBehaviorStrength + strength);
}

void Serotonin::enhanceImpulseControl(float strength) {
    pImpl->impulseControlStrength = std::min(1.0f, pImpl->impulseControlStrength + strength);
}

} // namespace nlm