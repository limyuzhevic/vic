#include "Neuromodulator.hpp"
#include <algorithm>
#include <random>
#include <numeric>

namespace nlm {

// Original implementations (kept for backward compatibility)

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f) {}
};

Dopamine::Dopamine() : pImpl(new Impl) {}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "DA";
}

float Dopamine::getLevel() const {
    return pImpl->level;
}

void Dopamine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Dopamine::getPlasticityFactor() const {
    // TODO PHASE 2: Implement real dopamine-modulated plasticity factor
    // PLACEHOLDER: Higher dopamine increases plasticity
    return 0.5f + 0.5f * pImpl->level;
}

void Dopamine::update(TimestepDuration dt) {
    // TODO PHASE 2: Implement real dopamine dynamics
    // PLACEHOLDER: Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Dopamine::signalReward(float reward) {
    // TODO PHASE 2: Implement real reward signaling
    // PLACEHOLDER: Burst of dopamine on reward
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate);
}

void Dopamine::signalRewardPredictionError(float error) {
    // TODO PHASE 2: Implement reward prediction error signaling
    // PLACEHOLDER: Dopamine responds to prediction error
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate);
}

// Enhanced Dopamine implementation

struct DopamineEnhanced::Impl {
    float level;
    float baseline;
    float integrationRate;
    float explorationBonus;
    float rewardDecayRate;
    float predictionErrorDecayRate;
    float recentRewards[100];
    float recentPredictionErrors[100];
    SimulationStep historyIndex;
    
    Impl() : level(0.0f), baseline(0.0f), integrationRate(0.05f), explorationBonus(0.1f),
             rewardDecayRate(0.02f), predictionErrorDecayRate(0.03f), historyIndex(0) {
        for (int i = 0; i < 100; ++i) {
            recentRewards[i] = 0.0f;
            recentPredictionErrors[i] = 0.0f;
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
    pImpl->baseline = pImpl->level * 0.1f;
}

float DopamineEnhanced::getPlasticityFactor() const {
    return 0.5f + 0.5f * pImpl->level;
}

void DopamineEnhanced::update(TimestepDuration dt) {
    float timeStep = static_cast<float>(dt);
    
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->rewardDecayRate * timeStep);
    
    // Integrate prediction error for learning
    float predictionError = getPredictionError();
    pImpl->level += predictionError * pImpl->predictionErrorDecayRate * timeStep;
    
    // Apply exploration bonus
    float recentExperience = getRecentExperience();
    if (recentExperience > 0.5f) {
        pImpl->level *= (1.0f - pImpl->explorationBonus * recentExperience);
    }
    
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
}

void DopamineEnhanced::signalReward(float reward) {
    pImpl->recentRewards[pImpl->historyIndex] = reward;
    pImpl->historyIndex = (pImpl->historyIndex + 1) % 100;
    
    pImpl->level = std::min(pImpl->level + reward * pImpl->integrationRate, 1.0f);
}

void DopamineEnhanced::signalRewardPredictionError(float error) {
    pImpl->recentPredictionErrors[pImpl->historyIndex] = error;
    
    pImpl->level += error * pImpl->integrationRate * 0.5f;
    
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
}

float DopamineEnhanced::getPredictionError() const {
    if (pImpl->historyIndex == 0 && pImpl->recentPredictionErrors[0] == 0.0f) {
        return 0.0f;
    }
    
    float sum = 0.0f;
    size_t count = 0;
    for (SimulationStep i = 0; i < 100; ++i) {
        if (pImpl->recentPredictionErrors[i] != 0.0f || i == static_cast<size_t>(pImpl->historyIndex)) {
            sum += pImpl->recentPredictionErrors[i];
            count++;
        }
    }
    
    return count > 0 ? sum / count : 0.0f;
}

float DopamineEnhanced::getRecentExperience() const {
    if (pImpl->historyIndex == 0 && pImpl->recentRewards[0] == 0.0f) {
        return 0.0f;
    }
    
    float sum = 0.0f;
    size_t count = 0;
    for (SimulationStep i = 0; i < 100; ++i) {
        if (pImpl->recentRewards[i] != 0.0f || i == static_cast<size_t>(pImpl->historyIndex)) {
            sum += pImpl->recentRewards[i];
            count++;
        }
    }
    
    return count > 0 ? sum / count : 0.0f;
}

// Curiosity Enhanced implementation

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
    return 0.5f + 0.5f * pImpl->level * pImpl->noveltySensitivity;
}

void CuriosityEnhanced::update(TimestepDuration dt, float novelty, float predictionError, float dtTime) {
    float timeStep = static_cast<float>(dtTime);
    
    float curiosityDrive = novelty * pImpl->noveltySensitivity + 
                          std::abs(predictionError) * pImpl->uncertaintySensitivity;
    
    if (curiosityDrive > pImpl->explorationThreshold) {
        pImpl->level = std::min(pImpl->level + curiosityDrive * pImpl->learningRate * timeStep, 1.0f);
    } else {
        pImpl->level = std::max(0.0f, pImpl->level - 0.01f * timeStep);
    }
}

// Novelty Enhanced implementation

struct NoveltyEnhanced::Impl {
    float sensitivity;
    float decayRate;
    float patternHistory[50];
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
    float timeStep = static_cast<float>(dt);
    
    pImpl->sensitivity = std::max(0.1f, pImpl->sensitivity - pImpl->decayRate * timeStep);
}

} // namespace nlm