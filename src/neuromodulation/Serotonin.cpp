#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float moodFactor;
    float impulsivityFactor;
    float socialFactor;
    float rewardResponse;
    float punishmentSensitivity;
    float dominantMood;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), 
             decayRate(0.08f), moodFactor(0.4f), impulsivityFactor(0.3f),
             socialFactor(0.3f), rewardResponse(0.5f), punishmentSensitivity(0.2f),
             dominantMood(0.5f) {}
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
    // Serotonin has complex effects on mood, impulsivity, and learning
    float moodEffect = std::abs(pImpl->level - 0.5f) * 2.0f;  // Inverted U shape
    float impulsivityEffect = (1.0f - pImpl->level) * pImpl->impulsivityFactor;
    float socialEffect = pImpl->level * pImpl->socialFactor;
    
    // Optimal serotonin level for learning is around 0.5
    float optimalEffect = std::exp(-(pImpl->level - 0.5f) * (pImpl->level - 0.5f) * 4.0f);
    
    return 1.0f + optimalEffect * moodEffect - impulsivityEffect * 0.3f + socialEffect * 0.2f;
}

void Serotonin::update(TimestepDuration dt) {
    // Serotonin dynamics include mood regulation and reward processing
    float decay = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
    
    // Mood adaptation based on experiences
    pImpl->dominantMood = (pImpl->dominantMood * 0.9f) + (pImpl->level * 0.1f);
}

void Serotonin::signalReward(float reward) {
    // Serotonin responds to both reward and punishment
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->rewardResponse);
}

void Serotonin::signalPunishment(float punishment) {
    // Serotonin responds to negative experiences with sensitivity
    pImpl->level = std::max(0.0f, pImpl->level - punishment * pImpl->punishmentSensitivity);
}

void Serotonin::increaseSocialEngagement(float sociality) {
    // Serotonin affects social behavior
    pImpl->level = std::min(pImpl->peak, pImpl->level + sociality * pImpl->socialFactor);
}

float Serotonin::getImpulsivityFactor() const {
    return pImpl->impulsivityFactor;
}

void Serotonin::setImpulsivityFactor(float factor) {
    pImpl->impulsivityFactor = factor;
}

float Serotonin::getSocialFactor() const {
    return pImpl->socialFactor;
}

void Serotonin::setSocialFactor(float factor) {
    pImpl->socialFactor = factor;
}

float Serotonin::getDominantMood() const {
    return pImpl->dominantMood;
}

} // namespace nlm