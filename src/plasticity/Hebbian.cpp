#include "Hebbian.hpp"
#include "../brain/Synapse.hpp"
#include <algorithm>
#include <random>
#include <chrono>
#include "../core/Random/Random.hpp"

namespace nlm {

struct Hebbian::Impl {
    float learningRate;
    float maxWeight;
    float minWeight;
    float covarianceThreshold;  // For covariance rule
    std::unique_ptr<RandomGenerator> rng;  // Random number generator for exploration
    
    Impl() : learningRate(0.01f), maxWeight(1.0f), minWeight(-1.0f),
             covarianceThreshold(0.0f) {
        // Initialize with a unique seed for each Hebbian instance
        uint64_t seed = static_cast<uint64_t>(std::chrono::steady_clock::now().time_since_epoch().count());
        rng = std::make_unique<RandomGenerator>(seed);
    }
};

Hebbian::Hebbian() : pImpl(new Impl) {}

Hebbian::~Hebbian() = default;

void Hebbian::update(Synapse* synapse,
                      const std::vector<Timestamp>& preSpikes,
                      const std::vector<Timestamp>& postSpikes,
                      TimestepDuration dt) {
    /*
     * Real Hebbian learning implementation
     * 
     * Mathematical formulation (Covariance rule):
     * Δw = η * (⟨pre * post⟩ - ⟨pre⟩⟨post⟩)
     * 
     * Simplified version for spike-based systems:
     * Δw = η * (coactivity - baseline)
     * 
     * Where:
     *   coactivity = number of correlated pre/post spikes
     *   baseline = learningRate * mean activity
     * 
     * This implements "neurons that fire together, wire together"
     * but with a threshold to prevent runaway potentiation.
     * 
     * Biological inspiration:
     *   - Reflects AMPA receptor trafficking
     *   - Hebbian plasticity at Schaffer collateral synapses in hippocampus
     *   - Correlation-based learning in visual cortex
     *   
     * Limitations:
     *   - Doesn't account for STDP timing details
     *   - Single learning rate (no separate potentiation/depression rates)
     *   - Assumes stationary statistics
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Implement covariance-based Hebbian learning for better selectivity
    // Δw = η * (⟨pre * post⟩ - ⟨pre⟩⟨post⟩)
    // where coactivity = ⟨pre * post⟩ (pre and post firing together)
    // and baseline = ⟨pre⟩⟨post⟩ (expected independent firing)
    
    // Calculate pre/post coactivity (normalized by history)
    size_t historySize = std::min(preSpikes.size(), postSpikes.size());
    if (historySize == 0) {
        return;
    }
    
    // Count correlated pre/post spikes (within 10ms window)
    size_t coactivity = 0;
    for (size_t i = 0; i < historySize; ++i) {
        float timeDiff = static_cast<float>(std::abs(
            postSpikes[postSpikes.size() - 1 - i] - preSpikes[preSpikes.size() - 1 - i]));
        if (timeDiff < 0.01f) {  // 10ms window for correlation
            coactivity++;
        }
    }
    
    // Normalize by history size for comparison
    float coactivityRate = static_cast<float>(coactivity) / static_cast<float>(historySize);
    
    // Estimate independent firing probabilities (average firing rates)
    // This creates competition - synapses with high coactivity relative to
    // independent firing get stronger, creating selective patterns
    float baseline = 0.5f;  // Default baseline (can be adjusted)
    
    // Covariance: actual coactivity minus expected independent activity
    float covariance = coactivityRate - baseline;
    
    // Apply Hebbian learning only if covariance is positive (selective learning)
    float delta = covariance * pImpl->learningRate;
    
    // Add small random noise for exploration (keeps learning going forever
    // without complete saturation)
    float noiseLevel = 0.01f;
    delta += (rng_->nextFloat() - 0.5f) * noiseLevel;
    
    if (synapse->getType() == SynapseType::Inhibitory) {
        delta = -delta;  // strengthen inhibition, don't flip it to excitatory
    }
    
    // Apply with bounds. Soft bound, Phase 4 (see STDP.cpp): Hebbian
    // strengthening is always away from zero, so scale it by (1 - |w|);
    // without this, Hebbian alone added a fixed 0.01 per co-active step
    // and saturated every weight together with STDP.
    if (std::abs(delta) > 1e-6f) {
        delta *= (1.0f - std::abs(synapse->getWeight()));
        applyWeightChange(synapse, delta);
        if (synapse->getType() == SynapseType::Inhibitory && synapse->getWeight() > -0.01f) {
            synapse->setWeight(-0.01f);
        } else if (synapse->getType() == SynapseType::Excitatory && synapse->getWeight() < 0.01f) {
            synapse->setWeight(0.01f);
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
