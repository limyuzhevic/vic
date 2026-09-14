#pragma once

#include "../core/Types/Types.hpp"
#include "NeuralPredictionModel.hpp"
#include <vector>
#include <memory>

namespace nlm {

// Prediction system: predicts future sensory states and consequences
// Uses neural population activity and memory systems for real predictive coding

class PredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Make prediction for next timestep
    // Uses neural forward model with working memory and episodic memory
    std::unique_ptr<SensoryInput> predictNextState(const SensoryInput& currentState);
    
    // Update predictions based on actual observation
    // Computes prediction error using neural dynamics
    void updatePredictions(const SensoryInput& predicted, const SensoryInput& actual);
    
    // Get prediction error
    float getPredictionError() const;
    
    // Get prediction confidence
    float getConfidence() const;
    
    // Get prediction history
    const std::vector<float>& getErrorHistory() const;
    void clearHistory();
    
    // Train prediction model
    // Updates neural prediction weights using STDP
    void train(const SensoryInput& observation);
    
    // Update prediction with reward signal
    void updateWithReward(float reward);
    
    // Get current neural prediction
    const std::vector<float>& getNeuralPrediction() const;
    
    // Set prediction system parameters
    void setPredictionGain(float gain);
    void setPredictionDecay(float decay);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
