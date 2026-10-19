#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <chrono>

namespace nlm {

struct Serotonin::Impl {
    // Mood and social behavior components
    float moodLevel;            // Current mood level (-1 to 1)
    float impulsivityControl;   // Impulsivity control level (0-1)
    float socialBehavior;       // Social behavior enhancement (0-1)
    float decodyState;          // Decody switching state (0=exploration, 1=exploitation)
    float socialState;          // Social behavior state (0=isolated, 1=social)
    
    // Biological parameters
    float minLevel;             // Minimum biological level (5 pM)
    float maxLevel;             // Maximum biological level (50 pM)
    float decayRate;            // Decay rate (0.4 /s)
    float releaseRate;          // Release rate (1.5 /s)
    
    // Temperature and social modulators
    float bodyTemperature;      // Body temperature influence
    float socialTemperature;    // Social temperature (social warmth)
    
    // Decision making components
    float riskTaking;           // Risk taking propensity (0-1)
    float rewardSensitivity;    // Reward sensitivity (0-1)
    float punishmentSensitivity; // Punishment sensitivity (0-1)
    
    // Temporal dynamics
    float timeSinceLastSocial;  // Time since last social interaction
    float timeSinceLastReward;  // Time since last reward
    
    // State tracking
    float peakLevel;            // Historical peak
    float minLevelObserved;     // Historical minimum
    float timeAboveHigh;        // Time above high threshold
    float timeAboveLow;         // Time above low threshold
    
    // History for temporal dynamics
    std::vector<float> levelHistory;
    std::vector<float> moodHistory;
    
    // Random number generator
    std::mt19937 rng;
    std::uniform_real_distribution<float> uniformDist;
    std::normal_distribution<float> normalDist;
    
