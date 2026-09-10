#pragma once

#include "PlasticityRule.hpp"
#include "../../neuromodulation/Neuromodulator.hpp"
#include "../../brain/Synapse.hpp"
#include <vector>

namespace nlm {

// Reward-modulated STDP (R-STDP)
// Implements spike-timing dependent plasticity modulated by reward signals
// Based on biological evidence that dopamine scales STDP learning rates
// 
// Mathematical formulation:
// Δw(t) = A+(t) * exp(-Δt/τ+) if Δt > 0 (pre before post)
// Δw(t) = A-(t) * exp(Δt/τ-) if Δt < 0 (post before pre)
// 
// Where A+(t) and A-(t) are reward-modulated amplitudes:
// A+(t) = A+_base * (1 + λ * D(t))  // Potentiation scaled by dopamine
// A-(t) = A-_base * (1 + λ * D(t))  // Depression scaled by dopamine
// 
// D(t) = Dopamine level [0.0, 1.0]
// λ = Reward modulation strength (typically 1.0-2.0)
// 
// Biological inspiration:
// - Dopamine modulates STDP in striatum and hippocampus
// - Rewards enhance LTP, suppress LTD
// - eligibility traces track pre-post spike timing
// - dopamine release gates weight changes

class RewardModulatedSTDP : public PlasticityRule {
public:
    RewardModulatedSTDP();
    ~RewardModulatedSTDP() override;
    
    // Update synaptic weights based on pre/post synaptic activity with reward modulation
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    const char* getName() const override;
    
    // Reward modulation parameters
    void setDopamineModulation(float lambda);    // Reward modulation strength
    float getDopamineModulation() const;
    
    void setBaselineLTPWeight(float weight);    // Baseline LTP amplitude
    float getBaselineLTPWeight() const;
    
    void setBaselineLTDWeight(float weight);    // Baseline LTD amplitude  
    float getBaselineLTDWeight() const;
    
    void setEligibilityTraceDecay(float tau);   // Eligibility trace decay time
    float getEligibilityTraceDecay() const;
    
    void setRewardIntegrationWindow(float window); // Time window for reward integration
    float getRewardIntegrationWindow() const;
    
    // Reward modulation configuration
    void configure(float baselineLTP, float baselineLTD, float tau,
                   float lambda, float rewardWindow);
    
    // Get current dopamine-modulated learning rates
    float getModulatedLTPWeight(float dopamineLevel) const;
    float getModulatedLTDWeight(float dopamineLevel) const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm