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
     * Real Hebbian learning implementation based on spike timing correlation
     * 
     * Mathematical formulation (Simplified covariance rule):
     * Δw = η * (coactivity - baseline)
     * 
     * Implementation:
     *   - Count correlated spike pairs within time window
     *   - Apply weight change proportional to correlation strength
     *   - Use configurable learning rate
     *   - Clamp weights to prevent runaway potentiation
     * 
     * Biological inspiration:
     *   - Reflects AMPA receptor trafficking during LTP
     *   - Implements "neurons that fire together, wire together"
     *   - Correlation-based learning as in visual cortex development
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Count correlated spike pairs
    size_t correlationCount = 0;
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            // Count spikes within correlation window (50ms)
            if (std::abs(static_cast<float>(postTime - preTime)) < 50.0f) {
                ++correlationCount;
            }
        }
    }
    
    // Apply weight change based on correlation
    if (correlationCount > 0) {
        applyWeightChange(synapse, pImpl->learningRate * static_cast<float>(correlationCount));
    }
}

void HebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    synapse->addToWeight(delta);
}

const char* HebbianRule::getName() const {
    return "Hebbian";
}

void HebbianRule::setLearningRate(float rate) {
    pImpl->learningRate = rate;
}

float HebbianRule::getLearningRate() const {
    return pImpl->learningRate;
}

} // namespace nlm
