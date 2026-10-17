#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baseline;
    float decayRate;
    
    // Mood and social components
    float moodLevel;
    float socialValue;
    float socialThreat;
    float socialThreshold;
    
    // Behavioral modulation
    float impulsivity;
    float anxietyLevel;
    float rewardSensitivity;
    
    // Temporal dynamics
    float moodTime;
    float socialTime;
    
    Impl() : level(0.0f), baseline(0.0f), decayRate(0.1f),
             moodLevel(0.0f), socialValue(0.0f), socialThreat(0.0f),
             socialThreshold(0.5f), impulsivity(0.0f), anxietyLevel(0.0f),
             rewardSensitivity(1.0f), moodTime(0.0f), socialTime(0.0f) {}
};

Serotonin::Serotonin() : pImpl(std::make_unique<Impl>()) {}

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Serotonin::getPlasticityFactor() const {
    // 5-HT modulates plasticity based on mood and social context
    float moodPlasticity = pImpl->moodLevel * 0.8f;
    float socialPlasticity = pImpl->socialValue * 1.2f;
    float threatPlasticity = pImpl->socialThreat * 0.6f;
    float impulsivityPlasticity = pImpl->impulsivity * 0.5f;
    float anxietyPlasticity = pImpl->anxietyLevel * 0.7f;
    float rewardSensitivityPlasticity = pImpl->rewardSensitivity * 1.0f;
    
    // Combine mood and social components
    float totalPlasticity = moodPlasticity + socialPlasticity + threatPlasticity + 
                           impulsivityPlasticity + anxietyPlasticity + rewardSensitivityPlasticity;
    
    // Saturate at reasonable levels
    return std::min(totalPlasticity, 2.5f);
}

void Serotonin::update(TimestepDuration dt) {
    // Decay mood and social signals
    pImpl->moodLevel *= 0.9f;
    pImpl->socialValue *= 0.8f;
    pImpl->socialThreat *= 0.7f;
    pImpl->impulsivity *= 0.95f;
    pImpl->anxietyLevel *= 0.85f;
    
    // Decay toward baseline
    float decay = 1.0f - pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = pImpl->baseline + (pImpl->level - pImpl->baseline) * decay;
}

void Serotonin::signalSocialReward(float socialValue) {
    // Social reward detection enhances serotonin release
    pImpl->socialValue = std::min(socialValue, 1.0f);
    pImpl->socialTime = 1.5f;  // Duration of social response
    
    // Reduce anxiety based on social reward
    pImpl->anxietyLevel = std::max(0.0f, pImpl->anxietyLevel - socialValue * 0.3f);
    
    // Increase reward sensitivity
    pImpl->rewardSensitivity = std::min(pImpl->rewardSensitivity + socialValue * 0.2f, 2.0f);
    
    // Update mood
    pImpl->moodLevel = std::min(pImpl->moodLevel + socialValue * 0.4f, 1.0f);
    
    // Update overall level
    pImpl->level += socialValue * 0.6f;
}

void Serotonin::signalSocialThreat(float threat) {
    // Social threat detection enhances serotonin release
    pImpl->socialThreat = std::min(threat, 1.0f);
    pImpl->socialTime = 1.0f;
    
    // Increase anxiety based on threat
    pImpl->anxietyLevel = std::min(pImpl->anxietyLevel + threat * 0.5f, 1.0f);
    
    // Decrease mood based on threat
    pImpl->moodLevel = std::max(0.0f, pImpl->moodLevel - threat * 0.3f);
    
    // Increase impulsivity as defensive response
    pImpl->impulsivity = std::min(pImpl->impulsivity + threat * 0.3f, 1.0f);
    
    // Update overall level
    pImpl->level += threat * 0.4f;
}

void Serotonin::setSocialThreshold(float threshold) {
    pImpl->socialThreshold = std::clamp(threshold, 0.0f, 1.0f);
}

Serotonin::~Serotonin() = default;

} // namespace nlm
