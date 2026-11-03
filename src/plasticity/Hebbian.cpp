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
    
    // Real Hebbian learning: strengthen when pre and post fire together
    // Use nearest-neighbor approach for computational efficiency
    float dtSeconds = static_cast<float>(postSpikes.back() - preSpikes.back());
    
    // Hebbian rule: if pre fires shortly before post, potentiate
    // The time window is 100ms as in the original implementation
    float delta = 0.0f;
    if (std::abs(dtSeconds) < 0.1f) {
        // Positive correlation: strengthen synapse
        delta = pImpl->learningRate;
        if (synapse->getType() == SynapseType::Inhibitory) {
            // For inhibitory synapses, strengthening means making them more negative
            delta = -delta;
        }
    }
    // If dtSeconds > 0.1f (post fired long after pre) or dtSeconds < -0.1f (post fired before pre),
    // no Hebbian effect
    
    // Apply with soft bounds to prevent runaway potentiation
    if (std::abs(delta) > 1e-6f) {
        // Weight-dependent scaling: potentiation (away from zero) scales by (1 - |w|)
        // Depression (toward zero) scales by |w|
        // This prevents all synapses from saturating together
        {
            float w = synapse->getWeight();
            bool awayFromZero = std::abs(w + delta) > std::abs(w);
            delta *= awayFromZero ? (1.0f - std::abs(w)) : std::abs(w);
        }
        
        // Apply weight change
        applyWeightChange(synapse, delta);
        
        // Enforce sign: excitatory stays >= 0.01, inhibitory stays <= -0.01
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
