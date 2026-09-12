#include "PlasticityRule.hpp"

namespace nlm {

bool PlasticityRule::isEnabled() const {
    return enabled_;
}

void PlasticityRule::setEnabled(bool enabled) {
    enabled_ = enabled;
}

struct HebbianRule::Impl {
    float learningRate;
    
    Impl() : learningRate(0.01f) {}
};

HebbianRule::HebbianRule() : pImpl(new Impl) {}

HebbianRule::~HebbianRule() = default;

void HebbianRule::update(Synapse* synapse,
                          const std::vector<Timestamp>& preSpikes,
                          const std::vector<Timestamp>& postSpikes,
                          TimestepDuration dt) {
    /*
     * Real Hebbian learning implementation based on spike correlation
     * 
     * Mathematical formulation (Rate-based Hebbian):
     * Δw = η * (⟨pre * post⟩ - ⟨pre⟩⟨post⟩)
     * 
     * Simplified version for spike-based systems:
     * Δw = η * (coactivity - baseline)
     * 
     * Where:
     *   coactivity = number of correlated pre/post spikes
     *   baseline = learningRate * mean spike count
     * 
     * This implements "neurons that fire together, wire together"
     * but with normalization to prevent runaway potentiation.
     * 
     * Biological inspiration:
     *   - Reflects LTP (Long-Term Potentiation) at synapses
     *   - Activity-dependent synaptic strengthening
     *   - Correlated neural firing leads to stronger connections
     *   
     * Limitations:
     *   - Doesn't account for STDP timing details (complementary to STDP)
     *   - Single learning rate (no separate potentiation/depression)
     *   - Assumes stationary statistics over learning period
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Count correlated spike pairs (coactivity)
    size_t coactivity = 0;
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            // Spikes within 50ms are considered correlated
            if (std::abs(static_cast<float>(postTime - preTime)) < 50.0f) {
                ++coactivity;
            }
        }
    }
    
    // Calculate baseline activity (expected coactivity by chance)
    // Simplified: proportion of pre vs post spikes times total pairs
    float baseline = pImpl->learningRate * static_cast<float>(std::sqrt(static_cast<double>(preSpikes.size() * postSpikes.size())));
    
    // Compute weight change: positive if coactivity exceeds baseline
    float delta = pImpl->learningRate * (static_cast<float>(coactivity) - baseline);
    
    // Apply with bounds
    if (std::abs(delta) > 1e-6f) {
        applyWeightChange(synapse, delta);
    }
}

void HebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    // Apply Hebbian bounds: weights typically range -0.5 to 0.5
    newWeight = std::clamp(newWeight, -0.5f, 0.5f);
    synapse->setWeight(newWeight);
}

const char* HebbianRule::getName() const {
    return "Hebbian";
}

void HebbianRule::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 0.1f);
}

float HebbianRule::getLearningRate() const {
    return pImpl->learningRate;
}

} // namespace nlm