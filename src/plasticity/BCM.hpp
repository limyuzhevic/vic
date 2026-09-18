#pragma once

#include "../core/Types/Types.hpp"
#include "Synapse.hpp"

namespace nlm {

// Enhanced Bienenstock-Cooper-Munro (BCM) rule with sliding threshold dynamics
// Implements "stable learning" through the BCM theory of synaptic modification
// Δw = η * θ * (post - θ2_prime)
// where θ2_prime is a sliding threshold that adapts to postsynaptic activity

class BCM : public PlasticityRule {
public:
    BCM();
    ~BCM() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Learning parameters
    void setLearningRate(float rate);
    float getLearningRate() const;
    void setTheta(float theta);                    // BCM threshold
    float getTheta() const;
    void setSigma(float sigma);                    // Stability threshold
    float getSigma() const;
    void setTimeConstant(float tau);               // Adaptation time constant
    float getTimeConstant() const;
    void setWeightBounds(float minWeight, float maxWeight);
    void setActivityNormalization(float phi);      // Activity normalization factor
    
    // Utility functions
    float computePresynapticActivity(const std::vector<Timestamp>& preSpikes, TimestepDuration dt);
    float computePostsynapticActivity(const std::vector<Timestamp>& postSpikes);
    void updateSlidingThreshold(float preActivity, TimestepDuration dt);
    float calculateWeightChange(float postActivity, TimestepDuration dt);
    
    // State management
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
