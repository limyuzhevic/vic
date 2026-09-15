#include "../plasticity/PlasticityRule.hpp"
#include <cmath>

namespace nlm {

// Implementation of AntiHebbian rule
void AntiHebbianRule::update(Synapse* synapse, 
                           const std::vector<Timestamp>& preSpikes,
                           const std::vector<Timestamp>& postSpikes,
                           TimestepDuration dt) {
    if (synapse) {
        // Anti-Hebbian: weaken connections when pre and post fire together
        // Δw = -A * exp(-|Δt|/τ) - synaptic depression for correlated activity
        
        float weightChange = 0.0f;
        float antiHebbianRate = 0.008f;  // Depression rate
        float timeConstant = 15.0f;      // Time constant for depression
        
        // Calculate weight change based on spike timing
        if (!preSpikes.empty() && !postSpikes.empty()) {
            // Simple implementation: reduce weight when both spike
            // Real implementation would use precise spike timing
            weightChange = -antiHebbianRate * 
                          (1.0f + std::cos(static_cast<float>(preSpikes.size()) * 0.1f)) *
                          std::exp(-static_cast<float>(postSpikes.size()) / timeConstant);
        }
        
        applyWeightChange(synapse, weightChange);
    }
}

} // namespace nlm
