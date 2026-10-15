#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

// Spike-Timing-Dependent Plasticity
// Enhanced implementation with advanced STDP mechanisms

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
    
    // Basic STDP parameters (backward compatibility)
    void setLTPWeight(float weight);    // Long-term potentiation weight
    float getLTPWeight() const;
    void setLTDWeight(float weight);    // Long-term depression weight
    float getLTDWeight() const;
    void setTimeConstant(float tau);    // STDP time constant (ms)
    float getTimeConstant() const;
    
    // Update parameters from config
    void configure(float ltpWeight, float ltdWeight, float tau);
    
    // Advanced STDP configuration methods
    
    // Core mechanism enablers
    void enableTripletSTDP(bool enable);
    void enableCalciumDynamics(bool enable);
    void enableNMDAReceptor(bool enable);
    void enableRewardModulation(bool enable);
    void enableNeuromodulators(bool enable);
    void enableMetaplasticity(bool enable);
    void enableMultiExpWindow(bool enable);
    
    // Biological parameters
    void setBackpropSpeed(float speed);
    void setNMDAProbability(float probability);
    void setCalciumDecayRate(float decayRate);
    void setFacilitationTraceTau(float tau);
    void setDepressionTraceTau(float tau);
    void setEligibilityTraceDecay(float decay);
    void setNeuromodulatorBaseline(float baseline);
    void setNeuromodulatorSensitivity(float sensitivity);
    void setMetaplasticityThreshold(float threshold);
    void setActivityHistoryWeight(float weight);
    
    // Configuration getters
    bool isTripletSTDPEnabled() const;
    bool isCalciumDynamicsEnabled() const;
    bool isNMDAReceptorEnabled() const;
    bool isRewardModulationEnabled() const;
    bool isNeuromodulatorsEnabled() const;
    bool isMetaplasticityEnabled() const;
    bool isMultiExpWindowEnabled() const;
    
    float getBackpropSpeed() const;
    float getNMDAProbability() const;
    float getCalciumDecayRate() const;
    float getFacilitationTraceTau() const;
    float getDepressionTraceTau() const;
    float getEligibilityTraceDecay() const;
    float getNeuromodulatorBaseline() const;
    float getNeuromodulatorSensitivity() const;
    float getMetaplasticityThreshold() const;
    float getActivityHistoryWeight() const;
    
    // Advanced configuration methods
    void configureAdvanced(float ltpWeight, float ltdWeight, float tau,
                          float backpropSpeed, float nmdaProbability,
                          float calciumDecayRate, float facilitationTau,
                          float depressionTau, float eligibilityDecay,
                          float neuromodulatorBaseline, float neuromodulatorSensitivity,
                          float metaplasticityThreshold, float activityHistoryWeight);
    
    // Preset models
    void configureStandard();           // Default standard STDP
    void configureFast();               // Fast learning STDP
    void configureSlow();               // Slow learning STDP
    void configureRewardSensitive();    // Reward-modulated STDP preset
    void configureCalciumDependent();   // Calcium-dependent STDP preset
    void configureNeuromodulatorDependent(); // Neuromodulator-dependent STDP preset
    void configureTripletEnhanced();    // Enhanced triplet STDP preset
    
    // State management
    void resetToDefaults();             // Reset to standard configuration
    
    // Debug/monitoring
    float getCalciumConcentration() const;
    float getNMDAConductance() const;
    float getFacilitationTrace() const;
    float getDepressionTrace() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Reward-modulated STDP (R-STDP)
// Enhanced implementation with reward integration
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
    
    // Reward modulation parameters
    void setRewardGain(float gain);
    float getRewardGain() const;
    void setRewardSignal(float signal);
    float getRewardSignal() const;
    void setTemporalDifference(float td);
    float getTemporalDifference() const;
    
    // Reward modulation features
    void enableAccumulation(bool enable);
    void enableEligibilityTrace(bool enable);
    void setAccumulationDecay(float decay);
    void setEligibilityDecay(float decay);
    
    bool isAccumulationEnabled() const;
    bool isEligibilityTraceEnabled() const;
    float getAccumulationDecay() const;
    float getEligibilityDecay() const;
    
    // Integration with base STDP
    void setBaseSTDP(STDP* stdp);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Spike-timing dependent plasticity with triplet interactions
// Enhanced implementation with pre-post-post and post-pre-pre interactions
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
    
    // Triplet interaction parameters
    void setPrePostPostWeight(float weight);
    float getPrePostPostWeight() const;
    void setPostPrePreWeight(float weight);
    float getPostPrePreWeight() const;
    void setFacilitationStrength(float strength);
    float getFacilitationStrength() const;
    void setDepressionStrength(float strength);
    float getDepressionStrength() const;
    
    // Synaptic resource management
    void enableSynapticResources(bool enable);
    void setResourceLimit(float limit);
    void setResourceRecovery(float recovery);
    
    bool isSynapticResourcesEnabled() const;
    float getResourceLimit() const;
    float getResourceRecovery() const;
    
    // Trace management
    void enableFacilitationTrace(bool enable);
    void enableDepressionTrace(bool enable);
    void setTraceDecay(float decay);
    
    bool isFacilitationTraceEnabled() const;
    bool isDepressionTraceEnabled() const;
    float getTraceDecay() const;
    
    // Integration with eligibility traces
    void setEligibilityTraceEnabled(bool enable);
    bool isEligibilityTraceEnabled() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