    Impl() : 
        moodLevel(0.0f),
        impulsivityControl(0.5f),
        socialBehavior(0.5f),
        decodyState(0.5f),  // Balanced initially
        socialState(0.5f),   // Balanced initially
        minLevel(5.0f),
        maxLevel(50.0f),
        decayRate(0.4f),
        releaseRate(1.5f),
        bodyTemperature(0.0f),
        socialTemperature(0.0f),
        riskTaking(0.5f),
        rewardSensitivity(0.5f),
        punishmentSensitivity(0.5f),
        timeSinceLastSocial(0.0f),
        timeSinceLastReward(0.0f),
        peakLevel(0.0f),
        minLevelObserved(0.0f),
        timeAboveHigh(0.0f),
        timeAboveLow(0.0f),
        uniformDist(0.0f, 1.0f),
        normalDist(0.0f, 0.1f) {
        
        levelHistory.reserve(1000);
        moodHistory.reserve(1000);
    }
};

Serotonin::Serotonin() : pImpl(std::make_unique<Impl>()) {}

Serotonin::~Serotonin() = default;

const char* Serotonin::getName() const {
    return "5-HT";
}

float Serotonin::getLevel() const {
    return (pImpl->moodLevel + pImpl->socialBehavior + pImpl->impulsivityControl) / 3.0f * pImpl->maxLevel;
}

void Serotonin::setLevel(float level) {
    // Clamp to biological range
    pImpl->moodLevel = std::clamp(level / pImpl->maxLevel * 0.8f, -1.0f, 1.0f);
    pImpl->socialBehavior = std::clamp((level / pImpl->maxLevel - 0.4f) * 1.5f, 0.0f, 1.0f);
    pImpl->impulsivityControl = std::clamp(1.0f - std::abs(pImpl->moodLevel), 0.0f, 1.0f);
    
    // Update decody state based on mood
    pImpl->decodyState = std::clamp(0.5f + pImpl->moodLevel * 0.3f, 0.0f, 1.0f);
    
    // Update social state based on social behavior
    pImpl->socialState = std::clamp(pImpl->socialBehavior * 2.0f - 0.5f, 0.0f, 1.0f);
    
    // Update history
    pImpl->levelHistory.push_back(getLevel());
    pImpl->moodHistory.push_back(pImpl->moodLevel);
    if (pImpl->levelHistory.size() > 1000) {
        pImpl->levelHistory.erase(pImpl->levelHistory.begin());
        pImpl->moodHistory.erase(pImpl->moodHistory.begin());
    }
    
    NLM_LOG_INFO("Serotonin level set: mood=" + std::to_string(pImpl->moodLevel) +
                 ", social=" + std::to_string(pImpl->socialBehavior) +
                 ", impulsivityControl=" + std::to_string(pImpl->impulsivityControl));
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin modulates flexibility and behavioral switching
    float moodFactor = std::abs(pImpl->moodLevel) * 0.3f;
    float socialFactor = pImpl->socialBehavior * 0.4f;
    float impulsivityFactor = (1.0f - pImpl->impulsivityControl) * 0.3f;
    
    return 0.5f + moodFactor + socialFactor + impulsivityFactor;
}

void Serotonin::update(TimestepDuration dt) {
    // Update mood dynamics with homeostatic regulation
    pImpl->moodLevel = std::clamp(pImpl->moodLevel + normalDist(pImpl->rng) * 0.05f, -1.0f, 1.0f);
    
    // Update social behavior based on time since last interaction
    if (pImpl->timeSinceLastSocial > 1.0f) {
        pImpl->socialBehavior = std::min(1.0f, pImpl->socialBehavior + 0.01f * static_cast<float>(dt));
    } else {
        pImpl->socialBehavior = std::max(0.0f, pImpl->socialBehavior - 0.05f * static_cast<float>(dt));
    }
    
    // Update impulsivity control
    pImpl->impulsivityControl = 0.5f + moodLevel * 0.3f;
    pImpl->impulsivityControl = std::clamp(pImpl->impulsivityControl, 0.0f, 1.0f);
    
    // Update decody state (exploration-exploitation balance)
    pImpl->decodyState = 0.5f + pImpl->moodLevel * 0.3f;
    pImpl->decodyState = std::clamp(pImpl->decodyState, 0.0f, 1.0f);
    
    // Update social state
    pImpl->socialState = pImpl->socialBehavior * 2.0f - 0.5f;
    pImpl->socialState = std::clamp(pImpl->socialState, 0.0f, 1.0f);
    
    // Update state tracking
    float currentLevel = getLevel();
    pImpl->peakLevel = std::max(pImpl->peakLevel, currentLevel);
    pImpl->minLevelObserved = std::min(pImpl->minLevelObserved, currentLevel);
    
    if (currentLevel > pImpl->maxLevel * 0.7f) {
        pImpl->timeAboveHigh += static_cast<float>(dt);
    }
    
    if (currentLevel < pImpl->maxLevel * 0.3f) {
        pImpl->timeAboveLow += static_cast<float>(dt);
    }
    
    // Update risk and reward sensitivity based on mood
    pImpl->riskTaking = std::clamp(0.5f + pImpl->moodLevel * 0.4f, 0.0f, 1.0f);
    pImpl->rewardSensitivity = std::clamp(0.5f + pImpl->moodLevel * 0.3f, 0.0f, 1.0f);
    pImpl->punishmentSensitivity = std::clamp(0.5f - pImpl->moodLevel * 0.3f, 0.0f, 1.0f);
    
    // Update time counters
    pImpl->timeSinceLastSocial += static_cast<float>(dt);
    pImpl->timeSinceLastReward += static_cast<float>(dt);
    
    NLM_LOG_INFO("Serotonin updated: mood=" + std::to_string(pImpl->moodLevel) +
                 ", social=" + std::to_string(pImpl->socialBehavior) +
                 ", decodyState=" + std::to_string(pImpl->decodyState));
}

void Serotonin::modulateMood(float moodLevel) {
    // Modulate mood level (-1 = negative, 1 = positive)
    pImpl->moodLevel = std::clamp(moodLevel, -1.0f, 1.0f);
    
    // Update social behavior based on mood
    pImpl->socialBehavior = std::clamp(moodLevel * 0.8f + 0.1f, 0.0f, 1.0f);
    
    NLM_LOG_INFO("Serotonin mood modulation: level=" + std::to_string(moodLevel) +
                 ", mood=" + std::to_string(pImpl->moodLevel) +
                 ", social=" + std::to_string(pImpl->socialBehavior));
}

float Serotonin::getMoodModulation() const {
    return pImpl->moodLevel;
}

void Serotonin::switchDecodyState() {
    // Switch between exploration (0) and exploitation (1)
    pImpl->decodyState = 1.0f - pImpl->decodyState;
    
    // Update social behavior based on decody state
    pImpl->socialBehavior = pImpl->decodyState * 0.5f;
    
    NLM_LOG_INFO("Serotonin decody state switched: newState=" + std::to_string(pImpl->decodyState));
}

bool Serotonin::isDecodySwitching() const {
    // Determine if currently switching
    return std::abs(pImpl->decodyState - 0.5f) > 0.2f;
}

float Serotonin::getImpulsivityControl() const {
    return pImpl->impulsivityControl;
}

void Serotonin::applyImpulsivityControl(float controlStrength) {
    // Apply impulsivity control (0-1)
    pImpl->impulsivityControl = std::clamp(controlStrength, 0.0f, 1.0f);
    
    // Update mood based on impulsivity control
    if (controlStrength > 0.7f) {
        pImpl->moodLevel = std::max(-0.5f, pImpl->moodLevel - 0.1f);
    }
    
    NLM_LOG_INFO("Serotonin impulsivity control applied: strength=" + std::to_string(controlStrength) +
                 ", control=" + std::to_string(pImpl->impulsivityControl));
}

float Serotonin::enhanceSocialBehavior(float socialInput) const {
    // Enhance social behavior based on serotonin level
    return socialInput * (1.0f + pImpl->socialBehavior * 0.5f);
}

NeuromodulatorInteraction Serotonin::getInteractionWith(const Neuromodulator& other) const {
    // Serotonin typically antagonizes with dopamine and norepinephrine
    const auto* other5ht = dynamic_cast<const Serotonin*>(&other);
    if (other5ht) {
        return NeuromodulatorInteraction::Additive;
    }
    
    const auto* da = dynamic_cast<const Dopamine*>(&other);
    if (da) {
        return NeuromodulatorInteraction::Antagonistic;
    }
    
    const auto* ne = dynamic_cast<const Norepinephrine*>(&other);
    if (ne) {
        return NeuromodulatorInteraction::Antagonistic;
    }
    
    const auto* ach = dynamic_cast<const Acetylcholine*>(&other);
    if (ach) {
        return NeuromodulatorInteraction::Neutral;
    }
    
    return NeuromodulatorInteraction::Neutral;
}

float Serotonin::getSynergyMultiplier(const Neuromodulator& other) const {
    if (getInteractionWith(other) == NeuromodulatorInteraction::Synergistic) {
        return 1.2f; // 20% increase in effect
    }
    return 1.0f;
}

float Serotonin::getAntagonismMultiplier(const Neuromodulator& other) const {
    if (getInteractionWith(other) == NeuromodulatorInteraction::Antagonistic) {
        return 0.6f; // 40% reduction in effect
    }
    return 1.0f;
}

float Serotonin::getHistoryAverage(unsigned int window) const {
    if (pImpl->levelHistory.empty()) return getLevel();
    
    unsigned int startIdx = std::max(0, static_cast<int>(pImpl->levelHistory.size()) - static_cast<int>(window));
    float sum = 0.0f;
    unsigned int count = 0;
    
    for (unsigned int i = startIdx; i < pImpl->levelHistory.size(); ++i) {
        sum += pImpl->levelHistory[i];
        count++;
    }
    
    return count > 0 ? sum / count : getLevel();
}

float Serotonin::getPeakLevel() const {
    return pImpl->peakLevel;
}

float Serotonin::getTimeAboveThreshold(float threshold) const {
    if (getLevel() > threshold) {
        return pImpl->timeAboveHigh;
    } else if (getLevel() < threshold * 0.5f) {
        return pImpl->timeAboveLow;
    }
    return 0.0f;
}

bool Serotonin::isWithinBiologicalRange(float level) const {
    return level >= pImpl->minLevel && level <= pImpl->maxLevel;
}

float Serotonin::getMinLevel() const {
    return pImpl->minLevel;
}

float Serotonin::getMaxLevel() const {
    return pImpl->maxLevel;
}

} // namespace nlm