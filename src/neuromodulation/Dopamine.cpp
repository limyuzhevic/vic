// Dopamine Implementation
// Extended reward signaling system with plasticity modulation

#include "Dopamine.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>
#include <algorithm>
#include <random>

namespace nlm {

struct Dopamine::Impl {
    // Reward signal dynamics
    static constexpr float BASE_REWARD_DECAY = 0.95f;      // Decay rate without input
    static constexpr float MAX_REWARD_LEVEL = 2.0f;        // Maximum possible reward level
    static constexpr float MIN_REWARD_LEVEL = 0.0f;        // Minimum possible reward level
    static constexpr float PLASTICITY_FACTOR_MIN = 0.1f;   // Minimum plasticity factor
    static constexpr float PLASTICITY_FACTOR_MAX = 2.0f;   // Maximum plasticity factor
    static constexpr float NOISE_VARIANCE = 0.01f;          // Reward signal noise
    static constexpr float TIME_CONSTANT = 50.0f;          // Reward signal time constant (ms)
    
    // Internal state
    float currentLevel;           // Current dopamine level
    float targetLevel;            // Target reward level
    float plasticityFactor;       // Plasticity modulation factor
    float rewardHistory[10];       // Recent reward history (for prediction error)
    size_t historyIndex;           // Current position in reward history
    
    // Reward prediction error components
    float predictedReward;        // Expected reward
    float predictionError;        // Reward prediction error signal
    
    // Time integration
    float timeSinceLastUpdate;     // Time since last update
    
    Impl() : currentLevel(0.0f), targetLevel(0.0f), plasticityFactor(1.0f),
             predictedReward(0.0f), predictionError(0.0f), timeSinceLastUpdate(0.0f),
             historyIndex(0) {
        // Initialize reward history
        for (int i = 0; i < 10; ++i) {
            rewardHistory[i] = 0.0f;
        }
    }
};

Dopamine::Dopamine() : pImpl(std::make_unique<Impl>()) {
}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "Dopamine";
}

float Dopamine::getLevel() const {
    return pImpl->currentLevel;
}

void Dopamine::setLevel(float level) {
    pImpl->currentLevel = std::clamp(level, pImpl->MIN_REWARD_LEVEL, pImpl->MAX_REWARD_LEVEL);
    pImpl->targetLevel = pImpl->currentLevel;
    NLM_LOG_INFO(std::string("Dopamine level set to ") + std::to_string(pImpl->currentLevel));
}

float Dopamine::getPlasticityFactor() const {
    return pImpl->plasticityFactor;
}

void Dopamine::update(TimestepDuration dt) {
    pImpl->timeSinceLastUpdate += dt;
    
    // Natural decay toward baseline
    if (pImpl->currentLevel > 0.0f) {
        float decayFactor = std::pow(pImpl->BASE_REWARD_DECAY, dt * 10.0f);  // Scale decay with time
        pImpl->currentLevel *= decayFactor;
    }
    
    // Add noise to biological realism
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<float> noise(0.0f, pImpl->NOISE_VARIANCE);
    
    pImpl->currentLevel += noise(gen);
    
    // Update plasticity factor based on current level
    updatePlasticityFactor();
    
    // Update prediction error
    updatePredictionError();
    
    // Clamp to valid range
    pImpl->currentLevel = std::clamp(pImpl->currentLevel, 
                                    pImpl->MIN_REWARD_LEVEL, 
                                    pImpl->MAX_REWARD_LEVEL);
}

void Dopamine::signalReward(float reward) {
    // Signal new reward
    if (reward != 0.0f) {
        // Update target level based on reward magnitude
        pImpl->targetLevel += reward * 0.1f;  // Gradual adaptation
        
        // Add noise for biological realism
        std::random_device rd;
        std::mt19937 gen(rd());
        std::normal_distribution<float> noise(0.0f, pImpl->NOISE_VARIANCE * 2.0f);
        
        float noisyReward = reward + noise(gen);
        pImpl->targetLevel += noisyReward * 0.5f;  // Strong response to reward
        
        // Store in history for prediction error calculation
        pImpl->rewardHistory[pImpl->historyIndex] = noisyReward;
        pImpl->historyIndex = (pImpl->historyIndex + 1) % 10;
        
        // Calculate immediate plasticity effect
        pImpl->plasticityFactor = std::min(pImpl->PLASTICITY_FACTOR_MAX,
                                          pImpl->plasticityFactor + reward * 0.02f);
        
        NLM_LOG_INFO(std::string("Dopamine reward signaled: ") + std::to_string(reward) +
                    std::string(", current level: ") + std::to_string(pImpl->currentLevel));
    }
}

void Dopamine::signalRewardPredictionError(float error) {
    // Process reward prediction error signal
    pImpl->predictionError = error;
    
    // Update predicted reward
    pImpl->predictedReward += error * 0.5f;
    
    // Modulate plasticity factor based on prediction error
    if (std::abs(error) > 0.1f) {  // Significant prediction error
        // Strong prediction error increases plasticity
        pImpl->plasticityFactor = std::min(pImpl->PLASTICITY_FACTOR_MAX,
                                          pImpl->plasticityFactor + std::abs(error) * 0.05f);
        
        // Positive prediction error (better than expected) should increase future expectations
        if (error > 0.0f) {
            pImpl->targetLevel += error * 0.1f;
        }
    }
    
    NLM_LOG_INFO(std::string("Dopamine prediction error: ") + std::to_string(error));
}

void Dopamine::updatePlasticityFactor() {
    // Update plasticity factor based on current dopamine level
    // Dopamine modulates synaptic plasticity strength
    
    // Higher dopamine levels generally increase plasticity (learning)
    float levelBasedPlasticity = pImpl->currentLevel / pImpl->MAX_REWARD_LEVEL;
    
    // Decay plasticity toward baseline over time
    float plasticityDecay = std::pow(0.98f, pImpl->timeSinceLastUpdate * 10.0f);
    pImpl->plasticityFactor *= plasticityDecay;
    
    // Blend level-based and decayed plasticity
    pImpl->plasticityFactor = (levelBasedPlasticity * 0.7f) + 
                              (pImpl->plasticityFactor * 0.3f);
    
    // Apply bounds
    pImpl->plasticityFactor = std::clamp(pImpl->plasticityFactor,
                                        pImpl->PLASTICITY_FACTOR_MIN,
                                        pImpl->PLASTICITY_FACTOR_MAX);
}

void Dopamine::updatePredictionError() {
    // Calculate reward prediction error based on recent history
    if (pImpl->historyIndex == 0) {
        // Not enough history for meaningful prediction error
        pImpl->predictionError = 0.0f;
        return;
    }
    
    // Calculate actual recent reward average
    float actualReward = 0.0f;
    float totalWeight = 0.0f;
    
    for (int i = 0; i < 10; ++i) {
        if (pImpl->rewardHistory[i] != 0.0f) {  // Non-zero entries are actual rewards
            actualReward += pImpl->rewardHistory[i];
            totalWeight += 1.0f;
        }
    }
    
    if (totalWeight > 0.0f) {
        actualReward /= totalWeight;
        
        // Compare with predicted reward
        pImpl->predictionError = actualReward - pImpl->predictedReward;
        
        // Update prediction for next time step
        pImpl->predictedReward = actualReward;
    } else {
        pImpl->predictionError = 0.0f;
    }
}

} // namespace nlm
