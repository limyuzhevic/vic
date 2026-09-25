#include "../neuromodulation/Serotonin.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float recoveryRate;
    float moodLevel;
    float anxietyLevel;
    float impulsivityLevel;
    std::vector<float> history;
    float socialModulation;
    float punishmentSensitivity;
    float rewardSensitization;
    
    Impl() : level(0.0f), baseline(0.5f), peak(1.0f), decayRate(0.05f), recoveryRate(0.1f),
             moodLevel(0.5f), anxietyLevel(0.5f), impulsivityLevel(0.5f),
             socialModulation(0.5f), punishmentSensitivity(0.5f), rewardSensitization(0.5f) {}
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
    // Update related modulations
    pImpl->moodLevel = std::min(1.0f, pImpl->level * 1.2f);
    pImpl->anxietyLevel = std::max(0.0f, 1.0f - pImpl->level * 0.8f);
    pImpl->impulsivityLevel = std::max(0.0f, 1.0f - pImpl->level * 0.6f);
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin modulates plasticity: higher levels increase stability, decrease learning rate
    // Medium levels optimize learning, very high levels suppress plasticity
    float basePlasticity = 0.8f + 0.2f * pImpl->level; // Start at 1.0, decay to 0.6
    return std::max(0.3f, std::min(1.0f, basePlasticity * (1.0f - pImpl->anxietyLevel * 0.3f)));
}

void Serotonin::update(TimestepDuration dt) {
    // Natural decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Recovery dynamics
    float recovery = pImpl->recoveryRate * static_cast<float>(dt);
    // In a real implementation, would track internal state
    pImpl->history.push_back(pImpl->level);
    if (pImpl->history.size() > 100) {
        pImpl->history.erase(pImpl->history.begin());
    }
}

void Serotonin::modulateMood(float positiveEvents, float negativeEvents, float socialContext) {
    // Update mood based on life events
    float moodChange = (positiveEvents * 0.6f - negativeEvents * 0.8f + socialContext * 0.4f) * 0.01f;
    pImpl->moodLevel = std::max(0.0f, std::min(1.0f, pImpl->moodLevel + moodChange));
    
    // Update serotonin level
    float levelChange = moodChange * 0.3f;
    setLevel(pImpl->level + levelChange);
}

void Serotonin::reduceAnxiety(float safety, float security, float predictability) {
    // Safety and predictability reduce anxiety
    float anxietyReduction = (safety + security + predictability) * 0.15f;
    pImpl->anxietyLevel = std::max(0.0f, pImpl->anxietyLevel - anxietyReduction);
    
    // This reduces cortisol and increases serotonin naturally
    float levelBoost = anxietyReduction * 0.2f;
    setLevel(pImpl->level + levelBoost);
}

void Serotonin::modulateImpulsivity(float rewardDelay, float punishmentProbability) {
    // Longer reward delays and higher punishment probability reduce impulsivity
    float delayEffect = std::min(1.0f, rewardDelay * 0.05f);
    float punishmentEffect = punishmentProbability * 0.7f;
    
    float impulsivityReduction = (delayEffect + punishmentEffect) * 0.4f;
    pImpl->impulsivityLevel = std::max(0.0f, pImpl->impulsivityLevel - impulsivityReduction);
    
    // Reduced impulsivity promotes serotonin release
    float levelBoost = impulsivityReduction * 0.15f;
    setLevel(pImpl->level + levelBoost);
}

void Serotonin::modulateSocialBehavior(float socialReward, float affiliation, float competition) {
    // Social rewards increase serotonin
    float socialBoost = (socialReward * 0.4f + affiliation * 0.3f - competition * 0.2f) * 0.02f;
    setLevel(pImpl->level + socialBoost);
    
    // Update social modulation factor
    pImpl->socialModulation = std::max(0.1f, std::min(1.0f, pImpl->socialModulation + socialBoost * 0.3f));
}

void Serotonin::signalPunishment(float severity, float predictability) {
    // Severe, predictable punishments increase serotonin
    float punishmentBoost = (severity * 0.3f + (1.0f - predictability) * 0.4f) * 0.015f;
    setLevel(pImpl->level + punishmentBoost);
}

void Serotonin::updateBehavioralStrategy(float outcomeValue, float confidence) {
    // Learning from outcomes affects serotonin levels
    float learningSignal = outcomeValue * confidence * 0.01f;
    if (learningSignal > 0.0f) {
        // Positive outcomes mildly increase serotonin
        setLevel(pImpl->level + learningSignal * 0.5f);
    } else {
        // Negative outcomes may initially decrease, then increase for learning
        setLevel(pImpl->level + learningSignal * 0.3f);
    }
}

void Serotonin::adaptToInternalState(float hunger, float fatigue, float hormonalState) {
    // Internal states affect serotonin levels
    float stateImpact = (hunger * 0.2f + fatigue * 0.15f + hormonalState * 0.25f) * 0.01f;
    setLevel(pImpl->level + stateImpact);
}

} // namespace nlm