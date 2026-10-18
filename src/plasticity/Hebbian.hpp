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
    
    void setLearningRate(float rate);
    float getLearningRate() const;
    
    void setMaxWeight(float maxWeight);
    float getMaxWeight() const;
    
    // Add support for online normalization
    void setOnlineNormalization(bool enabled);
    bool getOnlineNormalization() const;
    
    // Add support for heterosynaptic plasticity
    void setHeterosynapticPlasticity(bool enabled);
    bool getHeterosynapticPlasticity() const;
    void setLateralInhibition(float strength);
    float getLateralInhibition() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
