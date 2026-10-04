#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>

namespace nlm {

// Prediction system: predicts future sensory states and consequences
// Implements predictive coding with forward models

class PredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Make prediction for next timestep
    // Uses stored forward model to predict sensory consequences of actions
    std::unique_ptr<SensoryInput> predictNextState(const SensoryInput& currentState,
                                                   const class Action& action);
    
    // Update predictions based on actual observation
    // Computes prediction error and updates forward model
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
    
    // Process sensory input - update predictions and store for learning
    void processSensoryInput(const SensoryInput& input);
    
    // Get expected reward for predicted state
    float getExpectedReward(const SensoryInput& predictedState) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
};

} // namespace nlm
