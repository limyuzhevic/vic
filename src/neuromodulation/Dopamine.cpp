#include "Dopamine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Dopamine::Impl {
    Brain* brain;
    
    Impl() : brain(nullptr) {}
};

Dopamine::Dopamine()
    : pImpl(std::make_unique<Impl>())
    , brain_(nullptr)
    , dopamineLevel_(0.0f)
    , predictionError_(0.0f)
    , predictedReward_(0.0f)
    , expectedValue_(0.0f)
    , lastUpdateStep_(0)
    , predictionErrorLearningRate_(0.01f)
    , actionLearningRate_(0.01f)
    , rewardDecayRate_(0.95f)
    , actionValues_(10, 0.0f)  // 10 action types
{
}

Dopamine::~Dopamine() = default;

void Dopamine::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("Dopamine neuromodulation system initialized");
}

void Dopamine::updatePredictionError(float predictionError, SimulationStep currentTime) {
    // Store previous prediction error for history
    if (!predictionErrorHistory_.empty()) {
        predictionErrorHistory_.push_back(predictionError);
        if (predictionErrorHistory_.size() > 100) {
            predictionErrorHistory_.erase(predictionErrorHistory_.begin());
        }
    }
    
    predictionError_ = predictionError;
    
    // Update predicted reward using delta rule
    // predicted_reward += learning_rate * prediction_error
    predictedReward_ += predictionErrorLearningRate_ * predictionError;
    
    // Clamp predicted reward to reasonable range
    predictedReward_ = std::clamp(predictedReward_, -10.0f, 10.0f);
    
    // Update expected value (current expected outcome of actions)
    expectedValue_ += predictionErrorLearningRate_ * predictionError * 0.5f;
    expectedValue_ = std::clamp(expectedValue_, -5.0f, 5.0f);
    
    // Compute dopamine level based on prediction error
    // Phasic burst for positive prediction error (better than expected)
    // Phasic dip for negative prediction error (worse than expected)
    // Tonic baseline for maintaining predicted reward
    float phasicComponent = 0.0f;
    
    if (predictionError_ > 0.1f) {
        // Strong positive prediction error - burst of dopamine
        phasicComponent = 2.0f * std::exp(predictionError_ * 5.0f);
    } else if (predictionError_ < -0.1f) {
        // Strong negative prediction error - dip of dopamine
        phasicComponent = -2.0f * std::exp(-predictionError_ * 5.0f);
    }
    
    // Add tonic component (maintains baseline level around predicted reward)
    float tonicComponent = 1.0f + std::clamp(predictedReward_ * 0.5f, -0.5f, 0.5f);
    
    // Combine phasic and tonic components
    dopamineLevel_ = tonicComponent + phasicComponent;
    
    // Apply decay
    dopamineLevel_ *= rewardDecayRate_;
    dopamineLevel_ = std::clamp(dopamineLevel_, 0.0f, 5.0f);
    
    // Update action values based on prediction error
    updateActionValuesFromPredictionError(predictionError_);
}

void Dopamine::updateActionValuesFromPredictionError(float predictionError) {
    // When prediction error is large, it indicates that some actions were better or worse than expected
    // This suggests that action values need adjustment
    
    // Find action that led to this prediction error
    // In a full implementation, this would track which action produced the error
    
    // For now, we'll update all action values slightly based on overall prediction error
    float adjustment = predictionError * actionLearningRate_ * 0.1f;
    
    for (size_t i = 0; i < actionValues_.size(); ++i) {
        // Decay action values toward baseline
        actionValues_[i] *= 0.99f;
        
        // Adjust based on prediction error sign
        if (predictionError_ > 0.1f) {
            // Better than expected - suggest the last action was good
            // Update random action to reflect overall positive outcome
            size_t randomAction = (i + 1) % actionValues_.size();
            actionValues_[randomAction] += adjustment * 0.5f;
        } else if (predictionError_ < -0.1f) {
            // Worse than expected - suggest last action was bad
            actionValues_[i] -= adjustment * 0.5f;
        }
    }
}

float Dopamine::getActionValue(ActionType action) const {
    size_t actionIdx = static_cast<size_t>(action);
    if (actionIdx >= actionValues_.size()) {
        return 0.0f;
    }
    return actionValues_[actionIdx];
}

void Dopamine::updateActionValue(ActionType action, float reward, float outcomeValue,
                                float learningRate, SimulationStep currentTime) {
    size_t actionIdx = static_cast<size_t>(action);
    if (actionIdx >= actionValues_.size()) {
        return;
    }
    
    // Temporal difference learning: Q(s,a) += alpha * (reward + gamma * V(s') - Q(s,a))
    // Here: Q(s,a) += alpha * (reward - Q(s,a)) if gamma = 0
    float tdError = reward - actionValues_[actionIdx];
    
    // Update action value
    actionValues_[actionIdx] += learningRate * tdError;
    
    // Clamp to reasonable range
    actionValues_[actionIdx] = std::clamp(actionValues_[actionIdx], -5.0f, 5.0f);
}

float Dopamine::getPlasticityFactor() const {
    // Plasticity factor scales synaptic plasticity based on dopamine level
    // High dopamine increases LTP, low dopamine increases LTD
    
    if (dopamineLevel_ > 2.0f) {
        // High dopamine promotes LTP (long-term potentiation)
        return 1.0f + (dopamineLevel_ - 2.0f) * 0.3f;
    } else if (dopamineLevel_ < 0.5f) {
        // Low dopamine promotes LTD (long-term depression)
        return 0.2f + dopamineLevel_ * 0.4f;
    } else {
        // Normal dopamine level
        return 1.0f;
    }
}

} // namespace nlm