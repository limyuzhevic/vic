#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Serotonin::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float mood;
    float impulsivity;
    float socialAttentiveness;
    float decayRate;
    float timeToPeak;
    SimulationStep lastImpulseTime;
    float burstIntensity;
    
    Impl() : brain(nullptr), level(0.0f), baseline(0.0f), mood(0.0f), impulsivity(0.0f), 
             socialAttentiveness(0.0f), decayRate(0.03f), timeToPeak(100.0f),
             lastImpulseTime(0), burstIntensity(0.0f) {}
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
    // 5-HT affects mood, impulsivity, and social behavior
    pImpl->mood = level * 0.5f;
    pImpl->impulsivity = (1.0f - level) * 0.8f; // High 5-HT = low impulsivity
    pImpl->socialAttentiveness = level * 0.6f;
}

float Serotonin::getPlasticityFactor() const {
    // 5-HT modulates plasticity for mood-related learning
    return 1.2f - 0.5f * pImpl->level; // High 5-HT reduces certain types of plasticity
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    float decay = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
    
    // Natural fluctuations (simulating tonic firing)
    if (pImpl->burstIntensity > 0.01f) {
        float fluctuation = std::sin(static_cast<float>(pImpl->lastImpulseTime) * 0.1f) * pImpl->burstIntensity;
        pImpl->level += fluctuation * 0.1f;
        pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
    }
    
    // Update derived attributes
    pImpl->mood = pImpl->level * 0.5f;
    pImpl->impulsivity = (1.0f - pImpl->level) * 0.8f;
    pImpl->socialAttentiveness = pImpl->level * 0.6f;
}

void Serotonin::enhanceMood(float moodBoost) {
    pImpl->level = std::min(1.0f, pImpl->level + moodBoost);
    pImpl->mood = std::min(1.0f, pImpl->mood + moodBoost * 0.5f);
    pImpl->socialAttentiveness = std::min(1.0f, pImpl->socialAttentiveness + moodBoost * 0.3f);
    
    // Record this as an impulse
    pImpl->lastImpulseTime = 0; // Will be updated by caller
    pImpl->burstIntensity = std::min(0.5f, moodBoost);
    
    NLM_LOG_DEBUG("Serotonin mood boost: " + std::to_string(moodBoost));
}

void Serotonin::reduceImpulsivity(float impulseControl) {
    pImpl->impulsivity = std::max(0.0f, pImpl->impulsivity - impulseControl);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - impulseControl * 0.2f);
    
    NLM_LOG_DEBUG("Serotonin impulse control: " + std::to_string(impulseControl));
}

float Serotonin::getImpulsivityLevel() const {
    return pImpl->impulsivity;
}

float Serotonin::getSocialAttentivenessLevel() const {
    return pImpl->socialAttentiveness;
}

void Serotonin::socialInteractionFeedback(float socialQuality) {
    // Social feedback modulates serotonin levels
    float feedbackStrength = socialQuality * 0.3f;
    pImpl->level = std::min(1.0f, pImpl->level + feedbackStrength);
    pImpl->socialAttentiveness = std::min(1.0f, pImpl->socialAttentiveness + feedbackStrength * 0.5f);
    
    NLM_LOG_DEBUG("Serotonin social feedback: " + std::to_string(socialQuality));
}

float Serotonin::getSerotoninIndex() const {
    // Composite index representing overall serotonin tone
    return (pImpl->mood * 0.4f + pImpl->socialAttentiveness * 0.4f + 
            (1.0f - pImpl->impulsivity) * 0.2f);
}

} // namespace nlm