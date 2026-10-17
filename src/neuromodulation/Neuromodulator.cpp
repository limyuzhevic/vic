#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float noiseLevel;
    float adaptationRate;
    float predictionErrorHistory;
    std::vector<float> rewardHistory;
    Timestamp lastRewardTime;
    Timestamp lastPredictionErrorTime;
    
    Impl() 
        : level(0.0f)
        , baseline(0.0f)
        , peak(1.0f)
        , decayRate(0.05f)
        , releaseRate(1.0f)
        , noiseLevel(0.01f)
        , adaptationRate(0.1f)
        , predictionErrorHistory(0.0f)
        , lastRewardTime(-1.0f)
        , lastPredictionErrorTime(-1.0f) {
        rewardHistory.reserve(1000);
    }
};

Dopamine::Dopamine() : pImpl(new Impl) {}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "DA";
}

float Dopamine::getLevel() const {
    // Add some noise to simulate biological variability
    return pImpl->level + ((pImpl->rng ? pImpl->rng->uniformReal(-pImpl->noiseLevel, pImpl->noiseLevel) : 0.0f));
}

void Dopamine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Dopamine::getPlasticityFactor() const {
    // Real dopamine-modulated plasticity: combines reward prediction error and current level
    // Higher dopamine increases LTP potential, lower decreases LTD potential
    float basePlasticity = 0.5f;
    float levelFactor = pImpl->level * 0.5f;
    float predictionErrorFactor = pImpl->predictionErrorHistory * 0.3f;
    
    // Apply exponential modulation for realistic dynamics
    float plasticity = basePlasticity * (1.0f + levelFactor) * (1.0f + predictionErrorFactor);
    
    // Ensure reasonable bounds
    return std::clamp(plasticity, 0.1f, 2.0f);
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics with homeostatic regulation
    float timeSinceLastReward = (pImpl->lastRewardTime >= 0.0f) ? 
        static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now() - pImpl->lastRewardTime).count()) : 999999.0f;
    
    float timeSinceLastPredictionError = (pImpl->lastPredictionErrorTime >= 0.0f) ? 
        static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now() - pImpl->lastPredictionErrorTime).count()) : 999999.0f;
    
    // Decay towards baseline with time constant based on elapsed time
    float decayTimeConstant = 1000.0f / pImpl->decayRate;  // ms
    float decayFactor = std::exp(-timeSinceLastReward / decayTimeConstant);
    
    // Apply homeostatic adaptation
    pImpl->baseline = std::max(0.0f, pImpl->baseline + pImpl->adaptationRate * (0.5f - pImpl->baseline));
    
    // Smooth decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level * decayFactor);
    
    // Add prediction error contribution
    if (!pImpl->rewardHistory.empty()) {
        float recentPredictionError = pImpl->rewardHistory.back();
        pImpl->predictionErrorHistory = std::max(0.0f, pImpl->predictionErrorHistory - 0.1f * recentPredictionError);
    }
}

void Dopamine::signalReward(float reward) {
    // Real reward signaling with burst dynamics
    if (reward < 0.0f || reward > 1.0f) {
        return;  // Invalid reward value
    }
    
    // Store reward for prediction error calculation
    pImpl->rewardHistory.push_back(reward);
    if (pImpl->rewardHistory.size() > 1000) {
        pImpl->rewardHistory.erase(pImpl->rewardHistory.begin());
    }
    
    pImpl->lastRewardTime = std::chrono::system_clock::now();
    
    // Burst dynamics: immediate peak followed by decay
    float burstStrength = reward * pImpl->releaseRate;
    pImpl->level = std::min(pImpl->peak, pImpl->level + burstStrength);
    
    // Update prediction error history for plasticity factor
    pImpl->predictionErrorHistory = reward * 0.5f;
}

void Dopamine::signalRewardPredictionError(float error) {
    // Reward prediction error signaling with biologically realistic dynamics
    if (error < -1.0f || error > 1.0f) {
        return;  // Invalid error value
    }
    
    pImpl->lastPredictionErrorTime = std::chrono::system_clock::now();
    
    // Phasic response to prediction error
    float errorResponse = error * pImpl->releaseRate;
    
    if (error > 0.0f) {
        // Positive prediction error (unexpected reward): potentiation
        pImpl->level = std::min(pImpl->peak, pImpl->level + errorResponse);
        pImpl->predictionErrorHistory = error * 0.8f;
    } else {
        // Negative prediction error (unexpected omission): depression
        pImpl->level = std::max(pImpl->baseline, pImpl->level + errorResponse);
        pImpl->predictionErrorHistory = error * 0.6f;
    }
}

} // namespace nlm
