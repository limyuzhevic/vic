#include "Neuromodulator.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>

namespace nlm {

struct Dopamine::Impl {
    float level;                    // Current dopamine concentration (normalized)
    float rewardPredictionError;    // Current reward prediction error
    float rewardSmoothing;          // Exponential smoothing factor
    float learningRate;              // Learning rate for dopamine dynamics
    float baselineLevel;             // Baseline dopamine level
    float timeConstant;              // Time constant for dopamine dynamics (ms)
    
    // Reward history for computing prediction errors
    float lastExpectedReward;
    float lastActualReward;
    
    // Internal state for reward processing
    float integratedReward;          // Integrated reward over time
    float predictionErrorHistory[10]; // Recent prediction errors for smoothing
    size_t predictionErrorHistoryIndex;
    
    // Stress/ arousal modulation factors
    float stressLevel;               // Current stress level (0-1)
    float noveltyLevel;              // Current novelty detection level
    
    // Fatigue/dopamine depletion
    float dopaminePool;              // Available dopamine for signaling
    float depletionRate;             // Rate of dopamine depletion
    float recoveryRate;              // Rate of dopamine recovery
    
    Impl() : level(0.0f)
        , rewardPredictionError(0.0f)
        , rewardSmoothing(0.9f)
        , learningRate(0.01f)
        , baselineLevel(0.1f)
        , timeConstant(100.0f)
        , lastExpectedReward(0.0f)
        , lastActualReward(0.0f)
        , integratedReward(0.0f)
        , predictionErrorHistoryIndex(0)
        , stressLevel(0.0f)
        , noveltyLevel(0.0f)
        , dopaminePool(1.0f)
        , depletionRate(0.001f)
        , recoveryRate(0.01f) {}
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
    pImpl->level = std::clamp(level, 0.0f, 2.0f);
}

float Dopamine::getPlasticityFactor() const {
    // Real dopamine modulation of plasticity
    // Dopamine modulates synaptic strength based on reward prediction errors
    // Strong prediction errors (unexpected rewards) produce phasic bursts that enhance LTP
    // Negative prediction errors (omission of expected reward) reduce LTP
    
    float plasticityFactor = 1.0f;
    
    if (std::abs(pImpl->rewardPredictionError) > 0.01f) {
        // Phasic dopamine response to prediction error
        float errorMagnitude = std::min(std::abs(pImpl->rewardPredictionError), 1.0f);
        
        if (pImpl->rewardPredictionError > 0.0f) {
            // Unexpected reward: enhanced plasticity (LTP)
            plasticityFactor = 1.0f + errorMagnitude * 2.0f;  // Up to 3x enhancement
        } else {
            // Omitted reward: reduced plasticity (LTD)
            plasticityFactor = std::max(0.1f, 1.0f - errorMagnitude * 1.5f);  // Down to 10% of baseline
        }
    }
    
    // Apply neuromodulator-specific factors
    plasticityFactor *= (1.0f + pImpl->level * 0.5f);  // Higher dopamine = more modulation
    plasticityFactor = std::clamp(plasticityFactor, 0.0f, 5.0f);
    
    return plasticityFactor;
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics with multiple components
    
    // Convert dt to milliseconds
    float dtMs = static_cast<float>(dt) * 1000.0f;
    
    // Update reward prediction error integration
    float smoothedError = pImpl->rewardPredictionError;
    if (pImpl->predictionErrorHistoryIndex > 0) {
        float recentError = pImpl->predictionErrorHistory[(pImpl->predictionErrorHistoryIndex - 1) % 10];
        smoothedError = smoothedError * 0.5f + recentError * 0.5f;
    }
    
    // Update integrated reward with exponential decay
    pImpl->integratedReward *= std::exp(-dtMs / pImpl->timeConstant);
    
    // Update baseline level based on integrated reward
    pImpl->baselineLevel = 0.1f + std::tanh(pImpl->integratedReward) * 0.1f;
    
    // Update dopamine pool with depletion and recovery
    pImpl->dopaminePool -= pImpl->depletionRate * dtMs;
    pImpl->dopaminePool += pImpl->recoveryRate * dtMs * (1.0f - pImpl->dopaminePool);
    pImpl->dopaminePool = std::clamp(pImpl->dopaminePool, 0.0f, 2.0f);
    
    // Update stress and novelty levels
    pImpl->stressLevel *= std::exp(-dtMs / 500.0f);  // Stress decays over 500ms
    pImpl->noveltyLevel *= std::exp(-dtMs / 200.0f);  // Novelty decays over 200ms
    
    // Compute phasic dopamine response
    float phasicResponse = 0.0f;
    
    if (std::abs(smoothedError) > 0.01f) {
        // Phasic burst proportional to prediction error
        phasicResponse = smoothedError * pImpl->dopaminePool * 2.0f;
    }
    
    // Modulate phasic response with stress and novelty
    float modulation = 1.0f + pImpl->stressLevel * 0.5f - pImpl->noveltyLevel * 0.2f;
    phasicResponse *= modulation;
    
    // Update dopamine level with smooth integration
    float targetLevel = pImpl->baselineLevel + phasicResponse;
    float alpha = 0.1f;  // Smoothing factor
    pImpl->level = pImpl->level * (1.0f - alpha) + targetLevel * alpha;
    
    // Clamp to reasonable bounds
    pImpl->level = std::clamp(pImpl->level, 0.0f, 2.0f);
}

void Dopamine::signalReward(float reward) {
    // Process reward signal and update prediction error
    
    // Store previous expected and actual rewards
    pImpl->lastExpectedReward = pImpl->rewardPredictionError;
    pImpl->lastActualReward = reward;
    
    // Compute prediction error
    pImpl->rewardPredictionError = reward - pImpl->lastExpectedReward;
    
    // Store in history for smoothing
    pImpl->predictionErrorHistory[pImpl->predictionErrorHistoryIndex] = pImpl->rewardPredictionError;
    pImpl->predictionErrorHistoryIndex = (pImpl->predictionErrorHistoryIndex + 1) % 10;
    
    // Update integrated reward
    pImpl->integratedReward += reward * 0.1f;  // Weight recent rewards less
    pImpl->integratedReward = std::clamp(pImpl->integratedReward, -1.0f, 1.0f);
    
    // Deplete dopamine pool based on reward magnitude
    pImpl->dopaminePool -= std::min(reward * 0.2f, 0.5f);  // Larger rewards deplete more
    pImpl->dopaminePool = std::max(pImpl->dopaminePool, 0.0f);
    
    // Increase stress level slightly based on reward prediction error
    pImpl->stressLevel += std::abs(pImpl->rewardPredictionError) * 0.1f;
    pImpl->stressLevel = std::clamp(pImpl->stressLevel, 0.0f, 1.0f);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Directly set reward prediction error (for when prediction is explicit)
    pImpl->lastExpectedReward = error;  // In this case, we assume expected reward is 0
    pImpl->lastActualReward = 0.0f;     // Actual reward is omitted
    
    pImpl->rewardPredictionError = -error;  // Negative prediction error for omitted reward
    
    // Store in history
    pImpl->predictionErrorHistory[pImpl->predictionErrorHistoryIndex] = pImpl->rewardPredictionError;
    pImpl->predictionErrorHistoryIndex = (pImpl->predictionErrorHistoryIndex + 1) % 10;
}

} // namespace nlm
