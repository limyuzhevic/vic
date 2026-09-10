#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Synapse.hpp"

namespace nlm {

// Spike-timing dependent plasticity with triplet interactions
// Extends STDP with contributions from spike triplets (pre-pre, post-post)
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
    void setA_plus(float A_plus);          // LTP rate
    float getA_plus() const;
    
    void setA_minus(float A_minus);        // LTD rate
    float getA_minus() const;
    
    void setW_plus(float W_plus);          // Pre-post pair weight
    float getW_plus() const;
    
    void setW_minus(float W_minus);        // Post-pre pair weight
    float getW_minus() const;
    
    void setTimeConstant(float tau);       // STDP time constant
    float getTimeConstant() const;
    
    // Enable/disable triplet interactions
    void setTripletContribution(float contribution);
    float getTripletContribution() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
