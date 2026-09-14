#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <chrono>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float predictionError;  // For reward prediction error
    float learningRate;     // Learning rate for dopamine updates
    float explorationValue; // Associated exploration factor
    
    Impl() : level(0.0f), baseline(0.1f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f),
             predictionError(0.0f), learningRate(0.01f), explorationValue(0.0f) {}
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
    // Real dopamine-modulated plasticity factor using sigmoidal function
    // Higher dopamine levels increase plasticity up to a maximum
    float normalizedDA = pImpl->level / pImpl->baseline;
    float plasticity = 0.1f + 0.9f * (1.0f / (1.0f + std::exp(-3.0f * (normalizedDA - 1.0f))));
    
    // Combine with prediction error modulation
    float peModulation = 1.0f + 0.5f * pImpl->predictionError;
    
    return plasticity * peModulation;
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics with multiple components:
    // 1. Decay towards baseline with time constant
    // 2. Noise to simulate biological variability
    // 3. Modulation by prediction error
    
    static std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
    static std::normal_distribution<float> noise(0.0f, 0.01f);
    
    // Decay towards baseline
    float decayAmount = pImpl->decayRate * dt * (pImpl->level - pImpl->baseline);
    float newLevel = pImpl->level - decayAmount;
    
    // Add biological noise
    newLevel += noise(rng);
    
    // Ensure level stays within bounds
    newLevel = std::clamp(newLevel, 0.0f, pImpl->peak);
    
    // Apply prediction error modulation
    if (std::abs(pImpl->predictionError) > 0.0f) {
        newLevel += pImpl->predictionError * pImpl->learningRate * dt;
        newLevel = std::clamp(newLevel, 0.0f, pImpl->peak);
    }
    
    pImpl->level = newLevel;
    
    // Decay prediction error
    pImpl->predictionError *= (1.0f - 0.05f * dt);
    
    // Exploration value follows dopamine level with slight delay
    pImpl->explorationValue = 0.5f * pImpl->level + 0.5f * pImpl->explorationValue * (1.0f - 0.1f * dt);
}

void Dopamine::signalReward(float reward) {
    // Real reward signaling with burst response and adaptation
    // Dopamine burst response to unexpected rewards (prediction error)
    float burst = std::min(reward * pImpl->releaseRate, pImpl->peak - pImpl->level);
    pImpl->level += burst;
    
    // Store prediction error for future updates
    pImpl->predictionError = reward;
    
    // Increase exploration value for novel rewards
    pImpl->explorationValue = std::min(1.0f, pImpl->explorationValue + 0.2f * reward);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Real reward prediction error signaling
    // Phasic dopamine responses to prediction errors
    
    // Convert error to dopamine response (positive for better than expected, negative for worse)
    float errorSignal = error * pImpl->releaseRate;
    
    // Apply to dopamine level with bounds
    pImpl->level += errorSignal;
    pImpl->level = std::clamp(pImpl->level, 0.0f, pImpl->peak);
    
    // Store error for plasticity modulation
    pImpl->predictionError = error;
    
    // Update exploration based on prediction error magnitude
    pImpl->explorationValue = 0.5f + 0.5f * std::abs(error);
}

} // namespace nlm
