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
    
    void setLearningRate(float rate) override;
    float getLearningRate() const override;
    
    void setMaxWeight(float maxWeight);
    float getMaxWeight() const;
    
    // Weight bounds and thresholds (overrides from PlasticityRule)
    void setWeightBounds(float minWeight, float maxWeight) override;
    void setWeightChangeThreshold(float threshold) override;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
