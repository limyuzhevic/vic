#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

// Bienenstock-Cooper-Munro (BCM) rule
// Implements sliding threshold theory for stable cortical development
// Learning is gated by a threshold theta that adapts based on postsynaptic activity
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
    
    // Parameters
    void setTheta(float theta);
    float getTheta() const;
    void setBCMLearningRate(float rate);
    float getBCMLearningRate() const;
    void setBCMTimeConstant(float tau);
    float getBCMTimeConstant() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm