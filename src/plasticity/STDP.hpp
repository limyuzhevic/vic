#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

// Spike-Timing-Dependent Plasticity
// PLACEHOLDER - Phase 2 will implement real STDP

class STDP : public PlasticityRule {
public:
    STDP();
    ~STDP() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    void configure(float ltpWeight, float ltdWeight, float tau);

private:
    struct Impl;
    Impl* pImpl;
};

// Phase 5: Reward-modulated STDP (R-STDP)
// This extends the basic STDP to incorporate reward signals
// PLACEHOLDER: Complete implementation would integrate with neuromodulation system

} // namespace nlm
