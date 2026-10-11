#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

// Anti-Hebbian learning implementation: "neurons that fire together, wire apart"
// Δw = -η * (⟨pre * post⟩ - ⟨pre⟩⟨post⟩)  (opposite sign of Hebbian)
// Biological inspiration: synaptic depression when correlated activity occurs
// Used for forming inhibitory connections and balancing excitation/inhibition

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
    
    void setMaxDepression(float max);
    float getMaxDepression() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm