#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <memory>

namespace nlm {

// DopamineModulation: Implements reward prediction error and reinforcement learning
// NOT biological dopamine - computational model of reward processing
//
// Key mechanisms:
// - Reward prediction error computation (delta rule)
// - Phasic vs tonic dopamine signaling
// - Plasticity modulation (LTD/LTP balance)
// - Expectation updating
// - Motivation scaling for action selection

class Dopamine {
public:
    Dopamine();
    ~Dopamine();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Update with prediction error
    // prediction_error = received_reward - predicted_reward
    void updatePredictionError(float predictionError, SimulationStep currentTime);
    
    // Get current dopamine level
    float getLevel() const { return dopamineLevel_; }
    
    // Get plasticity factor (0-1 scaling of synaptic plasticity)
    float getPlasticityFactor() const { return plasticityFactor_; }
    
    // Get reward prediction
    float getPredictedReward() const { return predictedReward_; }
    
    // Get expected value of current action
    float getExpectedValue() const { return expectedValue_; }
    
    // Get action value estimate (for action selection)
    float getActionValue(ActionType action) const;
    
    // Update action value using temporal difference learning
    void updateActionValue(ActionType action, float reward, float outcomeValue, 
                          float learningRate, SimulationStep currentTime);
    
    // Get all action values
    const std::vector<float>& getActionValues() const { return actionValues_; }
    
    // Set prediction error learning rate
    void setPredictionErrorLearningRate(float rate) { predictionErrorLearningRate_ = rate; }
    
    // Set action learning rate
    void setActionLearningRate(float rate) { actionLearningRate_ = rate; }
    
    // Get prediction error history
    const std::vector<float>& getPredictionErrorHistory() const { return predictionErrorHistory_; }
    
    // Clear history
    void clear() {
        predictionErrorHistory_.clear();
        actionValues_.assign(actionValues_.size(), 0.0f);
        expectedValue_ = 0.0f;
        predictedReward_ = 0.0f;
    }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    
    // Current state
    float dopamineLevel_;
    float predictionError_;
    float predictedReward_;
    float expectedValue_;
    SimulationStep lastUpdateStep_;
    
    // Parameters
    float predictionErrorLearningRate_;
    float actionLearningRate_;
    float rewardDecayRate_;
    
    // Learning history
    std::vector<float> predictionErrorHistory_;
    std::vector<float> actionValues_;
    std::vector<SimulationStep> effectTimesteps_;
};

} // namespace nlm