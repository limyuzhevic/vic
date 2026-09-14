#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <cmath>
#include <unordered_map>

namespace nlm {

// NeuralPredictionModel: Forward model implemented as neural network
// Uses neural population activity to predict next state
// Implemented as connections between neural populations representing current and predicted states

class NeuralPredictionModel {
public:
    NeuralPredictionModel();
    ~NeuralPredictionModel();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Set up forward model connections
    // Connects populations that predict next state
    void setupForwardConnections(const std::vector<float>& currentPattern);
    
    // Predict next state using neural forward model
    // Returns predicted neural activity pattern
    std::vector<float> predictNextState(const std::vector<float>& currentPattern) const;
    
    // Compute prediction error at neural level
    // Compares predicted and actual neural activity
    float computeNeuralPredictionError(const std::vector<float>& predicted,
                                      const std::vector<float>& actual) const;
    
    // Update prediction weights using STDP
    void train(const std::vector<float>& currentPattern, const std::vector<float>& actualPattern);
    
    // Update prediction based on reward signals
    void updateWithReward(float reward);
    
    // Get prediction confidence
    float getPredictionConfidence() const;
    
    // Get current prediction
    const std::vector<float>& getCurrentPrediction() const;
    
    // Clear prediction
    void clear();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
};

} // namespace nlm
