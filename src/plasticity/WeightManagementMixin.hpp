#pragma once

#include "../brain/Synapse.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

// WeightManagementMixin provides common synaptic weight management functionality
// including clamping, logging, and validation
class WeightManagementMixin {
public:
    // Apply weight change with clamping to specified bounds
    // Logs all weight changes for debugging and monitoring
    static void applyClampedWeightChange(Synapse* synapse, 
                                        SynapticWeight delta,
                                        SynapticWeight minWeight,
                                        SynapticWeight maxWeight,
                                        const char* source = "") {
        if (!synapse) {
            NLM_LOG_ERROR("WeightManagementMixin: null synapse pointer");
            return;
        }
        
        if (std::abs(delta) < 1e-9f) {
            NLM_LOG_DEBUG("WeightManagementMixin: negligible weight change (delta = {:.6f})", delta);
            return;
        }
        
        NLM_LOG_INFO("WeightManagementMixin {}: applying delta = {:.6f} (weight: {:.6f} -> {:.6f})",
                     source ? source : "", delta, synapse->getWeight(), synapse->getWeight() + delta);
        
        // Apply weight change with bounds checking
        float newWeight = synapse->getWeight() + delta;
        
        // Apply clamping
        if (newWeight < minWeight) {
            NLM_LOG_WARNING("WeightManagementMixin: weight {:.6f} below minimum {:.6f}, clamping", 
                           newWeight, minWeight);
            newWeight = minWeight;
        } else if (newWeight > maxWeight) {
            NLM_LOG_WARNING("WeightManagementMixin: weight {:.6f} above maximum {:.6f}, clamping",
                           newWeight, maxWeight);
            newWeight = maxWeight;
        }
        
        // Only update if weight actually changed
        if (std::abs(newWeight - synapse->getWeight()) > 1e-6f) {
            synapse->setWeight(newWeight);
            NLM_LOG_INFO("WeightManagementMixin: weight updated to {:.6f}", newWeight);
        } else {
            NLM_LOG_DEBUG("WeightManagementMixin: weight unchanged after clamping");
        }
    }
    
    // Convenience method for common weight bounds (-1.0 to 1.0)
    static void applyStandardWeightChange(Synapse* synapse, 
                                        SynapticWeight delta,
                                        const char* source = "") {
        applyClampedWeightChange(synapse, delta, -1.0f, 1.0f, source);
    }
    
    // Validate weight value
    static bool validateWeight(SynapticWeight weight, SynapticWeight minWeight, SynapticWeight maxWeight) {
        if (weight < minWeight) {
            NLM_LOG_ERROR("WeightManagementMixin: invalid weight {:.6f} (must be >= {:.6f})", weight, minWeight);
            return false;
        }
        if (weight > maxWeight) {
            NLM_LOG_ERROR("WeightManagementMixin: invalid weight {:.6f} (must be <= {:.6f})", weight, maxWeight);
            return false;
        }
        return true;
    }
}; // class WeightManagementMixin

} // namespace nlm
