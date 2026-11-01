#include "Dopamine.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float explorationTarget;
    float predictionError;
    float learningRate;
    float memoryTrace;
    std::vector<float> history;
    
    Impl() : level(0.0f), baseline(0.05f), peak(1.0f), decayRate(0.05f), releaseRate(1.0f),
             explorationTarget(0.3f), predictionError(0.0f), learningRate(0.1f),
             memoryTrace(0.0f) {}
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
    // More sophisticated dopamine-modulated plasticity factor
    // Based on both current level and prediction error
    float levelFactor = pImpl->level * 0.5f;
    float errorFactor = std::abs(pImpl->predictionError) * 0.3f;
    float memoryFactor = pImpl->memoryTrace * 0.2f;
    
    return 0.5f + levelFactor + errorFactor + memoryFactor;
}

void Dopamine::update(TimestepDuration dt) {
    // Realistic dopamine dynamics
    float dt_f = static_cast<float>(dt);
    
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * dt_f);
    
    // Update memory trace (slow adaptation)
    pImpl->memoryTrace = pImpl->memoryTrace * 0.95f + pImpl->predictionError * 0.05f;
    pImpl->memoryTrace = std::clamp(pImpl->memoryTrace, -1.0f, 1.0f);
    
    // Keep history for analysis
    pImpl->history.push_back(pImpl->level);
    if (pImpl->history.size() > 1000) {
        pImpl->history.erase(pImpl->history.begin());
    }
}

void Dopamine::signalReward(float reward) {
    // More realistic reward signaling
    // Reward magnitude affects both level and prediction error
    float rewardImpact = std::min(std::abs(reward), 1.0f) * pImpl->releaseRate;
    
    if (reward > 0.0f) {
        // Positive reward: burst of dopamine (reward prediction)
        pImpl->level = std::min(pImpl->peak, pImpl->level + rewardImpact * 2.0f);
        pImpl->predictionError = reward * pImpl->learningRate;
    } else {
        // Negative reward: dip in dopamine (punishment prediction)
        pImpl->level = std::max(pImpl->baseline, pImpl->level - rewardImpact * 0.5f);
        pImpl->predictionError = reward * pImpl->learningRate * 0.5f;
    }
    
    // Ensure level is within bounds
    pImpl->level = std::clamp(pImpl->level, pImpl->baseline, pImpl->peak);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Reward prediction error signaling (realistic)
    // Error can be positive (better than expected) or negative (worse than expected)
    
    // Scale error to dopamine impact range
    float errorImpact = std::min(std::abs(error), 1.0f) * pImpl->releaseRate;
    
    if (error > 0.0f) {
        // Positive prediction error (better outcome than expected)
        // dopamine increase reinforces the predictive model
        pImpl->level = std::min(pImpl->peak, pImpl->level + errorImpact * 1.5f);
    } else if (error < 0.0f) {
        // Negative prediction error (worse outcome than expected)
        // dopamine decrease signals prediction update needed
        pImpl->level = std::max(pImpl->baseline, pImpl->level - errorImpact * 1.0f);
    }
    
    pImpl->predictionError = error;
}

float Dopamine::getExplorationTarget() const {
    // Calculate exploration target based on novelty and curiosity
    // Higher dopamine levels encourage exploration when novelty is high
    return pImpl->explorationTarget * pImpl->level;
}

std::vector<float> Dopamine::getHistory() const {
    return pImpl->history;
}

void Dopamine::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
