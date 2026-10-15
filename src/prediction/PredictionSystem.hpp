#pragma once

#include "../core/Types/Types.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>

namespace nlm {

// Enhanced prediction system with biologically plausible predictive coding
// Integrates SelfModel, working memory, and neuromodulation for intelligent predictions
// 
// Key features:
// - Multi-source prediction integration (SelfModel, working memory, forward models)
// - Neuromodulation scaling (dopamine, curiosity, novelty)
// - Hebbian-like learning from prediction errors
// - Confidence scaling based on prediction consistency
// - Neural prediction generation with biologically realistic dynamics

class PredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem();
    
    // Initialize with brain reference for integration with other systems
    void initialize(Brain* brain);
    
    // Make prediction for next timestep using integrated neural mechanisms
    std::unique_ptr<SensoryInput> predictNextState(const SensoryInput& currentState);
    
    // Update predictions based on actual observation and learn from errors
    void updatePredictions(const SensoryInput& predicted, const SensoryInput& actual);
    
    // Get prediction error (0 = perfect prediction, 1 = total error)
    float getPredictionError() const;
    
    // Get prediction confidence based on consistency of learned patterns
    float getConfidence() const;
    
    // Get prediction history for analysis and debugging
    const std::vector<float>& getErrorHistory() const;
    void clearHistory();
    
    // Train prediction model from experience
    void train(const SensoryInput& observation);
    
    // Get integrated prediction confidence from multiple sources
    float getIntegratedConfidence() const;
    
    // Set prediction parameters
    void setPredictionHorizon(size_t horizon) { pImpl->predictionHorizon = horizon; }
    void setMemoryWindowSize(size_t size) { pImpl->memoryWindowSize = size; }
    void setLearningRate(float rate) { pImpl->learningRate = rate; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
