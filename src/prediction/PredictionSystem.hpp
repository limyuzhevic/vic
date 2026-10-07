#pragma once

#include "../core/Types/Types.hpp"
#include "../environment/Observation.hpp"
#include <vector>
#include <memory>

namespace nlm {

// Prediction system: predicts future sensory states and consequences
// PLACEHOLDER - Phase 2 will implement real predictive coding

class PredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem();
    
    // Make prediction for next timestep
    // TODO PHASE 2: Implement real prediction
    std::unique_ptr<SensoryInput> predictNextState(const SensoryInput& currentState);
    
    // Update predictions based on actual observation
    void updatePredictions(const SensoryInput& predicted, const SensoryInput& actual);
    
    // Get prediction error
    float getPredictionError() const;
    
    // Get prediction confidence
    float getConfidence() const;
    
    // Get prediction history
    const std::vector<float>& getErrorHistory() const;
    void clearHistory();
    
    // Train prediction model
    void train(const SensoryInput& observation);
    
    // New neural prediction methods
    void extractNeuralPatternFromBrain(std::vector<float>& pattern);
    void convertSensoryToNeural(const SensoryInput& sensory, std::vector<float>& pattern);
    std::vector<float> findBestMatchingSequence(const std::vector<float>& pattern);
    float computePatternSimilarity(const std::vector<float>& a, const std::vector<float>& b) const;
    float computePredictionError(const std::vector<float>& actual, const std::vector<float>& predicted) const;
    std::unique_ptr<SensoryInput> createSensoryFromNeuralPattern(const std::vector<float>& pattern);
    std::vector<float> enhanceCurrentPattern(const std::vector<float>& pattern);
    void updateLearning(const SensoryInput& actualState, const SensoryInput& predictedState);
    void learnSequenceAssociation(const std::vector<float>& fromPattern, const std::vector<float>& toPattern, float strength);
    size_t findOrCreatePatternNeuron(const std::vector<float>& pattern);
    float getCurrentSimulationTime() const;
    void setCurrentSensorySize(size_t size);
    void setVisionPatternSize(size_t size);
    void setTouchPatternSize(size_t size);
    void setInternalPatternSize(size_t size);
    float getPredictionError(const SensoryInput& actual, const SensoryInput& predicted) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Configuration for pattern sizes
    size_t currentSensorySize;
    size_t visionPatternSize;
    size_t touchPatternSize;
    size_t internalPatternSize;
};

} // namespace nlm
