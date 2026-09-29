#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>

namespace nlm {

// Prediction system: predicts future sensory states and consequences
// Enhanced - now implements real predictive coding

class PredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem();
    
    // Make prediction for next timestep
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
    
    // Train prediction model
    void train(const SensoryInput& observation);
    
    // Use prediction error to modulate other systems
    void usePredictionError(float error);
    
    // Get derived signals
    float getCuriosityLevel() const;
    float getAttentionWeight() const;
    float getLearningSignal() const;
    
    // Reset system
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Derived signals
    float curiosityLevel_;
    float attentionWeight_;
    float learningSignal_;
    std::vector<std::vector<float>> observationHistory_;
    std::vector<float> averagePattern_;
    std::vector<float> lastPredictedState_;
    mutable std::mutex modelMutex_;
};

} // namespace nlm
