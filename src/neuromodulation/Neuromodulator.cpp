#pragma once

#include "Dopamine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float recoveryRate;
    float tonicLevel;
    float predictionErrorResponse;
    float rewardResponse;
    
    // State tracking
    float lastBurstTime;
    float timeSinceLastBurst;
    float eligibilityTrace;
    bool burstMode;
    
    // Integration with other systems
    float attentionModulation;
    float workingMemoryModulation;
    float episodicMemoryModulation;
    float conceptFormationModulation;
    float developmentModulation;
    
    Impl() : level(0.0f), baseline(0.05f), peak(1.0f), decayRate(0.1f), 
             releaseRate(2.0f), recoveryRate(0.05f), tonicLevel(0.05f),
             predictionErrorResponse(1.0f), rewardResponse(1.5f),
             lastBurstTime(0.0f), timeSinceLastBurst(0.0f),
             eligibilityTrace(0.0f), burstMode(false),
             attentionModulation(1.0f), workingMemoryModulation(1.0f),
             episodicMemoryModulation(1.0f), conceptFormationModulation(1.0f),
             developmentModulation(1.0f) {}
};

Dopamine::Dopamine() : pImpl(new Impl) {
    NLM_LOG_INFO("Dopamine neuromodulation system initialized");
}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "Dopamine";
}

float Dopamine::getLevel() const {
    return pImpl->level;
}

void Dopamine::setLevel(float level) {
    // Allow negative levels for prediction error signaling
    pImpl->level = std::clamp(level, -1.0f, 1.0f);
    pImpl->timeSinceLastBurst = 0.0f;
}

void Dopamine::signalReward(float reward) {
    // Phasic dopamine burst for unexpected rewards
    float burstAmount = reward * pImpl->rewardResponse;
    
    // Combined with existing level for burst effect
    float newLevel = pImpl->level + burstAmount;
    setLevel(newLevel);
    
    // Mark as burst for enhanced effects
    pImpl->burstMode = true;
    
    NLM_LOG_DEBUG("Dopamine burst on reward: " + std::to_string(reward) + ", new level: " + std::to_string(pImpl->level));
}

void Dopamine::signalPredictionError(float error) {
    // Dopamine responds to prediction errors
    float errorSignal = error * pImpl->predictionErrorResponse;
    
    // For negative prediction error (unexpected lack of reward), can decrease dopamine
    float newLevel = pImpl->level + errorSignal;
    setLevel(newLevel);
    
    NLM_LOG_DEBUG("Dopamine prediction error signal: " + std::to_string(error) + ", new level: " + std::to_string(pImpl->level));
}

void Dopamine::setBaseline(float baseline) {
    pImpl->baseline = std::clamp(baseline, 0.0f, 0.5f);
    if (pImpl->level < pImpl->baseline) {
        pImpl->level = pImpl->baseline;
    }
}

void Dopamine::setTonicLevel(float tonic) {
    pImpl->tonicLevel = std::clamp(tonic, 0.0f, 0.2f);
}

void Dopamine::setBurstDuration(float duration) {
    // Store burst duration for enhanced effects
    pImpl->releaseRate = std::clamp(duration, 0.5f, 3.0f);
}

float Dopamine::getPlasticityFactor() const {
    // Enhanced plasticity modulation based on dopamine level
    if (pImpl->level < 0) {
        // Negative dopamine strongly suppresses plasticity
        return std::max(0.0f, 0.3f + 0.5f * pImpl->level);
    } else if (pImpl->level > 0) {
        // Positive dopamine enhances plasticity, especially in burst mode
        float burstMultiplier = pImpl->burstMode ? 2.0f : 1.0f;
        return 0.3f + 0.7f * pImpl->level * burstMultiplier;
    } else {
        // Baseline plasticity
        return 0.5f;
    }
}

float Dopamine::getExcitabilityModulation() const {
    // Dopamine's effect on neural excitability
    if (pImpl->level > 0) {
        // Positive dopamine increases excitability (lowers threshold)
        return 1.0f - 0.3f * pImpl->level;
    } else {
        // Negative dopamine decreases excitability
        return 1.0f + 0.2f * std::abs(pImpl->level);
    }
}

float Dopamine::getWorkingMemoryModulation() const {
    // Dopamine's effect on working memory consolidation
    return pImpl->workingMemoryModulation;
}

float Dopamine::getEpisodicMemoryModulation() const {
    // Dopamine's effect on episodic memory encoding
    return pImpl->episodicMemoryModulation;
}

float Dopamine::getConceptFormationModulation() const {
    // Dopamine's effect on concept formation and pattern detection
    return pImpl->conceptFormationModulation;
}

float Dopamine::getAttentionModulation() const {
    // Dopamine's effect on attentional selection
    return pImpl->attentionModulation;
}

void Dopamine::setWorkingMemoryModulation(float modulation) {
    pImpl->workingMemoryModulation = std::clamp(modulation, 0.5f, 2.0f);
}

void Dopamine::setEpisodicMemoryModulation(float modulation) {
    pImpl->episodicMemoryModulation = std::clamp(modulation, 0.5f, 2.0f);
}

void Dopamine::setConceptFormationModulation(float modulation) {
    pImpl->conceptFormationModulation = std::clamp(modulation, 0.5f, 2.0f);
}

void Dopamine::setAttentionModulation(float modulation) {
    pImpl->attentionModulation = std::clamp(modulation, 0.5f, 2.0f);
}

void Dopamine::update(TimestepDuration dt) {
    // Realistic dopamine dynamics
    float timeStep = static_cast<float>(dt);
    
    // Update time since last burst
    pImpl->timeSinceLastBurst += timeStep;
    
    // Decay towards baseline
    if (pImpl->level > pImpl->baseline) {
        // Faster decay from high levels
        float decay = pImpl->decayRate * (2.0f - pImpl->level);
        pImpl->level = std::max(pImpl->baseline, pImpl->level - decay * timeStep);
    } else {
        // Slow recovery to baseline
        float recovery = pImpl->recoveryRate;
        pImpl->level = std::min(pImpl->baseline, pImpl->level + recovery * timeStep);
    }
    
    // Reduce burst mode over time
    if (pImpl->timeSinceLastBurst > 0.5f) {
        pImpl->burstMode = false;
    }
    
    // Decay eligibility trace
    if (std::abs(pImpl->eligibilityTrace) > 0.001f) {
        pImpl->eligibilityTrace *= (1.0f - pImpl->decayRate * timeStep);
    }
}

bool Dopamine::isBurstMode() const {
    return pImpl->burstMode && pImpl->timeSinceLastBurst < 0.3f;
}

float Dopamine::getPredictionErrorResponse() const {
    return pImpl->predictionErrorResponse;
}

float Dopamine::getRewardResponse() const {
    return pImpl->rewardResponse;
}

void Dopamine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->burstMode = false;
    pImpl->timeSinceLastBurst = 0.0f;
    pImpl->eligibilityTrace = 0.0f;
    NLM_LOG_INFO("Dopamine system reset");
}

} // namespace nlm