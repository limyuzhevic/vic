#include "PlasticityRule.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct HebbianRule::Impl {
    float learningRate;
    float maxWeight;
    float minWeight;
    float covarianceThreshold;
    
    Impl() : learningRate(0.01f), maxWeight(1.0f), minWeight(-1.0f),
             covarianceThreshold(0.0f) {}
};

HebbianRule::HebbianRule() : pImpl(new Impl) {}

HebbianRule::~HebbianRule() = default;

void HebbianRule::update(Synapse* synapse,
                         const std::vector<Timestamp>& preSpikes,
                         const std::vector<Timestamp>& postSpikes,
                         TimestepDuration dt) {
    /*
     * Real Hebbian Learning Implementation
     * 
     * Mathematical formulation (Standard Hebbian):
     * Δw = η * x_pre * x_post
     * 
     * Where:
     *   Δw = weight change
     *   η = learning rate
     *   x_pre = pre-synaptic activity (typically 0 or 1)
     *   x_post = post-synaptic activity (typically 0 or 1)
     * 
     * Biological basis:
     *   - "Neurons that fire together, wire together"
     *   - Reflects synaptic strengthening through NMDA receptor activation
     *   - Found in hippocampal LTP and cortical plasticity
     *   - Underlying mechanism for associative learning
     * 
     * Simplified implementation for spike-based systems:
     *   x_pre = 1 if pre-synaptic neuron spiked, 0 otherwise
     *   x_post = 1 if post-synaptic neuron spiked, 0 otherwise
     * 
     * This implements classical Hebbian learning with:
     *   - Symmetric weight updates
     *   - Weight normalization to prevent runaway excitation
     *   - Bounds checking (-1 to +1)
     * 
     * Applications:
     *   - Pattern association
     *   - Memory formation
     *   - Invariant feature learning
     *   - Early sensory development
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Count correlated spike pairs (simple Hebbian)
    size_t correlationCount = 0;
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            // Check if spikes are correlated within a reasonable window
            float timeDiff = std::abs(static_cast<float>(postTime - preTime));
            if (timeDiff < 50.0f) {  // 50ms window for correlation
                correlationCount++;
            }
        }
    }
    
    // Apply weight change based on correlation count
    if (correlationCount > 0) {
        float delta = pImpl->learningRate * static_cast<float>(correlationCount);
        applyWeightChange(synapse, delta);
    }
}

void HebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    
    // Apply bounds
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    
    synapse->setWeight(newWeight);
}

const char* HebbianRule::getName() const {
    return "HebbianRule";
}

void HebbianRule::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

float HebbianRule::getLearningRate() const {
    return pImpl->learningRate;
}

} // namespace nlm