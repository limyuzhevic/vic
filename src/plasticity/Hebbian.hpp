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
    
    // Advanced Hebbian learning features
    void enableAdvancedMode(bool enabled);
    bool isAdvancedModeEnabled() const;
    void setHomeostaticStrength(float strength);
    float getHomeostaticStrength() const;
    void setWeightRange(float range);
    float getWeightRange() const;
    void setTimeConstants(float ltpTc, float ltdTc);
    void setAdaptationStrength(float strength);
    float getAdaptationStrength() const;
    void recordSpikeStats(const std::vector<Timestamp>& spikes);
    float getAdaptiveLearningRate() const;
    void resetStats();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
