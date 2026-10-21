#pragma once

#include "PlasticityRule.hpp"
#include <vector>

namespace nlm {

// Eligibility traces for delayed reward signals
// Maintains eligibility traces that decay over time and can be modulated by rewards
class EligibilityTraceSTDP : public PlasticityRule {
public:
    EligibilityTraceSTDP();
    ~EligibilityTraceSTDP() override;

    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;

    // Eligibility trace parameters
    void setLearningRate(float rate);                    // Learning rate (η)
    float getLearningRate() const;
    void setTraceDecayRate(float rate);                  // Eligibility trace decay rate (λ)
    float getTraceDecayRate() const;
    void setMaxTraceValue(float max);                     // Maximum trace value
    float getMaxTraceValue() const;
    void setMinTraceValue(float min);                     // Minimum trace value
    float getMinTraceValue() const;
    void setRewardModulationGain(float gain);             // Reward modulation gain (γ)
    float getRewardModulationGain() const;

    // Trace management
    void updateEligibilityTrace(float trace, Timestamp spikeTime, Timestamp currentTime);  // Update trace with spike
    void updateTraceWithReward(float reward, Timestamp rewardTime);                         // Update trace with reward
    float getEligibilityTrace() const;                               // Get current eligibility trace
    float getTraceHistory(Timestamp time) const;                     // Get trace at specific time
    void resetTrace();                                                // Reset trace to zero
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm