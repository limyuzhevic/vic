#include "Serotonin.hpp"
#include <algorithm>

namespace nlm {

struct Serotonin::Impl {
    float level;
    float mood;
    float impulsivity;
    float socialBehavior;
    float decayRate;
    
    Impl() : level(0.0f), mood(0.5f), impulsivity(0.5f), socialBehavior(0.5f), decayRate(0.07f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

float Serotonin::getPlasticityFactor() const {
    // 5-HT affects exploration/exploitation balance
    return 1.0f - std::abs(mood - 0.5f) * 0.5f;
}

void Serotonin::update(TimestepDuration dt) {
    // Mood dynamics
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::signalMood(float moodLevel) {
    // Signal mood for behavior modulation
    pImpl->level = std::clamp(moodLevel, 0.0f, 1.0f);
    pImpl->mood = pImpl->level;
    
    // High serotonin reduces impulsivity
    pImpl->impulsivity = 1.0f - pImpl->level * 0.8f;
    
    // Low serotonin increases social behavior (empirically observed)
    pImpl->socialBehavior = pImpl->level * 0.5f + 0.3f;
}

} // namespace nlm
