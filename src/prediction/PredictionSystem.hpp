#pragma once

#include "../core/Types/Types.hpp"
#include "NeuralPrediction.hpp"
#include <vector>
#include <memory>

namespace nlm {

// Prediction system: predicts future sensory states and consequences
// Now implemented using the neural substrate via NeuralPrediction

class PredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Make prediction for next timestep using neural dynamics
    // Now implemented with real neural prediction
    std::unique_ptr<SensoryInput> predictNextState(const SensoryInput& currentState);
    
    // Update predictions based on actual observation and neural learning
    void updatePredictions(const SensoryInput& predicted, const SensoryInput& actual);
    
    // Get prediction error from neural prediction
    float getPredictionError() const;
    
    // Get prediction confidence from neural prediction
    float getConfidence() const;
    
    // Get prediction history
    const std::vector<float>& getErrorHistory() const;
    void clearHistory();
    
    // Train prediction model using neural substrate
    void train(const SensoryInput& observation);
    
    // Get underlying NeuralPrediction instance
    NeuralPrediction* getNeuralPrediction() const { return neuralPrediction_.get(); }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    std::unique_ptr<NeuralPrediction> neuralPrediction_;
};

} // namespace nlm
