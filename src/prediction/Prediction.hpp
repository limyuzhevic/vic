#pragma once

#include "../core/Config/Config.hpp"
#include "../brain/Brain.hpp"
#include "../prediction/NeuralPrediction.hpp"
#include <memory>

namespace nlm {

// Prediction: High-level prediction system interface
// Coordinates neural prediction mechanisms and integrates with brain loop
// Provides unified interface for prediction-based learning and decision making
// 
// Key features:
// - Uses NeuralPrediction for actual neural computation
// - Integrates with brain's working memory and neuromodulation
// - Provides prediction error signals for learning
// - Supports action-consequence prediction

class Prediction {
public:
    Prediction(std::shared_ptr<Config> config);
    ~Prediction();
    
    // Initialize with brain reference
    // Sets up neural prediction substrates and connections
    void initialize(Brain* brain);
    
    // Process current neural pattern and generate prediction
    // Uses neural substrate to predict next state based on learned sequences
    void processPrediction(const std::vector<float>& currentPattern);
    
    // Get current prediction error
    // Returns normalized error value (0 = perfect prediction, 1 = maximum error)
    float getPredictionError() const;
    
    // Get prediction confidence
    // Returns confidence level based on pattern matching consistency
    float getConfidence() const;
    
    // Apply learning signals to brain based on prediction error
    // Modulates neural plasticity using prediction error for learning
    void applyLearningSignals(Brain* brain, float predictionError);
    
    // Enable/disable prediction mechanisms
    void enableTemporalPrediction(bool enable) { neuralPrediction_->enableTemporalPrediction(enable); }
    void enableActionConsequencePrediction(bool enable) { neuralPrediction_->enableActionConsequencePrediction(enable); }
    
    // Get neurons involved in prediction
    std::vector<NeuronId> getPredictionNeurons() const;
    std::vector<NeuronId> getSequenceNeurons() const;

private:
    // Neural prediction engine
    std::unique_ptr<NeuralPrediction> neuralPrediction_;
    
    // Brain integration
    Brain* brain_;
    
    // Configuration
    std::shared_ptr<Config> config_;
    
    // Current state
    std::vector<float> currentPattern_;
    std::vector<float> lastPrediction_;
};

} // namespace nlm