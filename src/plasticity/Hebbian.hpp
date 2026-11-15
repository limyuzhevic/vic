#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

// Hebbian learning implementation
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
    
    // STDP parameters - for configuration from experiment config
    void configure(float learningRate, float maxWeight, float minWeight, float covarianceThreshold);
    
    void setLearningRate(float rate);
    float getLearningRate() const;
    
    void setMaxWeight(float maxWeight);
    float getMaxWeight() const;
    
    void setMinWeight(float minWeight);
    float getMinWeight() const;
    
    void setCovarianceThreshold(float threshold);
    float getCovarianceThreshold() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Reward-modulated Hebbian (complement to STDP)
// For reward prediction error modulated Hebbian learning
class RewardModulatedHebbian : public PlasticityRule {
public:
    RewardModulatedHebbian();
    ~RewardModulatedHebbian() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    void setLearningRate(float rate);
    float getLearningRate() const;
    
    void setPredictionErrorScale(float scale);
    float getPredictionErrorScale() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
