#pragma once

#include "PlasticityRule.hpp"
#include <memory>
#include <string>
#include <algorithm>

namespace nlm
{

// Homeostatic plasticity implementation
// Implements homeostatic plasticity to maintain network stability
// Includes synaptic scaling, intrinsic plasticity, and activity-dependent regulation

class HomeostaticPlasticity : public PlasticityRule {
public:
    HomeostaticPlasticity();
    ~HomeostaticPlasticity() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Homeostatic parameters
    void setTargetFiringRate(float target);
    void setPlasticityExponent(float exponent);
    void setLearningRate(float rate);
    void setSynapticScaling(bool enable);
    void setIntrinsicPlasticity(bool enable);
    void setActivityThreshold(float threshold);
    
    float getTargetFiringRate() const;
    float getPlasticityExponent() const;
    float getLearningRate() const;
    bool isSynapticScalingEnabled() const;
    bool isIntrinsicPlasticityEnabled() const;
    float getActivityThreshold() const;
    
    // Homeostatic mechanisms
    void updateActivityStatistics(const std::vector<Timestamp>& spikes, float dt);
    float calculateSynapticScaling(float currentRate, float targetRate) const;
    float calculateIntrinsicPlasticity(float currentRate, float targetRate) const;
    float calculateActivityDependentPlasticity(float activity) const;
    
    float getAverageFiringRate() const;
    float getFiringRateVariance() const;
    float getActivityFluctuation() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Synaptic scaling plasticity
class SynapticScaling : public PlasticityRule {
public:
    SynapticScaling();
    ~SynapticScaling() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Synaptic scaling parameters
    void setTargetFiringRate(float target);
    void setScalingExponent(float exponent);
    void setScalingRate(float rate);
    void setMinimumWeight(float minWeight);
    void setMaximumWeight(float maxWeight);
    
    float getTargetFiringRate() const;
    float getScalingExponent() const;
    float getScalingRate() const;
    float getMinimumWeight() const;
    float getMaximumWeight() const;
    
    // Synaptic scaling calculations
    float calculateScalingFactor(float currentRate, float targetRate) const;
    float getNetworkTotalActivity() const;
    void updateNetworkStatistics(Brain* brain);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Intrinsic plasticity
class IntrinsicPlasticity : public PlasticityRule {
public:
    IntrinsicPlasticity();
    ~IntrinsicPlasticity() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Intrinsic plasticity parameters
    void setTargetFiringRate(float target);
    void setPlasticityRate(float rate);
    void setActivityDependentGain(float gain);
    void setHomeostaticDrift(float drift);
    
    float getTargetFiringRate() const;
    float getPlasticityRate() const;
    float getActivityDependentGain() const;
    float getHomeostaticDrift() const;
    
    // Intrinsic plasticity mechanisms
    float calculateIntrinsicPlasticity(float currentRate, float targetRate) const;
    float getIntrinsicExcitability() const;
    void updateIntrinsicExcitability(float dt);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// BCM (Bienenstock-Cooper-Munro) plasticity with sliding threshold
class BCMPlasticity : public PlasticityRule {
public:
    BCMPlasticity();
    ~BCMPlasticity() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // BCM parameters
    void setTheta(float theta);      // Sliding threshold (θ)
    void setSigma(float sigma);      // Threshold sensitivity (σ)
    void setLearningRate(float rate);
    void setMaximumWeight(float maxWeight);
    void setMinimumWeight(float minWeight);
    void setThetaAdaptationRate(float rate);
    
    float getTheta() const;
    float getSigma() const;
    float getLearningRate() const;
    float getMaximumWeight() const;
    float getMinimumWeight() const;
    float getThetaAdaptationRate() const;
    
    // BCM dynamics
    void updateSlidingThreshold(float postActivity);
    float getSlidingThreshold() const;
    float calculateBCMChange(float postActivity, float theta) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Metaplastic homeostasis - plasticity of homeostatic mechanisms
class MetaplasticHomeostasis : public PlasticityRule {
public:
    MetaplasticHomeostasis();
    ~MetaplasticHomeostasis() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Metaplastic homeostasis parameters
    void setHomeostaticAdaptationRate(float rate);
    void setStabilityTarget(float target);
    void setFlexibilityTarget(float target);
    void setLearningHistoryCapacity(size_t capacity);
    void setMetaplasticThreshold(float threshold);
    
    float getHomeostaticAdaptationRate() const;
    float getStabilityTarget() const;
    float getFlexibilityTarget() const;
    size_t getLearningHistoryCapacity() const;
    float getMetaplasticThreshold() const;
    
    // Metaplastic homeostasis mechanisms
    void recordHomeostaticEvent(float magnitude, float time);
    float calculateHomeostaticAdaptation(float learningHistory) const;
    float getSystemStability() const;
    float getSystemFlexibility() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Homeostatic plasticity system coordinator
class HomeostaticPlasticitySystem {
public:
    HomeostaticPlasticitySystem();
    ~HomeostaticPlasticitySystem();
    
    // Add homeostatic plasticity rule
    void addHomeostaticPlasticityRule(std::unique_ptr<PlasticityRule> rule);
    
    // Get specific homeostatic rule types
    HomeostaticPlasticity* getHomeostaticPlasticity() const;
    SynapticScaling* getSynapticScaling() const;
    IntrinsicPlasticity* getIntrinsicPlasticity() const;
    BCMPlasticity* getBCMPlasticity() const;
    MetaplasticHomeostasis* getMetaplasticHomeostasis() const;
    
    // Get all homeostatic rules
    const std::vector<std::unique_ptr<PlasticityRule>>& getAllHomeostaticRules() const;
    
    // System-wide homeostatic plasticity update
    void updateAll(Brain* brain, SimulationStep currentStep, Timestamp currentTime);
    
    // Homeostatic system configuration
    void configureFromDefaults();
    void configureFromConfig(const class Config& config);
    
    // Homeostatic system statistics and diagnostics
    void logHomeostaticPlasticityState() const;
    float getSystemHomeostaticLoad() const;
    float getNetworkHomeostaticStability() const;
    
    // Reset system
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm