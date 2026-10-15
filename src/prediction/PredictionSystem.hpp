#pragma once

#include "../core/Types/Types.hpp"
#include "NeuralPrediction.hpp"
#include <vector>
#include <memory>

namespace nlm {

// Prediction system: predicts future sensory states and consequences using neural substrate
// Implements predictive coding through NeuralPrediction class
// Prediction emerges from learned synaptic dynamics and temporal sequences
// NOT a Transformer - purely neural dynamics-based prediction

class PredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Make prediction for next timestep using neural prediction system
    std::unique_ptr<SensoryInput> predictNextState(const SensoryInput& currentState);
    
    // Update predictions based on actual observation and compute prediction error
    void updatePredictions(const SensoryInput& predicted, const SensoryInput& actual);
    
    // Get prediction error (0 = perfect prediction, 1 = total error)
    float getPredictionError() const;
    
    // Get prediction confidence based on consistency of learned patterns
    float getConfidence() const;
    
    // Get prediction history (error over time)
    const std::vector<float>& getErrorHistory() const;
    void clearHistory();
    
    // Train prediction model with sensory observation
    void train(const SensoryInput& observation);
    
    // Get the underlying NeuralPrediction instance for advanced access
    NeuralPrediction* getNeuralPrediction() { return neuralPrediction_.get(); }
    
    // Check if prediction system has been initialized
    bool isInitialized() const { return neuralPrediction_ != nullptr; }
    
private:
    std::unique_ptr<NeuralPrediction> neuralPrediction_;
    
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
