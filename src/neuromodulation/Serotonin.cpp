#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float socialInfluence;
    float moodLevel;
    float impulsivityLevel;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.06f),
             socialInfluence(0.0f), moodLevel(0.0f), impulsivityLevel(0.0f) {}
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
    // Serotonin modulates social behavior and impulse control
    return 1.0f - pImpl->level * 0.2f; // High serotonin reduces plasticity
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Modulate mood and impulsivity
    pImpl->moodLevel = pImpl->level * 0.7f;
    pImpl->impulsivityLevel = (1.0f - pImpl->level) * 0.8f;
}

void Serotonin::signalMood(float moodStrength) {
    setLevel(std::min(pImpl->peak, getLevel() + moodStrength * 0.4f));
    moodLevel = pImpl->level * 0.7f;
}

void Serotonin::signalSocial(const std::vector<float>& socialInputs) {
    float socialSum = 0.0f;
    for (float input : socialInputs) socialSum += input;
    setLevel(std::min(pImpl->peak, getLevel() + socialSum * 0.3f / socialInputs.size()));
    socialInfluence = pImpl->level * 0.7f;
}

void Serotonin::signalImpulsivity(float impulsivityStrength) {
    setLevel(std::min(pImpl->peak, getLevel() + impulsivityStrength * 0.3f));
    impulsivityLevel = (1.0f - pImpl->level) * 0.8f;
}

void Serotonin::signalImpulseControl(float controlStrength) {
    setLevel(std::max(pImpl->baseline, getLevel() - controlStrength * 0.7f));
    impulsivityLevel = std::max(0.0f, impulsivityLevel - controlStrength * 0.5f);
}

float Serotonin::getSocialModulation() const {
    return pImpl->socialInfluence;
}

float Serotonin::getImpulseControl() const {
    return 1.0f - pImpl->impulsivityLevel;
}

} // namespace nlm