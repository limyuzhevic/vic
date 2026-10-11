#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>

namespace nlm {

// Prediction system: predicts future sensory states and consequences
// REAL implementation using NLM's neural substrate
//
// Key mechanisms:
// - Uses action-consequence predictions
// - Integrates with neural activity patterns
// - Drives attention and planning
// - Provides temporal structure

class PredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem();
    
    // Make prediction for next timestep based on current neural state
    std::unique_ptr<SensoryInput> predictNextState(const std::vector<float>& currentPattern);
    
    // Update predictions based on actual observation
    void updatePredictions(const SensoryInput& predicted, const SensoryInput& actual);
    
    // Get prediction error
    float getPredictionError() const;
    
    // Get prediction confidence
    float getConfidence() const;
    
    // Get prediction history
    const std::vector<float>& getErrorHistory() const;
    void clearHistory();
    
    // Train prediction model from neural pattern
    void trainFromPattern(const std::vector<float>& pattern);
    
    // Apply prediction effects on neural dynamics
    void applyPrediction(TimestepDuration dt);
    
    // Set prediction horizon (how far ahead to predict)
    void setPredictionHorizon(size_t horizon) { predictionHorizon_ = horizon; }
    
    // Get prediction horizon
    size_t getPredictionHorizon() const { return predictionHorizon_; }
    
    // Enable/disable prediction
    void enable(bool enable) { isEnabled_ = enable; }
    bool isEnabled() const { return isEnabled_; }
    
    // Get current prediction strength
    float getPredictionStrength() const;
    
    // Clear all predictions
    void clear();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    bool isEnabled_;
    size_t predictionHorizon_;
};

} // namespace nlm
