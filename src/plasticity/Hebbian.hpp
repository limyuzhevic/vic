#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

// Hebbian learning implementation
class Hebbian : public PlasticityRule {
public:
    Hebbian(nullptr);
    ~Hebbian(nullptr) override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName(nullptr) const override;
    
    void setLearningRate(float rate);
    float getLearningRate(nullptr) const;
    
    void setMaxWeight(float maxWeight);
    float getMaxWeight(nullptr) const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
