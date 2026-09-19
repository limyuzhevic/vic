#include "Serotonin.hpp"
#include <algorithm>

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float moodLevel;
    float impulsivity;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f), moodLevel(0.5f), impulsivity(0.5f) {}
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
    // Serotonin regulates mood and impulsivity
    // Higher mood enhances plasticity and learning
    // Lower impulsivity improves decision-making
    float plasticity = 1.0f + 0.3f * pImpl->moodLevel;
    plasticity *= (1.0f - 0.5f * pImpl->impulsivity);
    return std::max(0.0f, plasticity);
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    // Gradual reduction of changes
    pImpl->moodLevel = std::max(0.0f, pImpl->moodLevel - 0.01f * static_cast<float>(dt));
    pImpl->impulsivity = std::max(0.0f, pImpl->impulsivity - 0.005f * static_cast<float>(dt));
}

void Serotonin::improveMood(float amount) {
    // Improve mood state
    pImpl->moodLevel = std::min(1.0f, pImpl->moodLevel + amount);
    pImpl->level = std::min(pImpl->peak, pImpl->level + amount * 0.5f);
}

void Serotonin::reduceImpulsivity(float amount) {
    // Reduce impulsivity level
    pImpl->impulsivity = std::max(0.0f, pImpl->impulsivity - amount);
}

float Serotonin::getMoodLevel() const {
    return pImpl->moodLevel;
}

float Serotonin::getImpulsivity() const {
    return pImpl->impulsivity;
}

void Serotonin::reset() {
    pImpl->level = 0.0f;
    pImpl->moodLevel = 0.5f;
    pImpl->impulsivity = 0.5f;
}

} // namespace nlm
