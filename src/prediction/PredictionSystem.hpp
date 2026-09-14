#pragma once

#include "../prediction/NeuralPrediction.hpp"
#include "../neuromodulation/PredictionError.hpp"

namespace nlm {

// Prediction system: predicts future sensory states and consequences
// PLACEHOLDER - Phase 2 will implement real predictive coding

class PredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem();
    
    // Make prediction for next timestep
    // TODO PHASE 2: Implement real prediction
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
    
// Neural prediction access
    NeuralPrediction* getNeuralPrediction() { return neuralPrediction_.get(); }
    const NeuralPrediction* getNeuralPrediction() const { return neuralPrediction_.get(); }
    
    // Get prediction error signal (neuromodulator)
    const PredictionError* getPredictionError() const { return predictionError_.get(); }

private:
    // Neural prediction implementation
    std::unique_ptr<NeuralPrediction> neuralPrediction_;
    
    // Prediction error signal for neuromodulation
    std::unique_ptr<PredictionError> predictionError_;
    
    // Prediction system implementation
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
