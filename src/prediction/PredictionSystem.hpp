#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>

namespace nlm {

// Forward declarations
class Brain;
class NeuralWorkingMemory;
class Curiosity;
class Neuromodulator;

// Prediction system: predicts future sensory states and consequences
// Phase 2: Real predictive coding using neural substrate

class PredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem();
    
    // Initialize with brain systems
    void initialize(Brain* brain, NeuralWorkingMemory* workingMem,
                    Curiosity* curiosity, Neuromodulator* dopamine);
    
    // Make prediction for next timestep
    // Uses neural activity patterns from working memory
    std::unique_ptr<SensoryInput> predictNextState(const SensoryInput& currentState);
    
    // Update predictions based on actual observation
    // Computes prediction error and updates internal models
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
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
