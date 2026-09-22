#include "Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Neuromodulator::Impl {
    float level;
    float baseline;
    float plasticityEffect;
    float previousLevel;
    float integrationRate;
    TimestepDuration decayTime;
    
    Impl() : level(0.0f), baseline(0.0f), plasticityEffect(1.0f), 
             previousLevel(0.0f), integrationRate(0.1f), decayTime(1000.0f) {}
};

Neuromodulator::Neuromodulator() : pImpl(new Impl) {}

// Base neuromodulator implementation
const char* Neuromodulator::getName() const { return "Generic"; }

float Neuromodulator::getLevel() const { return pImpl->level; }

void Neuromodulator::setLevel(float level) { pImpl->level = level; }

float Neuromodulator::getPlasticityFactor() const { return pImpl->plasticityEffect; }

void Neuromodulator::update(TimestepDuration dt) {
    // Decay towards baseline
    float decayFactor = std::exp(-dt / pImpl->decayTime);
    pImpl->level = pImpl->baseline + (pImpl->level - pImpl->baseline) * decayFactor;
    
    pImpl->previousLevel = pImpl->level;
}

// Dopamine implementation - reward prediction error signal
struct Dopamine::Impl {
    float level;
    float predictionError;
    float rewardPrediction;
    float eligibilityTrace;
    float learningRate;
    float explorationBonus;
    float memoryConsolidation;
    
    Impl() : level(0.0f), predictionError(0.0f), rewardPrediction(0.0f),
             eligibilityTrace(0.0f), learningRate(0.001f), explorationBonus(0.5f),
             memoryConsolidation(0.0f) {}
};

Dopamine::Dopamine() : pImpl(new Impl) {}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const { return "Dopamine"; }

float Dopamine::getLevel() const { return pImpl->level; }

void Dopamine::setLevel(float level) { pImpl->level = level; }

float Dopamine::getPlasticityFactor() const { return 1.0f + pImpl->level * 0.5f; }

void Dopamine::update(TimestepDuration dt) {
    // Decay dopamine level over time
    pImpl->level *= std::exp(-dt * 0.001f);
    
    // Apply eligibility trace decay
    pImpl->eligibilityTrace *= std::exp(-dt * 0.01f);
    
    // Update memory consolidation based on dopamine level
    pImpl->memoryConsolidation = std::max(0.0f, pImpl->memoryConsolidation - dt * 0.0001f);
}

void Dopamine::signalReward(float reward) {
    pImpl->predictionError = reward - pImpl->rewardPrediction;
    pImpl->level += pImpl->predictionError * pImpl->learningRate;
    pImpl->eligibilityTrace = std::min(1.0f, pImpl->eligibilityTrace + 0.1f);
    pImpl->memoryConsolidation = std::min(1.0f, pImpl->memoryConsolidation + pImpl->level * 0.01f);
}

void Dopamine::signalRewardPredictionError(float error) {
    pImpl->predictionError = error;
    pImpl->level += error * pImpl->learningRate * pImpl->eligibilityTrace;
}

} // namespace nlm