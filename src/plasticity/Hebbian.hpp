#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

// Hebbian learning implementation - Covariance rule
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
    
    // Basic parameters
    void setLearningRate(float rate);
    float getLearningRate() const;
    
    void setMaxWeight(float maxWeight);
    float getMaxWeight() const;
    
    void setMinWeight(float minWeight);
    float getMinWeight() const;
    
    // Advanced parameters
    void setCovarianceThreshold(float threshold);
    float getCovarianceThreshold() const;
    
    void setActivityBaseline(float baseline);
    float getActivityBaseline() const;
    
    void setAdaptationRate(float rate);
    float getAdaptationRate() const;
    
    void setActivityWindow(float window);
    float getActivityWindow() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
