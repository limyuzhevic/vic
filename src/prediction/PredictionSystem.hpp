#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>
#include <random>
#include <algorithm>
#include <cmath>

namespace nlm {

// Prediction system: predicts future sensory states and consequences
// Uses neural substrate for predictive coding and forward models

class Brain;

class PredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Make prediction for next timestep using neural forward model
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
    
    // Train prediction model with sensory input
    void train(const SensoryInput& observation);
    
    // Update prediction system (called each brain step)
    void update(double timestep);
    
    // Get top-k most likely predictions
    std::vector<std::pair<float, std::unique_ptr<SensoryInput>>> getTopPredictions(int k) const;
    
    // Set prediction parameters
    void setPredictionHorizon(float horizon) { predictionHorizon_ = horizon; }
    void setLearningRate(float rate) { learningRate_ = rate; }
    void setConfidenceThreshold(float threshold) { confidenceThreshold_ = threshold; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Neural substrate components
    Brain* brain_;
    
    // Prediction parameters
    float predictionHorizon_;
    float learningRate_;
    float confidenceThreshold_;
    
    // Internal prediction model state
    std::vector<float> predictionWeights_;
    std::vector<float> predictionErrors_;
    std::mt19937 rng_;
};

} // namespace nlm
