#include "Norepinephrine.hpp"
#include <algorithm>

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float arousalBoost;
    float vigilanceFactor;
    float stressResponse;
    float decayRate;
    float releaseRate;
    
    // Stress and challenge tracking
    float stressLevel;
    float challengeLevel;
    float recoveryRate;
    
    // Arousal and alertness
    float alertness;
    float reactionTime;
    
    Impl() : level(0.0f), baseline(0.1f), arousalBoost(0.4f), 
             vigilanceFactor(0.3f), stressResponse(0.5f),
             decayRate(0.1f), releaseRate(0.6f), stressLevel(0.0f),
             challengeLevel(0.0f), recoveryRate(0.2f),
             alertness(0.5f), reactionTime(1.0f) {}
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
    // NE modulates plasticity based on arousal and stress
    // High arousal enhances plasticity for salient events
    // Stress can either enhance or impair learning depending on level
    
    float modulation = 0.5f + 0.5f * pImpl->level;
    
    // Arousal boost for novel/challenge events
    modulation += pImpl->arousalBoost * pImpl->challengeLevel;
    
    // Vigilance enhancement
    modulation += pImpl->vigilanceFactor * pImpl->alertness;
    
    // Stress effects: moderate stress enhances learning, high stress impairs it
    if (pImpl->stressLevel > 0.0f) {
        if (pImpl->stressLevel < 0.7f) {
            // Moderate stress: enhanced learning
            modulation += 0.3f * pImpl->stressLevel;
        } else {
            // High stress: impaired learning
            modulation *= (1.0f - (pImpl->stressLevel - 0.7f) * 2.0f);
        }
    }
    
    return std::clamp(modulation, 0.0f, 2.0f);
}

void Norepinephrine::update(TimestepDuration dt) {
    // NE dynamics: responds to stress, challenge, and environmental demands
    // Recovers from stress and maintains baseline arousal
    
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Stress recovery
    if (pImpl->stressLevel > 0.0f) {
        pImpl->stressLevel = std::max(0.0f, pImpl->stressLevel - pImpl->recoveryRate * static_cast<float>(dt));
    }
    
    // Alertness dynamics
    pImpl->alertness = 0.5f + 0.5f * pImpl->level;
    
    // Reaction time improves with arousal
    pImpl->reactionTime = 1.0f - 0.3f * pImpl->alertness;
}

void Norepinephrine::signalStress(float stressLevel) {
    // Respond to stress/challenge
    pImpl->stressLevel = std::min(1.0f, stressLevel);
    
    float stressBoost = stressLevel * pImpl->stressResponse;
    pImpl->level = std::min(1.0f, pImpl->level + stressBoost);
    
    // Challenge enhances arousal
    pImpl->challengeLevel = std::min(1.0f, stressLevel * 0.8f + pImpl->challengeLevel * 0.2f);
}

void Norepinephrine::signalChallenge(float challengeLevel) {
    // Respond to novel/challenge situations
    pImpl->challengeLevel = std::min(1.0f, challengeLevel);
    
    float challengeBoost = challengeLevel * pImpl->arousalBoost;
    pImpl->level = std::min(1.0f, pImpl->level + challengeBoost);
    
    // Improved vigilance for challenging tasks
    pImpl->vigilanceFactor = 0.3f + 0.4f * challengeLevel;
}

void Norepinephrine::signalRecovery() {
    // Recovery from stress improves with time and rest
    pImpl->stressLevel = std::max(0.0f, pImpl->stressLevel - 0.5f);
    pImpl->challengeLevel = std::max(0.0f, pImpl->challengeLevel - 0.3f);
}

float Norepinephrine::getStressLevel() const {
    return pImpl->stressLevel;
}

float Norepinephrine::getChallengeLevel() const {
    return pImpl->challengeLevel;
}

float Norepinephrine::getAlertness() const {
    return pImpl->alertness;
}

float Norepinephrine::getReactionTime() const {
    return pImpl->reactionTime;
}

} // namespace nlm
