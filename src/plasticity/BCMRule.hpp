#pragma once

#include "PlasticityRule.hpp"
#include "../../brain/Synapse.hpp"

namespace nlm {

// BCM (Bienenstock-Cooper-Munro) plasticity rule
// Homeostatic scaling based on average postsynaptic activity
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
    
    // Configuration parameters
    void setLearningRate(float rate);
    float getLearningRate() const;
    
    void setHebbianThreshold(float threshold);
    float getHebbianThreshold() const;
    
    void setHomeostaticTarget(float target);
    float getHomeostaticTarget() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm