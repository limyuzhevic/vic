#include "Dopamine.hpp"
#include "../brain/Brain.hpp"
#include <algorithm>
#include <unordered_map>
#include <cmath>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float facilitationRate;
    
    // Dopamine affects neural excitability
    float excitabilityFactor;
    float firingRateModulation;
    
    // For reward prediction error
    float rewardPredictionError;
    float learningRate;
    
    // Track dopamine-sensitive neurons
    std::unordered_map<uint64_t, float> dopamineSensitiveNeurons;
    std::unordered_map<uint64_t, float> firingHistory;
    
    // For motivation and action selection
    float motivationLevel;
    float actionBias;
    
    Impl() : 
        level(0.0f), 
        baseline(0.0f), 
        peak(1.0f), 
        decayRate(0.1f), 
        releaseRate(1.5f), 
        facilitationRate(0.4f),
        excitabilityFactor(1.0f),
        firingRateModulation(1.0f),
        rewardPredictionError(0.0f),
        learningRate(0.01f),
        motivationLevel(0.0f),
        actionBias(0.0f) {}
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
    // Enhanced dopamine plasticity factor
    return 0.5f + 0.5f * pImpl->level * pImpl->rewardPredictionError;
}

void Dopamine::update(TimestepDuration dt) {
    // Dopamine dynamics with more sophisticated modeling
    pImpl->level = std::max(pImpl->baseline, 
                           pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Facilitation from reward and motivation
    pImpl->level = std::min(pImpl->peak, pImpl->level + pImpl->facilitationRate * pImpl->motivationLevel);
    
    // Update firing rate modulation based on current dopamine level
    pImpl->firingRateModulation = 0.8f + 1.2f * pImpl->level;
}

void Dopamine::signalReward(float reward) {
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate * pImpl->learningRate);
    
    // Update motivation based on reward
    pImpl->motivationLevel = std::min(1.0f, pImpl->motivationLevel + reward * 0.5f);
}

void Dopamine::signalRewardPredictionError(float error) {
    pImpl->rewardPredictionError = error;
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate);
    
    // RL learning: positive prediction error increases future motivation
    if (error > 0.0f) {
        pImpl->motivationLevel = std::min(1.0f, pImpl->motivationLevel + error * 0.3f);
    }
}

void Dopamine::applyToBrain(Brain* brain) {
    if (!brain) return;
    
    // Apply dopamine effects to neural excitability
    float dopamineMod = std::min(2.0f, 1.0f + pImpl->level);  // Up to 2x excitability at max DA
    
    // Mark neurons as dopamine-sensitive for future application
    // This would be called by the brain during initialization or development
    // For now, we just update the firing history
    for (auto& pair : pImpl->dopamineSensitiveNeurons) {
        uint64_t neuronId = pair.first;
        float sensitivity = pair.second;
        pImpl->firingHistory[neuronId] += 0.1f * dopamineMod * sensitivity;
    }
    
    // Apply action bias for motivation-driven behavior
    pImpl->actionBias = pImpl->motivationLevel * 0.5f;
}

void Dopamine::updateMotivation(float timeStep) {
    // Motivation decays over time
    pImpl->motivationLevel = std::max(0.0f, pImpl->motivationLevel - 0.01f * timeStep);
    
    // Learning updates motivation based on prediction error
    pImpl->motivationLevel = std::min(1.0f, pImpl->motivationLevel + pImpl->rewardPredictionError * 0.05f);
}

} // namespace nlm