#pragma once

#include "PlasticityRule.hpp"
#include <memory>
#include <string>

namespace nlm
{

// Reward-modulated plasticity implementation
// Implements reward prediction error (RPE) modulated plasticity,
// dopamine-dependent reward modulation, asymmetric reward sensitivity,
// trial-based reward modulation, and credit assignment for delayed rewards

class RewardModulatedPlasticity : public PlasticityRule {
public:
    RewardModulatedPlasticity();
    ~RewardModulatedPlasticity() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Reward prediction error system
    void setPredictionError(float error);
    void setExpectedReward(float expected);
    void setActualReward(float actual);
    void setDopamineResponse(float response);
    
    // Reward-modulated learning parameters
    void setLearningRate(float rate);
    void setAsymmetricRewardSensitivity(bool enable);
    void setDelayedRewardModulation(bool enable);
    void setTrialBasedModulation(bool enable);
    void setCreditAssignmentWindow(float window);
    void setRewardHistoryCapacity(size_t capacity);
    
    // Reward prediction error calculation
    void calculatePredictionError();
    void updateEligibilityTrace(Synapse* synapse, float delta, float reward, float dt);
    
    // Reward-modulated STDP variants
    void enableRPESTDP(bool enable);
    void enableDopamineGatedSTDP(bool enable);
    void enableRewardAugmentedHebbian(bool enable);
    
    // Getters
    float getPredictionError() const;
    float getExpectedReward() const;
    float getActualReward() const;
    float getDopamineResponse() const;
    float getLearningRate() const;
    bool getAsymmetricRewardSensitivity() const;
    bool getDelayedRewardModulation() const;
    bool getTrialBasedModulation() const;
    float getCreditAssignmentWindow() const;
    
    // Reward statistics
    void recordRewardHistory(float reward, float time);
    float getAverageReward() const;
    float getRewardVolatility() const;
    float getExpectedRewardPrediction() const;
    
    // Reward-modulated plasticity mechanisms
    float calculateRewardModulationFactor() const;
    float calculateAsymmetricRewardSensitivity(float reward) const;
    float calculateDelayedRewardEffect(float reward, float timeSinceEvent) const;
    
    // RPE-based plasticity calculations
    float calculateRPEBasedChange(float baselineChange, float predictionError) const;
    float calculateTrialBasedRewardModulation(float reward, float timeWindow) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Reward prediction error (RPE) modulation
class RPEModulation : public PlasticityRule {
public:
    RPEModulation();
    ~RPEModulation() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // RPE parameters
    void setPredictionError(float error);
    void setLearningRate(float rate);
    void setRPEThreshold(float threshold);
    void setRPEIntegration(float integration);
    void setRewardHistoryCapacity(size_t capacity);
    
    float getPredictionError() const;
    float getLearningRate() const;
    float getRPEThreshold() const;
    float getRPEIntegration() const;
    
    // RPE calculation methods
    void updatePredictionError(float expected, float actual);
    float calculateRPEModulation(float delta) const;
    float calculateTemporalDifferenceError(float reward, float time) const;
    
    // RPE-based plasticity rules
    void enableTDLearning(bool enable);
    void enableDeltaLearning(bool enable);
    void enableActorCriticLearning(bool enable);
    
    bool isTDLearningEnabled() const;
    bool isDeltaLearningEnabled() const;
    bool isActorCriticLearningEnabled() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Dopamine-dependent reward modulation
class DopamineModulatedPlasticity : public PlasticityRule {
public:
    DopamineModulatedPlasticity();
    ~DopamineModulatedPlasticity() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Dopamine parameters
    void setDopamineLevel(float level);
    void setDopamineModulation(float modulation);
    void setRewardPredictionError(float error);
    void setTemporalDifference(float td);
    
    // Dopamine-specific plasticity
    void enablePhasicDopamineModulation(bool enable);
    void enableTonicDopamineModulation(bool enable);
    void enableRewardPredictionErrorModulation(bool enable);
    
    float getDopamineLevel() const;
    float getDopamineModulation() const;
    float getRewardPredictionError() const;
    float getTemporalDifference() const;
    
    bool isPhasicDopamineModulationEnabled() const;
    bool isTonicDopamineModulationEnabled() const;
    bool isRewardPredictionErrorModulationEnabled() const;
    
