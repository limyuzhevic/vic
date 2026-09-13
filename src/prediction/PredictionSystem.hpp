#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>

namespace nlm {

// Prediction system: predicts future sensory states and consequences
// Now implements real predictive coding using the neural substrate
//
// Key principles:
// - Prediction is encoded in patterns of neural activity
// - Learning occurs through prediction error signals
// - Integration with working memory, attention, and neuromodulation
// - Drives curiosity and exploration through prediction errors

class PredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem();
    
    // Initialize with brain reference for system integration
    void initialize(Brain* brain);
    
    // Make prediction for next timestep
    // Uses working memory and attention to generate predictions
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
    
    // Update prediction from actual sensory input
    void updatePredictionFromSensory(const SensoryInput& sensoryInput);
    
    // Update prediction based on attentional focus
    void updatePredictionFromAttention();
    
    // Update prediction based on working memory
    void updatePredictionFromWorkingMemory();
    
    // Compute prediction error for neuromodulation
    void computePredictionError();
    
    // Reset prediction system state
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
