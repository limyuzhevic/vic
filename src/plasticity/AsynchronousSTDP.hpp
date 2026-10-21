#pragma once

#include "PlasticityRule.hpp"
#include <vector>

namespace nlm {

// Asynchronous STDP accounts for delayed spike transmissions
// by maintaining eligibility traces that decay over time
class AsynchronousSTDP : public PlasticityRule {
public:
    AsynchronousSTDP();
    ~AsynchronousSTDP() override;

    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;

    // Asynchronous STDP parameters
    void setLearningRate(float rate);                    // Global learning rate
    float getLearningRate() const;
    void setTimeConstant(float tau);                    // Decay time constant (ms)
    float getTimeConstant() const;
    void setDecayRate(float rate);                       // Trace decay rate
    float getDecayRate() const;
    void setMaxTrace(float max);                         // Maximum eligibility trace value
    float getMaxTrace() const;

    // Update eligibility trace with spike time
    void updateEligibilityTrace(float trace, Timestamp spikeTime, Timestamp currentTime);
    // Get current eligibility trace
    float getEligibilityTrace() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm