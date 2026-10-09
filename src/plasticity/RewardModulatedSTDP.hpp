#pragma once

#include "STDP.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include <memory>

namespace nlm {

// Reward-Modulated STDP (R-STDP)
// Integrates dopamine reward prediction errors with spike-timing dependent plasticity
// Implements a biologically realistic model of dopamine-modulated synaptic learning
// Based on reinforcement learning theory and neurophysiological findings

class RewardModulatedSTDP : public PlasticityRule {
public:
    RewardModulatedSTDP();
    ~RewardModulatedSTDP() override;
    
    // PlasticityRule interface
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Reward modulation control
    void setDopamineModulation(std::shared_ptr<Dopamine> dopamine);
    void setBaseSTDP(std::shared_ptr<STDP> stdp);
    void setThreshold(float threshold);
    void setModulationStrength(float strength);
    
    // R-STDP specific parameters
    void setLTPModulation(float ltpMod);
    void setLTDModulation(float ltdMod);
    void setTimeConstant(float tau);
    
    // Integration with dopamine system
    void synchronizeWithDopamine();
    void reset();
    
    // Statistics and monitoring
    struct RSTDPStats {
        float totalRPEApplied;
        float totalWeightChanges;
        float plasticityEfficiency;
        float explorationInfluence;
        float exploitationInfluence;
        float adaptationRate;
    };
    
    const RSTDPStats& getStats() const;
    
    // Advanced functionality
    void integrateWithValueFunction(float valueChange);
    void adaptToEnvironmentComplexity(float complexity);
    void applyPolicyGradient(float policyGradient);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm