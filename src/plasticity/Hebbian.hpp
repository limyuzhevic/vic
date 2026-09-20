#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

// Enhanced Hebbian learning implementation with biological mechanisms
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
    
    // Enhanced biological mechanisms
    void setNMDAConductance(float conductance);
    float getNMDAConductance() const;
    
    void setCalciumThreshold(float threshold);
    float getCalciumThreshold() const;
    
    void setMetaplasticLearningRate(float rate);
    float getMetaplasticLearningRate() const;
    
private:
    struct Impl;
    Impl* pImpl;
    
    // Biological computation methods
    float calculateNMDAActivation(const std::vector<Timestamp>& preSpikes,
                                  const std::vector<Timestamp>& postSpikes,
                                  TimestepDuration dt) const;
    
    float calculateCovarianceChange(const std::vector<Timestamp>& preSpikes,
                                  const std::vector<Timestamp>& postSpikes,
                                  TimestepDuration dt) const;
    
    float calculateMeanActivity(const std::vector<Timestamp>& spikes) const;
    
    float calculateMetaplasticityFactor(Synapse* synapse) const;
    
    float calculateExperienceFactor(Synapse* synapse) const;
    
    void tagForConsolidation(Synapse* synapse, float delta) const;
};

} // namespace nlm
