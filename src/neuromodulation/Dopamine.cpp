#include "Dopamine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float learningRate;
    float predictionError;
    float expectedValue;
    float eligibilityTrace;
    std::vector<float> rewardHistory;
    std::vector<float> predictedRewards;
    std::vector<float> actualRewards;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f),
             learningRate(0.1f), predictionError(0.0f), expectedValue(0.0f), eligibilityTrace(0.0f) {}
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
    // Enhanced dopamine plasticity factor
    // Dopamine modulates both learning rate and memory consolidation
    float predictionErrorModulation = 0.5f + 0.5f * std::abs(pImpl->predictionError);
    float learningModulation = pImpl->learningRate * (1.0f + pImpl->level * 0.5f);
    return std::min(1.5f, std::max(0.3f, predictionErrorModulation * learningModulation));
}

void Dopamine::update(TimestepDuration dt) {
    // Natural decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Eligibility trace decay
    pImpl->eligibilityTrace = std::max(0.0f, pImpl->eligibilityTrace - 0.05f * static_cast<float>(dt));
}

void Dopamine::signalReward(float reward) {
    // Full reward signaling with prediction error computation
    pImpl->rewardHistory.push_back(reward);
    if (pImpl->rewardHistory.size() > 100) {
        pImpl->rewardHistory.erase(pImpl->rewardHistory.begin());
    }
    
    // Burst of dopamine on unexpected reward
    float rewardPrediction = pImpl->expectedValue;
    float predictionError = reward - rewardPrediction;
    
    // Update prediction error
    pImpl->predictionError = predictionError;
    
    // Strong dopamine response to positive prediction error
    if (predictionError > 0.0f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + predictionError * pImpl->releaseRate * 2.0f);
    }
    
    // Update expected value (temporal difference learning)
    pImpl->expectedValue = 0.9f * pImpl->expectedValue + 0.1f * reward;
}

void Dopamine::signalRewardPredictionError(float error) {
    // Reward prediction error signaling
    pImpl->predictionError = error;
    
    // Phasic dopamine response to prediction error
    if (error > 0.0f) {
        // Positive prediction error: reward better than expected
        pImpl->level = std::min(pImpl->peak, pImpl->level + error * pImpl->releaseRate);
    } else if (error < 0.0f) {
        // Negative prediction error: reward worse than expected
        pImpl->level = std::max(pImpl->baseline, pImpl->level + error * pImpl->releaseRate * 0.5f);
    }
    
    // Update eligibility trace
    pImpl->eligibilityTrace = std::min(1.0f, pImpl->eligibilityTrace + std::abs(error) * 0.1f);
}

void Dopamine::signalExtinction(float reward) {
    // Signaling extinction (expected reward not received)
    float error = -reward;
    pImpl->predictionError = error;
    
    // Gradual reduction of dopamine response
    pImpl->level = std::max(pImpl->baseline, pImpl->level - 0.2f * reward);
    
    // Update expected value
    pImpl->expectedValue = std::max(0.0f, pImpl->expectedValue - 0.1f * reward);
}

void Dopamine::signalOmission(float reward) {
    // Signaling reward omission (expected reward not received)
    float error = -1.0f; // Large negative prediction error
    pImpl->predictionError = error;
    
    // Sharp decrease in dopamine
    pImpl->level = std::max(pImpl->baseline, pImpl->level - 0.5f);
    
    // Reset expected value
    pImpl->expectedValue = std::max(0.0f, pImpl->expectedValue - 0.2f);
}

void Dopamine::updateWorkingMemory(float predictionError) {
    // Dopamine modulates working memory updating based on prediction error
    if (std::abs(predictionError) > 0.1f) {
        // Significant prediction error triggers working memory update
        float updateSignal = std::abs(predictionError) * 0.01f;
        setLevel(pImpl->level + updateSignal);
        
        // Update expected value for working memory
        pImpl->expectedValue = pImpl->expectedValue * 0.9f + predictionError * 0.1f;
    }
}

void Dopamine::updateRewardPrediction(const std::vector<float>& predictedRewards, 
                                     const std::vector<float>& actualRewards) {
    // Update reward prediction model
    pImpl->predictedRewards = predictedRewards;
    pImpl->actualRewards = actualRewards;
    
    // Compute prediction error across all rewards
    if (predictedRewards.size() == actualRewards.size() && !predictedRewards.empty()) {
        float totalError = 0.0f;
        for (size_t i = 0; i < predictedRewards.size(); ++i) {
            totalError += actualRewards[i] - predictedRewards[i];
        }
        pImpl->predictionError = totalError / static_cast<float>(predictedRewards.size());
        
        // Update level based on aggregate prediction error
        setLevel(pImpl->level + pImpl->predictionError * 0.01f);
    }
}

void Dopamine::assignCredit(float predictionError, float eligibilityTrace) {
    // Credit assignment for reinforcement learning
    // Assigns responsibility for prediction error to appropriate actions/states
    
    pImpl->eligibilityTrace = eligibilityTrace;
    
    // Compute credit assignment
    float credit = predictionError * eligibilityTrace;
    
    // Update expected value with credit
    pImpl->expectedValue += credit * 0.01f;
    
    // Update dopamine level based on credit
    setLevel(pImpl->level + credit * 0.005f);
}

} // namespace nlm