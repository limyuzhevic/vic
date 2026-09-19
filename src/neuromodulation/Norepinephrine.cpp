#include "Norepinephrine.hpp"
#include <algorithm>

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float arousalLevel;
    float stressLevel;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f), arousalLevel(0.0f), stressLevel(0.0f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

const char* Norepinephrine::getName() const {
    return "NE";
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Norepinephrine::getPlasticityFactor() const {
    // Norepinephrine affects arousal and stress
    // Higher arousal increases alertness and plasticity
    // Higher stress can impair plasticity
    float plasticity = 1.0f + 0.3f * pImpl->arousalLevel;
    if (pImpl->stressLevel > 0.5f) {
        plasticity *= (1.0f - (pImpl->stressLevel - 0.5f) * 0.5f);
    }
    return plasticity;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    // Gradual reduction of arousal and stress
    pImpl->arousalLevel = std::max(0.0f, pImpl->arousalLevel - 0.005f * static_cast<float>(dt));
    pImpl->stressLevel = std::max(0.0f, pImpl->stressLevel - 0.003f * static_cast<float>(dt));
}

void Norepinephrine::increaseAlertness(float amount) {
    // Increase arousal and alertness
    pImpl->arousalLevel = std::min(1.0f, pImpl->arousalLevel + amount);
    pImpl->level = std::min(pImpl->peak, pImpl->level + amount * 0.5f);
}

void Norepinephrine::decreaseStress(float amount) {
    // Reduce stress level
    pImpl->stressLevel = std::max(0.0f, pImpl->stressLevel - amount);
}

float Norepinephrine::getArousalLevel() const {
    return pImpl->arousalLevel;
}

float Norepinephrine::getStressLevel() const {
    return pImpl->stressLevel;
}

void Norepinephrine::reset() {
    pImpl->level = 0.0f;
    pImpl->arousalLevel = 0.0f;
    pImpl->stressLevel = 0.0f;
}

} // namespace nlm
