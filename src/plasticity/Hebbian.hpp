#pragma once

#include "PlasticityRule.hpp"
#include <memory>
#include <string>

namespace nlm
{

// Comprehensive Hebbian plasticity implementation with multiple learning rules
// Includes classic Hebbian, anti-Hebbian, BCM theory, and homeostatic plasticity

class Hebbian : public PlasticityRule {
public:
    Hebbian();
    ~Hebbian() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Parameters
    void setLearningRate(float rate) override;
    float getLearningRate() const override;
    void setBaselineThreshold(float threshold);
    float getBaselineThreshold() const;
    void setHomeostaticTarget(float target);
    float getHomeostaticTarget() const;
    void setMaxWeight(float maxWeight);
    float getMaxWeight() const;
    
    // Hebbian rule variants
    void enableCovarianceRule(bool enable);
    bool isCovarianceRuleEnabled() const;
    void enableBCMTheory(bool enable);
    bool isBCMTheoryEnabled() const;
    void enableAntiHebbian(bool enable);
    bool isAntiHebbianEnabled() const;
    void enableHomeostaticPlasticity(bool enable);
    bool isHomeostaticPlasticityEnabled() const;
    
    // Activity statistics for rule selection
    float getPreSpikeHistory() const;
    float getPostSpikeHistory() const;
    void resetActivityHistory();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Anti-Hebbian learning: neurons that fire together, wire apart
class AntiHebbian : public PlasticityRule {
public:
    AntiHebbian();
    ~AntiHebbian() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Parameters
    void setDepressionRate(float rate);
    float getDepressionRate() const;
    void setBaselineFiring(float firing);
    float getBaselineFiring() const;
    void setMinimumWeight(float weight);
    float getMinimumWeight() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Bienenstock-Cooper-Munro (BCM) theory with sliding threshold
class BCM : public PlasticityRule {
public:
    BCM();
    ~BCM() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Parameters
    void setTheta(float theta);      // Sliding threshold (θ)
    void setSigma(float sigma);      // Threshold sensitivity (σ)
    void setLearningRate(float rate);
    void setMaximumWeight(float maxWeight);
    void setMinimumWeight(float minWeight);
    float getTheta() const;
    float getSigma() const;
    float getLearningRate() const;
    float getMaximumWeight() const;
    float getMinimumWeight() const;
    
    // BCM dynamics
    void updateSlidingThreshold(float postActivity);
    float getSlidingThreshold() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Homeostatic plasticity to maintain network stability
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
    
    // Parameters
    void setTargetFiringRate(float target);
    void setPlasticityExponent(float exponent);
    void setLearningRate(float rate);
    void setRegulationStrength(float strength);
    float getTargetFiringRate() const;
    float getPlasticityExponent() const;
    float getLearningRate() const;
    float getRegulationStrength() const;
    
    // Homeostatic mechanisms
    void setSynapticScaling(bool enable);
    void setIntrinsicPlasticity(bool enable);
    bool isSynapticScalingEnabled() const;
    bool isIntrinsicPlasticityEnabled() const;
    
    // Activity statistics
    void updateActivityStatistics(const std::vector<Timestamp>& spikes, float dt);
    float getAverageFiringRate() const;
    float getFiringRateVariance() const;
    float getActivityFluctuation() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Metaplastic learning rule interactions
class MetaplasticHebbian : public PlasticityRule {
public:
    MetaplasticHebbian();
    ~MetaplasticHebbian() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Metaplastic parameters
    void setMetaplasticityRate(float rate);
    void setLearningHistoryCapacity(size_t capacity);
    void setStabilityFactor(float factor);
    void setExplorationBonus(float bonus);
    float getMetaplasticityRate() const;
    size_t getLearningHistoryCapacity() const;
    float getStabilityFactor() const;
    float getExplorationBonus() const;
    
    // Learning history
    void recordLearningEvent(float magnitude, float time);
    float getAverageLearningMagnitude() const;
    float getLearningVolatility() const;
    float getStabilityIndex() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Co-activation based Hebbian rules
class CoactivationHebbian : public PlasticityRule {
public:
    CoactivationHebbian();
    ~CoactivationHebbian() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Co-activation parameters
    void setCoactivationWindow(float window);
    void setCoactivationThreshold(float threshold);
    void setStrengtheningRate(float rate);
    void setWeakeningRate(float rate);
    float getCoactivationWindow() const;
    float getCoactivationThreshold() const;
    float getStrengtheningRate() const;
    float getWeakeningRate() const;
    
    // Co-activation mechanisms
    float calculateCoactivationScore(const std::vector<Timestamp>& preSpikes,
                                    const std::vector<Timestamp>& postSpikes,
                                    TimestepDuration dt) const;
    void updateCoactivationHistory(const std::vector<Timestamp>& spikes);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Adaptive Hebbian with rate-based learning
class RateBasedHebbian : public PlasticityRule {
public:
    RateBasedHebbian();
    ~RateBasedHebbian() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Rate-based parameters
    void setPreSynapticRate(float rate);
    void setPostSynapticRate(float rate);
    void setRateLearningRate(float rate);
    void setRateTimeConstant(float tau);
    float getPreSynapticRate() const;
    float getPostSynapticRate() const;
    float getRateLearningRate() const;
    float getRateTimeConstant() const;
    
    // Rate dynamics
    void updateSynapticRates(const std::vector<Timestamp>& spikes, float dt);
    float calculateRateBasedChange(float preRate, float postRate) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
