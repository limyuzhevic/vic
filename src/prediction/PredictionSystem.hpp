#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>

namespace nlm {

// Prediction system: predicts future sensory states and consequences
// NOW IMPLEMENTED with real neural prediction system

class PredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem();
    
    // Initialize with brain reference for neural prediction
    void initialize(Brain* brain);
    
    // Generate prediction for next timestep based on current state
    // Uses NeuralPrediction for forward model
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
    
    // NeuralPrediction access
    NeuralPrediction* getNeuralPrediction() { return neuralPrediction_.get(); }
    const NeuralPrediction* getNeuralPrediction() const { return neuralPrediction_.get(); }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Integrated neural prediction system
    std::unique_ptr<NeuralPrediction> neuralPrediction_;
};

} // namespace nlm
