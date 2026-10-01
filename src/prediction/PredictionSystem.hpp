#pragma once

#include "../core/Types/Types.hpp"
#include "../sensory/SensoryInput.hpp"
#include <vector>
#include <memory>

namespace nlm {

// Prediction system: predicts future sensory states and consequences
// Implements simple pattern-based prediction for initial integration

class PredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem();
    
    // Initialize prediction system
    void initialize(class Brain* brain);
    
    // Make prediction for next timestep based on current state
    std::unique_ptr<SensoryInput> predictNextState(const SensoryInput& currentState);
    
    // Update predictions based on actual observation
    void updatePredictions(const SensoryInput& predicted, const SensoryInput& actual);
    
    // Get prediction error
    float getPredictionError() const;
    
    // Get prediction confidence
    float getConfidence() const;
    
    // Get prediction history
    const std::vector<float>& getErrorHistory() const;
    void clearHistory();
    
    // Train prediction model with observation
    void train(const SensoryInput& observation);
    
    // Get recent prediction errors for neuromodulation
    std::vector<float> getRecentErrors(size_t count) const;
    
    // Compute prediction error magnitude (absolute value)
    float getPredictionErrorMagnitude() const;
    
    // Get prediction statistics
    struct PredictionStats {
        float meanError;
        float maxError;
        float minError;
        float variance;
    };
    PredictionStats getStats() const;
    
    // Store prediction in working memory
    void storePrediction(const SensoryInput& prediction, const SensoryInput& actual, float error);
    
    // Update confidence based on prediction performance
    void updateConfidence(float error, bool predictionCorrect);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm