#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>
#include <unordered_map>

namespace nlm {

// Forward declaration
class Brain;

// Prediction system: predicts future sensory states and consequences
// Phase 6: Implements real predictive coding with neural substrate
//
// Key mechanisms:
// - Forward model prediction of sensory input
// - Prediction error signals for learning
// - Temporal sequence prediction
// - Confidence-based exploration
// - Integration with working memory and attention

class PredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Make prediction for next timestep based on current state
    // Uses working memory and sensory history to predict future states
    std::unique_ptr<SensoryInput> predictNextState(const SensoryInput& currentState);
    
    // Update predictions based on actual observation and prediction error
    void updatePredictions(const SensoryInput& predicted, const SensoryInput& actual);
    
    // Get prediction error signal
    float getPredictionError() const;
    
    // Get prediction confidence (0.0 - 1.0)
    float getConfidence() const;
    
    // Get prediction error history
    const std::vector<float>& getErrorHistory() const;
    void clearHistory();
    
    // Train prediction model with observation
    void train(const SensoryInput& observation);
    
    // Update prediction system (called each simulation step)
    void update(TimestepDuration dt);
    
    // Get predicted neural activity pattern
    const std::vector<float>& getPredictedPattern() const { return predictedPattern_; }
    
    // Store pattern in system for future prediction
    void storePattern(const std::vector<float>& pattern, SimulationStep step);
    
    // Get sequence of recent patterns
    std::vector<std::pair<std::vector<float>, SimulationStep>> getPatternHistory(size_t count) const;
    
    // Clear all predictions
    void clear();
    
    // Get current prediction level (0.0 - 1.0)
    float getPredictionLevel() const { return predictionLevel_; }
    
    // Set prediction parameters
    void setPredictionHorizon(float horizon) { predictionHorizon_ = horizon; }
    void setPredictionAccuracy(float accuracy) { predictionAccuracy_ = accuracy; }
    void setPredictionConfidence(float confidence) { predictionConfidence_ = confidence; }
    
    // Get sequence memory for pattern learning
    std::vector<std::vector<float>> getSequenceMemory() const { return sequenceMemory_; }
    
    // Learn from sequence of observations
    void learnSequence(const std::vector<SensoryInput>& sequence);
    
private:
    // Compute prediction based on current state and history
    void computePrediction(const SensoryInput& currentState);
    
    // Update temporal sequence prediction
    void updateSequencePrediction();
    
    // Calculate prediction error
    float calculatePredictionError(const SensoryInput& predicted, const SensoryInput& actual);
    
    // Update prediction confidence based on accuracy
    void updateConfidence(float error);
    
    // Apply prediction to working memory
    void applyPredictionToMemory();
    
    // Store prediction in sequence memory
    void storePredictionInMemory(const std::vector<float>& pattern);
    
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    
    // Prediction state
    std::vector<float> predictedPattern_;
    float predictionError_;
    float confidence_;
    float predictionLevel_;
    float predictionHorizon_;
    float predictionAccuracy_;
    float predictionConfidence_;
    
    // History
    std::vector<float> errorHistory_;
    std::vector<std::vector<float>> sequenceMemory_;
    std::vector<SimulationStep> patternTimestamps_;
    
    // Temporal prediction
    size_t predictionStep_;
    size_t sequencePosition_;
    
    // Pattern learning
    size_t patternWindowSize_;
    size_t minSequenceLength_;
};

} // namespace nlm
