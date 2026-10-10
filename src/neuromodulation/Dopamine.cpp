#include "Dopamine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baselineLevel;
    float rewardPredictionError;
    float expectedValue;
    float rewardMemory;
    float plasticityModulator;
};

Dopamine::Dopamine() : pImpl(new Impl()) {
    pImpl->level = 0.0f;
    pImpl->baselineLevel = 0.01f;
    pImpl->rewardPredictionError = 0.0f;
    pImpl->expectedValue = 0.0f;
    pImpl->rewardMemory = 0.0f;
    pImpl->plasticityModulator = 1.0f;
}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "Dopamine (DA)";
}

float Dopamine::getLevel() const {
    return pImpl->level;
}

void Dopamine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    
    // Dopamine affects reward prediction error
    pImpl->rewardPredictionError = pImpl->level * 2.0f - 1.0f;
    
    // Dopamine modulates expected value
    pImpl->expectedValue = pImpl->level;
    
    // Dopamine strongly affects plasticity
    pImpl->plasticityModulator = 0.5f + pImpl->level * 1.5f;
    
    NLM_LOG_DEBUG("Dopamine level set to " + std::to_string(level) + 
                  ", RPE: " + std::to_string(pImpl->rewardPredictionError) +
                  ", plasticity: " + std::to_string(pImpl->plasticityModulator));
}

float Dopamine::getPlasticityFactor() const {
    return pImpl->plasticityModulator;
}

void Dopamine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level *= std::pow(0.98f, dt);
    if (pImpl->level < pImpl->baselineLevel) {
        pImpl->level = pImpl->baselineLevel;
    }
    
    // Update reward memory with exponential decay
    pImpl->rewardMemory *= std::pow(0.9f, dt);
}

void Dopamine::signalReward(float reward) {
    // Positive reward increases dopamine
    setLevel(std::min(pImpl->level + reward * 0.5f, 1.0f));
    
    // Update reward memory
    pImpl->rewardMemory += reward * 0.1f;
    NLM_LOG_INFO("Dopamine reward signal: " + std::to_string(reward) + 
                 ", level: " + std::to_string(pImpl->level));
}

void Dopamine::signalRewardPredictionError(float error) {
    // RPE directly affects dopamine level
    float rpeLevel = (error + 1.0f) * 0.5f;  // Normalize error to [0, 1]
    rpeLevel = std::clamp(rpeLevel, 0.0f, 1.0f);
    setLevel(rpeLevel);
    
    NLM_LOG_DEBUG("Dopamine RPE signal: " + std::to_string(error) + 
                  ", level: " + std::to_string(pImpl->level));
}

void Dopamine::signalPleasurableExperience() {
    // Pleasurable experiences strengthen reward pathways
    setLevel(std::min(pImpl->level + 0.1f, 1.0f));
}

} // namespace nlm