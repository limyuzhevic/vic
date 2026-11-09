#pragma once

#include "PlasticityRule.hpp"
#include <vector>

namespace nlm {

// Spike-Timing-Dependent Plasticity
// Implements real STDP with spike-history dependent updates

class STDP : public PlasticityRule {
public:
    STDP();
    ~STDP() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // STDP parameters
    void setLTPWeight(float weight);    // Long-term potentiation weight
    float getLTPWeight() const;
    void setLTDWeight(float weight);    // Long-term depression weight
    float getLTDWeight() const;
    void setTimeConstant(float tau);    // STDP time constant (ms)
    float getTimeConstant() const;
    
    // Update parameters from config
    void configure(float ltpWeight, float ltdWeight, float tau);
    
    // Reward-modulated STDP (R-STDP) - Enhanced STDP with neuromodulation
    class RewardModulatedSTDP : public PlasticityRule {
    public:
        RewardModulatedSTDP();
        ~RewardModulatedSTDP() override;
        
        void update(Synapse* synapse,
                     const std::vector<Timestamp>& preSpikes,
                     const std::vector<Timestamp>& postSpikes,
                     TimestepDuration dt) override;
        void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
        const char* getName() const override { return "R-STDP"; }
        
        // R-STDP specific parameters
        void setDopamineFactor(float factor);  // Scales STDP with dopamine
        float getDopamineFactor() const;
        void set neuromodulationGain(float gain);  // Modulation strength
        float getNeuromodulationGain() const;
        
        // Eligibility trace management
        void setEligibilityTraceDecay(float decayRate);  // For trace persistence
        float getEligibilityTraceDecay() const;
        
        // Configure from config
        void configure(float ltpWeight, float ltdWeight, float tau,
                       float dopamineFactor = 1.0f, float neuromodulationGain = 1.0f);
        
    private:
        struct Impl;
        Impl* pImpl;
    };
    
    // Spike-timing dependent plasticity with triplet interactions
    // Captures residual calcium dynamics and multiple spike interactions
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
        
        // Triplet STDP parameters
        void setUmax(float umax);      // Maximum utilization for facilitation
        float getUmax() const;
        void settauF(float tauF);      // Facilitation time constant
        float gettauF() const;
        void settauD(float tauD);      // Depression time constant
        float gettauD() const;
        
        // Configure from config
        void configure(float umax, float tauF, float tauD);
        
    private:
        struct Impl;
        Impl* pImpl;
    };
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
