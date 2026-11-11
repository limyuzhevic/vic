// Neuromodulator.cpp - Enhanced for Phase 6

#include "Neuromodulator.hpp"
#include <algorithm>
#include <random>

namespace nlm {

struct DopamineEnhanced::Impl {
    float level;
    float baseline;
    float rewardHistory[100];  // Track recent rewards
    float predictionErrorHistory[100];  // Track recent prediction errors
    SimulationStep historyIndex;
    float integrationRate;  // Rate of adaptation
    float explorationBonus;  // Bonus for exploration
    float rewardDecayRate;  // Rate of reward decay
    float predictionErrorDecayRate;  // Rate of prediction error decay
    
    Impl() : level(0.0f), baseline(0.0f), historyIndex(0),
             integrationRate(0.05f), explorationBonus(0.1f),
             rewardDecayRate(0.02f), predictionErrorDecayRate(0.03f) {
        for (int i = 0; i < 100; ++i) {
            rewardHistory[i] = 0.0f;
            predictionErrorHistory[i] = 0.0f;
        }
    }
};

DopamineEnhanced::DopamineEnhanced() : pImpl(new Impl) {}

DopamineEnhanced::~DopamineEnhanced() = default;

const char* DopamineEnhanced::getName() const {
    return "DA_Enhanced";
}

float DopamineEnhanced::getLevel() const {
    return pImpl->level;
}

void DopamineEnhanced::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    pImpl->baseline = pImpl->level * 0.1f;  // Baseline is 10% of current level
}

float DopamineEnhanced::getPlasticityFactor() const {
    // Enhanced plasticity modulation based on dopamine level
    // Higher dopamine -> higher plasticity
    return 0.5f + 0.5f * pImpl->level;
}

void DopamineEnhanced::update(TimestepDuration dt) {
    // Enhanced dopamine dynamics with adaptive learning
    float timeStep = static_cast<float>(dt);
    
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->rewardDecayRate * timeStep);
    
    // Integrate prediction error for learning
    float predictionError = getPredictionError();
    pImpl->level += predictionError * pImpl->predictionErrorDecayRate * timeStep;
    
    // Apply exploration bonus based on recent experience
    float recentExperience = getRecentExperience();
    if (recentExperience > 0.5f) {
        // High recent experience -> lower exploration bonus
        pImpl->level *= (1.0f - pImpl->explorationBonus * recentExperience);
    }
    
    // Clamp to reasonable range
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
}

void DopamineEnhanced::signalReward(float reward) {
    // Record reward in history for later use
    pImpl->rewardHistory[pImpl->historyIndex] = reward;
    pImpl->historyIndex = (pImpl->historyIndex + 1) % 100;
    
    // Burst of dopamine on reward
    pImpl->level = std::min(pImpl->level + reward * pImpl->integrationRate, 1.0f);
}

void DopamineEnhanced::signalRewardPredictionError(float error) {
    // Record prediction error in history
    pImpl->predictionErrorHistory[pImpl->historyIndex] = error;
    
    // Adapt based on prediction error
    // Positive error (worse than expected) -> increase level
    // Negative error (better than expected) -> decrease level
    pImpl->level += error * pImpl->integrationRate * 0.5f;
    
    // Clamp to prevent runaway
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
}

float DopamineEnhanced::getPredictionError() const {
    // Calculate average prediction error from history
    float sum = 0.0f;
    for (int i = 0; i < 100; ++i) {
        sum += pImpl->predictionErrorHistory[i];
    }
    return sum / 100.0f;
}

float DopamineEnhanced::getRecentExperience() const {
    // Calculate average recent experience from reward history
    float sum = 0.0f;
    int count = 0;
    for (int i = 0; i < 100; ++i) {
        if (pImpl->rewardHistory[i] > 0.0f) {
            sum += pImpl->rewardHistory[i];
            count++;
        }
    }
    return count > 0 ? sum / count : 0.0f;
}

// Enhanced Curiosity system with better exploration integration
struct CuriosityEnhanced::Impl {
    float level;
    float noveltySensitivity;
    float uncertaintySensitivity;
    float explorationThreshold;
    float learningRate;
    
    Impl() : level(0.0f), noveltySensitivity(0.8f), uncertaintySensitivity(0.6f),
             explorationThreshold(0.3f), learningRate(0.1f) {}
};

CuriosityEnhanced::CuriosityEnhanced() : pImpl(new Impl) {}

CuriosityEnhanced::~CuriosityEnhanced() = default;

const char* CuriosityEnhanced::getName() const {
    return "CURIOSITY_Enhanced";
}

float CuriosityEnhanced::getLevel() const {
    return pImpl->level;
}

void CuriosityEnhanced::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float CuriosityEnhanced::getPlasticityFactor() const {
    // Curiosity enhances plasticity for exploration
    return 0.5f + 0.5f * pImpl->level * pImpl->noveltySensitivity;
}

void CuriosityEnhanced::update(TimestepDuration dt, float novelty, float predictionError, float dtTime) {
    // Enhanced curiosity dynamics
    float timeStep = static_cast<float>(dtTime);
    
    // Integrate novelty and prediction error
    float curiosityDrive = novelty * pImpl->noveltySensitivity + 
                          std::abs(predictionError) * pImpl->uncertaintySensitivity;
    
    // Adapt level based on curiosity drive
    if (curiosityDrive > pImpl->explorationThreshold) {
        // High curiosity drive -> increase curiosity level
        pImpl->level = std::min(pImpl->level + curiosityDrive * pImpl->learningRate * timeStep, 1.0f);
    } else {
        // Low curiosity drive -> decay towards baseline
        pImpl->level = std::max(0.0f, pImpl->level - 0.01f * timeStep);
    }
}

float CuriosityEnhanced::getNoveltyLevel() const {
    return level;  // Use level as novelty level
}

// Enhanced Novelty detection with pattern matching
struct NoveltyEnhanced::Impl {
    float sensitivity;
    float decayRate;
    float patternHistory[50];  // Store recent patterns
    SimulationStep historyIndex;
    
    Impl() : sensitivity(0.7f), decayRate(0.1f), historyIndex(0) {
        for (int i = 0; i < 50; ++i) {
            patternHistory[i] = 0.0f;
        }
    }
};

NoveltyEnhanced::NoveltyEnhanced() : pImpl(new Impl) {}

NoveltyEnhanced::~NoveltyEnhanced() = default;

const char* NoveltyEnhanced::getName() const {
    return "NOVELTY_Enhanced";
}

float NoveltyEnhanced::getLevel() const {
    return pImpl->sensitivity;
}

void NoveltyEnhanced::setLevel(float level) {
    pImpl->sensitivity = std::clamp(level, 0.0f, 1.0f);
}

float NoveltyEnhanced::getPlasticityFactor() const {
    return pImpl->sensitivity;
}

void NoveltyEnhanced::update(TimestepDuration dt) {
    // Novelty detection based on pattern changes
    float timeStep = static_cast<float>(dt);
    
    // Decay sensitivity over time
    pImpl->sensitivity = std::max(0.1f, pImpl->sensitivity - pImpl->decayRate * timeStep);
    
    // TODO: Implement actual novelty detection based on sensory input patterns
    // For now, return current sensitivity level
}