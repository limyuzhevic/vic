#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>
#include <unordered_map>

namespace nlm {

// PredictiveCoding: Forward models and prediction error minimization
// Implements the predictive coding framework where the brain generates
// predictions about sensory input and minimizes prediction error
// 
// This mechanism enables:
// - Forward models for sensory prediction
// - Prediction error signals for learning
// - Hierarchical processing and integration
// - Bayesian inference and belief updating
// - Efficient coding through prediction error minimization
//
// The predictive coding framework consists of:
// 1. Forward models: Predict sensory consequences of actions/state
// 2. Backward models: Update internal state based on prediction errors
// 3. Prediction error signals: Drive learning and adaptation
// 4. Hierarchical processing: Multiple levels of prediction

class PredictiveCoding {
public:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    PredictiveCoding();
    ~PredictiveCoding();
    
    // Initialize with brain reference and coding parameters
    void initialize(class Brain* brain, float predictionGain = 0.1f, 
                    size_t hierarchyLevels = 3, float learningRate = 0.01f);
    
    // Set sensory input for prediction
    void setSensoryInput(const std::vector<float>& input);
    
    // Get prediction error
    float getPredictionError() const;
    
    // Get current predictions
    std::vector<float> getCurrentPredictions() const;
    
    // Compute predictions using forward model
    void computePredictions(const std::vector<float>& currentState);
    
    // Update internal state using prediction error
    void updateState(const std::vector<float>& predictionError);
    
    // Get prediction error statistics
    float getMeanSquaredError() const;
    float getMaxError() const;
    float getPredictionAccuracy() const;
    
    // Get hierarchy level statistics
    size_t getActiveHierarchyLevel() const;
    std::vector<float> getLevelErrors() const;
    std::vector<float> getLevelPredictions() const;
    
    // Get prediction history
    const std::vector<float>& getErrorHistory() const;
    const std::vector<std::vector<float>>& getPredictionHistory() const;
    
    // Clear history and reset
    void clearHistory();
    
    // Enable/disable predictive coding
    void enable(bool enable);
    bool isEnabled() const;
    
    // Configure prediction parameters
    void setPredictionGain(float gain);
    void setLearningRate(float rate);
    void setHierarchyLevels(size_t levels);
    void setErrorThreshold(float threshold);
    
    // Hierarchical processing
    void setActiveLevel(size_t level);
    void enableLevel(size_t level, bool enable);
    bool isLevelEnabled(size_t level) const;
    
    // Forward model operations
    void trainForwardModel(const std::vector<float>& state, 
                          const std::vector<float>& predictedInput);
    std::vector<float> predictInput(const std::vector<float>& state) const;
    
    // Backward model operations
    void trainBackwardModel(const std::vector<float>& predictionError,
                           const std::vector<float>& state);
    std::vector<float> updateState(const std::vector<float>& predictionError,
                                  const std::vector<float>& previousState) const;
    
    // Get learning progress
    float getLearningProgress() const;
    size_t getTrainingSamplesProcessed() const;
    
private:
    PredictiveCoding(const PredictiveCoding&) = delete;
    PredictiveCoding& operator=(const PredictiveCoding&) = delete;
};

} // namespace nlm