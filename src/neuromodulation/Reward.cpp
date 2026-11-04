// Implementation of Reward class with real reward computation
#include "Reward.hpp"
#include "core/Constants.hpp"
#include <cmath>
#include <algorithm>
#include <random>

namespace nlm {

struct Reward::Impl {
    float currentValue;
    float accumulatedReward;
    std::vector<float> history;
    std::shared_ptr<Observation> lastObservation;
    
    Impl() : currentValue(0.0f), accumulatedReward(0.0f) {
        history.reserve(1000);
    }
};

Reward::Reward() : pImpl(std::make_unique<Impl>()) {}

Reward::~Reward() = default;

float Reward::getValue() const {
    return pImpl->currentValue;
}

void Reward::setValue(float value) {
    pImpl->currentValue = value;
}

void Reward::add(float delta) {
    pImpl->accumulatedReward += delta;
    pImpl->currentValue += delta;
    pImpl->history.push_back(delta);
    if (pImpl->history.size() > 1000) {
        pImpl->history.erase(pImpl->history.begin());
    }
}

void Reward::reset() {
    pImpl->currentValue = 0.0f;
    pImpl->accumulatedReward = 0.0f;
    pImpl->history.clear();
    pImpl->lastObservation.reset();
}

// Enhanced reward computation with multiple components
float Reward::computeReward(const Observation& observation) {
    // Store observation for future reference
    pImpl->lastObservation = std::make_shared<Observation>(observation);
    
    float reward = 0.0f;
    
    // 1. Novelty-based reward (exploration bonus)
    if (observation.getNovelty() > 0.0f) {
        reward += computeNoveltyReward(observation.getNovelty());
    }
    
    // 2. Progress-based reward (achievement)
    if (observation.getProgress() > 0.0f) {
        reward += computeProgressReward(observation.getProgress());
    }
    
    // 3. Prediction error reward (learning signal)
    float predictionError = observation.getPredictionError();
    if (std::abs(predictionError) > 0.0f) {
        reward += computePredictionErrorReward(predictionError);
    }
    
    // 4. Curiosity-driven reward
    if (observation.getCuriosity() > 0.0f) {
        reward += computeCuriosityReward(observation.getCuriosity());
    }
    
    // 5. State-based reward (immediate outcomes)
    reward += computeStateBasedReward(observation);
    
    // 6. Integration bonus (combination of signals)
    reward += computeIntegrationBonus(observation);
    
    // Apply temporal and eligibility factors
    reward *= computeTemporalFactor();
    reward *= computeEligibilityFactor(observation);
    
    // Clamp to reasonable range
    reward = std::clamp(reward, -10.0f, 10.0f);
    
    // Update current value and history
    setValue(reward);
    
    return reward;
}

// Helper methods for reward computation
float Reward::computeNoveltyReward(float novelty) const {
    // Higher novelty = larger exploration bonus
    return novelty * Constants::NOVELTY_REWARD_SCALE;
}

float Reward::computeProgressReward(float progress) const {
    // S-shaped function for progress rewards
    return Constants::PROGRESS_MAX_REWARD * (progress / (progress + Constants::PROGRESS_CURVE_FACTOR));
}

float Reward::computePredictionErrorReward(float predictionError) const {
    // Large prediction errors provide strong learning signals
    return std::abs(predictionError) * Constants::PREDICTION_ERROR_REWARD_SCALE;
}

float Reward::computeCuriosityReward(float curiosity) const {
    // Curiosity drives exploration
    return curiosity * Constants::CURIOSITY_REWARD_SCALE;
}

float Reward::computeStateBasedReward(const Observation& observation) const {
    float reward = 0.0f;
    
    // Vision-based rewards (resource finding, obstacle avoidance)
    const auto& vision = observation.getVision();
    for (size_t i = 0; i < vision.size(); ++i) {
        // Bright objects might be resources
        if (vision[i] > 0.8f) {
            reward += Constants::VISION_REWARD_SCALE * vision[i];
        }
        // Dark objects might be hazards
        else if (vision[i] < 0.2f) {
            reward -= Constants::VISION_PUNISHMENT_SCALE * (0.2f - vision[i]);
        }
    }
    
    // Touch-based rewards (contact with objects)
    const auto& touch = observation.getTouch();
    for (float t : touch) {
        if (t > 0.5f) {
            reward += Constants::TOUCH_REWARD_SCALE * t;
        }
    }
    
    // Internal state rewards (energy, resources)
    const auto& internal = observation.getInternal();
    for (float val : internal) {
        if (val > 0.7f) {
            reward += Constants::INTERNAL_REWARD_SCALE * val;
        }
    }
    
    return reward;
}

float Reward::computeIntegrationBonus(const Observation& observation) const {
    // Combine multiple reward signals for better stability
    float novelty = observation.getNovelty();
    float curiosity = observation.getCuriosity();
    float predictionError = observation.getPredictionError();
    
    // Integration bonus encourages balanced exploration and exploitation
    float bonus = (novelty * Constants::INTEGRATION_NOVELTY_WEIGHT + 
                   curiosity * Constants::INTEGRATION_CURIOUSITY_WEIGHT +
                   std::abs(predictionError) * Constants::INTEGRATION_PREDICTION_WEIGHT);
    
    return bonus * Constants::INTEGRATION_BONUS_SCALE;
}

float Reward::computeTemporalFactor() const {
    // Temporal decay of reward signals
    static float lastRewardTime = 0.0f;
    static float temporalDecay = 1.0f;
    
    // Simple exponential decay
    temporalDecay = temporalDecay * Constants::TEMPORAL_DECAY_RATE + Constants::TEMPORAL_BASE_RATE;
    
    return temporalDecay;
}

float Reward::computeEligibilityFactor(const Observation& observation) const {
    // Eligibility traces for long-term credit assignment
    static float eligibility = 0.0f;
    
    float predictionError = observation.getPredictionError();
    eligibility = eligibility * Constants::ELIGIBILITY_DECAY + std::abs(predictionError);
    
    return std::min(eligibility, Constants::MAX_ELIGIBILITY);
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm