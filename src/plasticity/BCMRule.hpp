#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

// Bienenstock-Cooper-Munro (BCM) rule
// Real implementation of sliding threshold learning
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
    void setSigma(float sigma);
    float getSigma() const;
    void setTauPlus(float tau);
    float getTauPlus() const;
    void setTauMinus(float tau);
    float getTauMinus() const;
    void setLearningRate(float rate);
    float getLearningRate() const;
    
    // Configure all parameters
    void configure(float sigma, float tauPlus, float tauMinus, float learningRate);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm