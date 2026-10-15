#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>

namespace nlm {

// Prediction system: predicts future sensory states and consequences
// Phase 6: Integrated with neural substrate for real-time prediction
//
// Key mechanisms:
// - Predicts next sensory states based on current neural activity
// - Computes prediction errors to drive learning
// - Integrates with neuromodulation (dopamine) for reward prediction
// - Provides anticipation for action planning
// - Connected to working memory for expectation management

class PredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem();
    
    // Make prediction for next timestep based on current neural state
    std::unique_ptr<SensoryInput> predictNextState(const class Brain& brain);
    
    // Update predictions based on actual observation
    void updatePredictions(const SensoryInput& predicted, const SensoryInput& actual);
    
    // Get prediction error for learning
    float getPredictionError() const;
    
    // Get prediction confidence
    float getConfidence() const;
    
    // Get prediction history
    const std::vector<float>& getErrorHistory() const;
    void clearHistory();
    
    // Train prediction model from experience
    void train(const SensoryInput& observation, float reward);
    
    // Apply neuromodulation effects on prediction
    void applyNeuromodulation(float neuromodulatorLevel);
    
    // Reset prediction system
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
