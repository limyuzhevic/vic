#pragma once

#include "PlasticityRule.hpp"
#include "../../brain/Synapse.hpp"

namespace nlm {

// Anti-Hebbian plasticity rule: decreases synaptic weight when neurons fire together
class AntiHebbianRule : public PlasticityRule {
public:
    AntiHebbianRule();
    ~AntiHebbianRule() override;
    
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Configuration parameters
    void setLearningRate(float rate);
    float getLearningRate() const;
    
    void setMinWeight(float weight);
    float getMinWeight() const;
    
    void setMaxWeight(float weight);
    float getMaxWeight() const;
    
    void setTargetWeight(float weight);
    float getTargetWeight() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm