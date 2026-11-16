#include "Histamine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Histamine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float wakefulness;
    float cognition;
    float appetite;
    float motorActivity;
    float decayRate;
    float timeToPeak;
    SimulationStep lastWakeTime;
    float sleepPressure;
    
    Impl() : brain(nullptr), level(0.0f), baseline(0.0f), wakefulness(0.0f), 
             cognition(0.0f), appetite(0.0f), motorActivity(0.0f), decayRate(0.02f),
             timeToPeak(150.0f), lastWakeTime(0), sleepPressure(1.0f) {}
};

Histamine::Histamine() : pImpl(new Impl) {}

Histamine::~Histamine() = default;

const char* Histamine::getName() const {
    return "H1";
}

float Histamine::getLevel() const {
    return pImpl->level;
}

void Histamine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    // H1 affects wakefulness, cognition, and motor activity
    pImpl->wakefulness = level * 0.4f;
    pImpl->cognition = level * 0.3f;
    pImpl->motorActivity = level * 0.3f;
    pImpl->appetite = (1.0f - level) * 0.5f; // High H1 = low appetite
}

float Histamine::getPlasticityFactor() const {
    // H1 modulates plasticity for wake-related learning
    return 1.0f + 0.3f * pImpl->level;
}

void Histamine::update(TimestepDuration dt) {
    // Decay towards baseline
    float decay = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
    
    // Sleep pressure decay
    pImpl->sleepPressure = std::max(0.0f, pImpl->sleepPressure - 0.001f * static_cast<float>(dt));
    
    // Natural wakefulness fluctuations
    if (pImpl->lastWakeTime > 0) {
        float timeSinceWake = static_cast<float>(pImpl->currentStep) - pImpl->lastWakeTime;
        if (timeSinceWake < 100.0f) {
            float wakeDecay = std::exp(-timeSinceWake / 50.0f);
            pImpl->level *= wakeDecay;
        }
    }
    
    // Update derived attributes
    pImpl->wakefulness = pImpl->level * 0.4f;
    pImpl->cognition = pImpl->level * 0.3f;
    pImpl->motorActivity = pImpl->level * 0.3f;
    pImpl->appetite = (1.0f - pImpl->level) * 0.5f;
}

void Histamine::increaseWakefulness(float wakeBoost) {
    pImpl->level = std::min(1.0f, pImpl->level + wakeBoost);
    pImpl->wakefulness = std::min(1.0f, pImpl->wakefulness + wakeBoost * 0.5f);
    pImpl->cognition = std::min(1.0f, pImpl->cognition + wakeBoost * 0.3f);
    pImpl->motorActivity = std::min(1.0f, pImpl->motorActivity + wakeBoost * 0.4f);
    
    // Record wake time
    pImpl->lastWakeTime = 0; // Will be updated by caller
    
    NLM_LOG_DEBUG("Histamine wakefulness boost: " + std::to_string(wakeBoost));
}

void Histamine::decreaseSleepPressure(float sleepGain) {
    pImpl->sleepPressure = std::min(1.0f, pImpl->sleepPressure + sleepGain);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - sleepGain * 0.3f);
    
    NLM_LOG_DEBUG("Histamine sleep pressure increase: " + std::to_string(sleepGain));
}

float Histamine::getWakefulnessLevel() const {
    return pImpl->wakefulness;
}

float Histamine::getCognitiveLevel() const {
    return pImpl->cognition;
}

float Histamine::getMotorActivityLevel() const {
    return pImpl->motorActivity;
}

float Histamine::getAppetiteLevel() const {
    return pImpl->appetite;
}

void Histamine::cognitiveChallengeFeedback(float challengeQuality) {
    // Cognitive challenge modulates histamine levels
    float feedbackStrength = challengeQuality * 0.2f;
    pImpl->level = std::min(1.0f, pImpl->level + feedbackStrength);
    pImpl->cognition = std::min(1.0f, pImpl->cognition + feedbackStrength * 0.7f);
    pImpl->motorActivity = std::min(1.0f, pImpl->motorActivity + feedbackStrength * 0.3f);
    
    NLM_LOG_DEBUG("Histamine cognitive feedback: " + std::to_string(challengeQuality));
}

float Histamine::getAlertLevel() const {
    // Alert level considering wakefulness and sleep pressure
    float alertness = pImpl->wakefulness * 0.7f;
    float sleepiness = 1.0f - pImpl->sleepPressure * 0.5f;
    return alertness - sleepiness;
}

} // namespace nlm