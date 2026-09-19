#pragma once

#include "PlasticityRule.hpp"
#include <string>

namespace nlm {

// Spike-Timing-Dependent Plasticity with calcium dynamics
// Implements precise spike timing models with biological accuracy

class STDP : public PlasticityRule {
public:
    STDP();
    ~STDP() override;
    
    // Update synaptic weights based on spike timing
    // Implements calcium-based STDP model with spike history tracking
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Core STDP parameters
    void setLTPWeight(float weight);    // Long-term potentiation weight (A+)
    float getLTPWeight() const;
    void setLTDWeight(float weight);    // Long-term depression weight (A-)
    float getLTDWeight() const;
    void setTimeConstant(float tau);    // STDP time constant (ms)
    float getTimeConstant() const;
    
    // Calcium dynamics parameters
    void setCalciumConductance(float conductance);
    float getCalciumConductance() const;
    void setCalciumDecayRate(float rate);
    float getCalciumDecayRate() const;
    void setCalciumThresholdLTP(float threshold);
    float getCalciumThresholdLTP() const;
    void setCalciumThresholdLTD(float threshold);
    float getCalciumThresholdLTD() const;
    void setCalciumBaseline(float baseline);
    float getCalciumBaseline() const;
    
    // Spike history tracking
    void setHistoryDecayRate(float rate);
    float getHistoryDecayRate() const;
    void setMaxHistorySize(size_t maxSize);
    size_t getMaxHistorySize() const;
    
    // Neuromodulatory control
    void setNeuromodulatorLevel(float level);
    float getNeuromodulatorLevel() const;
    void setNeuromodulatorInfluence(float influence);
    float getNeuromodulatorInfluence() const;
    void enableNeuromodulation(bool enable);
    bool isNeuromodulationEnabled() const;
    
    // Weight dependence parameters
    void setWeightDependenceLTP(float dependence);
    float getWeightDependenceLTP() const;
    void setWeightDependenceLTD(float dependence);
    float getWeightDependenceLTD() const;
    void setMaxWeightTrace(float maxTrace);
    float getMaxWeightTrace() const;
    
    // Triplet interactions
    void setTripletTrace(float trace);
    float getTripletTrace() const;
    void setTripletDecayRate(float rate);
    float getTripletDecayRate() const;
    
    // Configuration from global parameters
    void configure(float ltpWeight, float ltdWeight, float tau,
                   float caConductance, float caDecayRate,
                   float caThresholdLTP, float caThresholdLTD,
                   float historyDecayRate, float neuromodulatorLevel,
                   float weightDependenceLTP, float weightDependenceLTD);
    
    // Statistics and debugging
    size_t getTotalUpdates() const;
    float getCumulativeWeightChange() const;
    
    // Spike history management
    void recordPreSpike(SynapseId synapseId, Timestamp time);
    void recordPostSpike(SynapseId synapseId, Timestamp time);
    float getPreSpikeHistory(SynapseId synapseId, Timestamp referenceTime) const;
    float getPostSpikeHistory(SynapseId synapseId, Timestamp referenceTime) const;
    void clearHistory(SynapseId synapseId);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Reward-modulated STDP (R-STDP) - combines STDP with reward signals
class RewardModulatedSTDP : public PlasticityRule {
public:
    RewardModulatedSTDP(float rewardMagnitude = 1.0f);
    ~RewardModulatedSTDP() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override { return "R-STDP"; }
    
    void setRewardMagnitude(float magnitude);
    float getRewardMagnitude() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Spike-timing dependent plasticity with triplet interactions
// More biologically accurate model accounting for three-spike correlations
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
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
