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

// Enhanced STDP rules
    // Reward-modulated STDP (R-STDP)
    class RewardModulatedSTDP : public PlasticityRule {
    public:
        void update(Synapse* synapse,
                    const std::vector<Timestamp>& preSpikes,
                    const std::vector<Timestamp>& postSpikes,
                    TimestepDuration dt) override;
        void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
        const char* getName() const override { return "R-STDP"; }
        
        // Reward prediction error
        void setPredictionError(float error);
        float getPredictionError() const;
    };
    
    // Spike-timing dependent plasticity with triplet interactions
    class TripletSTDP : public PlasticityRule {
    public:
        void update(Synapse* synapse,
                    const std::vector<Timestamp>& preSpikes,
                    const std::vector<Timestamp>& postSpikes,
                    TimestepDuration dt) override;
        void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
        const char* getName() const override { return "TripletSTDP"; }
        
        // Triplet parameters
        void setTripletParameters(float t1, float t2, float t3);
        void getTripletParameters(float& t1, float& t2, float& t3) const;
    };
    
    // Spike-timing dependent plasticity with resource competition
    class ResourceConstrainedSTDP : public PlasticityRule {
    public:
        void update(Synapse* synapse,
                    const std::vector<Timestamp>& preSpikes,
                    const std::vector<Timestamp>& postSpikes,
                    TimestepDuration dt) override;
        void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
        const char* getName() const override { return "ResourceConstrainedSTDP"; }
        
        // Resource parameters
        void setResourceCapacity(float capacity);
        float getResourceCapacity() const;
        void setCompetitionFactor(float factor);
        float getCompetitionFactor() const;
    };
    
    // Spike-timing dependent plasticity with activity-dependent learning rates
    class NeuromodulatedSTDP : public PlasticityRule {
    public:
        void update(Synapse* synapse,
                    const std::vector<Timestamp>& preSpikes,
                    const std::vector<Timestamp>& postSpikes,
                    TimestepDuration dt) override;
        void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
        const char* getName() const override { return "NeuromodulatedSTDP"; }
        
        // Neuromodulation parameters
        void setDopamineLevel(float level);
        float getDopamineLevel() const;
        void setCuriosityLevel(float level);
        float getCuriosityLevel() const;
        void setPredictionErrorLevel(float level);
        float getPredictionErrorLevel() const;
    };