    // Dopamine dynamics
    void updateDopamineDynamics(float reward, float time);
    float calculatePhasicDopamineResponse(float predictionError) const;
    float calculateTonicDopamineBaseline() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Asymmetric reward sensitivity (larger rewards have larger effects)
class AsymmetricRewardSensitivity : public PlasticityRule {
public:
    AsymmetricRewardSensitivity();
    ~AsymmetricRewardSensitivity() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Asymmetric sensitivity parameters
    void setSensitivityExponent(float exponent);
    void setSensitivityThreshold(float threshold);
    void setMaximumAsymmetry(float maxAsym);
    void setBaselineSensitivity(float baseline);
    
    float getSensitivityExponent() const;
    float getSensitivityThreshold() const;
    float getMaximumAsymmetry() const;
    float getBaselineSensitivity() const;
    
    // Asymmetric sensitivity calculations
    float calculateAsymmetricModulation(float reward) const;
    float getAsymmetryFactor(float reward) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Trial-based reward modulation
class TrialBasedRewardModulation : public PlasticityRule {
public:
    TrialBasedRewardModulation();
    ~TrialBasedRewardModulation() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Trial-based parameters
    void setTrialHistoryCapacity(size_t capacity);
    void setRewardMemoryDecay(float decay);
    void setTrialWeight(float weight);
    void setRewardIntegration(float integration);
    
    size_t getTrialHistoryCapacity() const;
    float getRewardMemoryDecay() const;
    float getTrialWeight() const;
    float getRewardIntegration() const;
    
    // Trial-based modulation
    void recordTrialOutcome(float reward, float time);
    float getRecentRewardAverage(float timeWindow) const;
    float calculateTrialBasedModulation(float baselineChange) const;
    
    // Trial dynamics
    void updateTrialDynamics(float dt);
    void resetTrialHistory();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Credit assignment for delayed rewards
class CreditAssignmentPlasticity : public PlasticityRule {
public:
    CreditAssignmentPlasticity();
    ~CreditAssignmentPlasticity() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Credit assignment parameters
    void setCreditAssignmentWindow(float window);
    void setEligibilityTraceDecay(float decay);
    void setTraceLearningRate(float rate);
    void setRewardDelayTolerance(float tolerance);
    void setGradientBasedAssignment(bool enable);
    
    float getCreditAssignmentWindow() const;
    float getEligibilityTraceDecay() const;
    float getTraceLearningRate() const;
    float getRewardDelayTolerance() const;
    bool getGradientBasedAssignment() const;
    
    // Credit assignment mechanisms
    void updateEligibilityTrace(Synapse* synapse, float delta, float reward, float dt);
    float calculateCreditAssignment(float trace, float reward, float delay) const;
    void enableBackpropagation(bool enable);
    void enableMonteCarloLearning(bool enable);
    
    bool isBackpropagationEnabled() const;
    bool isMonteCarloLearningEnabled() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Comprehensive reward-modulated plasticity system coordinator
class RewardModulatedPlasticitySystem {
public:
    RewardModulatedPlasticitySystem();
    ~RewardModulatedPlasticitySystem();
    
    // Add reward-modulated plasticity rule
    void addRewardPlasticityRule(std::unique_ptr<PlasticityRule> rule);
    
    // Get specific reward-modulated rule types
    RewardModulatedPlasticity* getRewardModulatedPlasticity() const;
    RPEModulation* getRPEModulation() const;
    DopamineModulatedPlasticity* getDopamineModulatedPlasticity() const;
    AsymmetricRewardSensitivity* getAsymmetricRewardSensitivity() const;
    TrialBasedRewardModulation* getTrialBasedRewardModulation() const;
    CreditAssignmentPlasticity* getCreditAssignmentPlasticity() const;
    
    // Get all reward-modulated rules
    const std::vector<std::unique_ptr<PlasticityRule>>& getAllRewardRules() const;
    
    // System-wide reward-modulated plasticity update
    void updateAll(Brain* brain, SimulationStep currentStep, Timestamp currentTime);
    
    // Reward system configuration
    void configureFromDefaults();
    void configureFromConfig(const class Config& config);
    
    // Reward system statistics and diagnostics
    void logRewardPlasticityState() const;
    float getSystemRewardPlasticityLoad() const;
    float getNetworkRewardLearningRate() const;
    
    // Reset system
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm