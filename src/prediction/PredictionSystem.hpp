#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>
#include "../brain/Brain.hpp"

namespace nlm {

// Prediction system: predicts future sensory states and consequences
// Integrated with NLM's neural substrate

class PredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem();
    
    // Initialize prediction system with brain reference
    void initialize(Brain* brain);
    
    // Make prediction for next timestep using neural substrate
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
    
    // Train prediction model using neural activity
    void train(const SensoryInput& observation);
    
    // Get brain reference
    Brain* getBrain() const;
    
    // Update prediction system dynamics
    void update(TimestepDuration dt);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
};

} // namespace nlm
