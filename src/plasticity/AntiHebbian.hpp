#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

// Anti-Hebbian learning implementation
// Anti-Hebbian plasticity: decreases synaptic weight when pre- and postsynaptic neurons fire together
// Implements "neurons that fire together, disconnect together" principle
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
    
    // Parameters
    void setLearningRate(float rate);
    float getLearningRate() const;
    void setInhibitionStrength(float strength);
    float getInhibitionStrength() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm