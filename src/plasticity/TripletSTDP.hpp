#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

// Spike-timing dependent plasticity with triplet interactions
// Implements the triplet STDP rule from Brunel & Bhopal (2001)
// Accounts for spontaneous spike activity in neural populations

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
    void setAPlus(float value);           // Potentiation amplitude for (pre, post) pair
    float getAPlus() const;
    void setAMinus(float value);          // Depression amplitude for (post, pre) pair
    float getAMinus() const;
    
    // Triplet STDP parameters
    void setAPlusTau1(float value);       // Potentiation for (pre', post) triplet
    float getAPlusTau1() const;
    void setAMinusTau1(float value);      // Depression for (post', pre) triplet
    float getAMinusTau1() const;
    void setAPlusTau2(float value);        // Potentiation for (pre, post') triplet
    float getAPlusTau2() const;
    void setAMinusTau2(float value);      // Depression for (post, pre') triplet
    float getAMinusTau2() const;
    
    // Time constants
    void setTauPlus(float value);         // Potentiation time constant (ms)
    float getTauPlus() const;
    void setTauMinus(float value);        // Depression time constant (ms)
    float getTauMinus() const;
    void setTauPlus1(float value);        // Triplet potentiation time constant 1 (ms)
    float getTauPlus1() const;
    void setTauPlus2(float value);        // Triplet potentiation time constant 2 (ms)
    float getTauPlus2() const;
    void setTauMinus1(float value);       // Triplet depression time constant 1 (ms)
    float getTauMinus1() const;
    void setTauMinus2(float value);       // Triplet depression time constant 2 (ms)
    float getTauMinus2() const;
    
    // Weight bounds
    void setMinWeight(float value);
    float getMinWeight() const;
    void setMaxWeight(float value);
    float getMaxWeight() const;
    
    // Configuration methods
    void configure(float A_plus, float A_minus, float tau_plus, float tau_minus);
    void configure(float A_plus, float A_minus, float tau_plus, float tau_minus,
                   float A_plus_tau1, float A_minus_tau1, float A_plus_tau2, float A_minus_tau2,
                   float tau_plus1, float tau_plus2, float tau_minus1, float tau_minus2);
    void configureBiologically(float learningRate, float weightRange);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm