#pragma once

#include "PlasticityRule.hpp"
#include "../brain/Synapse.hpp"

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
    
    // Enhanced helper methods for improved plasticity
    private:
    float getAverageFiringRateEstimate(const std::vector<Timestamp>& preSpikes,
                                       const std::vector<Timestamp>& postSpikes) const;
    
    float getActivityDependentLearningRate(float activity) const;
    
    float getSynapticNormalizationFactor(float weight) const;
    
    float getActivityDependentBoundsFactor(float activity, float weight) const;
    
    float getSynapticCompetitionFactor(Synapse* synapse) const;
    
    float getMetaplasticityFactor(Synapse* synapse) const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
