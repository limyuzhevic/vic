#pragma once

#include "PlasticityRule.hpp"
#include "../../core/Types/Types.hpp"
#include <vector>

namespace nlm {

// Spike-timing dependent plasticity with triplet interactions
// Implements triplet-based STDP for better biological accuracy
// Captures pre-post-pre and post-pre-post spike interactions
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
    void setA_plus(float aPlus);       // Potentiation amplitude
    float getA_plus() const;
    void setA_minus(float aMinus);     // Depression amplitude
    float getA_minus() const;
    void set_tau_plus(float tauPlus);  // LTP time constant
    float get_tau_plus() const;
    void set_tau_minus(float tauMinus); // LTD time constant
    float get_tau_minus() const;
    void set_b(float b);               // Triplet interaction parameter
    float get_b() const;
    
    // Update parameters from config
    void configure(float aPlus, float aMinus, float tauPlus, float tauMinus, float b);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm