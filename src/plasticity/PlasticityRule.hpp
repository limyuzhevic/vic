#pragma once

#include "../../core/Logger/Logger.hpp"
#include "../../brain/Synapse.hpp"
#include <vector>

namespace nlm {

// Abstract base class for plasticity rules
class PlasticityRule {
public:
    virtual ~PlasticityRule() = default;
    
    // Update synaptic weights based on pre/post synaptic activity
    virtual void update(Synapse* synapse, 
                         const std::vector<Timestamp>& preSpikes,
                         const std::vector<Timestamp>& postSpikes,
                         TimestepDuration dt) = 0;
    
    // Apply weight change
    virtual void applyWeightChange(Synapse* synapse, SynapticWeight delta) = 0;
    
    // Get rule name
    virtual const char* getName() const = 0;
    
    // Check if rule is enabled
    bool isEnabled() const;
    void setEnabled(bool enabled);
    
protected:
    PlasticityRule() : enabled_(true) {}
    
private:
    bool enabled_;
};

// Concrete plasticity rule implementations
class Hebbian;           // Real Hebbian implementation
class AntiHebbianRule;   // Real anti-Hebbian implementation
class BCMRule;           // Real BCM rule implementation
class RewardModulatedSTDP; // Real R-STDP implementation
class TripletSTDP;       // Real triplet STDP implementation

} // namespace nlm