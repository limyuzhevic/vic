#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

// Bienenstock-Cooper-Munro (BCM) rule implementation
// BCM learning with dynamic threshold theta(t) that increases with postsynaptic activity
// Implements sliding threshold theory of synaptic modification
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
    void setTheta(float theta);           // Sliding threshold
    float getTheta() const;
    void setTauTheta(float tau);         // Threshold dynamics time constant
    float getTauTheta() const;
    void setLTPThreshold(float ltpThreshold);
    float getLTPThreshold() const;
    void setLTDThreshold(float ltdThreshold);
    float getLTDThreshold() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm