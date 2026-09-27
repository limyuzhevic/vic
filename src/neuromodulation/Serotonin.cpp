#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct Serotonin::Impl {
    float level;                 // Current concentration (0.0 to 1.0)
    float baseline;              // Baseline concentration
    
    // For mood regulation
    float mood;                  // Current mood (0.0 = negative, 1.0 = positive)
    float socialBonding;         // Level of social bonding/instinct
    float impulsivity;           // Level of impulsivity (0.0 to 1.0)
    
    // For reward processing
    float rewardSensitivity;     // Sensitivity to rewards
    float punishmentSensitivity;  // Sensitivity to punishments
    float rewardHistory[20];      // Recent reward samples
    size_t rewardHistoryIndex;    // Current position in history
    
    // For learning modulation
    float learningRate;           // Learning rate modulation
    float extinctionRate;         // Rate of extinction learning
    float approachWithdrawal;     // Approach vs withdrawal tendency
    
    // Statistics
    float totalRewardsReceived;   // Total rewards experienced
    float totalPunishments;       // Total punishments experienced
    uint64_t socialInteractions;   // Number of social interactions
    
    Impl() : level(0.05f), baseline(0.05f),
             mood(0.5f), socialBonding(0.3f), impulsivity(0.5f),
             rewardSensitivity(0.5f), punishmentSensitivity(0.3f),
             learningRate(0.1f), extinctionRate(0.05f),
             approachWithdrawal(0.0f), totalRewardsReceived(0.0f),
             totalPunishments(0.0f), socialInteractions(0) {
        for (int i = 0; i < 20; ++i) {
            rewardHistory[i] = 0.0f;
        }
        rewardHistoryIndex = 0;
    }
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

const char* Serotonin::getName() const {
    return "Serotonin (5-HT)";
}

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Serotonin::getPlasticityFactor() const {
    /*
     * Serotonin modulates plasticity based on:
     * 1. Mood: Positive mood enhances reward learning
     * 2. Impulsivity: High impulsivity increases exploration
     * 3. Social bonding: Social interactions modulate learning
     * 4. Reward/punishment history: Shapes future learning
     */
    
    // Mood modulation
    float moodModulation = 1.0f + 0.5f * (pImpl->mood - 0.5f);
    
    // Impulsivity modulation
    float impulsivityModulation = 1.0f + 0.3f * pImpl->impulsivity;
    
    // Social bonding modulation
    float socialModulation = 1.0f + 0.2f * pImpl->socialBonding;
    
    // Reward sensitivity modulation
    float rewardModulation = 1.0f + pImpl->rewardSensitivity;
    
    // Punishment sensitivity modulation (decreases plasticity for negative outcomes)
    float punishmentModulation = 1.0f - 0.5f * pImpl->punishmentSensitivity;
    
    return moodModulation * impulsivityModulation * socialModulation * rewardModulation * punishmentModulation;
}

void Serotonin::update(TimestepDuration dt) {
    /*
     * Update serotonin concentration based on:
     * 1. Reward prediction error (negative prediction error increases serotonin)
     * 2. Social interactions (positive social outcomes increase serotonin)
     * 3. Punishment (punishments decrease serotonin)
     * 4. Novelty (novel stimuli can increase or decrease serotonin)
     */
    
    // Natural decay
    float decayRate = 0.05f * dt;
    pImpl->level += (pImpl->baseline - pImpl->level) * decayRate;
    
    // Social interaction boost
    if (pImpl->socialInteractions > 0) {
        float socialBoost = std::min(1.0f, pImpl->socialInteractions * 0.01f) * 0.3f;
        pImpl->level += socialBoost * dt;
        pImpl->socialInteractions = 0; // Reset after boost
    }
    
    // Clamp to reasonable bounds
    pImpl->level = std::clamp(pImpl->level, 0.01f, 0.3f);
}

void Serotonin::signalReward(float reward) {
    pImpl->totalRewardsReceived += reward;
    
    // Update reward history
    pImpl->rewardHistory[pImpl->rewardHistoryIndex] = reward;
    pImpl->rewardHistoryIndex = (pImpl->rewardHistoryIndex + 1) % 20;
    
    // Boost serotonin for rewards
    float rewardBoost = reward * 0.2f;
    pImpl->level += rewardBoost;
    
    // Update mood based on reward
    pImpl->mood = (pImpl->mood * 0.9f) + (reward * 0.1f);
    pImpl->mood = std::clamp(pImpl->mood, 0.0f, 1.0f);
}

void Serotonin::signalPunishment(float punishment) {
    pImpl->totalPunishments += punishment;
    
    // Reduce serotonin for punishments
    float punishmentReduction = punishment * 0.15f;
    pImpl->level -= punishmentReduction;
    
    // Reduce mood based on punishment
    pImpl->mood = (pImpl->mood * 0.8f) + (0.2f * punishment);
    pImpl->mood = std::max(0.0f, pImpl->mood - punishment * 0.1f);
}

void Serotonin::signalSocialInteraction(float interactionValue) {
    pImpl->socialInteractions++;
    
    if (interactionValue > 0.5f) {
        // Positive social interaction increases serotonin
        float boost = interactionValue * 0.1f;
        pImpl->level += boost;
        pImpl->mood = std::min(1.0f, pImpl->mood + boost * 0.5f);
    } else if (interactionValue < -0.5f) {
        // Negative social interaction decreases serotonin
        float reduction = std::min(0.1f, -interactionValue * 0.1f);
        pImpl->level -= reduction;
        pImpl->mood = std::max(0.0f, pImpl->mood - reduction * 0.5f);
    }
}

float Serotonin::getMood() const {
    return pImpl->mood;
}

void Serotonin::setMood(float mood) {
    pImpl->mood = std::clamp(mood, 0.0f, 1.0f);
}

float Serotonin::getImpulsivity() const {
    return pImpl->impulsivity;
}

void Serotonin::setImpulsivity(float impulsivity) {
    pImpl->impulsivity = std::clamp(impulsivity, 0.0f, 1.0f);
}

float Serotonin::getRewardSensitivity() const {
    return pImpl->rewardSensitivity;
}

void Serotonin::setRewardSensitivity(float sensitivity) {
    pImpl->rewardSensitivity = std::clamp(sensitivity, 0.0f, 1.0f);
}

float Serotonin::getPunishmentSensitivity() const {
    return pImpl->punishmentSensitivity;
}

void Serotonin::setPunishmentSensitivity(float sensitivity) {
    pImpl->punishmentSensitivity = std::clamp(sensitivity, 0.0f, 1.0f);
}

float Serotonin::getSocialBonding() const {
    return pImpl->socialBonding;
}

void Serotonin::setSocialBonding(float bonding) {
    pImpl->socialBonding = std::clamp(bonding, 0.0f, 1.0f);
}

float Serotonin::getLearningRate() const {
    return pImpl->learningRate;
}

void Serotonin::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

float Serotonin::getApproachWithdrawal() const {
    return pImpl->approachWithdrawal;
}

void Serotonin::setApproachWithdrawal(float tendency) {
    pImpl->approachWithdrawal = std::clamp(tendency, -1.0f, 1.0f);
}

float Serotonin::getTotalRewards() const {
    return pImpl->totalRewardsReceived;
}

float Serotonin::getTotalPunishments() const {
    return pImpl->totalPunishments;
}

float Serotonin::getExtinctionRate() const {
    return pImpl->extinctionRate;
}

void Serotonin::setExtinctionRate(float rate) {
    pImpl->extinctionRate = std::clamp(rate, 0.0f, 1.0f);
}

} // namespace nlm