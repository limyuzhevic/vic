#pragma once

#include "../core/Types/Types.hpp"
#include "../sensory/SensoryInput.hpp"
#include <vector>
#include <memory>
#include <deque>

namespace nlm {

// Prediction system: predicts future sensory states and consequences
// Integrates with brain systems for coherent cognitive processing
//
// Key mechanisms:
// - Neural pattern prediction using brain substrate
// - Prediction error signaling for learning
// - Integration with attention and working memory
// - Confidence-based action selection

class PredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem();

    // Initialize with brain reference for integration
    void initialize(Brain* brain);

    // Make prediction for next sensory state using neural patterns
    std::unique_ptr<SensoryInput> predictNextState(const SensoryInput& currentState);

    // Update predictions based on actual sensory observation
    void updatePredictions(const SensoryInput& predicted, const SensoryInput& actual);

    // Get prediction error signal for learning
    float getPredictionError() const;

    // Get prediction confidence
    float getConfidence() const;

    // Get prediction error history
    const std::vector<float>& getErrorHistory() const;

    // Clear prediction history
    void clearHistory();

    // Train prediction model with observed sensory data
    void train(const SensoryInput& observation);

    // Update prediction based on neural activity patterns
    void updateFromNeuralActivity(const std::vector<float>& neuralPattern);

    // Get most similar pattern from memory for prediction
    std::vector<float> getSimilarPattern(const std::vector<float>& queryPattern) const;

    // Set prediction learning rate
    void setLearningRate(float rate);

    // Set prediction memory size
    void setMemorySize(size_t size);

    // Get current prediction error
    float getCurrentError() const;

    // Get prediction system status
    bool isReady() const;

    // Reset prediction system
    void reset();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;

    Brain* brain_;

    // Calculate pattern similarity for prediction
    static float calculatePatternSimilarity(const std::vector<float>& pattern1,
                                           const std::vector<float>& pattern2);
};

} // namespace nlm

