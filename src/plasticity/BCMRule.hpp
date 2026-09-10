#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Synapse.hpp"

namespace nlm {

// Bienenstock-Cooper-Munro (BCM) rule: Sliding threshold for synaptic modification
// Activity-dependent threshold for synaptic plasticity
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
    void setTheta(float theta);           // Sliding threshold
    float getTheta() const;
    
    void setLearningRate(float rate);     // Learning rate
    float getLearningRate() const;
    
    void setTau(float tau);               // Time constant for threshold adaptation
    float getTau() const;
    
    // Update threshold based on average activity
    void updateThreshold(float activity);
    
    // Get current threshold
    float getCurrentThreshold() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
