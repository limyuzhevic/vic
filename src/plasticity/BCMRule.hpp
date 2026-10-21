#pragma once

#include "PlasticityRule.hpp"
#include <vector>

namespace nlm {

// BCM (Bienenstock-Cooper-Munro) learning rule
// Implements a sliding threshold mechanism for homeostatic stability
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

    // BCM rule parameters
    void setLearningRate(float rate);                    // Learning rate (η)
    float getLearningRate() const;
    void setThreshold(float threshold);                  // BCM threshold (θ)
    float getThreshold() const;
    void setThetaMinus(float thetaMinus);                // θ- for long-term threshold
    float getThetaMinus() const;
    void setThetaPlus(float thetaPlus);                  // θ+ for short-term threshold
    float getThetaPlus() const;
    void setMaxWeight(float max);                         // Maximum weight bound
    float getMaxWeight() const;
    void setMinWeight(float min);                         // Minimum weight bound
    float getMinWeight() const;

    // BCM internal state
    void updateThreshold(Timestamp currentTime);          // Update sliding threshold
    void resetThreshold();                                // Reset threshold to θ-
    float getThreshold() const;                           // Get current threshold
    float getSlidingThreshold() const;                    // Get current sliding threshold value
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm