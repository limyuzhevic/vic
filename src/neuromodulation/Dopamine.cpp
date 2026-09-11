#include "Dopamine.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include <algorithm>
#include <cmath>
#include <thread>
#include <mutex>
#include <deque>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float accumulationRate;
    float predictionErrorGain;
    
    // Reward history
    std::deque<float> rewardHistory;
    std::deque<float> errorHistory;
    size_t historySize;
    
    // Neuromodulatory signaling
    bool isBursting;
    TimestepDuration burstDuration;
    TimestepDuration burstTimer;
    
    // Internal state for RL-inspired dynamics
    float predictionError;
    float rewardPrediction;
    float temporalDifference;
    
    // Integration with brain systems
    class Brain* brain;
    
    // Resource management
    mutable std::mutex mutex;
    
    Impl() 
        : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f)
        , accumulationRate(0.3f), predictionErrorGain(0.5f)
        , historySize(100)
        , isBursting(false), burstDuration(0.1), burstTimer(0.0)
        , predictionError(0.0f), rewardPrediction(0.0f), temporalDifference(0.0f)
        , brain(nullptr) {}
};

Dopamine::Dopamine() : pImpl(new Impl) {}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "DA";
}

float Dopamine::getLevel() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->level;
}

void Dopamine::setLevel(float level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->level = std::clamp(level, 0.0f, pImpl->peak);
}

float Dopamine::getPlasticityFactor() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Enhanced plasticity factor based on dopamine level
    // This implements a more realistic neuromodulatory effect on synaptic plasticity
    float basePlasticity = 0.5f;
    
    // Dopamine modulates plasticity with a non-linear relationship
    // Low dopamine reduces plasticity, medium dopamine increases it, high dopamine stabilizes
    float plasticityFactor = basePlasticity + 0.6f * std::tanh(pImpl->level / 0.5f);
    
    // Apply prediction error modulation
    float errorModulation = 1.0f + pImpl->predictionErrorGain * pImpl->predictionError;
    
    // Combine factors with saturation
    float result = plasticityFactor * errorModulation;
    return std::clamp(result, 0.1f, 2.0f);
}

void Dopamine::update(TimestepDuration dt) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (dt <= 0.0) return;
    
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update burst timer
    if (pImpl->isBursting) {
        pImpl->burstTimer -= static_cast<float>(dt);
        if (pImpl->burstTimer <= 0.0f) {
            pImpl->isBursting = false;
        }
    }
    
    // Temporal difference learning updates
    float tdLearning = 0.01f * pImpl->temporalDifference * static_cast<float>(dt);
    pImpl->level += tdLearning;
    pImpl->level = std::clamp(pImpl->level, 0.0f, pImpl->peak);
    
    // Maintain reward history
    if (pImpl->rewardHistory.size() >= pImpl->historySize) {
        pImpl->rewardHistory.pop_front();
    }
    pImpl->rewardHistory.push_back(pImpl->level);
}

void Dopamine::signalReward(float reward) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Compute prediction error
    pImpl->predictionError = reward - pImpl->rewardPrediction;
    pImpl->temporalDifference = pImpl->predictionError;
    
    // Update reward prediction with temporal difference learning
    pImpl->rewardPrediction += pImpl->accumulationRate * pImpl->predictionError;
    
    // Dopamine burst response to unexpected reward
    float expected = pImpl->rewardPrediction;
    float surprise = std::abs(pImpl->predictionError);
    
    // Dopamine responds more strongly to unexpected rewards
    float burstIntensity = reward * (1.0f + 2.0f * surprise);
    
    // Apply burst signal
    pImpl->level = std::min(pImpl->peak, pImpl->level + burstIntensity);
    
    // Start burst mode
    pImpl->isBursting = true;
    pImpl->burstTimer = pImpl->burstDuration;
    
    // Record in history
    pImpl->errorHistory.push_back(pImpl->predictionError);
    if (pImpl->errorHistory.size() > pImpl->historySize) {
        pImpl->errorHistory.pop_front();
    }
    
    NLM_LOG_INFO("Dopamine burst: reward=" + std::to_string(reward) + 
                 " | prediction_error=" + std::to_string(pImpl->predictionError) +
                 " | level=" + std::to_string(pImpl->level));
}

void Dopamine::signalRewardPredictionError(float error) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Store prediction error
    pImpl->predictionError = error;
    pImpl->temporalDifference = error;
    
    // Dopaminergic response to prediction error
    // Positive error (better than expected) increases dopamine
    // Negative error (worse than expected) decreases dopamine
    float errorResponse = error * pImpl->predictionErrorGain;
    
    pImpl->level = std::clamp(pImpl->level + errorResponse, 0.0f, pImpl->peak);
    
    // Record error history
    pImpl->errorHistory.push_back(error);
    if (pImpl->errorHistory.size() > pImpl->historySize) {
        pImpl->errorHistory.pop_front();
    }
    
    NLM_LOG_INFO("Dopamine prediction error: error=" + std::to_string(error) +
                 " | level=" + std::to_string(pImpl->level));
}

// Helper functions for dopamine dynamics
float Dopamine::getPredictionError() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->predictionError;
}

float Dopamine::getRewardPrediction() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->rewardPrediction;
}

void Dopamine::reset() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->level = pImpl->baseline;
    pImpl->predictionError = 0.0f;
    pImpl->rewardPrediction = 0.0f;
    pImpl->temporalDifference = 0.0f;
    pImpl->rewardHistory.clear();
    pImpl->errorHistory.clear();
    pImpl->isBursting = false;
    pImpl->burstTimer = 0.0f;
}

const std::vector<float>& Dopamine::getRewardHistory() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return std::vector<float>(pImpl->rewardHistory.begin(), pImpl->rewardHistory.end());
}

const std::vector<float>& Dopamine::getErrorHistory() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return std::vector<float>(pImpl->errorHistory.begin(), pImpl->errorHistory.end());
}

void Dopamine::setBaseline(float baseline) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->baseline = baseline;
    pImpl->level = std::min(pImpl->level, baseline);
}

void Dopamine::setReleaseRate(float rate) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->releaseRate = rate;
}

void Dopamine::setDecayRate(float rate) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->decayRate = rate;
}

void Dopamine::setPredictionErrorGain(float gain) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->predictionErrorGain = gain;
}

void Dopamine::initialize(Brain* brainPtr) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->brain = brainPtr;
    NLM_LOG_INFO("Dopamine system initialized with brain connection");
}

} // namespace nlm