#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>

namespace nlm {

// Prediction system: predicts future sensory states and consequences
// PLACEHOLDER - Phase 2 will implement real predictive coding

class PredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem();
    
    // Update prediction with current neural activity pattern
    // This implements predictive coding: predicting what sensory input should be
    // based on current internal state (working memory, recurrent activity)
    void update(const std::vector<float>& neuralPattern, SimulationStep step, Timestamp time);
    
    // Make prediction for next timestep
    // TODO PHASE 2: Implement real prediction using NLM's neural substrate
    std::unique_ptr<SensoryInput> predictNextState(const std::vector<float>& currentPattern);
    
    // Update predictions based on actual sensory observation
    void updatePredictions(const SensoryInput& predicted, const SensoryInput& actual);
    
    // Get prediction error (lower is better)
    float getPredictionError() const;
    
    // Get prediction confidence
    float getConfidence() const;
    
    // Get prediction history
    const std::vector<float>& getErrorHistory() const;
    void clearHistory();
    
    // Train prediction model with observed pattern
    void train(const std::vector<float>& observation);
    
    // Get best predicted next state
    std::vector<float> getPredictedNextState() const;
    
    // Get most likely action based on predictions
    MotorCommand getPredictedAction() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
