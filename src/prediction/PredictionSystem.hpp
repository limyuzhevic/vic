#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>

namespace nlm {

// Prediction system: predicts future sensory states and consequences
// Implements predictive coding using neural population dynamics
// Based on biological prediction error minimization (predictive coding theory)

class PredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem();
    
    // Initialize prediction system with neural components
    void initialize(Brain* brain, RandomGenerator* rng);
    
    // Make prediction for next timestep using current neural state
    // Uses population vectors and temporal correlations to predict future states
    std::unique_ptr<SensoryInput> predictNextState(const SensoryInput& currentState);
    
    // Update predictions based on actual observation and compute prediction error
    // Implements predictive coding: error drives learning and updates predictions
    void updatePredictions(const SensoryInput& predicted, const SensoryInput& actual);
    
    // Get prediction error (magnitude of difference between predicted and actual)
    float getPredictionError() const;
    
    // Get prediction confidence (inverse of error variance)
    float getConfidence() const;
    
    // Get prediction error history for analysis
    const std::vector<float>& getErrorHistory() const;
    void clearHistory();
    
    // Train prediction model using recent experiences
    // Extracts patterns and builds predictive weights
    void train(const SensoryInput& observation);
    
    // Process prediction through neural substrate
    // Implements neural prediction using working memory patterns
    void processPrediction(std::unique_ptr<SensoryInput>& prediction);
    
    // Get average prediction across recent history
    SensoryInput getAveragePrediction() const;
    
    // Check if prediction system is properly integrated
    bool isIntegrated() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
