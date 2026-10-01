#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Synapse.hpp"

namespace nlm {

// Abstract base class for plasticity rules
// PLACEHOLDER - Phase 2 will implement real plasticity rules

class PlasticityRule {
public:
    virtual ~PlasticityRule() = default;
    
    // Update synaptic weights based on pre/post synaptic activity
    // TODO PHASE 2: Implement real plasticity
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

} // namespace nlm
