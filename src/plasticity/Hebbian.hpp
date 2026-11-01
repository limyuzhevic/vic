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
    
    // Set learning parameters for better balance with STDP
    void setLearningParameters(float hebbianRate, float maxHebbianWeight = 0.5f, float minHebbianWeight = 0.05f);
    
    // Get learning parameters
    float getHebbianLearningRate() const;
    float getHebbianMaxWeight() const;
    float getHebbianMinWeight() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
