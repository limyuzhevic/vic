#pragma once

#include "PlasticityRule.hpp"
#include "../../brain/Synapse.hpp"

namespace nlm {

// Spike-Timing-Dependent Plasticity (STDP)
// Advanced implementation with triplet interactions, calcium dynamics, and voltage dependence

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
    
    // Advanced STDP parameters
    void setTripletInteraction(bool enable);        // Enable/disable triplet interactions
    void setCalciumDynamics(bool enable);           // Enable calcium dynamics
    void setVoltageDependence(bool enable);          // Enable voltage dependence
    void setMetaplasticity(bool enable);            // Enable weight-dependent plasticity
    
    // Neuron type selection
    enum class NeuronType {
        PYRAMIDAL, INTERNEURON, SPINY_STRIATE, UNKNOWN
    };
    
    void setNeuronType(NeuronType type);
    NeuronType getNeuronType() const;
    
    // Update parameters from config
    void configure(float ltpWeight, float ltdWeight, float tau);
    
    // Performance optimization settings
    void enableVectorization(bool enable);
    void enableParallelProcessing(bool enable);
    void enableEarlyTermination(bool enable);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Reward-modulated STDP (R-STDP)
// Integrates STDP with dopaminergic signals for reinforcement learning
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
    
    // Reward modulation parameters
    void setLearningRate(float rate);              // Overall learning rate
    void setDopamineGain(float gain);              // Dopamine signal gain
    void setPredictionErrorWeight(float weight);   // Reward prediction error weighting
    
    // Eligibility trace management
    float getEligibilityTrace() const;
    void updateEligibilityTrace(float delta, float decayRate);
    void setEligibilityTrace(float trace);
    
    // Integration with base STDP
    void setBaseSTDP(STDP* stdp);
    
private:
    struct Impl;
    Impl* pImpl;
    STDP* baseSTDP;
};

// Spike-timing dependent plasticity with triplet interactions
// Advanced STDP incorporating B1 (paired-pulse potentiation) and B2 (paired-pulse depression)
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
    void setB1Weight(float weight);                // Paired-pulse potentiation weight
    void setB2Weight(float weight);                // Paired-pulse depression weight
    void setB1TimeConstant(float tau);             // Time constant for B1
    void setB2TimeConstant(float tau);             // Time constant for B2
    
    // Triplet interaction windows
    void setTripletWindow(float window);           // Time window for triplet interactions (ms)
    void setTripletMaxSpikes(int max);             // Maximum spikes per triplet
    
    // Integration with base STDP
    void setBaseSTDP(STDP* stdp);
    
private:
    struct Impl;
    Impl* pImpl;
    STDP* baseSTDP;
};

} // namespace nlm
