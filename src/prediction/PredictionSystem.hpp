#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>

namespace nlm {

// Prediction system: predicts future sensory states and consequences
// REAL IMPLEMENTATION - Phase 6 integration complete

class PredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem();
    
    // Initialize with brain reference for neural integration
    void initialize(Brain* brain);
    
    // Make prediction for next timestep
    // REAL implementation using neural substrate
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
    
    // Direct access to neural prediction components for advanced usage
    const std::vector<float>& getPredictionNeurons() const;
    const std::vector<std::vector<float>>& getSequenceMemory() const;
    
    // Get prediction error signal
    const PredictionErrorSignal::ErrorComponents& getErrorComponents() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
