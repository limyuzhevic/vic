#pragma once

#include "../core/Types/Types.hpp"
#include "../motor/Action.hpp"
#include <vector>
#include <memory>

namespace nlm {

// Forward declarations for systems that prediction integrates with
class Brain;
class PredictionError;
class Curiosity;
class NeuralWorkingMemory;
class NeuralEpisodicMemory;
class Hebbian;

// Forward declarations for prediction system
class SensoryInput;
class InternalSignals;

// Prediction system: predicts future sensory states and consequences
// Real implementation: uses neural substrate for predictive coding and temporal dynamics

class PredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem();
    
    // Initialize prediction system with brain reference for neural integration
    void initialize(Brain* brain);
    
    // Make prediction for next timestep using neural substrate
    std::unique_ptr<SensoryInput> predictNextState(const SensoryInput& currentState);
    
    // Update predictions based on actual observation with neural error computation
    void updatePredictions(const SensoryInput& predicted, const SensoryInput& actual);
    
    // Get prediction error
    float getPredictionError() const;
    
    // Get prediction confidence (based on prediction error)
    float getConfidence() const;
    
    // Get prediction history
    const std::vector<float>& getErrorHistory() const;
    void clearHistory();
    
    // Train prediction model using neural plasticity
    void train(const SensoryInput& observation);
    
    // Configure prediction system parameters
    void setLearningRate(float rate);
    float getLearningRate() const;
    
    // Get current prediction model weights
    const std::vector<float>& getPredictionWeights() const;
    
    // Enable/disable prediction training
    void enableTraining(bool enable);
    bool isTrainingEnabled() const;
    
private:
    void initializePredictionModel(size_t inputSize);
    std::vector<float> extractNeuralActivity() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
