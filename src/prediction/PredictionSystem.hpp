#pragma once

#include "../core/Types/Types.hpp"
#include "../sensory/SensoryInput.hpp"
#include <vector>
#include <memory>

namespace nlm {

// Prediction system: predicts future sensory states and consequences
// Implements predictive coding using neural substrate

class PredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem();
    
    // Update prediction system with new sensory input
    // Called by Brain at each timestep with working memory content
    void updateSensoryInput(const std::vector<float>& sensoryData);
    
    // Generate prediction for next sensory state
    // Uses current state and learned temporal patterns
    std::unique_ptr<SensoryInput> predictNextState(const SensoryInput& currentState);
    
    // Update predictions based on actual observed state
    // Computes prediction error and updates internal model
    void updatePredictions(const SensoryInput& predicted, const SensoryInput& actual);
    
    // Train prediction model with new observations
    // Learns from prediction errors to improve future predictions
    void train(const SensoryInput& observation);
    
    // Get prediction error (0 = perfect prediction, higher = larger error)
    float getPredictionError() const;
    
    // Get prediction confidence (0 = no confidence, 1 = high confidence)
    float getConfidence() const;
    
    // Get prediction history for analysis and debugging
    const std::vector<float>& getErrorHistory() const;
    
    // Clear prediction history (used for episodic memory consolidation)
    void clearHistory();
    
    // Get current prediction model weights (for learning analysis)
    const std::vector<float>& getPredictionModel() const;
    
    // Set prediction parameters
    void setDecayRate(float rate) { pImpl->decayRate = rate; }
    void setLearningRate(float rate) { pImpl->learningRate = rate; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
