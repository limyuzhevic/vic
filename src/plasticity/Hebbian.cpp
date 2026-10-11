# Hebbian.cpp - Real Hebbian Learning Implementation
// Implements the Phase 2 Hebbian plasticity rule
// Real "fire together, wire together" learning mechanism

#include "Hebbian.hpp"
#include "../brain/Synapse.hpp"
#include "../brain/Neuron.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

// HebbianRule Implementation
// Implements the classic Hebbian learning rule: Δw ∝ x_pre * x_post
// Weight change proportional to product of pre- and post-synaptic activity

HebbianRule::HebbianRule() : pImpl(new Impl()) {
    // Initialize with biologically plausible parameters
    pImpl->learningRate = 0.01f;  // Learning rate (0.01-0.1 typical for Hebbian)
    pImpl->minimumWeight = 0.01f; // Minimum weight to prevent zero connections
    pImpl->maximumWeight = 1.0f;  // Maximum weight (normalized)
    pImpl->baselineWeight = 0.1f; // Baseline connection strength
}

HebbianRule::~HebbianRule() = default;

void HebbianRule::setLearningRate(float rate) {
    // Constrain learning rate to biologically plausible range
    pImpl->learningRate = std::clamp(rate, 0.001f, 0.1f);
}

float HebbianRule::getLearningRate() const {
    return pImpl->learningRate;
}

void HebbianRule::update(Synapse* synapse, 
                        const std::vector<Timestamp>& preSpikes,
                        const std::vector<Timestamp>& postSpikes,
                        TimestepDuration dt) {
    if (!synapse) return;
    
    // Calculate pre- and post-synaptic activity levels
    float preActivity = calculateActivityLevel(preSpikes, dt);
    float postActivity = calculateActivityLevel(postSpikes, dt);
    
    // Hebbian learning rule: Δw = η * pre * post
    // Where η is learning rate, pre/post are activity levels
    SynapticWeight delta = pImpl->learningRate * preActivity * postActivity;
    
    // Apply weight change with bounds checking
    SynapticWeight currentWeight = synapse->getWeight();
    SynapticWeight newWeight = currentWeight + delta;
    
    // Enforce biologically plausible weight bounds
    newWeight = std::clamp(newWeight, pImpl->minimumWeight, pImpl->maximumWeight);
    
    // Apply the weight change
    synapse->setWeight(newWeight);
    
    // Also update synaptic efficacy (short-term plasticity component)
    float &efficacy = const_cast<float&>(synapse->getEfficacy());
    efficacy = std::min(1.0f, efficacy + delta * 0.1f);  // Slower efficacy change
}

void HebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Direct weight change application (used by other plasticity rules)
    SynapticWeight currentWeight = synapse->getWeight();
    SynapticWeight newWeight = currentWeight + delta;
    
    // Enforce bounds
    newWeight = std::clamp(newWeight, pImpl->minimumWeight, pImpl->maximumWeight);
    synapse->setWeight(newWeight);
}

const char* HebbianRule::getName() const {
    return "Hebbian";
}

// Helper function to calculate synaptic activity level from spike history
float HebbianRule::calculateActivityLevel(const std::vector<Timestamp>& spikeHistory, TimestepDuration dt) {
    if (spikeHistory.empty()) {
        return 0.0f;
    }
    
    // Count recent spikes (within last 50ms)
    const float recentWindow = 50.0f;  // milliseconds
    size_t recentSpikeCount = 0;
    
    for (Timestamp spikeTime : spikeHistory) {
        if (spikeTime > 0.0f && (static_cast<float>(dt) - spikeTime) <= recentWindow) {
            ++recentSpikeCount;
        }
    }
    
    // Convert spike count to activity level (0-1 range)
    return std::min(1.0f, static_cast<float>(recentSpikeCount) * 0.1f);
}

} // namespace nlm
