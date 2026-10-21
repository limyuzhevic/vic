#pragma once

#include "PlasticityRule.hpp"
#include <vector>

namespace nlm {

// Dopamine-gated STDP (D-STDP)
// Modulates STDP with dopamine signals for reward-modulated learning
class DopamineGatedSTDP : public PlasticityRule {
public:
    DopamineGatedSTDP();
    ~DopamineGatedSTDP() override;

    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;

    // Dopamine-gated STDP parameters
    void setLTPWeight(float weight);                    // Potentiation weight (A+)
    float getLTPWeight() const;
    void setLTDWeight(float weight);                    // Depression weight (A-)
    float getLTDWeight() const;
    void setTimeConstant(float tau);                    // STDP time constant (ms)
    float getTimeConstant() const;
    void setDopamineGain(float gain);                   // Dopamine modulation gain (γ)
    float getDopamineGain() const;
    void setBaselineDopamine(float baseline);           // Baseline dopamine level
    float getBaselineDopamine() const;
    void setMaxWeight(float max);                        // Maximum weight bound
    float getMaxWeight() const;
    void setMinWeight(float min);                        // Minimum weight bound
    float getMinWeight() const;

    // Dopamine modulation
    void updateDopamine(float dopamineLevel);           // Update dopamine level
    float getDopamineLevel() const;                     // Get current dopamine level
    void setDopamineModulation(bool enabled);            // Enable/disable dopamine modulation
    bool isDopamineModulationEnabled() const;            // Check if dopamine modulation is enabled
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm