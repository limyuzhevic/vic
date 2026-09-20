#include "Dopamine.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>

namespace nlm {

struct Dopamine::Impl {
    float currentLevel;
    float plasticityFactor;
    float temporalIntegral;  // Integral of error signals over time
    float predictedReward;
    float lastError;
    float errorTimeConstant;
    float dopamineTimeConstant;
    float ltpWeight;  // Long-term potentiation weight
    float ltdWeight;  // Long-term depression weight
    
    Impl() : currentLevel(0.0f), plasticityFactor(1.0f), temporalIntegral(0.0f),
             predictedReward(0.0f), lastError(0.0f), errorTimeConstant(0.1f),
             dopamineTimeConstant(10.0f), ltpWeight(0.01f), ltdWeight(0.008f) {}
};

Dopamine::Dopamine() : pImpl(new Impl) {}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "Dopamine";
}

float Dopamine::getLevel() const {
    return pImpl->currentLevel;
}

void Dopamine::setLevel(float level) {
    pImpl->currentLevel = std::clamp(level, 0.0f, 10.0f);
    // Update plasticity factor based on current level
    pImpl->plasticityFactor = 1.0f + pImpl->currentLevel * 0.1f;
}

float Dopamine::getPlasticityFactor() const {
    return pImpl->plasticityFactor;
}

void Dopamine::update(TimestepDuration dt) {
    // Dopamine dynamics: decay over time unless reinforced
    pImpl->currentLevel *= std::exp(-dt * pImpl->dopamineTimeConstant);
    
    // Integrate prediction error signals
    pImpl->temporalIntegral *= std::exp(-dt * pImpl->errorTimeConstant);
    
    // Decay plasticity weights
    pImpl->ltpWeight *= std::exp(-dt * 0.01f);
    pImpl->ltdWeight *= std::exp(-dt * 0.01f);
}

void Dopamine::signalReward(float reward) {
    // Simple reward signal increases dopamine
    float rewardSignal = reward * 2.0f;
    pImpl->currentLevel += rewardSignal;
    
    // Reset temporal integral for new reward
    pImpl->temporalIntegral = rewardSignal;
    
    NLM_LOG_DEBUG("Dopamine signaling reward: " + std::to_string(reward) +
                 " (level: " + std::to_string(pImpl->currentLevel) + ")");
}

void Dopamine::signalRewardPredictionError(float error) {
    // Reward prediction error is primary driver of learning
    pImpl->lastError = error;
    pImpl->currentLevel += error * 5.0f;  // Error signal scaled
    
    // Integrate error for temporal credit assignment
    pImpl->temporalIntegral += error * 0.001f;
    
    // Update plasticity weights based on error
    if (error > 0.0f) {
        // Positive error (better than expected) -> strengthen
        pImpl->ltpWeight += error * 0.001f;
        pImpl->plasticityFactor *= (1.0f + error * 0.01f);
    } else if (error < 0.0f) {
        // Negative error (worse than expected) -> weaken
        pImpl->ltdWeight += std::abs(error) * 0.001f;
        pImpl->plasticityFactor *= (1.0f - std::abs(error) * 0.01f);
    }
    
    NLM_LOG_DEBUG("Dopamine signaling prediction error: " + std::to_string(error) +
                 " (level: " + std::to_string(pImpl->currentLevel) + ")");
}

void Dopamine::computePredictionError(float predicted, float actual) {
    float error = actual - predicted;
    signalRewardPredictionError(error);
}

float Dopamine::getTemporalIntegral() const {
    return pImpl->temporalIntegral;
}

void Dopamine::resetTemporalIntegral() {
    pImpl->temporalIntegral = 0.0f;
}

float Dopamine::getLTDWeight() const {
    return pImpl->ltdWeight;
}

float Dopamine::getLTPWeight() const {
    return pImpl->ltpWeight;
}

} // namespace nlm