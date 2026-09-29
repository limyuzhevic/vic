#pragma once

#include "STDP.hpp"
#include <vector>
#include <cmath>

namespace nlm {

// Triplet Spike-Timing-Dependent Plasticity
// Captures non-Markovian temporal dependencies with three-spike interactions
// Implements pre-pre-post and pre-post-post triplet rules
// Based on experimental findings in biological systems
class TripletSTDP : public PlasticityRule {
public:
    TripletSTDP();
    ~TripletSTDP() override;
    
    // Update with spike timing and triplet interactions
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Triplet parameters
    void setA1Plus(float a1plus);       // Pre-pre-post potentiation
    float getA1Plus() const;
    void setA1Minus(float a1minus);     // Pre-pre-post depression
    float getA1Minus() const;
    void setA2Plus(float a2plus);       // Pre-post-post potentiation
    float getA2Plus() const;
    void setA2Minus(float a2minus);     // Pre-post-post depression
    float getA2Minus() const;
    void setTauPlus(float tauPlus);     // Potentiation time constant
    float getTauPlus() const;
    void setTauMinus(float tauMinus);   // Depression time constant
    float getTauMinus() const;
    
    // Triplet rule configuration
    void configure(float a1plus, float a1minus, float a2plus, float a2minus,
                   float tauPlus, float tauMinus);
    
    // History management for triplet detection
    void recordPreSpike(Synapse* synapse, Timestamp timestamp);
    void recordPostSpike(Synapse* synapse, Timestamp timestamp);
    
    // Weight bounds for triplet interactions
    void setMinWeight(float minWeight);
    void setMaxWeight(float maxWeight);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
