#include "Hebbian.hpp"
#include "../brain/Synapse.hpp"
#include <algorithm>

namespace nlm {

struct Hebbian::Impl {
    float learningRate;
    float maxWeight;
    float minWeight;
    float covarianceThreshold;  // For covariance rule
    
    Impl() : learningRate(0.01f), maxWeight(1.0f), minWeight(-1.0f),
             covarianceThreshold(0.0f) {}
};

Hebbian::Hebbian() : pImpl(new Impl) {}

Hebbian::~Hebbian() = default;

void Hebbian::update(Synapse* synapse,
                      const std::vector<Timestamp>& preSpikes,
                      const std::vector<Timestamp>& postSpikes,
                      TimestepDuration dt) {
    // Covariance Hebbian learning: "neurons that fire together, wire together"
    // IMPROVEMENT: More selective learning to prevent uniform saturation
    // Original implementation learns from ANY co-activity, causing all
    // co-active synapses to potentiate equally.
    // New implementation tracks coactivity relative to baseline and ignores
    // correlated patterns that don't show significant activity covariance.
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate correlation strength relative to baseline
    // Simplified covariance: coactivity - expected coactivity based on individual firing rates
    // Nearest-neighbour: one correlated recent pair = one step (not all historic pairs)
    // Timestamps are in seconds, so the 100ms window is 0.1s.
    float dtSeconds = static_cast<float>(postSpikes.back() - preSpikes.back());
    bool isCorrelated = std::abs(dtSeconds) < 0.1f;
    
    // Get pre and post spike counts from recent history
    size_t preSpikeCount = preSpikes.size();
    size_t postSpikeCount = postSpikes.size();
    
    // Calculate baseline expected coactivity (product of firing rates)
    // Use firing rate if available, otherwise use normalized spike counts
    float baselineCorrelation = static_cast<float>(preSpikeCount) * static_cast<float>(postSpikeCount) / 100.0f;  // Normalize by typical history length
    
    // Calculate actual correlation strength (inverse of timing difference)
    float actualCorrelation = 0.0f;
    if (isCorrelated && dtSeconds != 0.0f) {
        actualCorrelation = 1.0f / std::abs(dtSeconds);  // Stronger correlation for tighter timing
    }
    
    // Compute learning signal: only learn if actual > baseline + threshold
    // This prevents uniform potentiation from correlated but weak patterns
    float learningSignal = std::max(0.0f, actualCorrelation - baselineCorrelation);
    
    // Only apply Hebbian learning if there's significant covariance
    if (learningSignal > 0.001f) {
        float delta = pImpl->learningRate * learningSignal;
        if (synapse->getType() == SynapseType::Inhibitory) {
            delta = -delta;  // strengthen inhibition, don't flip it to excitatory
        }
        
        // Apply with selective bounds. Soft bound, Phase 4: Hebbian
        // strengthening is always away from zero, so scale it by (1 - |w|);
        // but add an additional selectivity factor to prevent uniform saturation.
        if (std::abs(delta) > 1e-6f) {
            // Multiple factors to control saturation:
            // 1. Weight proximity to bound (1 - |w|)
            // 2. Learning signal strength (prevents learning from weak patterns)
            // 3. Synaptic efficacy (normalizes by overall synapse health)
            float efficacy = synapse->getEfficacy();
            
            // Selectivity factor: stronger learning when pattern is distinct
            float selectivityFactor = std::min(1.0f, learningSignal / 2.0f);
            
            // Adaptive learning: reduce rate when network is excited (prevents runaway)
            float excitationFactor = 1.0f;
            float weight = synapse->getWeight();
            if (std::abs(weight) > 0.8f) {
                excitationFactor = 0.5f;  // Strong weights learn slower
            }
            
            delta *= (1.0f - std::abs(weight)) * selectivityFactor * efficacy * excitationFactor;
            
            applyWeightChange(synapse, delta);
            if (synapse->getType() == SynapseType::Inhibitory && synapse->getWeight() > -0.01f) {
                synapse->setWeight(-0.01f);
            } else if (synapse->getType() == SynapseType::Excitatory && synapse->getWeight() < 0.01f) {
                synapse->setWeight(0.01f);
            }
        }
    }
}

void Hebbian::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* Hebbian::getName() const {
    return "Hebbian";
}

void Hebbian::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

float Hebbian::getLearningRate() const {
    return pImpl->learningRate;
}

void Hebbian::setMaxWeight(float maxWeight) {
    pImpl->maxWeight = std::clamp(maxWeight, 0.0f, 10.0f);
}

float Hebbian::getMaxWeight() const {
    return pImpl->maxWeight;
}

} // namespace nlm
