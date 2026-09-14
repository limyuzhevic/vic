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
    
    // Make prediction for next timestep
    // TODO PHASE 2: Implement real prediction
    std::unique_ptr<SensoryInput> predictNextState(const SensoryInput& currentState);
    
    // Update predictions based on actual observation
    void updatePredictions(const SensoryInput& predicted, const SensoryInput& actual);
    
    // Main update method called during brain step loop
    // Encodes sensory input into state representation, predicts next state,
    // computes prediction error, and updates internal model
    void update(const SensoryInput& currentSensory, float neuralState,
                const std::vector<float>& workingMemory);
    
    // Get prediction error
    float getPredictionError() const;
    
    // Get prediction confidence
    float getConfidence() const;
    
    // Get prediction history
    const std::vector<float>& getErrorHistory() const;
    void clearHistory();
    
    // Train prediction model
    void train(const SensoryInput& observation);
    
    // Get current model state
    const std::vector<float>& getModelState() const { return pImpl->currentModel; }
    
    // Get prediction statistics
    size_t getSuccessfulPredictions() const { return pImpl->successfulPredictions; }
    size_t getTotalPredictions() const { return pImpl->totalPredictions; }
    
    // Get average confidence
    float getAverageConfidence() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
