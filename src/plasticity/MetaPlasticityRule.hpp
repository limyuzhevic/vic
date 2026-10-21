#pragma once

#include "PlasticityRule.hpp"
#include <vector>

namespace nlm {

// Metaplasticity rule
// Plasticity that regulates plasticity itself, controlling the overall learning capacity
class MetaPlasticityRule : public PlasticityRule {
public:
    MetaPlasticityRule();
    ~MetaPlasticityRule() override;

    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;

    // Metaplasticity parameters
    void setMetaPlasticityRate(float rate);              // Metaplasticity rate (γ)
    float getMetaPlasticityRate() const;
    void setPlasticityThreshold(float threshold);        // Plasticity threshold (θp)
    float getPlasticityThreshold() const;
    void setHebbianSaturationThreshold(float threshold); // Hebbian saturation threshold
    float getHebbianSaturationThreshold() const;
    void setMetaPlasticityEnabled(bool enabled);         // Enable/disable metaplasticity
    bool isMetaPlasticityEnabled() const;                // Check if metaplasticity is enabled

    // Metaplasticity implementation
    void updateMetaPlasticityState(float hebbianActivity, float stdpActivity);  // Update metaplasticity state
    void modulateLearningRate(float &rate, float hebbianActivity, float stdpActivity) const;  // Modulate learning rate based on metaplasticity
    float calculateMetaPlasticityDelta(float hebbianActivity, float stdpActivity) const;    // Calculate metaplasticity delta
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm