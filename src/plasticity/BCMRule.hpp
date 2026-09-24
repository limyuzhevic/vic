#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

// BCM (Bienenstock-Cooper-Munro) learning implementation
// Implements BCM theory of synaptic modification with dynamic sliding threshold
class BCMRule : public PlasticityRule {
public:
    BCMRule();
    ~BCMRule() override;
    
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // BCM parameters
    void setLearningRate(float rate);
    float getLearningRate() const;
    void setThetaPlus(float thetaPlus);
    float getThetaPlus() const;
    void setThetaMinus(float thetaMinus);
    float getThetaMinus() const;
    void setTauTheta(float tauTheta);
    float getTauTheta() const;
    
private:
    float calculatePostActivity(const std::vector<Timestamp>& postSpikes) const;
    
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm