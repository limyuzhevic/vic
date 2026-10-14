#pragma once

#include "STDP.hpp"

namespace nlm {

// Triplet STDP (STDP with triplet interactions)
// Extends standard STDP to include three-spike interactions
// (pre-post-post, post-post-pre, post-pre-pre)
// 
// Biological basis:
// - Captures synaptic tagging and marking
// - Accounts for presynaptic neuromodulator release
// - Reflects calcium dynamics in dendritic compartments
// 
// Mathematical formulation (Brostoff and Gütig, 2012):
// Δw = w(t) + φ(Δt) * Σ I_i(Δt)
// 
// Where I_i represents triplet integrals:
// I_pre = ∫ Ppre * Pred(Δt) dΔt
// I_post = ∫ Ppost * Pred(Δt) dΔt
// I_cross = ∫ Ppre * Ppost * Pred(Δt) dΔt

class TripletSTDP : public STDP {
public:
    TripletSTDP();
    ~TripletSTDP() override;
    
    using STDP::update;  // Inherit base class update
    
    // Override with triplet interaction version
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    
    // Triplet parameters
    void setTripletTimeConstant(float tau1, float tau2);  // Fast and slow time constants
    void setTripletWeights(float w1, float w2, float w3);   // Triplet weights
    
    float getTripletTimeConstant1() const;
    float getTripletTimeConstant2() const;
    float getTripletWeight1() const;
    float getTripletWeight2() const;
    float getTripletWeight3() const;
    
    // Enable/disable triplet interactions
    void enableTripletInteractions(bool enable);
    bool areTripletInteractionsEnabled() const;
    
    // Get current triplet contribution
    float getTripletContribution() const;

private:
    struct Impl;
    Impl* pImpl;
    bool tripletEnabled;
};

} // namespace nlm
