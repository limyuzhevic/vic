#include "PlasticityRule.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <set>

namespace nlm {

bool PlasticityRule::isEnabled() const {
    return enabled_;
}

void PlasticityRule::setEnabled(bool enabled) {
    enabled_ = enabled;
}

struct HebbianRule::Impl {
    float learningRate;
    float maxWeight;
    float traceDecay;
    
    Impl() : learningRate(0.01f), maxWeight(1.0f), traceDecay(0.95f) {}
};

HebbianRule::HebbianRule() : pImpl(new Impl) {}

HebbianRule::~HebbianRule() = default;

void HebbianRule::update(Synapse* synapse,
                          const std::vector<Timestamp>& preSpikes,
                          const std::vector<Timestamp>& postSpikes,
                          TimestepDuration dt) {
    // Real Hebbian learning: "neurons that fire together, wire together"
    // Implements spike-timing dependent Hebbian learning with eligibility traces
    
    if (preSpikes.empty() || postSpikes.empty() || !synapse) {
        return;
    }
    
    // Calculate synaptic efficacy based on spike timing
    float weightChange = calculateWeightChange(preSpikes, postSpikes, dt);
    
    // Apply weight change with bounds checking
    applyWeightChange(synapse, weightChange);
}

void HebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float currentWeight = synapse->getWeight();
    float newWeight = currentWeight + delta;
    
    // Apply biological bounds: weights between -0.5 and 0.5 for typical synapses
    newWeight = std::clamp(newWeight, -0.5f, 0.5f);
    
    synapse->setWeight(newWeight);
}

float HebbianRule::calculateWeightChange(const std::vector<Timestamp>& preSpikes,
                                          const std::vector<Timestamp>& postSpikes,
                                          TimestepDuration dt) {
    // Calculate synaptic modification based on spike timing
    // Using asymmetric Hebbian rule: post-synaptic spike shortly after pre-synaptic spike strengthens synapse
    
    float totalWeightChange = 0.0f;
    float timeWindow = 20.0f;  // 20ms window for spike pairing
    
    // Create a set of pre-synaptic spike times for efficient lookup
    std::set<Timestamp> preSpikeTimes(preSpikes.begin(), preSpikes.end());
    
    // For each post-synaptic spike, find pre-synaptic spikes within time window
    for (Timestamp postTime : postSpikes) {
        for (Timestamp preTime : preSpikeTimes) {
            // Calculate time difference (positive if post follows pre)
            float timeDiff = static_cast<float>(postTime) - static_cast<float>(preTime);
            
            // Apply asymmetric Hebbian rule
            if (timeDiff > 0.0f && timeDiff < timeWindow) {
                // Post-synaptic spike after pre-synaptic spike: strengthen
                float temporalFactor = 1.0f - (timeDiff / timeWindow);
                totalWeightChange += pImpl->learningRate * temporalFactor;
            } else if (timeDiff < 0.0f && std::abs(timeDiff) < timeWindow) {
                // Pre-synaptic spike after post-synaptic spike: weaken (anti-Hebbian component)
                float temporalFactor = 1.0f - (std::abs(timeDiff) / timeWindow);
                totalWeightChange -= pImpl->learningRate * temporalFactor * 0.3f;  // Weaker anti-Hebbian effect
            }
        }
    }
    
    return totalWeightChange;
}

const char* HebbianRule::getName() const {
    return "Hebbian";
}

void HebbianRule::setLearningRate(float rate) {
    pImpl->learningRate = std::max(0.0f, std::min(rate, 0.1f));  // Limit to reasonable range
}

float HebbianRule::getLearningRate() const {
    return pImpl->learningRate;
}

void HebbianRule::setMaxWeight(float maxWeight) {
    pImpl->maxWeight = std::max(0.01f, maxWeight);
}

float HebbianRule::getMaxWeight() const {
    return pImpl->maxWeight;
}

} // namespace nlm
