#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

// Spike-timing dependent plasticity with triplet interactions
// PLACEHOLDER - Phase 2
class TripletSTDP : public PlasticityRule {
public:
    TripletSTDP();
    ~TripletSTDP() override;
    
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
    void setTimeConstant(float tau);    // STDP time constant (ms)
    float getTimeConstant() const;
    
    // Triplet trace parameters
    void setPreTraceDecay(float decay);  // Presynaptic trace decay
    float getPreTraceDecay() const;
    void setPostTraceDecay(float decay); // Postsynaptic trace decay
    float getPostTraceDecay() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
