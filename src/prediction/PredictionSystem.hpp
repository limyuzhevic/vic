#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>

namespace nlm {

// Prediction system: predicts future sensory states and consequences
// PLACEHOLDER - Phase 2 will implement real predictive coding

class PredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem();
    
    // Initialize with brain for neural integration
    void initialize(Brain* brain);
    
    // Make prediction for next timestep based on current neural activity
    // Uses predictive coding with prediction error minimization
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
    
    // Train prediction model with new observation
    // Updates internal predictive models based on experience
    void train(const SensoryInput& observation);
    
    // Update prediction system for new timestep
    void update(double timestep);
    
    // Get most active prediction neurons
    std::vector<NeuronId> getActivePredictionNeurons(size_t count) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
