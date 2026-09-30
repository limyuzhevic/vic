#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>
#include <unordered_map>
#include <cmath>

namespace nlm {

class NeuralPrediction;
class RandomGenerator;

class PredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem();
    
    // Initialize with brain reference for neural prediction
    void initialize(NeuralPrediction* neuralPrediction, RandomGenerator* rng);
    
    // Make prediction using neural dynamics
    std::unique_ptr<SensoryInput> predictNextState(const SensoryInput& currentState);
    
    // Update predictions based on actual observation using prediction error
    void updatePredictions(const SensoryInput& predicted, const SensoryInput& actual);
    
    // Get prediction error
    float getPredictionError() const;
    
    // Get prediction confidence (0.0 to 1.0)
    float getConfidence() const;
    
    // Get prediction history
    const std::vector<float>& getErrorHistory() const;
    void clearHistory();
    
    // Train prediction model using neural plasticity
    void train(const SensoryInput& observation, float reward);
    
    // Reset prediction system
    void reset();
    
    // Get neural prediction statistics
    float getPredictionStability() const;
    float getPredictionAccuracy() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
