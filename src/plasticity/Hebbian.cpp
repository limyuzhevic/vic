#include "../plasticity/PlasticityRule.hpp"

namespace nlm {

struct Hebbian::Impl {
    float learningRate;
    float maxWeight;
    float minWeight;
    float covarianceThreshold;  // For covariance rule
    
    Impl() : learningRate(0.01f), maxWeight(1.0f), minWeight(-1.0f),
             covarianceThreshold(0.0f) {}
};

Hebbian::Hebbian() : pImpl(std::make_unique<Impl>()) {}

Hebbian::~Hebbian() = default;

void Hebbian::update(Synapse* synapse,
                       const std::vector<Timestamp>& preSpikes,
                       const std::vector<Timestamp>& postSpikes,
                       TimestepDuration dt) {
    if (!pImpl) return;
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
     
      // Apply with bounds. Soft bound using common helper.
      if (std::abs(delta) > 1e-6f) {
          // Scale by weight using common helper
          PlasticityRule::applySoftBounds(synapse, delta);
          // Enforce sign using common helper
          PlasticityRule::enforceSynapseWeightBounds(synapse);
      }
}

void Hebbian::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!pImpl || !synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* Hebbian::getName() const {
    return "Hebbian";
}

void Hebbian::setLearningRate(float rate) {
    if (!pImpl) return;
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

float Hebbian::getLearningRate() const {
    if (!pImpl) return 0.0f;
    return pImpl->learningRate;
}

void Hebbian::setMaxWeight(float maxWeight) {
    if (!pImpl) return;
    pImpl->maxWeight = std::clamp(maxWeight, 0.0f, 10.0f);
}

float Hebbian::getMaxWeight() const {
    if (!pImpl) return 0.0f;
    return pImpl->maxWeight;
}

} // namespace nlm
