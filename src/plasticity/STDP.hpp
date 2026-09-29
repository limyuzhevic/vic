#pragma once

#include "PlasticityRule.hpp"
#include "../../core/Types/Types.hpp"

namespace nlm {

// Forward declarations for implementation details
struct STDPImpl;
struct RewardModulatedSTDPImpl;
struct TripletSTDPImpl;

// Spike-Timing-Dependent Plasticity
// Real STDP implementation based on spike-timing correlation (Phase 2+)

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
    
private:
    struct Impl;
    Impl* pImpl;
};

// Reward-modulated STDP (R-STDP)
// Real R-STDP implementation with dopamine-modulated plasticity (Phase 2+)

class RewardModulatedSTDP : public PlasticityRule {
public:
    RewardModulatedSTDP();
    ~RewardModulatedSTDP() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // R-STDP parameters
    void setLTPWeight(float weight);    // Long-term potentiation weight
    float getLTPWeight() const;
    void setLTDWeight(float weight);    // Long-term depression weight
    float getLTDWeight() const;
    void setTimeConstant(float tau);    // STDP time constant (ms)
    float getTimeConstant() const;
    void setDopamineScale(float scale); // Dopamine modulation factor
    float getDopamineScale() const;
    
    // Update parameters from config
    void configure(float ltpWeight, float ltdWeight, float tau, float dopamineScale);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Spike-timing dependent plasticity with triplet interactions
// Real triplet STDP implementation including Ca2+ dynamics (Phase 2+)

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
    void setAPlus(float aPlus);      // A+ for LTP (pre-post pairing)
    float getAPlus() const;
    void setAMinus(float aMinus);     // A- for LTD (post-pre pairing)
    float getAMinus() const;
    void setAU(float aU);            // A_U for unpaired spikes
    float getAU() const;
    void setTauPlus(float tauPlus);   // τ+ for pre-post influence
    float getTauPlus() const;
    void setTauMinus(float tauMinus); // τ- for post-pre influence
    float getTauMinus() const;
    void setTauU(float tauU);         // τ_U for unpaired influence
    float getTauU() const;
    
    // Update parameters from config
    void configure(float aPlus, float aMinus, float aU,
                   float tauPlus, float tauMinus, float tauU);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
