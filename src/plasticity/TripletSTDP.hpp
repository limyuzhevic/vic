#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

// Triplet STDP with pre-post pairing and post-post interactions
// Captures complex synaptic dynamics over longer time windows
// Essential for burst-based learning in biological neural networks

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
    
    // Triplet interaction parameters
    void setA1(float a1);    // First amplitude (A1)
    float getA1() const;
    void setA2(float a2);    // Second amplitude (A2)
    float getA2() const;
    void setA3(float a3);    // Third amplitude (A3)
    float getA3() const;
    
    // Time constants
    void setTau1(float tau1);  // First time constant (τ1)
    float getTau1() const;
    void setTau2(float tau2);  // Second time constant (τ2)
    float getTau2() const;
    
    // Baseline weight
    void setBaselineWeight(float w0);
    float getBaselineWeight() const;
    
    // Configure all parameters
    void configure(float a1, float a2, float a3, float tau1, float tau2);
    
private:
    struct Impl;
    Impl* pImpl;
    
    // Helper functions for triplet detection
    static size_t countRecentSpikes(Timestamp spikeTime, 
                                    const std::vector<Timestamp>& spikes,
                                    float timeWindow);
    static size_t countTripletPatterns(Timestamp firstTime,
                                        Timestamp middleTime,
                                        const std::vector<Timestamp>& firstSpikes,
                                        const std::vector<Timestamp>& middleSpikes,
                                        float timeWindow);
};

} // namespace nlm