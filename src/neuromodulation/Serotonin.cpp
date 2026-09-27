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
    float moodModifier;  // How 5-HT affects mood
    float impulsivityModifier;  // How 5-HT affects impulsivity
    float socialBehaviorModifier;  // How 5-HT affects social behavior
    float rapheNucleusActivation;  // DRN neuron firing rate
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.12f),
             releaseRate(1.0f), moodModifier(1.2f), impulsivityModifier(0.8f),
             socialBehaviorModifier(1.3f), rapheNucleusActivation(0.0f) {}
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
    // Serotonin modulates plasticity for mood and social behavior
    float moodEffect = pImpl->level * pImpl->moodModifier;
    float socialEffect = pImpl->level * pImpl->socialBehaviorModifier;
    float impulsivityEffect = pImpl->level * (1.0f - pImpl->impulsivityModifier);  // Lower impulsivity = higher plasticity
    return 0.5f + moodEffect * 0.3f + socialEffect * 0.4f - impulsivityEffect * 0.2f;  // Net plasticity effect
}

void Serotonin::update(TimestepDuration dt) {
    // 5-HT decays towards baseline but is released during positive experiences
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // DRN firing rate tracks 5-HT level
    pImpl->rapheNucleusActivation = getLevel();
}

void Serotonin::signalPositiveExperience(float experienceQuality) {
    // 5-HT is released during positive experiences, social interactions
    setLevel(std::min(pImpl->peak, pImpl->level + experienceQuality * pImpl->releaseRate * 1.5f));
}

void Serotonin::signalSocialBonding(float socialStrength) {
    // 5-HT increases significantly during social bonding
    float socialBoost = socialStrength * 1.5f;
    setLevel(std::min(pImpl->peak, getLevel() + socialBoost * pImpl->releaseRate));
}

float Serotonin::getMoodEffect() const {
    return pImpl->moodModifier * getLevel();
}

float Serotonin::getSocialBehaviorEffect() const {
    return pImpl->socialBehaviorModifier * getLevel();
}

float Serotonin::getRapheNucleusActivity() const {
    return pImpl->rapheNucleusActivation;
}

} // namespace nlm
