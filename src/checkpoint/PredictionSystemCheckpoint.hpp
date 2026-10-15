// PredictionSystem checkpoint data structures

#pragma once

#include <vector>
#include <memory>
#include "../core/Types/Types.hpp"
#include "../sensory/SensoryInput.hpp"

namespace nlm {

// PredictionModel for prediction system
struct PredictionModelCheckpoint {
    // Model parameters (weights)
    std::vector<float> weights;
    
    // Model architecture information
    std::vector<size_t> layerSizes;
    std::string modelType;
    
    // Performance metrics
    float accuracy;
    float confidence;
    float errorRate;
    
    // Training statistics
    size_t trainingCount;
    float learningRate;
    float regularization;
    
    // Temporal aspects
    SimulationStep lastUpdated;
    float temporalDecay;
    
    // Integration information
    std::vector<float> integratedState;
    bool hasSelfModelIntegration;
    bool hasWorkingMemoryIntegration;
    
    PredictionModelCheckpoint()
        : accuracy(0), confidence(0), errorRate(0)
        , trainingCount(0), learningRate(0.01f), regularization(0.001f)
        , lastUpdated(0), temporalDecay(0.9f)
        , hasSelfModelIntegration(false), hasWorkingMemoryIntegration(false) {}
};

// PredictionSystem checkpoint state
struct PredictionSystemCheckpoint {
    // Current prediction model
    PredictionModelCheckpoint currentModel;
    
    // Prediction history
    std::vector<float> errorHistory;
    std::vector<float> confidenceHistory;
    std::vector<SimulationStep> predictionTimestamps;
    
    // System parameters
    size_t predictionHorizon;
    size_t memoryWindowSize;
    float learningRate;
    float predictionThreshold;
    
    // State tracking
    float currentPredictionError;
    float currentConfidence;
    SimulationStep lastPredictionStep;
    
    // Integration state
    std::vector<float> integratedSelfModelState;
    std::vector<float> integratedWorkingMemoryState;
    std::vector<float> integratedNeuromodulationState;
    
    // Performance tracking
    size_t successfulPredictions;
    size_t failedPredictions;
    float averagePredictionTime;
    
    PredictionSystemCheckpoint()
        : predictionHorizon(10), memoryWindowSize(100), learningRate(0.01f), predictionThreshold(0.5f)
        , currentPredictionError(0), currentConfidence(0), lastPredictionStep(0)
        , successfulPredictions(0), failedPredictions(0), averagePredictionTime(0.0f) {}
};

// PredictionSystem save/load interface
class PredictionSystemSaverLoader {
public:
    virtual ~PredictionSystemSaverLoader() = default;
    
    // Save prediction system state
    virtual bool save(const PredictionSystemCheckpoint& checkpoint) const = 0;
    
    // Load prediction system state
    virtual bool load(PredictionSystemCheckpoint& checkpoint) = 0;
    
    // Create empty checkpoint
    virtual PredictionSystemCheckpoint createEmptyCheckpoint() const = 0;
    
    // Get prediction model confidence
    virtual float getModelConfidence(const PredictionModelCheckpoint& model) const = 0;
};

} // namespace nlm
