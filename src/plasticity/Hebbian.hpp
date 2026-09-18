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
    
    void setBaselinePlasticity(float baseline);
    float getBaselinePlasticity() const;
    
    void setPotentiationThreshold(float threshold);
    float getPotentiationThreshold() const;
    
    void setDepressionThreshold(float threshold);
    float getDepressionThreshold() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
