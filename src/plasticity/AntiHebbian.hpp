#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

// Anti-Hebbian rule: decrease weight when neurons fire together
// Implements homeostatic plasticity to prevent runaway excitation
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
    void setDecayRate(float rate);
    float getDecayRate() const;
    void setTargetWeight(float weight);
    float getTargetWeight() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm