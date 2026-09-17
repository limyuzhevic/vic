// Reward.cpp - Implementation
#include "Reward.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Reward::Impl {
    Brain* brain;
    float rewardLevel;
    float expectedReward;
    float predictionError;
    float eligibilityTrace;
    
    Impl() : brain(nullptr), rewardLevel(0.0f), expectedReward(0.0f), 
             predictionError(0.0f), eligibilityTrace(0.0f) {}
};

Reward::Reward() : pImpl(new Impl) {}

Reward::~Reward() = default;

void Reward::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Reward system initialized");
}

void Reward::signalReward(float reward) {
    if (!pImpl->brain) return;
    
    // Apply reward signal
    pImpl->rewardLevel = reward;
    pImpl->predictionError = std::max(0.0f, reward - pImpl->expectedReward);
    
    // Update eligibility traces
    pImpl->eligibilityTrace = std::min(1.0f, pImpl->eligibilityTrace + 0.5f);
    
    // Apply reward to synapses via eligibility traces
    if (pImpl->eligibilityTrace > 0.0f) {
        auto* stdp = pImpl->brain->getSTDP();
        if (stdp) {
            stdp->applyRewardSignal(pImpl->predictionError, pImpl->eligibilityTrace);
        }
    }
}

void Reward::signalRewardPredictionError(float error) {
    pImpl->predictionError = error;
    pImpl->rewardLevel = std::max(0.0f, 1.0f - std::abs(error));
}

void Reward::update(TimestepDuration dt) {
    if (!pImpl->brain) return;
    
    // Decay eligibility trace
    pImpl->eligibilityTrace *= 0.9f;
    
    // Update expected reward based on current experience
    // This would use prediction error to update expectations
    pImpl->expectedReward = pImpl->expectedReward * 0.9f + pImpl->rewardLevel * 0.1f;
}

float Reward::getLevel() const {
    return pImpl->rewardLevel;
}

float Reward::getExpectedReward() const {
    return pImpl->expectedReward;
}

float Reward::getPredictionError() const {
    return pImpl->predictionError;
}

float Reward::getPlasticityFactor() const {
    // Dopamine factor based on prediction error
    return 1.0f + pImpl->predictionError * 0.5f;
}

} // namespace nlm