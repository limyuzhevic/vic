#include "Serotonin.hpp"
#include <algorithm>

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baseline;
    float moodEffect;
    float socialBonding;
    float sleepEffect;
    float impulsivityMod;
    float decayRate;
    float releaseRate;
    
    // Social and emotional state tracking
    float socialCohesion;
    float aggressionLevel;
    float rewardProcessing;
    float aversionProcessing;
    
    // Sleep and recovery
    float sleepQuality;
    float wakefulness;
    float recoveryProgress;
    
    Impl() : level(0.0f), baseline(0.1f), moodEffect(0.3f), 
             socialBonding(0.4f), sleepEffect(0.2f), impulsivityMod(0.3f),
             decayRate(0.1f), releaseRate(0.7f), socialCohesion(0.5f),
             aggressionLevel(0.1f), rewardProcessing(0.5f), aversionProcessing(0.5f),
             sleepQuality(0.5f), wakefulness(0.5f), recoveryProgress(0.0f) {}
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
    // Serotonin modulates plasticity based on mood, social state, and sleep
    // High serotonin promotes stability and habit formation
    // Low serotonin increases exploratory plasticity but reduces stability
    
    float modulation = 0.5f + 0.5f * pImpl->level;
    
    // Mood effect: positive mood stabilizes learning
    modulation += pImpl->moodEffect * pImpl->level;
    
    // Social bonding: promotes social learning and imitation
    modulation += pImpl->socialBonding * pImpl->socialCohesion;
    
    // Sleep effect: good sleep enhances memory consolidation
    modulation += pImpl->sleepEffect * pImpl->sleepQuality;
    
    // Impulsivity: low serotonin increases impulsivity and risk-taking
    float impulsivityPenalty = (1.0f - pImpl->level) * pImpl->impulsivityMod;
    modulation -= impulsivityPenalty;
    
    // Aversion processing: high aversion can enhance threat learning
    if (pImpl->aversionProcessing > 0.7f) {
        modulation += 0.2f;
    }
    
    return std::clamp(modulation, 0.0f, 2.0f);
}

void Serotonin::update(TimestepDuration dt) {
    // Serotonin dynamics: maintains emotional balance, promotes sleep/recovery
    // Supports social behavior and mood regulation
    
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Recovery progress increases with time and sleep
    pImpl->recoveryProgress += 0.01f * static_cast<float>(dt);
    
    // Sleep quality improves with recovery
    pImpl->sleepQuality += 0.02f * pImpl->recoveryProgress - 0.01f * static_cast<float>(dt);
    pImpl->sleepQuality = std::clamp(pImpl->sleepQuality, 0.0f, 1.0f);
    
    // Wakefulness increases with serotonin level
    pImpl->wakefulness = 0.3f + 0.7f * pImpl->level;
}

void Serotonin::signalSocialBonding(float bondingLevel) {
    // Respond to social connection and bonding
    pImpl->socialCohesion = std::min(1.0f, bondingLevel);
    
    float bondingBoost = bondingLevel * pImpl->socialBonding;
    pImpl->level = std::min(1.0f, pImpl->level + bondingBoost);
    
    // Reduces aggression and improves mood
    pImpl->aggressionLevel *= (1.0f - bondingLevel * 0.3f);
    pImpl->moodEffect += bondingLevel * 0.2f;
}

void Serotonin::signalReward(float reward) {
    // Serotonergic response to reward, especially social/reward prediction
    float rewardBoost = reward * pImpl->releaseRate;
    pImpl->level = std::min(1.0f, pImpl->level + rewardBoost);
    
    // Improves reward processing and reduces aversion
    pImpl->rewardProcessing = std::min(1.0f, pImpl->rewardProcessing + reward * 0.1f);
    pImpl->aversionProcessing = std::max(0.0f, pImpl->aversionProcessing - reward * 0.05f);
}

void Serotonin::signalAversion(float aversion) {
    // Response to negative events, threat, or stress
    float aversionBoost = aversion * pImpl->releaseRate * 0.5f;
    pImpl->level = std::min(1.0f, pImpl->level + aversionBoost);
    
    // Increases aversion processing
    pImpl->aversionProcessing = std::min(1.0f, pImpl->aversionProcessing + aversion * 0.1f);
    
    // Temporarily reduces social bonding
    pImpl->socialCohesion = std::max(0.0f, pImpl->socialCohesion - aversion * 0.1f);
}

void Serotonin::signalSleep(float sleepQuality) {
    // Sleep improves serotonin levels and recovery
    pImpl->sleepQuality = sleepQuality;
    
    // Good sleep boosts serotonin for recovery
    if (sleepQuality > 0.7f) {
        float sleepBoost = (sleepQuality - 0.7f) * 0.5f;
        pImpl->level = std::min(1.0f, pImpl->level + sleepBoost);
    }
    
    // Sleep enhances mood regulation
    pImpl->moodEffect += sleepQuality * 0.1f;
}

float Serotonin::getSocialCohesion() const {
    return pImpl->socialCohesion;
}

float Serotonin::getAggressionLevel() const {
    return pImpl->aggressionLevel;
}

float Serotonin::getRewardProcessing() const {
    return pImpl->rewardProcessing;
}

float Serotonin::getAversionProcessing() const {
    return pImpl->aversionProcessing;
}

float Serotonin::getSleepQuality() const {
    return pImpl->sleepQuality;
}

float Serotonin::getWakefulness() const {
    return pImpl->wakefulness;
}

} // namespace nlm
