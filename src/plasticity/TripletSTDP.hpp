// Triplet STDP header
#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

// Spike-timing dependent plasticity with triplet interactions
// Based on Markov and Tsodyks (1992) model with pre-post-pre and post-pre-post triplets
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
    
    // Triplet STDP parameters
    void setAplus(float weight);                  // Pre-post-pre triplet potentiation
    float getAplus() const;
    void setAminus(float weight);                 // Post-pre-post triplet depression
    float getAminus() const;
    void setAtimes(float weight);                 // Pre-post interaction term
    float getAtimes() const;
    void setBpre(float weight);                   // Pre-only depression
    float getBpre() const;
    void setBpost(float weight);                  // Post-only depression
    float getBpost() const;
    void setTau1(float tau);                      // Time constant for pre spikes
    float getTau1() const;
    void setTau2(float tau);                      // Time constant for post spikes
    float getTau2() const;
    void setMinWeight(float minWeight);           // Minimum synaptic weight
    float getMinWeight() const;
    void setMaxWeight(float maxWeight);           // Maximum synaptic weight
    float getMaxWeight() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
