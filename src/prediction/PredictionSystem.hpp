#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>
#include <unordered_map>

namespace nlm {

// Prediction system: predicts future sensory states and consequences
// Implements predictive coding based on hierarchical Bayesian models
// Inspired by predictive processing in the brain
class PredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem();
    
    // Make prediction for next timestep
    // Uses hierarchical predictive models and free energy minimization
    std::unique_ptr<SensoryInput> predictNextState(const SensoryInput& currentState);
    
    // Update predictions based on actual observation
    // Computes prediction error using Bayesian inference
    void updatePredictions(const SensoryInput& predicted, const SensoryInput& actual);
    
    // Get prediction error
    // Returns surprise/error signal for learning
    float getPredictionError() const;
    
    // Get prediction confidence
    // Returns the certainty of the prediction
    float getConfidence() const;
    
    // Get prediction history
    const std::vector<float>& getErrorHistory() const;
    void clearHistory();
    
    // Train prediction model
    // Updates internal models based on sensory experience
    void train(const SensoryInput& observation);
    
    // Set prediction parameters
    void setLearningRate(float rate);
    void setPredictionHorizon(float horizon);
    void setPrecisionWeight(float weight);
    
    // Get current parameters
    float getLearningRate() const { return learningRate_; }
    float getPredictionHorizon() const { return predictionHorizon_; }
    float getPrecisionWeight() const { return precisionWeight_; }
    
    // Get prediction statistics
    float getMeanPredictionError() const;
    float getMaxPredictionError() const;
    float getMinPredictionError() const;
    
    // Check if prediction error is significant
    bool isSignificantPredictionError() const;
    
    // Reset prediction system
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Current parameters
    float learningRate_;
    float predictionHorizon_;
    float precisionWeight_;
};

} // namespace nlm
