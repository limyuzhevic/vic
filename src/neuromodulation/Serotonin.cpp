#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float moodRegulation;
    float impulseControl;
    float socialLearningBoost;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f),
             moodRegulation(1.2f), impulseControl(1.3f), socialLearningBoost(1.4f) {}
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
    // Serotonin regulates mood and impulse control
    return 1.1f + 0.3f * pImpl->level;
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::modulateMood(float level) {
    // Improve mood regulation
    pImpl->level = std::min(pImpl->peak, pImpl->level + level * moodRegulation);
}

void Serotonin::regulateImpulsivity(float factor) {
    // Enhance impulse control
    pImpl->impulseControl = std::max(1.0f, pImpl->impulseControl + factor * 0.5f);
}

void Serotonin::enhanceSocialLearning(float factor) {
    // Boost social learning and cooperative behavior
    pImpl->socialLearningBoost = std::max(1.0f, pImpl->socialLearningBoost + factor * 0.6f);
}

} // namespace nlm
