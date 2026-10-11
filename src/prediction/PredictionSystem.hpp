#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>
#include <unordered_map>
#include <deque>

namespace nlm {

// Forward declarations
class NeuralPrediction;
class NeuralWorkingMemory;

// Prediction system: predicts future sensory states and consequences using neural patterns
// Real predictive coding implementation using brain's neural substrate
class PredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem();
    
    // Initialize with brain reference and working memory
    void initialize(Brain* brain, NeuralWorkingMemory* workingMemory);
    
    // Make prediction for next timestep based on learned neural patterns
    std::unique_ptr<SensoryInput> predictNextState(const SensoryInput& currentState);
    
    // Update predictions based on actual observation and compute prediction error
    void updatePredictions(const SensoryInput& predicted, const SensoryInput& actual);
    
    // Get prediction error (0 = perfect prediction, 1 = total error)
    float getPredictionError() const;
    
    // Get prediction confidence based on pattern matching quality (0-1)
    float getPredictionConfidence() const;
    
    // Get prediction history for analysis
    const std::deque<float>& getErrorHistory() const;
    void clearHistory();
    
    // Train prediction model using sensory observations
    void train(const SensoryInput& observation);
    
    // Process incoming sensory input for prediction
    void processSensoryInput(const SensoryInput& input);
    
    // Get the underlying neural prediction system for advanced operations
    NeuralPrediction* getNeuralPrediction() const { return neuralPrediction_.get(); }
    
    // Get current working memory patterns
    std::vector<float> getWorkingMemoryPatterns() const;
    
    // Enable/disable predictive coding mechanisms
    void enableTemporalPrediction(bool enable) { temporalPredictionEnabled_ = enable; }
    void enableActionConsequencePrediction(bool enable) { actionConsequencePredictionEnabled_ = enable; }
    
private:
    // Convert neural pattern to SensoryInput
    std::unique_ptr<SensoryInput> neuralPatternToSensoryInput(const std::vector<float>& pattern) const;
    
    // Convert SensoryInput to neural pattern
    std::vector<float> sensoryInputToNeuralPattern(const SensoryInput& input) const;
    
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Integrated neural systems
    std::unique_ptr<NeuralPrediction> neuralPrediction_;
    NeuralWorkingMemory* workingMemory_;
    Brain* brain_;
    
    // Configuration
    bool temporalPredictionEnabled_;
    bool actionConsequencePredictionEnabled_;
    
    // State tracking
    std::deque<float> errorHistory_;
};

} // namespace nlm
