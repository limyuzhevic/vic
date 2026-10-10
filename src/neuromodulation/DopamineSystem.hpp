#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>

namespace nlm {

// Forward declarations
class Neuromodulator;
class NeuromodulationEngine;

// Temporal difference learning parameters
struct TDLearningParams {
    float alpha;  // Learning rate
    float gamma;  // Discount factor
    float lambda;  // Eligibility trace decay
    float epsilon;  // Exploration rate
    
    TDLearningParams()
        : alpha(0.001f), gamma(0.99f), lambda(0.9f), epsilon(0.1f) {}
};

// Reward prediction error with multi-step lookahead
struct RewardPredictionError {
    float tdError;                    // Current TD error
    float intrinsicError;              // Intrinsic curiosity error
    float predictionError;             // General prediction error
    float rewardPrediction;            // Predicted reward
    float actualReward;                // Actual received reward
    float eligibilityTrace;            // Eligibility trace value
    
    RewardPredictionError()
        : tdError(0.0f), intrinsicError(0.0f), predictionError(0.0f),
          rewardPrediction(0.0f), actualReward(0.0f), eligibilityTrace(0.0f) {}
};

// Dopamine system state for complex dynamics
struct DopamineSystemState {
    float phasicComponent;            // Phasic (burst) firing
    float tonicComponent;             // Tonic baseline firing
    float totalConcentration;         // Total dopamine level
    float receptorSaturation;         // D1/D2 receptor saturation
    float intracellularStorage;       // Cytosolic dopamine storage
    float releaseProbability;         // Probability of release
    float reuptakeRate;               // Rate of reuptake into vesicles
    float degradationRate;            // Rate of degradation
    float homeostasisValue;           // Homeostatic set point
    float plasticityModulation;       // Plasticity modulation factor
    float rewardSensitivity;          // Sensitivity to rewards
    float predictionErrorSensitivity;  // Sensitivity to prediction errors
    
    DopamineSystemState()
        : phasicComponent(0.0f), tonicComponent(0.2f), totalConcentration(0.2f),
          receptorSaturation(0.0f), intracellularStorage(0.0f), releaseProbability(0.1f),
          reuptakeRate(0.05f), degradationRate(0.01f), homeostasisValue(0.2f),
          plasticityModulation(1.0f), rewardSensitivity(1.0f), 
          predictionErrorSensitivity(1.0f) {}
};

// Advanced dopamine learning rules
struct DopamineLearningRule {
    enum class RuleType {
        RESCORLA_WALTERS,      // Classic Rescorla-Wagner
        TEMPORAL_DIFFERENCE,   // Temporal difference learning
        SHAPED_REWARD,         // Shaped reward learning
        INTRINSIC_CURIOUSITY,   // Intrinsic curiosity modulation
        PREDICTION_ERROR,      // Pure prediction error
        MULTI_STEP_LOOKAHEAD   // Multi-step lookahead
    };
    
    RuleType type;
    float weight;
    float strength;
    float decayRate;
    float adaptationRate;
    
    DopamineLearningRule()
        : type(RuleType::TEMPORAL_DIFFERENCE), weight(1.0f), strength(1.0f),
          decayRate(0.1f), adaptationRate(0.01f) {}
};

// Dopamine system: Enhanced reward and reinforcement learning
class DopamineSystem {
public:
    DopamineSystem();
    ~DopamineSystem();
    
    // Disable copying, enable moving
    DopamineSystem(const DopamineSystem&) = delete;
    DopamineSystem& operator=(const DopamineSystem&) = delete;
    DopamineSystem(DopamineSystem&&) noexcept;
    DopamineSystem& operator=(DopamineSystem&&) noexcept;
    
    // Initialization
    bool initialize(std::shared_ptr<Brain> brain, std::shared_ptr<NeuromodulationEngine> engine);
    
    // Main update step
    void update(TimestepDuration dt, SimulationStep step);
    
    // Reward processing
    void processReward(float reward, float predictedReward = 0.0f);
    void signalPhasicResponse(float magnitude);
    void signalTonicResponse(float baseline);
    
    // Learning system integration
    void computeTemporalDifferenceError(float predictedValue, float actualValue, float reward, float nextPredictedValue);
    void updateEligibilityTraces(SimulationStep step);
    void updateMultiStepPredictions(const std::vector<float>& futureRewards);
    
    // Neuromodulatory effects
    void modulatePlasticity(float& synapticWeight, float predictionError) const;
    void modulateLearningRate(float& learningRate, float novelty) const;
    void modulateAttention(float& attentionWeight, float predictionError) const;
    
    // State management
    void updateState(TimestepDuration dt);
    void reset();
    
    // Accessors
    float getPhasicComponent() const;
    float getTonicComponent() const;
    float getTotalConcentration() const;
    float getPlasticityModulation() const;
    float getRewardPredictionError() const;
    float getIntrinsicCuriosity() const;
    
    // Configuration
    void setTDLearningParams(const TDLearningParams& params);
    void setLearningRules(const std::vector<DopamineLearningRule>& rules);
    void enableIntrinsicCuriosity(bool enable) { enableIntrinsicCuriosity_ = enable; }
    void enableMultiStepLearning(bool enable) { enableMultiStepLearning_ = enable; }
    
    // History and statistics
    const std::vector<float>& getRewardHistory() const;
    const std::vector<float>& getPredictionErrorHistory() const;
    const std::vector<float>& getTonicLevelHistory() const;
    void clearHistory();
    
    // Integration with other neuromodulators
    void integrateWithCuriosity(float curiosityLevel);
    void integrateWithNovelty(float noveltyLevel);
    void integrateWithPredictionError(float predictionError);
    
    // Safety and validation
    bool validateSafetyConstraints() const;
    void enforceHomeostasis();
    
private:
    struct Impl;
    Impl* pImpl;
    
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<NeuromodulationEngine> engine_;
    
    bool enableIntrinsicCuriosity_;
    bool enableMultiStepLearning_;
};

} // namespace nlm