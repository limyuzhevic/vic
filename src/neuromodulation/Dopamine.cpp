#include "Dopamine.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    // Dopamine baseline and modulation
    float baselineLevel;
    float currentLevel;
    float peakLevel;
    float decayRate;
    float reboundRate;
    
    // Reward prediction error tracking
    float predictionError;
    float expectedReward;
    float predictionErrorRate;
    
    // Reward signaling
    float lastReward;
    float rewardHistory[100];  // Rolling window
    size_t historyIndex;
    
    // Plasticity modulation factors
    float ltpModulation;
    float ltdModulation;
    float overallPlasticityFactor;
    
    // Time tracking
    float timeSinceLastReward;
    float timeSinceLastPredictionError;
    
    Impl()
        : baselineLevel(0.1f)
        , currentLevel(0.1f)
        , peakLevel(0.0f)
        , decayRate(0.01f)
        , reboundRate(0.005f)
        , predictionError(0.0f)
        , expectedReward(0.0f)
        , predictionErrorRate(0.0f)
        , lastReward(0.0f)
        , ltpModulation(1.0f)
        , ltdModulation(1.0f)
        , overallPlasticityFactor(1.0f)
        , timeSinceLastReward(0.0f)
        , timeSinceLastPredictionError(0.0f)
    {
        for (int i = 0; i < 100; ++i) {
            rewardHistory[i] = 0.0f;
        }
        historyIndex = 0;
    }
};

Dopamine::Dopamine() : pImpl(new Impl()) {}
Dopamine::~Dopamine() { delete pImpl; }

const char* Dopamine::getName() const {
    return "Dopamine";
}

float Dopamine::getLevel() const {
    return pImpl->currentLevel;
}

void Dopamine::setLevel(float level) {
    pImpl->currentLevel = std::clamp(level, -1.0f, 2.0f);  // Allow negative (LTD) and high positive (LTP)
}

float Dopamine::getPlasticityFactor() const {
    return pImpl->overallPlasticityFactor;
}

void Dopamine::update(TimestepDuration dt) {
    // Natural decay towards baseline
    float decayAmount = pImpl->decayRate * dt;
    pImpl->currentLevel = std::max(pImpl->baselineLevel, 
                                  pImpl->currentLevel - decayAmount);
    
    // Recovery from high peaks
    if (pImpl->currentLevel < pImpl->baselineLevel) {
        float recoverAmount = pImpl->reboundRate * dt;
        pImpl->currentLevel = std::min(pImpl->baselineLevel,
                                      pImpl->currentLevel + recoverAmount);
    }
    
    // Time tracking
    pImpl->timeSinceLastReward += dt;
    pImpl->timeSinceLastPredictionError += dt;
}

void Dopamine::signalReward(float reward) {
    pImpl->lastReward = reward;
    pImpl->timeSinceLastReward = 0.0f;
    
    // Update reward history
    pImpl->rewardHistory[pImpl->historyIndex] = reward;
    pImpl->historyIndex = (pImpl->historyIndex + 1) % 100;
    
    // Compute prediction error
    float predictionError = reward - pImpl->expectedReward;
    
    // Update expected reward (exponential moving average)
    pImpl->expectedReward = 0.95f * pImpl->expectedReward + 0.05f * reward;
    
    // Compute prediction error rate
    pImpl->predictionError = predictionError;
    pImpl->predictionErrorRate = std::abs(predictionError) / std::max(pImpl->timeSinceLastPredictionError + 0.001f, 1.0f);
    
    // Dopamine response: Phasic burst for unexpected reward (prediction error)
    float phasicResponse = predictionError * 0.5f;
    
    // Sustained component: average reward rate
    float avgReward = 0.0f;
    for (int i = 0; i < 100; ++i) {
        avgReward += pImpl->rewardHistory[i];
    }
    avgReward /= 100;
    
    float tonicResponse = avgReward * 0.1f;
    
    // Total dopamine level
    float newLevel = pImpl->baselineLevel + phasicResponse + tonicResponse;
    newLevel = std::clamp(newLevel, -1.0f, 2.0f);
    
    // Update peak for plasticity calculations
    if (newLevel > pImpl->peakLevel) {
        pImpl->peakLevel = newLevel;
    }
    
    setLevel(newLevel);
    
    NLM_LOG_INFO("Dopamine: reward=" + std::to_string(reward) + 
                 " error=" + std::to_string(predictionError) +
                 " level=" + std::to_string(getLevel()));
}

void Dopamine::signalRewardPredictionError(float error) {
    pImpl->predictionError = error;
    pImpl->predictionErrorRate = std::abs(error);
    pImpl->timeSinceLastPredictionError = 0.0f;
    
    // Phasic dopamine for prediction error
    float phasicResponse = error * 0.5f;
    
    float newLevel = pImpl->baselineLevel + phasicResponse;
    newLevel = std::clamp(newLevel, -1.0f, 2.0f);
    
    setLevel(newLevel);
    
    NLM_LOG_INFO("Dopamine: prediction error=" + std::to_string(error) +
                 " level=" + std::to_string(getLevel()));
}

} // namespace nlm