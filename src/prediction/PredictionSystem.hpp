#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>

namespace nlm {

// Prediction system: predicts future sensory states and consequences
// PHASE 2: Now integrated with NeuralPrediction for real predictive coding

class NeuralPrediction;

class PredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem();
    
    // Initialize with brain reference and neural prediction
    void initialize(Brain* brain);
    
    // Make prediction for next timestep
    // Now uses NeuralPrediction's neural substrate
    std::unique_ptr<SensoryInput> predictNextState(const SensoryInput& currentState);
    
    // Update predictions based on actual observation
    // Computes prediction error and updates neural prediction
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
    
    // Access to underlying neural prediction
    NeuralPrediction* getNeuralPrediction() { return neuralPrediction_.get(); }
    const NeuralPrediction* getNeuralPrediction() const { return neuralPrediction_.get(); }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Underlying neural prediction system
    std::unique_ptr<NeuralPrediction> neuralPrediction_;
    
    // Integration state
    Brain* brain_;
};

} // namespace nlm
