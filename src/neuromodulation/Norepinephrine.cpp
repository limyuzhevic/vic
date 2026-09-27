#include "Norepinephrine.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float arousalModifier;  // How NE affects arousal/vigilance
    float stressResponseModifier;  // How NE affects stress response
    float locusCoeruleusActivation;  // LC neuron firing rate
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.15f),
             releaseRate(1.0f), arousalModifier(1.5f), stressResponseModifier(1.8f),
             locusCoeruleusActivation(0.0f) {}
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
    // Norepinephrine enhances plasticity for arousal and stress response
    float arousalEffect = pImpl->level * pImpl->arousalModifier;
    float stressEffect = pImpl->level * pImpl->stressResponseModifier;
    return 0.5f + arousalEffect * 0.5f + stressEffect * 0.5f;  // Modulated plasticity
}

void Norepinephrine::update(TimestepDuration dt) {
    // NE decays towards baseline but is released during arousal/stress
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // LC firing rate tracks NE level
    pImpl->locusCoeruleusActivation = getLevel();
}

void Norepinephrine::signalArousal(float arousalLevel) {
    // NE is released during arousal, novelty, or stress
    setLevel(std::min(pImpl->peak, pImpl->level + arousalLevel * pImpl->releaseRate));
}

void Norepinephrine::signalStress(float stressLevel) {
    // NE increases significantly during stress responses
    float stressBoost = stressLevel * 2.0f;  // Stress has stronger effect
    setLevel(std::min(pImpl->peak, getLevel() + stressBoost * pImpl->releaseRate));
}

float Norepinephrine::getArousalEffect() const {
    return pImpl->arousalModifier * getLevel();
}

float Norepinephrine::getStressResponseEffect() const {
    return pImpl->stressResponseModifier * getLevel();
}

float Norepinephrine::getLocusCoeruleusActivity() const {
    return pImpl->locusCoeruleusActivation;
}

} // namespace nlm
