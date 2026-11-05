#pragma once

#include "../prediction/PredictionSystem.hpp"
#include "../prediction/NeuralPrediction.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include <memory>
#include <vector>

namespace nlm {

class Brain;

class BrainPrediction {
public:
    BrainPrediction() = default;
    ~BrainPrediction() = default;
    
    // Configuration and setup
    void configureFromConfig(const Config* config);
    void setPredictionParameters(float predictionConfidence, float predictionHorizon, float errorThreshold);
    
    // Prediction updates (called during step)
    void updateAllPrediction(Brain* brain, TimestepDuration timestep);
    void updatePredictionSystem(Brain* brain, const SensoryInput& sensoryInput);
    void updatePredictionError(Brain* brain, const SensoryInput& sensoryInput, TimestepDuration timestep);
    void updatePredictionConfidence(Brain* brain, TimestepDuration timestep);
    
    // Prediction access and control
    PredictionSystem* getPredictionSystem() { return predictionSystem_.get(); }
    NeuralPrediction* getNeuralPrediction() { return neuralPrediction_.get(); }
    
    // Prediction statistics and diagnostics
    void collectPredictionStats(Brain* brain);
    float getPredictionConfidence() const { return predictionConfidence_; }
    float getPredictionHorizon() const { return predictionHorizon_; }
    float getErrorThreshold() const { return errorThreshold_; }
    float getPredictionAccuracy() const { return predictionAccuracy_; }
    
    // Prediction system management
    void resetPrediction(Brain* brain);
    void enablePrediction(Brain* brain, bool enable);
    void setPredictionHorizon(Brain* brain, float horizon);
    void processPredictions(Brain* brain, const std::vector<Action>& actionHistory);
    
    // Prediction-specific processing
    void generatePredictions(Brain* brain, const SensoryInput& currentInput);
    void evaluatePredictionAccuracy(Brain* brain, const Action& actualAction);
    void updatePredictionModel(Brain* brain, const SensoryInput& input, float reward);
    void handlePredictionFailures(Brain* brain, const SensoryInput& input);
    
private:
    std::unique_ptr<PredictionSystem> predictionSystem_;
    std::unique_ptr<NeuralPrediction> neuralPrediction_;
    
    // Prediction statistics
    float predictionConfidence_ = 0.5f;
    float predictionHorizon_ = 1.0f;
    float errorThreshold_ = 0.1f;
    float predictionAccuracy_ = 0.5f;
    
    // Configuration parameters
    size_t predictionWindow_ = 10;
    float confidenceDecay_ = 0.95f;
    float rewardPredictionWeight_ = 0.3f;
    float noveltyPredictionWeight_ = 0.7f;
    float errorCorrectionFactor_ = 0.5f;
    
    // Helper methods
    void initializePredictionSystems(Brain* brain);
    void validatePredictionState(Brain* brain) const;
    void updatePredictionConnections(Brain* brain);
    void processPredictionLearning(Brain* brain, TimestepDuration timestep);
    
    friend class BrainCore; // For internal access
};

} // namespace nlm