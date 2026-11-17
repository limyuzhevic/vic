#pragma once

#include "../core/Types/Types.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../brain/BrainSystem.hpp"
#include <memory>

namespace nlm {

// PredictionSystemWrapper: Wraps the core prediction system
// Provides brain-level interface to prediction capabilities
//
// Key mechanisms:
// - Manages prediction system lifecycle
// - Coordinates with neuromodulation for prediction error
// - Provides brain-level prediction interface
// - Manages prediction confidence and error signals

class PredictionSystemWrapper {
public:
    PredictionSystemWrapper();
    ~PredictionSystemWrapper();
    
    // Initialize prediction system
    void initialize(BrainSystem* brainSystem);
    
    // Core prediction operations
    // Generate predictions based on current state
    std::vector<float> predictNext(const std::vector<float>& currentState,
                                   const std::vector<ActionType>& possibleActions);
    
    // Evaluate action predictions
    float evaluateAction(ActionType action,
                        const std::vector<float>& currentState,
                        const std::vector<float>& expectedOutcome);
    
    // Update prediction system with actual outcomes
    void updatePredictions(const std::vector<float>& actualState,
                          const std::vector<ActionType>& actualActions,
                          float reward);
    
    // Get prediction confidence
    float getPredictionConfidence(const std::vector<float>& state) const;
    
    // Get prediction error signal
    float getPredictionError() const;
    
    // Get prediction statistics
    size_t getPredictionCount() const { return predictionCount_; }
    float getAveragePredictionError() const { return averagePredictionError_; }
    
    // Clear prediction cache
    void clearPredictions();
    
    // Set brain reference for dependency injection
    void setBrain(BrainSystem* brainSystem) { brainSystem_ = brainSystem; }
    
private:
    BrainSystem* brainSystem_;
    
    // Core prediction system
    std::unique_ptr<PredictionSystem> predictionSystem_;
    
    // Prediction statistics
    size_t predictionCount_;
    float averagePredictionError_;
    
    // Current predictions cache
    std::unordered_map<std::string, float> predictionCache_;
};

} // namespace nlm