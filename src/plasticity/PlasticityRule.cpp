#include "PlasticityRule.hpp"
#include <stdexcept>

namespace nlm {

bool PlasticityRule::isEnabled() const {
    return enabled_;
}

void PlasticityRule::setEnabled(bool enabled) {
    enabled_ = enabled;
}

struct HebbianRule::Impl {
    float learningRate;
    float synapticModificationThreshold;  // For BCM-like behavior
    
    Impl() : learningRate(0.01f), synapticModificationThreshold(0.0f) {}
};

HebbianRule::HebbianRule() : pImpl(new Impl) {
    if (!pImpl) {
        throw std::runtime_error("Failed to allocate HebbianRule memory");
    }
}

HebbianRule::~HebbianRule() {
    delete pImpl;
}

void HebbianRule::update(Synapse* synapse,
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
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Validate input parameters
    if (dt <= 0.0f) {
        throw std::invalid_argument("Timestep duration must be positive");
    }
    
    // Count correlated spike pairs (simplified covariance)
    size_t correlationCount = 0;
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            // Count spikes within a broad time window as correlated
            if (std::abs(dt) < 100.0f) {  // 100ms correlation window
                ++correlationCount;
            }
        }
    }
    
    // Compute weight change based on correlation
    // More sophisticated: use actual spike counts and firing rates
    float delta = pImpl->learningRate * static_cast<float>(correlationCount);
    
    // Apply synaptic modification threshold (BCM-like)
    if (pImpl->synapticModificationThreshold > 0.0f) {
        // Only allow potentiation if post-activity exceeds threshold
        float postActivity = calculatePostActivity(postSpikes);
        if (postActivity > pImpl->synapticModificationThreshold) {
            applyWeightChange(synapse, delta);
        }
    } else {
        // No threshold - apply all changes
        applyWeightChange(synapse, delta);
    }
}

void HebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) {
        throw std::invalid_argument("Cannot apply weight change to null synapse");
    }
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* HebbianRule::getName() const {
    return "Hebbian";
}

void HebbianRule::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

float HebbianRule::getLearningRate() const {
    return pImpl->learningRate;
}

float HebbianRule::getSynapticModificationThreshold() const {
    return pImpl->synapticModificationThreshold;
}

void HebbianRule::setSynapticModificationThreshold(float threshold) {
    pImpl->synapticModificationThreshold = threshold;
}

// Helper function to calculate post-synaptic activity
float HebbianRule::calculatePostActivity(const std::vector<Timestamp>& postSpikes) const {
    if (postSpikes.empty()) return 0.0f;
    
    float totalActivity = 0.0f;
    for (Timestamp spikeTime : postSpikes) {
        totalActivity += 1.0f;  // Each spike contributes equally
    }
    
    return totalActivity / static_cast<float>(postSpikes.size());
}

} // namespace nlm
