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
    
// Make prediction for next timestep
    // TODO PHASE 2: Implement real prediction using NLM's neural substrate
    // For Phase 6: Use neural substrate to generate predictions
    std::unique_ptr<SensoryInput> predictNextState(const SensoryInput& currentState);
    
    // Get prediction error
    float getPredictionError() const;
    
    // Get prediction confidence
    float getConfidence() const;
    
    // Get prediction history
    const std::vector<float>& getErrorHistory() const;
    void clearHistory();
    
    // Train prediction model
    void train(const SensoryInput& observation);
    
    // Get last observation used for training
    const SensoryInput* getLastObservation() const;
    
    // Integrate prediction with attention and working memory
    void applyPredictionToAttention();
    
    // Update prediction based on sensory input and brain state
    void updatePrediction(const SensoryInput& sensoryInput);
    
    // Generate action predictions based on internal state
    void generateActionPredictions(const std::vector<float>& internalState);
    
    // Calculate prediction uncertainty
    float getPredictionUncertainty() const;
    
    // Get expected sensory input
    const SensoryInput* getExpectedInput() const;
    
    // Set expected input from prediction
    void setExpectedInput(std::unique_ptr<SensoryInput> expected);
    
    // Clear prediction cache
    void clearPredictionCache();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
