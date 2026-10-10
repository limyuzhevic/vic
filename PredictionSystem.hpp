#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <deque>
#include <memory>

namespace nlm {

// Prediction system: predicts future sensory states and consequences
// PLACEHOLDER - Phase 2 will implement real predictive coding

class PredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem();
    
    // Initialize prediction system with brain reference
    void initialize(Brain* brain);
    
    // Generate prediction for next timestep
    std::vector<float> generatePrediction(SimulationStep currentStep);
    
    // Update predictions based on actual observation
    float updateWithObservation(const std::vector<float>& actualState, 
                               SimulationStep currentStep);
    
    // Get prediction error
    float getPredictionError() const;
    
    // Get prediction confidence
    float getPredictionConfidence() const;
    
    // Get prediction history
    const std::deque<float>& getErrorHistory() const;
    
    // Clear prediction history
    void clearHistory();
    
    // Get prediction level (0-1 scale)
    float getPredictionLevel() const;
    
    // Get prediction accuracy (0-1 scale)
    float getPredictionAccuracy() const;
    
    // Get most recent prediction
    const std::vector<float>& getMostRecentPrediction() const;
    
    // Add prediction to history
    void addToPredictionHistory(float predictionError);
    
    // Compute prediction error between predicted and actual states
    float computePredictionError(const std::vector<float>& predicted,
                                const std::vector<float>& actual) const;
    
    // Update prediction weights based on error and reward
    void updatePredictionWeights(float error, float reward);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm