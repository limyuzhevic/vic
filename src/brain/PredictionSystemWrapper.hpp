#pragma once

#include "Brain.hpp"
#include "../prediction/PredictionSystem.hpp"
#include <vector>

namespace nlm {

class PredictionSystemWrapper {
public:
    PredictionSystemWrapper() = default;
    
    void initialize(Brain* brain);
    
    // Update prediction system
    void update(Brain& brain);
    void updatePrediction(Brain& brain);
    
    // Prediction error computation
    void computePredictionError(Brain& brain, const class SensoryInput& input);
    void updatePredictionBasedOnInput(Brain& brain, const std::vector<float>& sensoryInput);
    
    // Confidence and uncertainty
    float getPredictionConfidence(Brain& brain) const;
    float getPredictionErrorLevel(Brain& brain) const;
    float getUncertainty(Brain& brain) const;
    
    // Prediction-based actions
    std::vector<float> generatePredictions(Brain& brain, size_t horizon) const;
    void applyPredictionsToPlanning(Brain& brain, const std::vector<float>& predictions);
    
    // Forward model
    void updateForwardModel(Brain& brain, const std::vector<float>& sensoryInput);
    std::vector<float> predictNextState(Brain& brain, const std::vector<float>& currentState) const;
    
    void logPredictionSystemStatus(Brain& brain) const;
    
private:
    // Internal prediction system
    void initializePredictionSystem(Brain& brain);
    
    // State prediction
    std::vector<float> predictStateTransition(const std::vector<float>& currentState,
                                            TimestepDuration dt) const;
    
    // Error-based learning
    void updatePredictionWeights(Brain& brain, float error);
    void applyPredictionLearning(Brain& brain, float learningRate);
    
    // Model updating
    void updateModelParameters(Brain& brain);
    void adjustPredictionHorizon(Brain& brain);
    
    // Prediction confidence
    float computePredictionConfidence(const std::vector<float>& predictionError) const;
    
    // Internal state
    std::vector<float> currentPredictedState_;
    std::vector<std::vector<float>> predictionHistory_;
    float predictionConfidence_;
};

} // namespace nlm
