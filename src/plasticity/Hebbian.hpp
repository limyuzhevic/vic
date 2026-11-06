#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

// Hebbian learning implementation
class Hebbian : public PlasticityRule {
public:
    Hebbian();
    ~Hebbian() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    void setLearningRate(float rate);
    float getLearningRate() const;
    
    void setMaxWeight(float maxWeight);
    float getMaxWeight() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Enhanced Hebbian learning rules
    // Variance-normalized Hebbian learning
    class VarianceNormalizedHebbian : public Hebbian {
    public:
        void update(Synapse* synapse,
                    const std::vector<Timestamp>& preSpikes,
                    const std::vector<Timestamp>& postSpikes,
                    TimestepDuration dt) override;
        void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
        const char* getName() const override { return "VarianceNormalizedHebbian"; }
        
        // Variance parameters
        void setLearningRate(float rate);
        float getLearningRate() const;
        void setMinCorrelation(float minCorr);
        float getMinCorrelation() const;
    };
    
    // Competitive Hebbian learning
    class CompetitiveHebbian : public Hebbian {
    public:
        void update(Synapse* synapse,
                    const std::vector<Timestamp>& preSpikes,
                    const std::vector<Timestamp>& postSpikes,
                    TimestepDuration dt) override;
        void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
        const char* getName() const override { return "CompetitiveHebbian"; }
        
        // Competition parameters
        void setMaxExcitatoryWeight(float maxWeight);
        float getMaxExcitatoryWeight() const;
        void setCompetitionStrength(float strength);
        float getCompetitionStrength() const;
    };
    
    // Credit assignment Hebbian learning
    class CreditAssignmentHebbian : public Hebbian {
    public:
        void update(Synapse* synapse,
                    const std::vector<Timestamp>& preSpikes,
                    const std::vector<Timestamp>& postSpikes,
                    TimestepDuration dt) override;
        void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
        const char* getName() const override { return "CreditAssignmentHebbian"; }
        
        // Credit assignment parameters
        void setDecayRate(float rate);
        float getDecayRate() const;
        void setDistalSpikeWeight(float weight);
        float getDistalSpikeWeight() const;
    };
    
    // Long-term potentiation/depression Hebbian learning
    class LTPP_DHebbian : public Hebbian {
    public:
        void update(Synapse* synapse,
                    const std::vector<Timestamp>& preSpikes,
                    const std::vector<Timestamp>& postSpikes,
                    TimestepDuration dt) override;
        void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
        const char* getName() const override { return "LTPP_DHebbian"; }
        
        // LTP/LTD parameters
        void setLTPRate(float rate);
        float getLTPRate() const;
        void setLTDRate(float rate);
        float getLTDRate() const;
        void setLTPThreshold(float threshold);
        float getLTPThreshold() const;
        void setLTDThreshold(float threshold);
        float getLTDThreshold() const;
    };

} // namespace nlm
