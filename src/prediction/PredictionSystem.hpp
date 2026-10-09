#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>

namespace nlm {

// Prediction system: predicts future sensory states and consequences
// Uses neural activity patterns, working memory, and episodic memory for real predictive coding

class PredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Make prediction for next timestep using neural substrate
    std::unique_ptr<SensoryInput> predictNextState(const SensoryInput& currentState);
    
    // Update predictions based on actual observation and compute prediction error
    void updatePredictions(const SensoryInput& predicted, const SensoryInput& actual);
    
    // Get current prediction error signal
    float getPredictionError() const;
    
    // Get prediction confidence based on prediction error and neural activity
    float getConfidence() const;
    
    // Get prediction error history for analysis
    const std::vector<float>& getErrorHistory() const;
    
    // Clear prediction history
    void clearHistory();
    
    // Train prediction model from sensory observation
    void train(const SensoryInput& observation);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
