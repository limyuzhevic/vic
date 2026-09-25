#pragma once

#include "PlasticityRule.hpp"
#include "../../core/Types/Types.hpp"
#include <vector>

namespace nlm {

// Spike-Timing-Dependent Plasticity
// Real implementation with biologically accurate parameters

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
    
    // STDP parameters
    void setLTPWeight(float weight);    // Long-term potentiation weight
    float getLTPWeight() const;
    void setLTDWeight(float weight);    // Long-term depression weight
    float getLTDWeight() const;
    void setTauPlus(float tauPlus);     // LTP time constant (ms)
    float getTauPlus() const;
    void setTauMinus(float tauMinus);   // LTD time constant (ms)
    float getTauMinus() const;
    void setTimeConstant(float tau);    // Backward compatibility
    float getTimeConstant() const;     // Backward compatibility
    
    // Additional parameters
    void setEligibilityDecay(float decayRate); // Decay rate for eligibility traces
    float getEligibilityDecay() const;
    void setMinWeight(float minWeight); // Minimum synaptic weight
    float getMinWeight() const;
    void setMaxWeight(float maxWeight); // Maximum synaptic weight
    float getMaxWeight() const;
    
    // Update parameters from config
    void configure(float ltpWeight, float ltdWeight, float tau) override;
    void configureFull(float ltpWeight, float ltdWeight,
                       float tauPlus, float tauMinus,
                       float minWeight, float maxWeight,
                       float eligibilityDecay);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Reward-modulated STDP (R-STDP)
// Implements reward-modulated plasticity using eligibility traces
// Forward declaration - actual implementation in RewardModulatedSTDP.cpp
class RewardModulatedSTDP;

// Spike-timing dependent plasticity with triplet interactions
// Forward declaration - actual implementation in TripletSTDP.cpp
class TripletSTDP;

} // namespace nlm
