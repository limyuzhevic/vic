#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>

namespace nlm {

// Forward declaration to avoid circular dependency
class SensoryInput;

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
    
    // Reset system state
    void reset();
    
    // Get average prediction error
    float getAverageError() const;
    
    // Check if predictions are stable
    bool isStable() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
