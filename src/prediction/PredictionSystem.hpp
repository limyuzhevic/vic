#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>
#include "../brain/Brain.hpp"

namespace nlm {

// Prediction system: predicts future sensory states and consequences using neural activity patterns
// Phase 2 implementation - integrates with episodic memory for pattern completion

class PredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem();
    
    // Initialize with brain reference for accessing memory systems
    void initialize(Brain* brain);
    
    // Make prediction for next timestep using neural activity patterns
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
    
    // Train prediction model using episodic memory
    void train(const SensoryInput& observation);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    Brain* brain_;
};

} // namespace nlm
