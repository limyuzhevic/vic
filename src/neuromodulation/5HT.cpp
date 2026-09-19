#include "5HT.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float moodRegulationSensitivity;
    float socialFeedbackSensitivity;
    float impulsivityModulationStrength;
    float moodRegulationFactor;
    float decisionMakingFlexibilityFactor;
    float impulsivityModulationFactor;
    
    Impl()
        : level(0.0f), baseline(0.02f), peak(1.0f)
        , decayRate(0.05f), releaseRate(1.0f)
        , moodRegulationSensitivity(1.0f), socialFeedbackSensitivity(1.0f)
        , impulsivityModulationStrength(1.0f)
        , moodRegulationFactor(1.0f), decisionMakingFlexibilityFactor(1.0f), impulsivityModulationFactor(1.0f) {}
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
    // 5-HT modulates plasticity based on mood and decision making
    // Higher 5-HT levels promote mood stability and flexible decision making
    float moodBasedPlasticity = pImpl->level * pImpl->moodRegulationSensitivity;
    return 0.3f + 0.7f * moodBasedPlasticity;
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline with time constant influenced by decayRate
    // Serotonin has slower dynamics than ACh and NE
    float decay = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
    
    // Update mood regulation and decision making factors
    pImpl->moodRegulationFactor = 1.0f + pImpl->level * pImpl->moodRegulationSensitivity;
    pImpl->decisionMakingFlexibilityFactor = 1.0f + 1.5f * pImpl->level;
    pImpl->impulsivityModulationFactor = std::max(0.1f, 1.0f - pImpl->level * pImpl->impulsivityModulationStrength);
}

void Serotonin::signalReward(float rewardOutcome) {
    // 5-HT release in response to reward outcomes for mood regulation
    // Positive outcomes promote learning and mood stability
    float rewardResponse = rewardOutcome * 0.3f * pImpl->releaseRate;
    pImpl->level = std::min(pImpl->peak, pImpl->level + rewardResponse);
}

void Serotonin::signalSocialFeedback(float socialFeedback) {
    // 5-HT responds to social feedback for social behavior regulation
    // Social acceptance promotes serotonergic activity
    float socialResponse = socialFeedback * 0.4f * pImpl->releaseRate * pImpl->socialFeedbackSensitivity;
    pImpl->level = std::min(pImpl->peak, pImpl->level + socialResponse);
}

void Serotonin::signalImpulsivity(float impulsivityLevel) {
    // 5-HT inhibits impulsive responses
    // Lower impulsivity levels trigger serotonergic activity to promote control
    if (impulsivityLevel > 0.5f) {
        float inhibitionResponse = (1.0f - impulsivityLevel) * 0.5f * pImpl->releaseRate;
        pImpl->level = std::min(pImpl->peak, pImpl->level + inhibitionResponse);
    }
}

void Serotonin::signalSleepModulation(float sleepPressure) {
    // 5-HT modulates sleep-wake cycle
    // Higher sleep pressure reduces serotonergic activity (serotonin promotes wakefulness)
    float sleepResponse = (1.0f - sleepPressure) * 0.2f * pImpl->releaseRate;
    pImpl->level = std::min(pImpl->peak, pImpl->level + sleepResponse);
}

float Serotonin::getMoodRegulationFactor() const {
    return pImpl->moodRegulationFactor;
}

float Serotonin::getDecisionMakingFlexibilityFactor() const {
    return pImpl->decisionMakingFlexibilityFactor;
}

float Serotonin::getImpulsivityModulationFactor() const {
    return pImpl->impulsivityModulationFactor;
}

void Serotonin::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->moodRegulationFactor = 1.0f;
    pImpl->decisionMakingFlexibilityFactor = 1.0f;
    pImpl->impulsivityModulationFactor = 1.0f;
}

void Serotonin::setBaseline(float baseline) {
    pImpl->baseline = std::clamp(baseline, 0.0f, 0.1f);
}

void Serotonin::setDecayRate(float decayRate) {
    pImpl->decayRate = std::clamp(decayRate, 0.01f, 1.0f);
}

void Serotonin::setReleaseRate(float releaseRate) {
    pImpl->releaseRate = std::clamp(releaseRate, 0.5f, 5.0f);
}

void Serotonin::setMoodRegulationSensitivity(float sensitivity) {
    pImpl->moodRegulationSensitivity = std::clamp(sensitivity, 0.1f, 5.0f);
}

void Serotonin::setSocialFeedbackSensitivity(float sensitivity) {
    pImpl->socialFeedbackSensitivity = std::clamp(sensitivity, 0.1f, 5.0f);
}

void Serotonin::setImpulsivityModulationStrength(float strength) {
    pImpl->impulsivityModulationStrength = std::clamp(strength, 0.1f, 5.0f);
}

} // namespace nlm