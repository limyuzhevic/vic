#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <cmath>

namespace nlm {

// Forward declarations
class NeuromodulationEngine;

// Multi-step prediction state
struct MultiStepPredictionState {
    std::vector<float> predictionValues;      // Predictions at different time horizons
    std::vector<float> targetValues;          // Target values at different time horizons
    std::vector<float> rewardPredictions;     // Reward predictions at different time horizons
    std::vector<float> predictionErrors;      // Errors at different time horizons
    std::vector<float> eligibilityTraces;     // Eligibility traces for each horizon
    std::vector<float> learningRates;         // Learning rates for each horizon
    std::vector<float> discountFactors;       // Discount factors for each horizon
    std::vector<bool> isActive;               // Whether prediction is active
    
    MultiStepPredictionState() {
        // Initialize for up to 10 time horizons
        predictionValues.resize(10, 0.0f);
        targetValues.resize(10, 0.0f);
        rewardPredictions.resize(10, 0.0f);
        predictionErrors.resize(10, 0.0f);
        eligibilityTraces.resize(10, 0.0f);
        learningRates.resize(10, 0.01f);
        discountFactors.resize(10, 0.9f);
        isActive.resize(10, false);
    }
};

// Prediction error computation state
struct PredictionErrorState {
    float globalPredictionError;              // Global prediction error across all horizons
    float temporalGradient;                   // Gradient of error over time
    float predictionVariance;                 // Variance in predictions
    float uncertaintyEstimate;                // Estimate of prediction uncertainty
    float confidenceValue;                    // Confidence in predictions
    float errorMagnitude;                     // Magnitude of prediction errors
    float errorDirection;                     // Direction of errors (over/under prediction)
    float cumulativeError;                    // Cumulative error over time
    float errorDecayRate;                      // Rate at which errors decay
    float errorAdaptationRate;                 // Rate at which error estimates adapt
    
    PredictionErrorState()
        : globalPredictionError(0.0f), temporalGradient(0.0f), predictionVariance(0.0f),
          uncertaintyEstimate(0.0f), confidenceValue(0.5f), errorMagnitude(0.0f),
          errorDirection(0.0f), cumulativeError(0.0f), errorDecayRate(0.05f),
          errorAdaptationRate(0.1f) {}
};

// Advanced prediction error system: Multi-step lookahead and TD learning
class PredictionErrorSystem {
public:
    PredictionErrorSystem();
    ~PredictionErrorSystem();
    
    // Disable copying, enable moving
    PredictionErrorSystem(const PredictionErrorSystem&) = delete;
    PredictionErrorSystem& operator=(const PredictionErrorSystem&) = delete;
    PredictionErrorSystem(PredictionErrorSystem&&) noexcept;
    PredictionErrorSystem& operator=(PredictionErrorSystem&&) noexcept;
    
    // Initialization
    bool initialize(std::shared_ptr<Brain> brain, std::shared_ptr<NeuromodulationEngine> engine);
    
    // Main update step
    void update(TimestepDuration dt, SimulationStep step);
    
    // Prediction error computation
    void computeError(float predictedValue, float actualValue, float reward = 0.0f);
    void computeMultiStepError(const std::vector<float>& predictions, 
                              const std::vector<float>& targets,
                              const std::vector<float>& rewards);
    void updatePredictions(float newPrediction, float newTarget, float newReward);
    
    // Eligibility trace management
    void updateEligibilityTraces(SimulationStep step);
    void decayEligibilityTraces(TimestepDuration dt);
    
    // Learning system integration
    void applyTemporalDifferenceLearning(float tdError, SimulationStep step);
    void applyRewardPredictionLearning(float error, float reward);
    void applyIntrinsicCuriosityLearning(float novelty, float predictionError);
    
    // State management
    void updateState(TimestepDuration dt);
    void reset();
    
    // Accessors
    float getGlobalPredictionError() const;
    float getErrorMagnitude() const;
    float getTemporalGradient() const;
    float getUncertaintyEstimate() const;
    float getPredictionErrorForHorizon(int horizon) const;
    
    // Configuration
    void setMaxHorizons(int horizons);
    void setDiscountFactor(float factor) { pImpl->defaultDiscountFactor = factor; }
    void setLearningRate(float rate) { pImpl->defaultLearningRate = rate; }
    void setEligibilityTraceDecay(float decay) { pImpl->eligibilityTraceDecay = decay; }
    
    // Integration with other systems
    void integrateWithDopamine(float dopamineLevel);
    void integrateWithCuriosity(float curiosityLevel);
    void integrateWithNovelty(float noveltyLevel);
    
    // Multi-step lookahead
    std::vector<float> computeFuturePredictions(float currentPrediction, int horizons) const;
    std::vector<float> computeFutureRewards(float currentReward, int horizons) const;
    float computeMultiStepTDError(float currentValue, float currentReward,
                                 const std::vector<float>& futureRewards) const;
    
    // History and statistics
    const std::vector<float>& getPredictionErrorHistory() const;
    const std::vector<std::vector<float>>& getMultiStepErrorHistory() const;
    const std::vector<float>& getPredictionHistory() const;
    const std::vector<float>& getTargetHistory() const;
    void clearHistory();
    
    // Safety and validation
    bool validateSafetyConstraints() const;
    void enforceBounds();
    
    // Export/import
    void exportState(std::ostream& stream) const;
    void importState(std::istream& stream);
    
private:
    struct Impl;
    Impl* pImpl;
    
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<NeuromodulationEngine> engine_;
    
    float defaultDiscountFactor;
    float defaultLearningRate;
    float eligibilityTraceDecay;
    
    bool enableMultiStepLearning_;
    bool enableIntrinsicCuriosityLearning_;
};

} // namespace nlm