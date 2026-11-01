#include "Hebbian.hpp"
#include "../brain/Synapse.hpp"
#include <algorithm>

namespace nlm {

struct Hebbian::Impl {
    float learningRate;
    float maxWeight;
    float minWeight;
    float covarianceThreshold;  // For covariance rule
    
    Impl() : learningRate(0.005f), maxWeight(0.5f), minWeight(0.05f),  // More conservative parameters
             covarianceThreshold(0.0f) {}
};
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
    
    // Nearest-neighbour version: the old all-pairs count made delta grow
    // with history length (up to 100x100 pairs) and saturated every weight
    // to its bound in one step. One correlated recent pair = one step.
    // Timestamps are in seconds, so the 100ms window is 0.1s.
    float dtSeconds = static_cast<float>(postSpikes.back() - preSpikes.back());
    float delta = std::abs(dtSeconds) < 0.1f ? pImpl->learningRate : 0.0f;
    if (synapse->getType() == SynapseType::Inhibitory) {
        delta = -delta;  // strengthen inhibition, don't flip it to excitatory
    }
    
    // Apply with bounds. Soft bound, Phase 4 (see STDP.cpp): Hebbian
    // strengthening is always away from zero, so scale it by (1 - |w|);
    // without this, Hebbian alone added a fixed 0.01 per co-active step
    // and saturated every weight together with STDP.
    if (std::abs(delta) > 1e-6f) {
        delta *= (1.0f - std::abs(synapse->getWeight()));
        
        // Better balance with STDP: reduce Hebbian contribution when STDP
        // has already made significant changes to the synapse
        float stdpInfluence = 1.0f - std::abs(synapse->getWeight()) * 0.5f;
        delta *= stdpInfluence * 0.8f;  // Reduce Hebbian effect to 80% of original
        
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

// New learning parameter methods for better balance with STDP
void Hebbian::setLearningParameters(float hebbianRate, float maxHebbianWeight, float minHebbianWeight) {
    pImpl->learningRate = std::clamp(hebbianRate, 0.0f, 1.0f);
    pImpl->maxWeight = std::clamp(maxHebbianWeight, 0.0f, 1.0f);
    pImpl->minWeight = std::clamp(minHebbianWeight, 0.0f, pImpl->maxWeight);
}

float Hebbian::getHebbianLearningRate() const {
    return pImpl->learningRate;
}

float Hebbian::getHebbianMaxWeight() const {
    return pImpl->maxWeight;
}

float Hebbian::getHebbianMinWeight() const {
    return pImpl->minWeight;
}

} // namespace nlm
