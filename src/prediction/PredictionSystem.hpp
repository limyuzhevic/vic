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
    
    // Update prediction system state
    void update(float dt);
    
    // Make prediction for next timestep
    // TODO PHASE 2: Implement real prediction
    std::unique_ptr<SensoryInput> predictNextState(const SensoryInput& currentState);
    
    // Update predictions based on actual observation
    void updatePredictions(const SensoryInput& predicted, const SensoryInput& actual);
    
    // Get prediction error
    float getPredictionError() const;
    
    // Get prediction change (temporal derivative)
    float getPredictionChange() const;
    
    // Get prediction surprise
    float getSurprise() const;
    
    // Get prediction confidence
    float getConfidence() const;
    
    // Get prediction history
    const std::vector<float>& getErrorHistory() const;
    void clearHistory();
    
    // Train prediction model
    void train(const SensoryInput& observation);
    
private:
    struct Impl {
        float predictionError;
        float predictionChange;
        float surprise;
        float confidence;
        std::vector<float> errorHistory;
        
        Impl() : predictionError(0.0f), predictionChange(0.0f), surprise(0.0f), confidence(0.5f) {}
    };
    
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};
