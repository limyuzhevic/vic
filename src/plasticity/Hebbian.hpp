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
    
    // Advanced covariance computation
    float computeCovarianceUpdate(const std::vector<Timestamp>& preSpikes,
                                  const std::vector<Timestamp>& postSpikes);
    double computeSpikeCoincidence(const std::vector<Timestamp>& preSpikes,
                                   const std::vector<Timestamp>& postSpikes);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
