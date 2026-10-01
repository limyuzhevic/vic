#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include "../sensory/SensoryInput.hpp"
#include <vector>
#include <memory>

namespace nlm {

// PredictionSystem: High-level predictive coding interface for the brain
// Integrates NeuralPrediction for detailed temporal and action-consequence predictions
// Provides prediction error signals and confidence monitoring

class PredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Make prediction for next timestep based on current sensory state
    // Uses learned temporal sequences to predict future sensory input
    std::unique_ptr<SensoryInput> predictNextState(const SensoryInput& currentState);
    
    // Update predictions based on actual observation
    // Computes prediction error and modulates neural learning
    void updatePredictions(const SensoryInput& predicted, const SensoryInput& actual);
    
    // Get prediction error (0 = perfect prediction, 1 = total error)
    float getPredictionError() const;
    
    // Get prediction confidence based on consistency of learned patterns
    float getConfidence() const;
    
    // Get prediction history for analysis
    const std::vector<float>& getErrorHistory() const;
    void clearHistory();
    
    // Train prediction model with new observation
    void train(const SensoryInput& observation);
    
    // Advanced prediction capabilities (integrated with NeuralPrediction)
    
    // Generate prediction for specific action
    std::vector<float> predictActionConsequence(ActionType action,
                                                const std::vector<float>& currentState);
    
    // Multi-step prediction: predict N steps into the future
    std::vector<std::vector<float>> predictMultipleSteps(
        const std::vector<float>& currentState,
        size_t numSteps);
    
    // Enable/disable prediction mechanisms
    void enableTemporalPrediction(bool enable) { temporalPredictionEnabled_ = enable; }
    void enableActionConsequencePrediction(bool enable) { actionConsequenceEnabled_ = enable; }
    
    // Configuration
    void setPredictionHorizon(size_t steps) { predictionHorizon_ = steps; }
    void setSequenceMemorySize(size_t size) { sequenceMemorySize_ = size; }
    
    // Check if prediction system is active
    bool isTemporalPredictionEnabled() const { return temporalPredictionEnabled_; }
    bool isActionConsequencePredictionEnabled() const { return actionConsequenceEnabled_; }
    
    // Get current prediction state
    float getCurrentError() const { return predictionError_; }
    float getCurrentConfidence() const { return predictionConfidence_; }
    
    // Clear all learned predictions
    void clearPredictions();
    
    // Get statistics
    size_t getPredictionCount() const { return predictionCount_; }
    size_t getTrainingCount() const { return trainingCount_; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Brain reference
    Brain* brain_;
    
    // Configuration
    bool temporalPredictionEnabled_;
    bool actionConsequenceEnabled_;
    size_t predictionHorizon_;
    size_t sequenceMemorySize_;
    
    // State
    float predictionError_;
    float predictionConfidence_;
    size_t predictionCount_;
    size_t trainingCount_;
    std::vector<float> errorHistory_;
};

} // namespace nlm
