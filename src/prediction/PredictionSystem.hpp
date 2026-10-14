#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>
#include <map>
#include <unordered_set>

namespace nlm {

class NeuralPrediction;
class NeuralWorkingMemory;
class AttentionalSelection;
class NeuralPlanner;
class STDP;
class Hebbian;

// Enhanced prediction system: generates real-time predictions from sensory input,
// computes prediction errors, and integrates with neural plasticity, attention, 
// working memory, and action planning

class PredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem();
    
    // Initialize prediction system with brain reference
    void initialize(Brain* brain);
    
    // Make prediction for next timestep based on current sensory state
    std::unique_ptr<SensoryInput> predictNextState(const SensoryInput& currentState);
    
    // Update predictions based on actual observation and compute prediction error
    void updatePredictions(const SensoryInput& predicted, const SensoryInput& actual);
    
    // Get prediction error (0.0 = perfect prediction, 1.0 = maximum error)
    float getPredictionError() const;
    
    // Get prediction confidence (0.0 = no confidence, 1.0 = high confidence)
    float getConfidence() const;
    
    // Get prediction history
    const std::vector<float>& getErrorHistory() const;
    void clearHistory();
    
    // Train prediction model with observation
    void train(const SensoryInput& observation);
    
    // Get temporal predictions for multiple future steps
    std::vector<std::unique_ptr<SensoryInput>> predictMultipleSteps(
        const SensoryInput& currentState, size_t numSteps);
    
    // Get prediction error components (what aspect was unexpected)
    struct ErrorComponents {
        float sensoryError;    // Primary sensory prediction error
        float temporalError;   // Temporal sequence prediction error
        float actionError;     // Action consequence prediction error
        float totalError;
    };
    ErrorComponents getErrorComponents() const;
    
    // Integrate prediction with attention system
    void updateAttentionBasedOnPrediction(const SensoryInput& predicted);
    
    // Integrate prediction with working memory
    void updateWorkingMemoryFromPredictions(const SensoryInput& predicted);
    
    // Get prediction-based action recommendations
    std::vector<ActionType> getPredictedActionRecommendations() const;
    
    // Enhance neural plasticity based on prediction errors
    void applyPredictionErrorPlasticity(float error, const SensoryInput& currentState);
    
    // Update prediction based on neuromodulatory signals
    void updateWithNeuromodulation(float dopamine, float curiosity);
    
    // Enable/disable prediction mechanisms
    void enableTemporalPrediction(bool enable) { temporalPredictionEnabled_ = enable; }
    void enableActionConsequencePrediction(bool enable) { actionConsequencePredictionEnabled_ = enable; }
    void enableMultiScalePrediction(bool enable) { multiScalePredictionEnabled_ = enable; }
    
    // Configuration
    void setPredictionHorizon(size_t steps) { predictionHorizon_ = steps; }
    void setSequenceMemorySize(size_t size) { sequenceMemorySize_ = size; }
    void setTemporalPredictionGain(float gain) { temporalPredictionGain_ = gain; }
    void setActionPredictionGain(float gain) { actionPredictionGain_ = gain; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Configuration
    size_t predictionHorizon_;
    size_t sequenceMemorySize_;
    bool temporalPredictionEnabled_;
    bool actionConsequencePredictionEnabled_;
    bool multiScalePredictionEnabled_;
    float temporalPredictionGain_;
    float actionPredictionGain_;
    
    // State
    Brain* brain_;
    std::unique_ptr<NeuralPrediction> neuralPrediction_;
};

} // namespace nlm
