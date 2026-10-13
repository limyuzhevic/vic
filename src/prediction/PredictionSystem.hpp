#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>

namespace nlm {

// Forward declaration for Brain
class Brain;

// Prediction system: predicts future sensory states and consequences
// IMPLEMENTED - Real predictive coding using neural substrate

class PredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Make prediction for next timestep
    // Uses neural dynamics and learned patterns to predict future states
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
    
    // Train prediction model using neural plasticity
    void train(const SensoryInput& observation);
    
    // Get predicted future state based on current brain state
    std::vector<float> getPredictedState() const { return pImpl->predictedStates; }
    
    // Get last observation state
    std::vector<float> getLastObservation() const { return pImpl->observationStates; }
    
    // Set prediction parameters
    void setLearningRate(float rate) { pImpl->learningRate = rate; }
    void setDecayRate(float rate) { pImpl->decayRate = rate; }
    
    // Store predicted input for comparison with actual
    void setPredictedInput(const std::unique_ptr<SensoryInput>& input) {
        pImpl->predictedInput = std::make_unique<SensoryInput>(*input);
    }
    
    // Get stored predicted input
    const SensoryInput& getPredictedInput() const { return *pImpl->predictedInput; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
};

} // namespace nlm
