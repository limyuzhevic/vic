#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

// Covariance-based Hebbian learning implementation
// "neurons that fire together, wire together" with statistical correlation
// Δw = η * (⟨pre * post⟩ - ⟨pre⟩⟨post⟩)

class Hebbian : public PlasticityRule {
public:
    Hebbian();
    ~Hebbian() override;
    
    // Update synaptic weights based on covariance of pre/post spike activity
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Hebbian parameters
    void setLearningRate(float rate);
    float getLearningRate() const;
    
    void setMaxWeight(float maxWeight);
    float getMaxWeight() const;
    
    void setMinWeight(float minWeight);
    float getMinWeight() const;
    
    void setLearningWindow(float windowMs);
    float getLearningWindow() const;
    
    void setCovarianceThreshold(float threshold);
    float getCovarianceThreshold() const;
    
    void setHomeostaticTarget(float target);
    float getHomeostaticTarget() const;
    
    void setHomeostaticRate(float rate);
    float getHomeostaticRate() const;
    
    void setWeightNormalization(bool enable);
    bool getWeightNormalization() const;
    
    void setUseCovarianceLearning(bool enable);
    bool getUseCovarianceLearning() const;
    
    void setRateBasedLearning(bool enable);
    bool getRateBasedLearning() const;
    
    // Get current weight
    float getCurrentWeight(Synapse* synapse) const;
    
    // Get correlation statistics
    float getPreSpikeRate(const std::vector<Timestamp>& spikes, TimestepDuration totalTime) const;
    float getPostSpikeRate(const std::vector<Timestamp>& spikes, TimestepDuration totalTime) const;
    float getCovariance(const std::vector<Timestamp>& preSpikes, 
                       const std::vector<Timestamp>& postSpikes, 
                       TimestepDuration windowMs) const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
