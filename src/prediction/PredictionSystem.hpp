#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>
#include "../brain/Brain.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/TemporalRelation.hpp"

namespace nlm {

// Prediction system: predicts future sensory states using neural mechanisms
// NOW IMPLEMENTED - uses working memory, concepts, and temporal relations

class PredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem();
    
    // Make prediction for next timestep using neural substrate
    // Uses working memory patterns, concepts, and temporal dynamics
    std::unique_ptr<SensoryInput> predictNextState(const SensoryInput& currentState);
    
    // Update predictions based on actual observation
    // Uses multiple error computation mechanisms (simple + neural)
    void updatePredictions(const SensoryInput& predicted, const SensoryInput& actual);
    
    // Get prediction error
    float getPredictionError() const;
    
    // Get prediction confidence
    float getConfidence() const;
    
    // Get prediction history
    const std::vector<float>& getErrorHistory() const;
    void clearHistory();
    
    // Train prediction model using experience
    // Uses working memory patterns and concept knowledge
    void train(const SensoryInput& observation);
    
    // Get current prediction pattern
    const std::vector<float>& getCurrentPredictionPattern() const;
    
    // Get neural prediction buffer
    const std::vector<std::vector<float>>& getNeuralPredictionBuffer() const;
    
    // Set prediction parameters
    void setPredictionBufferSize(size_t size) { predictionBufferSize_ = size; }
    void setLearningRate(float rate) { learningRate_ = rate; }
    void setPredictionHorizon(float horizon) { predictionHorizon_ = horizon; }
    
    // Integration with Brain class
    void setBrainContext(Brain* brain) { brainContext_ = brain; }
    Brain* getBrainContext() const { return brainContext_; }
    
    // Neural prediction computation methods
    float computePatternSimilarity(const std::vector<float>& a, const std::vector<float>& b) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Brain context for integration
    Brain* brainContext_;
    
    // Prediction configuration
    size_t predictionBufferSize_;
    float learningRate_;
    float predictionHorizon_;
};

} // namespace nlm
