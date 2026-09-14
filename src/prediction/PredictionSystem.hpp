#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>

namespace nlm {

// Prediction system: predicts future sensory states and consequences
// Real neural prediction using LIF dynamics, synaptic plasticity, and temporal sequences

class PredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem();
    
    // Configure prediction system with brain reference
    void configure(Brain* brain);
    
    // Make prediction for next timestep using neural dynamics
    std::unique_ptr<SensoryInput> predictNextState(const SensoryInput& currentState);
    
    // Update predictions based on actual observation and learn from prediction errors
    void updatePredictions(const SensoryInput& predicted, const SensoryInput& actual);
    
    // Get prediction error (0 = perfect prediction, 1 = total error)
    float getPredictionError() const;
    
    // Get prediction confidence based on pattern consistency
    float getConfidence() const;
    
    // Get prediction history for analysis
    const std::vector<float>& getErrorHistory() const;
    void clearHistory();
    
    // Train prediction model with new observation
    void train(const SensoryInput& observation);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm