#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>

namespace nlm {

// Prediction system: predicts future sensory states and consequences
// Real implementation based on predictive coding and forward models

class PredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem();
    
    // Make prediction for next timestep based on current state
    // Uses learned forward model to predict next sensory state
    std::unique_ptr<SensoryInput> predictNextState(const SensoryInput& currentState);
    
    // Update predictions based on actual observation
    // Computes prediction error and updates internal model
    void updatePredictions(const SensoryInput& predicted, const SensoryInput& actual);
    
    // Get prediction error magnitude
    float getPredictionError() const;
    
    // Get prediction confidence (higher = more certain)
    float getConfidence() const;
    
    // Get prediction error history
    const std::vector<float>& getErrorHistory() const;
    void clearHistory();
    
    // Train prediction model on observed data
    void train(const SensoryInput& observation);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
