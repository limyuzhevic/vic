#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

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
    void setThetaM(float thetaM);        // Sliding threshold parameter
    float getThetaM() const;
    void setThetaPlus(float thetaPlus);  // LTP threshold
    float getThetaPlus() const;
    void setThetaMinus(float thetaMinus); // LTD threshold
    float getThetaMinus() const;
    void setThetaBar(float thetaBar);     // Threshold sliding rate
    float getThetaBar() const;
    
    // Configuration helper
    void configure(float rate, float thetaM, float thetaPlus, float thetaMinus, float thetaBar);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
