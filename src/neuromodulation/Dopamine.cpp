#include "Dopamine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Dopamine::Impl {
    class Brain* brain;
    float level;
    float predictionErrorLevel;
    float rewardHistory[100];
    int historyIndex;
    int historySize;
    float decayRate;
    float plasticityModulator;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , predictionErrorLevel(0.0f)
        , historyIndex(0)
        , historySize(0)
        , decayRate(0.05f)
        , plasticityModulator(1.0f) {
        for (int i = 0; i < 100; ++i) rewardHistory[i] = 0.0f;
    }
};

Dopamine::Dopamine() : pImpl(new Impl) {}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "Dopamine";
}

float Dopamine::getLevel() const {
    return pImpl->level;
}

void Dopamine::setLevel(float level) {
    pImpl->level = level;
}

float Dopamine::getPlasticityFactor() const {
    return pImpl->plasticityModulator;
}

void Dopamine::update(TimestepDuration dt) {
    // Decay dopamine level over time
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Dopamine::signalReward(float reward) {
    // Add reward to history
    pImpl->historyIndex = (pImpl->historyIndex + 1) % 100;
    pImpl->rewardHistory[pImpl->historyIndex] = reward;
    if (pImpl->historySize < 100) pImpl->historySize++;
    
    // Update dopamine level based on reward
    // Positive reward increases dopamine, negative reward decreases it
    float rewardSignal = std::max(0.0f, reward) * 0.5f;  // Scale factor
    pImpl->level += rewardSignal;
    
    // Limit maximum dopamine level
    pImpl->level = std::min(1.0f, pImpl->level);
    
    NLM_LOG_INFO("Dopamine signal: " + std::to_string(reward) + " -> level: " + std::to_string(pImpl->level));
}

void Dopamine::signalRewardPredictionError(float error) {
    // Update prediction error level
    pImpl->predictionErrorLevel = std::abs(error) * 0.5f;
    
    // Update plasticity modulator based on prediction error
    // Positive prediction error increases plasticity, negative error decreases it
    if (error > 0.0f) {
        pImpl->plasticityModulator = std::min(2.0f, pImpl->plasticityModulator + 0.1f);
    } else {
        pImpl->plasticityModulator = std::max(0.2f, pImpl->plasticityModulator - 0.1f);
    }
    
    NLM_LOG_INFO("Dopamine prediction error: " + std::to_string(error) + ", plasticity modulator: " + std::to_string(pImpl->plasticityModulator));
}

} // namespace nlm