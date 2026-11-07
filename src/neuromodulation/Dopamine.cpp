#include "Dopamine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float plasticityFactor;
    float learningRate;
    std::vector<float> rewardHistory;
    std::vector<float> errorHistory;
    float baselineLevel;
    float elevationDuration;
    float decayRate;
    
    Impl() : level(0.0f), plasticityFactor(1.0f), learningRate(0.01f),
             baselineLevel(0.0f), elevationDuration(1.0f), decayRate(0.1f) {}
};

Dopamine::Dopamine() : pImpl(std::make_unique<Impl>()) {}

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
    return pImpl->plasticityFactor;
}

void Dopamine::update(TimestepDuration dt) {
    // Decay dopamine level over time
    pImpl->level *= (1.0f - pImpl->decayRate * dt);
    
    // Update plasticity factor based on current level
    pImpl->plasticityFactor = 1.0f + pImpl->level * 0.5f;
}

void Dopamine::signalReward(float reward) {
    pImpl->level += reward;
    pImpl->rewardHistory.push_back(reward);
    
    // Boost plasticity temporarily
    pImpl->plasticityFactor = 1.5f;
    
    NLM_LOG_DEBUG("Dopamine signaling reward: " + std::to_string(reward));
}

void Dopamine::signalRewardPredictionError(float error) {
    // Positive error = unexpected reward = learning
    // Negative error = missed reward = adjustment
    pImpl->level += error;
    pImpl->errorHistory.push_back(error);
    
    // Adjust plasticity based on prediction error
    pImpl->plasticityFactor *= (1.0f + error * 0.1f);
    
    NLM_LOG_DEBUG("Dopamine signaling prediction error: " + std::to_string(error));
}

const std::vector<float>& Dopamine::getRewardHistory() const {
    return pImpl->rewardHistory;
}

void Dopamine::clearRewardHistory() {
    pImpl->rewardHistory.clear();
    pImpl->errorHistory.clear();
}

void Dopamine::setLearningRate(float rate) {
    pImpl->learningRate = rate;
}

float Dopamine::getLearningRate() const {
    return pImpl->learningRate;
}

void Dopamine::applyModulation(float globalInhibition, float globalExcitation) {
    // Apply dopamine effects on neural excitability
    float excitabilityMod = pImpl->level * 0.5f;
    if (excitabilityMod > 0.0f) {
        // Dopamine increases excitability (lowers effective threshold)
        // This is a placeholder for actual neuromodulatory effects
        NLM_LOG_DEBUG("Applying dopamine modulation: level=" + std::to_string(pImpl->level) +
                     ", excitability_mod=" + std::to_string(excitabilityMod));
    }
}

} // namespace nlm