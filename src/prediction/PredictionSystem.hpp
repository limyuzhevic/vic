#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>

namespace nlm {

// Prediction system: predicts future sensory states and consequences
// Uses neural activity patterns to predict upcoming sensory states
// Integrates with concept formation and memory systems for better predictions

class PredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem();
    
    // Make prediction for next timestep based on current neural state
    std::unique_ptr<SensoryInput> predictNextState(const SensoryInput& currentState);
    
    // Update predictions based on actual observation
    // Computes prediction error and updates internal model
    void updatePredictions(const SensoryInput& predicted, const SensoryInput& actual);
    
    // Get current prediction error (difference between predicted and actual)
    float getPredictionError() const;
    
    // Get prediction confidence (how certain the system is)
    float getConfidence() const;
    
    // Get prediction error history for analysis
    const std::vector<float>& getErrorHistory() const;
    void clearHistory();
    
    // Train prediction model with new observations
    void train(const SensoryInput& observation);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
