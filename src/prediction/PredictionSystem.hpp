#pragma once

#include "../core/Types/Types.hpp"
#include "../sensory/SensoryInput.hpp"
#include "NeuralPrediction.hpp"
#include "../motor/Action.hpp"
#include <vector>
#include <memory>

namespace nlm {

// Forward declarations for circular dependencies
class Brain;
class Dopamine;
class NeuralWorkingMemory;
class NeuralEpisodicMemory;

// Prediction system: predicts future sensory states and consequences using neural computation
// Implements predictive coding with working memory, episodic memory, and neuromodulation integration

class PredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Make prediction for next timestep using neural computation
    std::unique_ptr<SensoryInput> predictNextState(const SensoryInput& currentState);
    
    // Update predictions based on actual observation and compute prediction error
    void updatePredictions(const SensoryInput& predicted, const SensoryInput& actual);
    
    // Train prediction model using working memory patterns
    void train(const SensoryInput& observation);
    
    // Get prediction error
    float getPredictionError() const;
    
    // Get prediction confidence
    float getConfidence() const;
    
    // Get prediction history
    const std::vector<float>& getErrorHistory() const;
    void clearHistory();
    
    // Get neural prediction component
    NeuralPrediction* getNeuralPrediction() { return neuralPrediction_.get(); }
    
    // Enable/disable prediction
    void enablePrediction(bool enable) { predictionEnabled_ = enable; }
    bool isEnabled() const { return predictionEnabled_; }
    
    // Set prediction parameters
    void setPredictionHorizon(size_t steps) { predictionHorizon_ = steps; }
    void setSequenceMemorySize(size_t size) { sequenceMemorySize_ = size; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Neural prediction component
    std::unique_ptr<NeuralPrediction> neuralPrediction_;
    
    // Configuration
    size_t predictionHorizon_;
    size_t sequenceMemorySize_;
    bool predictionEnabled_;
    
    // Integration state
    Brain* brain_;
    SimulationStep lastStep_;
    
    // Helper methods
    float computePredictionErrorFromCurrent(const std::vector<float>& current, 
                                            const std::vector<float>& prediction);
    void updateNeuromodulation();
    void updateWorkingMemoryWithPrediction(const std::vector<float>& prediction);
    void updateEpisodicMemoryWithError(float error,
                                       const std::vector<float>& predicted,
                                       const std::vector<float>& actual);
    void storeCurrentExperienceInEpisodicMemory(const SensoryInput& input);
    void applySensoryHebbianLearning(const std::vector<float>& sensoryData);
};

} // namespace nlm
