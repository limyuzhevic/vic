#include "Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float gain;
    
    // Reward prediction error system
    float predictedValue;
    float expectedPredictionError;
    
    // Temporally discounting system
    std::vector<float> recentRewards;
    size_t rewardHistorySize;
    
    // Burst dynamics
    float burstThreshold;
    bool isBursting;
    float burstDecayRate;
    
    Impl() : 
        level(0.0f), 
        baseline(0.05f),           // Non-zero baseline level
        peak(1.0f),                // Maximum burst level
        decayRate(0.15f),          // Decay back to baseline
        releaseRate(2.0f),         // Rate of increase on reward
        gain(0.5f),                // Response gain
        predictedValue(0.0f),      // Predicted value
        expectedPredictionError(0.0f), // Expected prediction error
        rewardHistorySize(10),
        burstThreshold(0.8f),      // Level to trigger burst mode
        isBursting(false),
        burstDecayRate(0.8f)      // Fast decay in burst mode
    {
        recentRewards.reserve(rewardHistorySize);
    }
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
    // Enhanced dopamine-modulated plasticity with sigmoidal relationship
    // Plasticity factor increases with dopamine level but saturates
    float scaledLevel = pImpl->level / pImpl->baseline;
    
    // Sigmoidal relationship: moderate response at low levels, 
    // stronger at high levels but with diminishing returns
    float factor = 1.0f / (1.0f + std::exp(-3.0f * (scaledLevel - 1.0f)));
    
    // Apply gain modulation
    return factor * pImpl->gain;
}

void Dopamine::update(TimestepDuration dt) {
    float timeStep = static_cast<float>(dt);
    
    if (pImpl->isBursting) {
        // Fast decay in burst mode
        pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->burstDecayRate * timeStep);
        
        // Check if burst is over
        if (pImpl->level < pImpl->burstThreshold) {
            pImpl->isBursting = false;
        }
    } else {
        // Standard decay towards baseline
        float decayAmount = pImpl->decayRate * timeStep;
        if (pImpl->level > pImpl->baseline) {
            pImpl->level = std::max(pImpl->baseline, pImpl->level - decayAmount);
        } else {
            pImpl->level = std::min(pImpl->baseline, pImpl->level + decayAmount * 0.5f); // Slow recovery
        }
    }
    
    // Update reward history
    if (!pImpl->recentRewards.empty() && pImpl->recentRewards.size() >= pImpl->rewardHistorySize) {
        pImpl->recentRewards.erase(pImpl->recentRewards.begin());
    }
    pImpl->recentRewards.push_back(0.0f); // Will be updated by signalReward
}

void Dopamine::signalReward(float reward) {
    // Enhanced reward signaling with burst dynamics and temporal integration
    
    // Compute reward prediction error (simplified: reward - predicted value)
    float predictionError = reward - pImpl->predictedValue;
    
    // Update predicted value with temporal difference
    pImpl->predictedValue += 0.1f * predictionError;
    
    // Update expected prediction error (baseline expectation)
    pImpl->expectedPredictionError += 0.05f * std::abs(predictionError - pImpl->expectedPredictionError);
    
    // Store reward in history
    if (!pImpl->recentRewards.empty() && pImpl->recentRewards.size() >= pImpl->rewardHistorySize) {
        pImpl->recentRewards.erase(pImpl->recentRewards.begin());
    }
    pImpl->recentRewards.push_back(reward);
    
    // Compute integrated reward signal
    float integratedReward = reward;
    if (pImpl->recentRewards.size() > 1) {
        // Compute average of recent rewards
        float sum = 0.0f;
        for (float r : pImpl->recentRewards) {
            sum += r;
        }
        integratedReward = sum / pImpl->recentRewards.size();
    }
    
    // Enhanced burst dynamics based on prediction error magnitude
    float burstAmount = integratedReward * pImpl->releaseRate;
    
    // Check for burst conditions
    if (predictionError > pImpl->expectedPredictionError * 2.0f) {
        // Strong prediction error triggers burst firing
        pImpl->isBursting = true;
        pImpl->level = std::min(pImpl->peak, pImpl->level + burstAmount * 2.0f);
        NLM_LOG_DEBUG("Dopamine burst triggered: prediction error = " + std::to_string(predictionError));
    } else {
        // Standard response
        pImpl->level = std::min(pImpl->peak, pImpl->level + burstAmount);
    }
    
    // Modulate release rate based on baseline level
    pImpl->releaseRate = 1.5f + (pImpl->level / pImpl->baseline) * 0.5f;
}

void Dopamine::signalRewardPredictionError(float error) {
    // Direct reward prediction error signaling (learning signal)
    
    // Compute temporal derivative of prediction error
    static float lastError = 0.0f;
    float errorChange = error - lastError;
    lastError = error;
    
    // Update expected prediction error with smoothing
    pImpl->expectedPredictionError += 0.1f * std::abs(errorChange);
    
    // Enhance prediction error response
    float errorStrength = std::abs(error) * 2.0f;  // Amplify small errors
    
    // Update dopamine level based on prediction error
    if (error > 0) {
        // Positive prediction error: unexpected reward
        pImpl->level = std::min(pImpl->peak, pImpl->level + errorStrength * pImpl->releaseRate * 0.5f);
    } else {
        // Negative prediction error: unexpected omission
        pImpl->level = std::max(pImpl->baseline, pImpl->level + errorStrength * pImpl->releaseRate * 0.3f);
    }
    
    // Clear burst state on prediction error
    pImpl->isBursting = false;
}

} // namespace nlm
