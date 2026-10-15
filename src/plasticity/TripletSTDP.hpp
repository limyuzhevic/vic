#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

// Spike-timing dependent plasticity with triplet interactions
// Enhanced implementation with pre-post-post and post-pre-pre interactions

class TripletSTDP : public PlasticityRule {
public:
    TripletSTDP();
    ~TripletSTDP() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override { return "TripletSTDP"; }
    
    // Triplet interaction parameters
    void setPrePostPostWeight(float weight);
    float getPrePostPostWeight() const;
    void setPostPrePreWeight(float weight);
    float getPostPrePreWeight() const;
    void setFacilitationStrength(float strength);
    float getFacilitationStrength() const;
    void setDepressionStrength(float strength);
    float getDepressionStrength() const;
    
    // Synaptic resource management
    void enableSynapticResources(bool enable);
    void setResourceLimit(float limit);
    void setResourceRecovery(float recovery);
    
    bool isSynapticResourcesEnabled() const;
    float getResourceLimit() const;
    float getResourceRecovery() const;
    
    // Trace management
    void enableFacilitationTrace(bool enable);
    void enableDepressionTrace(bool enable);
    void setTraceDecay(float decay);
    
    bool isFacilitationTraceEnabled() const;
    bool isDepressionTraceEnabled() const;
    float getTraceDecay() const;
    
    // Integration with eligibility traces
    void setEligibilityTraceEnabled(bool enable);
    bool isEligibilityTraceEnabled() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm