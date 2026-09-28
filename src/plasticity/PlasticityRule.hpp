#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Synapse.hpp"
#include <vector>

namespace nlm {

// Abstract base class for plasticity rules
// This is the base class for all Hebbian plasticity implementations
// Phase 6 implementations extend this for real neural computation

class PlasticityRule {
public:
    PlasticityRule() : enabled_(true) {}
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
    bool isEnabled() const { return enabled_; }
    void setEnabled(bool enabled) { enabled_ = enabled; }
    
private:
    bool enabled_;
};

} // namespace nlm
