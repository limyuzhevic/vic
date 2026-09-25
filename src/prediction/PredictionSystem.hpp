#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>

namespace nlm {

// Prediction system: predicts future sensory states and consequences
// Integrates with NeuralPrediction for actual predictive coding

class PredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem();
    
    // Initialize with brain reference for neural substrate access
    void initialize(Brain* brain);
    
    // Make prediction for next timestep using neural substrate
    std::unique_ptr<SensoryInput> predictNextState(const SensoryInput& currentState);
    
    // Update predictions based on actual observation and compute error
    void updatePredictions(const SensoryInput& predicted, const SensoryInput& actual);
    
    // Get prediction error
    float getPredictionError() const;
    
    // Get prediction confidence based on pattern consistency
    float getConfidence() const;
    
    // Get prediction history
    const std::vector<float>& getErrorHistory() const;
    void clearHistory();
    
    // Train prediction model with new observation
    void train(const SensoryInput& observation);
    
    // Update prediction system with neural activity patterns
    void updateWithNeuralActivity(const std::vector<float>& neuralActivity, SimulationStep timestep);
    
    // Get current prediction accuracy score
    float getPredictionAccuracy() const;
    
    // Automatic prediction update during agent processing
    void automaticUpdate(const SensoryInput& currentSensoryInput, SimulationStep timestep);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
