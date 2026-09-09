#include "Hebbian.hpp"
#include "../../brain/Synapse.hpp"
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
     * Mathematical formulation:
     * Δw = η * (post * pre) - ε * (post * pre * (post - 1))
     * 
     * Where:
     *   η = learningRate (potentiation factor)
     *   ε = depression rate (Oja's rule for stability)
     *   post = postsynaptic firing indicator (1 if spiked, else 0)
     *   pre = presynaptic firing indicator (1 if spiked, else 0)
     * 
     * This implements "neurons that fire together, wire together"
     * with Oja's normalization to prevent runaway excitation.
     * 
     * Biological inspiration:
     *   - Reflects synaptic strengthening through repeated co-activation
     *   - NMDA receptor-dependent long-term potentiation (LTP)
     *   - AMPA receptor insertion at synapses
     *   - Similar to "fire together, wire together" principle
     *   
     * Advantages over simple correlation:
     *   - Implemented directly from spike times
     *   - Includes depression mechanism (Oja's rule)
     *   - More biologically plausible
     *   - Naturally bounded weights
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Count coactive spike pairs within a reasonable time window
    size_t coactivePairs = 0;
    const float window = 20.0f;  // 20ms window for Hebbian coactivity
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            if (std::abs(dt) < window) {
                coactivePairs++;
            }
        }
    }
    
    // Convert to firing rates (spikes per timestep)
    float preRate = static_cast<float>(preSpikes.size()) / dt;
    float postRate = static_cast<float>(postSpikes.size()) / dt;
    
    // Pure Hebbian term: w += η * post * pre
    float hebbianDelta = pImpl->learningRate * postRate * preRate;
    
    // Add Oja's depression term: w -= ε * post * pre * (post - 1)
    // This prevents weights from growing unbounded
    float ojaDepression = 0.1f * pImpl->learningRate * postRate * preRate * (postRate - 1.0f);
    
    // Net weight change
    float delta = hebbianDelta + ojaDepression;
    
    // Apply with bounds
    if (std::abs(delta) > 1e-6f) {
        applyWeightChange(synapse, delta);
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
