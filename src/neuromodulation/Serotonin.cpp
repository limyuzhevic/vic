#include "Serotonin.hpp"
#include <algorithm>

namespace nlm {

struct Serotonin::Impl {
    float level;
    float socialLearningBias;
    float moodModulation;
    float impulsivityReduction;
    float baseline;
    float decayRate;
    
    Impl() : level(0.0f), socialLearningBias(0.5f), moodModulation(1.0f), 
             impulsivityReduction(0.9f), baseline(0.0f), decayRate(0.02f) {}
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
    // Serotonin modulates mood and social learning
    return 1.0f + pImpl->moodModulation * pImpl->level * 0.5f;
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::signalSocialInteraction(float intensity) {
    pImpl->socialLearningBias = std::clamp(intensity, 0.0f, 1.0f);
}

void Serotonin::setSocialLearningBias(float bias) {
    pImpl->socialLearningBias = std::clamp(bias, 0.0f, 1.0f);
}

} // namespace nlm