#pragma once

#include "../plasticity/PlasticityRule.hpp"

namespace nlm {

// Anti-Hebbian plasticity rule: “neurons that fire together, disconnect together”
// Implements competitive learning to suppress correlated activity and promote synaptic diversity
// Anti-HebbianRule decreases synaptic weights when pre- and post-synaptic neurons fire together,
// implementing negative correlation learning to maintain network stability and balance.

class AntiHebbian : public PlasticityRule {
public:
    AntiHebbian();
    ~AntiHebbian() override;
    
    // Update synaptic weights based on anti-Hebbian learning principle
    // Anti-Hebbian rule: “fire together, wire apart” - decreases weights for correlated activity
    void update(Synapse* synapse, 
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    
    // Apply weight change with anti-Hebbian constraints
    // Applies weight changes with competitive dynamics and homeostatic regulation
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    // Get rule name
    const char* getName() const override;
    
    // Set inhibition threshold for anti-Hebbian learning
    // Higher threshold increases inhibition strength for correlated activity
    void setInhibitionThreshold(float threshold);
    
    // Get current inhibition threshold
    float getInhibitionThreshold() const;
    
    // Set maximum synaptic weight
    void setMaxWeight(float maxWeight);
    
    // Get maximum synaptic weight
    float getMaxWeight() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm