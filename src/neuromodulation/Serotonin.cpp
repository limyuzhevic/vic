#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baselineLevel;
    float moodLevel;
    float impulsivityLevel;
    float socialBehaviorModifier;
    float rewardSensitivity;
    float anxietyLevel;
};

Serotonin::Serotonin() : pImpl(new Impl()) {
    pImpl->level = 0.0f;
    pImpl->baselineLevel = 0.03f;
    pImpl->moodLevel = 0.5f;
    pImpl->impulsivityLevel = 0.5f;
    pImpl->socialBehaviorModifier = 1.0f;
    pImpl->rewardSensitivity = 1.0f;
    pImpl->anxietyLevel = 0.5f;
}

Serotonin::~Serotonin() = default;

const char* Serotonin::getName() const {
    return "Serotonin (5-HT)";
}

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    
    // Serotonin affects mood
    pImpl->moodLevel = 0.3f + pImpl->level * 2.33f;
    if (pImpl->moodLevel > 1.0f) pImpl->moodLevel = 1.0f;
    
    // Serotonin affects impulsivity - low 5-HT = high impulsivity
    pImpl->impulsivityLevel = 0.8f - pImpl->level * 0.6f;
    if (pImpl->impulsivityLevel < 0.2f) pImpl->impulsivityLevel = 0.2f;
    
    // Serotonin affects social behavior
    pImpl->socialBehaviorModifier = 0.5f + pImpl->level * 1.0f;
    
    // Serotonin affects reward sensitivity
    pImpl->rewardSensitivity = 0.8f + pImpl->level * 1.2f;
    if (pImpl->rewardSensitivity > 2.0f) pImpl->rewardSensitivity = 2.0f;
    
    // Low 5-HT increases anxiety
    pImpl->anxietyLevel = 0.2f + (1.0f - pImpl->level) * 1.25f;
    if (pImpl->anxietyLevel > 1.0f) pImpl->anxietyLevel = 1.0f;
    
    NLM_LOG_DEBUG("Serotonin level set to " + std::to_string(level) + 
                  ", mood: " + std::to_string(pImpl->moodLevel) +
                  ", impulsivity: " + std::to_string(pImpl->impulsivityLevel));
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin modulates plasticity based on mood and anxiety
    // Optimal mood enhances learning, high anxiety impairs it
    float moodBenefit = pImpl->moodLevel * 0.5f;
    float anxietyCost = pImpl->anxietyLevel * 0.4f;
    return std::clamp(1.0f + moodBenefit - anxietyCost, 0.1f, 2.0f);
}

void Serotonin::update(TimestepDuration dt) {
    // Gradual decay
    pImpl->level *= std::pow(0.93f, dt);
    
    // Anxiety decays more slowly
    pImpl->anxietyLevel *= std::pow(0.95f, dt);
}

void Serotonin::signalSocialInteraction() {
    // Social interactions boost serotonin
    setLevel(std::min(pImpl->level + 0.1f, 1.0f));
}

void Serotonin::signalRewardExperience(float reward) {
    // Reward experiences influence serotonin dynamics
    if (reward > 0.5f) {
        setLevel(std::min(pImpl->level + reward * 0.2f, 1.0f));
    }
}

void Serotonin::signalSocialLearning() {
    // Social learning enhances serotonin for prosocial behavior
    pImpl->socialBehaviorModifier = std::min(pImpl->socialBehaviorModifier * 1.1f, 2.0f);
    NLM_LOG_INFO("Serotonin-enhanced social learning activated");
}

} // namespace nlm