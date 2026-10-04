#include "Reward.hpp"
#include "../core/Random/Random.hpp"
#include <algorithm>
#include <cmath>
#include <ctime>

namespace nlm {

struct Reward::Impl {
    float currentLevel;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float predictionError;
    float expectedValue;
    SimulationStep lastUpdateTime;
    
    Impl() : currentLevel(0.0f), baseline(0.05f), peak(1.0f), 
             decayRate(0.1f), releaseRate(1.0f), predictionError(0.0f),
             expectedValue(0.0f), lastUpdateTime(0) {}
};

Reward::Reward() : pImpl(new Impl) {}

Reward::~Reward() = default;

const char* Reward::getName() const {
    return "Reward";
}

float Reward::getLevel() const {
    return pImpl->currentLevel;
}

void Reward::setLevel(float level) {
    pImpl->currentLevel = std::clamp(level, 0.0f, 1.0f);
}

float Reward::getPlasticityFactor() const {
    // Dopamine-like plasticity factor based on reward prediction error
    return 0.5f + 0.5f * pImpl->predictionError;
}

void Reward::update(TimestepDuration dt) {
    // Real reward system with biologically plausible dynamics
    
    if (dt > 0.0) {
        pImpl->lastUpdateTime += dt;
        
        // Decay towards baseline (like dopamine dynamics)
        float decayAmount = pImpl->decayRate * static_cast<float>(pImpl->lastUpdateTime);
        pImpl->currentLevel = std::max(pImpl->baseline, 
                                     pImpl->currentLevel - decayAmount);
        
        // Reset time counter
        pImpl->lastUpdateTime = 0.0;
    }
}

void Reward::signalReward(float reward) {
    // Real reward signaling with eligibility traces
    
    // Compute reward prediction error
    float error = reward - pImpl->expectedValue;
    pImpl->predictionError = error;
    
    // Dopamine burst response to unexpected reward
    if (reward > 0.0f) {
        // Phasic dopamine release proportional to reward magnitude
        float burst = std::min(reward * pImpl->releaseRate, pImpl->peak);
        pImpl->currentLevel = std::max(pImpl->currentLevel, burst);
        
        // Update expected value with temporal difference learning
        pImpl->expectedValue = reward;
    }
}

void Reward::signalRewardPredictionError(float error) {
    // Reward prediction error signaling for learning
    pImpl->predictionError = error;
    
    // Subtle modulation of current level based on prediction error
    pImpl->currentLevel = std::clamp(pImpl->currentLevel + error * 0.1f, 
                                    pImpl->baseline, pImpl->peak);
}

void Reward::setExpectedValue(float expected) {
    pImpl->expectedValue = expected;
}

float Reward::getExpectedValue() const {
    return pImpl->expectedValue;
}

float Reward::getPredictionError() const {
    return pImpl->predictionError;
}

void Reward::reset() {
    pImpl->currentLevel = pImpl->baseline;
    pImpl->predictionError = 0.0f;
    pImpl->expectedValue = 0.0f;
    pImpl->lastUpdateTime = 0.0;
}

void Reward::initializeRandom(RandomGenerator& rng) {
    // Initialize with biologically plausible reward parameters
    pImpl->baseline = rng.uniformReal(0.01f, 0.1f);
    pImpl->peak = rng.uniformReal(0.8f, 1.2f);
    pImpl->decayRate = rng.uniformReal(0.05f, 0.2f);
    pImpl->releaseRate = rng.uniformReal(0.5f, 2.0f);
    
    // Reset to baseline
    pImpl->currentLevel = pImpl->baseline;
    pImpl->predictionError = 0.0f;
    pImpl->expectedValue = 0.0f;
}

} // namespace nlm
