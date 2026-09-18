#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>
#include "../sensory/SensoryInput.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../motor/Action.hpp"

namespace nlm {

// Prediction system: predicts future sensory states and consequences
// PLACEHOLDER - Phase 2 will implement real predictive coding

class PredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem();
    
    // Make prediction for next timestep
    // TODO PHASE 2: Implement real prediction
    std::unique_ptr<SensoryInput> predictNextState(const SensoryInput& currentState);
    
    // Update predictions based on actual observation
    void updatePredictions(const SensoryInput& predicted, const SensoryInput& actual);
    
    // Get prediction error
    float getPredictionError() const;
    
    // Get prediction confidence
    float getConfidence() const;
    
    // Get prediction history
    const std::vector<float>& getErrorHistory() const;
    void clearHistory();
    
    // Train prediction model
    void train(const SensoryInput& observation);
    
    // ========== PREDICTION SYSTEM INTEGRATION ==========
    
    // Update prediction from sensory input
    void updateFromSensory(const SensoryInput& input);
    
    // Update prediction from working memory content
    void updateFromWorkingMemory(const NeuralWorkingMemory& wm);
    
    // Update prediction from episodic memory
    void updateFromEpisodicMemory(const NeuralEpisodicMemory& em);
    
    // Update action plans based on predictions
    void updateActionPlans(const std::vector<MotorCommand>& actions);
    
    // Get predicted action plans
    const std::vector<MotorCommand>& getActionPlans() const;
    
    // Clear action plans
    void clearActionPlans();
    
    // Compute expected reward based on action
    float computeExpectedReward(const MotorCommand& action) const;
    
    // Set learning rate for prediction updates
    void setLearningRate(float rate) { learningRate_ = rate; }
    
    // Set prediction horizon (number of timesteps ahead)
    void setPredictionHorizon(size_t horizon) { predictionHorizon_ = horizon; }
    
    // Set reward prediction threshold
    void setRewardPredictionThreshold(float threshold) { rewardThreshold_ = threshold; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Integration parameters
    float learningRate_;
    size_t predictionHorizon_;
    float rewardThreshold_;
    
    // Current prediction state
    std::vector<MotorCommand> actionPlans_;
};

} // namespace nlm
