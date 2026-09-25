#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>

namespace nlm {

// Prediction system: predicts future sensory states and consequences
// REAL IMPLEMENTATION - Uses neural substrate for predictive coding

class PredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem();
    
    // Make prediction for next timestep using neural substrate
    // Returns predicted sensory input based on current state and neural dynamics
    std::unique_ptr<SensoryInput> predictNextState(const SensoryInput& currentState);
    
    // Update predictions based on actual observation
    // Computes prediction error using neural similarity metrics
    void updatePredictions(const SensoryInput& predicted, const SensoryInput& actual);
    
    // Get prediction error (lower is better)
    float getPredictionError() const;
    
    // Get prediction confidence (higher is better)
    float getConfidence() const;
    
    // Get prediction history for analysis
    const std::vector<float>& getErrorHistory() const;
    void clearHistory();
    
    // Train prediction model from experience
    // Learns patterns from observed sensory inputs using neural plasticity
    void train(const SensoryInput& observation);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
