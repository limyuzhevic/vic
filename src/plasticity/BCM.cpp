#include "../plasticity/PlasticityRule.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

// Implementation of BCM (Bienenstock-Cooper-Munro) rule
void BCMRule::update(Synapse* synapse, 
                    const std::vector<Timestamp>& preSpikes,
                    const std::vector<Timestamp>& postSpikes,
                    TimestepDuration dt) {
    if (synapse) {
        // BCM rule: sliding threshold for synaptic modification
        // "Hebbian learning until threshold, anti-Hebbian beyond threshold"
        // 
        // Get current synaptic weight
        float currentWeight = synapse->getWeight();
        
        // Compute BCM synaptic modification parameter (tau)
        // tau = w^2 (square of current weight)
        float tau = currentWeight * currentWeight;
        
        // Compute BCM sliding threshold (theta)
        // theta = 0.5 + 0.5*tanh(w/2) - typical BCM threshold dynamics
        float theta = 0.5f + 0.5f * std::tanh(currentWeight / 2.0f);
        
        // Compute BCM weight change
        // Δw = <post> * (post - theta) - activity-dependent modification
        
        float averagePostActivity = static_cast<float>(postSpikes.size()) * 0.01f;  // Simplified
        float weightChange = averagePostActivity * (averagePostActivity - theta);
        
        // Apply bounds and plasticity constraints
        weightChange = std::clamp(weightChange, -0.01f, 0.01f);
        
        applyWeightChange(synapse, weightChange);
    }
}

} // namespace nlm
