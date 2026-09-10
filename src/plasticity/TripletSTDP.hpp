#pragma once

#include "PlasticityRule.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

// Triplet STDP (Spike Timing-Dependent Plasticity with Triplet Interactions)
// Implements STDP with triplet spike interactions: pre-post, post-pre, and pre-post-post
// Based on biological evidence of synaptic learning rules involving spike triplets
// 
// Mathematical formulation (Song et al., 2000):
// For pre-post spike pair with Δt = t_post - t_pre > 0 (pre before post):
//   Δw = A1+ * exp(-Δt/τ+) + A2+ * exp(-Δt/τ+) * (number of post spikes)
// 
// For post-pre spike pair with Δt = t_post - t_pre < 0 (post before pre):
//   Δw = A1- * exp(Δt/τ-) + A2- * exp(Δt/τ-) * (number of pre spikes)
// 
// Where:
//   A1+ = First spike potentiation amplitude
//   A2+ = Second spike potentiation amplitude (requires pre-post-post triplet)
//   A1- = First spike depression amplitude
//   A2- = Second spike depression amplitude (requires post-pre-pre triplet)
//   τ+ = Time constant for potentiation
//   τ- = Time constant for depression
// 
// Biological inspiration:
// - Captures triplet interactions observed in hippocampal slices
// - Accounts for second spike interactions in STDP
// - Matches experimental data with complex spike patterns
// - Provides more biologically realistic learning rules

class TripletSTDP : public PlasticityRule {
public:
    TripletSTDP();
    ~TripletSTDP() override;
    
    // Update synaptic weights based on pre/post synaptic activity with triplet interactions
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    const char* getName() const override;
    
    // Triplet interaction parameters
    void setA1Plus(float weight);      // First spike potentiation amplitude
    float getA1Plus() const;
    
    void setA2Plus(float weight);      // Second spike potentiation amplitude
    float getA2Plus() const;
    
    void setA1Minus(float weight);     // First spike depression amplitude
    float getA1Minus() const;
    
    void setA2Minus(float weight);     // Second spike depression amplitude
    float getA2Minus() const;
    
    void setTauPlus(float tau);        // Potentiation time constant
    float getTauPlus() const;
    
    void setTauMinus(float tau);       // Depression time constant
    float getTauMinus() const;
    
    // Triplet configuration
    void setTripletWindow(float window);  // Time window for triplet interactions
    float getTripletWindow() const;
    
    void configure(float a1Plus, float a2Plus, float a1Minus, float a2Minus,
                   float tauPlus, float tauMinus, float window);
    
    // Compute triplet contributions
    float computeTripletLTP(const std::vector<Timestamp>& preSpikes,
                           const std::vector<Timestamp>& postSpikes,
                           TimestepDuration currentTime) const;
    
    float computeTripletLTD(const std::vector<Timestamp>& preSpikes,
                           const std::vector<Timestamp>& postSpikes,
                           TimestepDuration currentTime) const;
    
    // Get contribution from specific spike pair types
    float getPrePostContribution(float dt) const;
    float getPostPreContribution(float dt) const;
    float getPrePostPostContribution(float dt, int postCount) const;
    float getPostPrePreContribution(float dt, int preCount) const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm