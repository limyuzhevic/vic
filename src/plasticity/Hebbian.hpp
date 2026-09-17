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
    
    // Configuration parameters
    void setLearningRate(float rate);
    float getLearningRate() const;
    
    void setMaxWeight(float maxWeight);
    float getMaxWeight() const;
    
    void setMinWeight(float minWeight);
    float getMinWeight() const;
    
    void setTimeConstant(float tau);
    float getTimeConstant() const;
    
    void setCovarianceThreshold(float threshold);
    float getCovarianceThreshold() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
