#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

// Spike-timing dependent plasticity with triplet interactions
// Implements more biologically realistic STDP with three-factor interactions
// Captures complex temporal dynamics including NMDA and voltage-gated calcium currents
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
    
    // Triplet parameters
    void setLTPWeight(float weight);       // Potentiation weight (A+)
    float getLTPWeight() const;
    void setLTDWeight(float weight);       // Depression weight (A-)
    float getLTDWeight() const;
    void setTimeConstant(float tau);       // Time constant (τ, ms)
    float getTimeConstant() const;
    void setA2Weight(float weight);        // Second-order potentiation (A2)
    float getA2Weight() const;
    void setB2Weight(float weight);        // Second-order depression (B2)
    float getB2Weight() const;
    void setNMDATheta(float theta);        // NMDA threshold voltage
    float getNMDATheta() const;
    
    // Update parameters from config
    void configure(float ltpWeight, float ltdWeight, float tau, float a2Weight, float b2Weight);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm