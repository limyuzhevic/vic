#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

// Spike-Timing-Dependent Plasticity
// PLACEHOLDER - Phase 2 will implement real STDP

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
// Phase 6: Real reward-modulated STDP with eligibility traces
// Combines spike-timing with reward prediction error for adaptive learning
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
    void setLearningRate(float rate);
    float getLearningRate() const;
    void setPredictionErrorGain(float gain);
    float getPredictionErrorGain() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Spike-timing dependent plasticity with triplet interactions
// Phase 6: Advanced triplet STDP with presynaptic and postsynaptic traces
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
    
    // Triplet parameters
    void setTripletWeight(float weight);
    float getTripletWeight() const;
    void setSTDPWeight(float weight);
    float getSTDPWeight() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Competitive Hebbian plasticity with Winner-Take-All (WTA)
// Phase 6: Selective learning through synaptic competition
// Preserves selectivity by penalizing co-activation of similar neurons
class CompetitiveHebbianRule : public PlasticityRule {
public:
    CompetitiveHebbianRule();
    ~CompetitiveHebbianRule() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override { return "CompetitiveHebbian"; }
    
    // Competition parameters
    void setCompetitionStrength(float strength);
    float getCompetitionStrength() const;
    void setSelectivityTarget(float target);
    float getSelectivityTarget() const;
    
    // Calculate competitive penalty based on neural similarity
    float calculateCompetitivePenalty(class Neuron* source, class Neuron* target,
                                       const std::vector<class Neuron*>& allNeurons) const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm

} // namespace nlm
