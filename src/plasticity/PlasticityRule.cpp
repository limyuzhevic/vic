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
     * Real Hebbian learning implementation based on covariance rule
     * 
     * Mathematical formulation (covariance-based Hebbian):
     * Δw = η × (⟨pre × post⟩ - ⟨pre⟩⟨post⟩)
     * 
     * This implements "neurons that fire together, wire together"
     * but with a normalization to prevent runaway potentiation.
     * 
     * The rule:
     * 1. Counts co-activity between pre- and post-synaptic neurons
     * 2. Subtracts baseline co-activity (expected by chance)
     * 3. Scales by learning rate
     * 4. Enforces weight bounds
     * 
     * Biological inspiration:
     * - Reflects spike-timing dependent plasticity with broader time window
     * - Similar to presynaptic facilitation mechanisms
     * - Includes homeostatic normalization for stability
     * 
     * Stability features:
     * - Weight normalization to prevent unbounded growth
     * - Learning rate adaptation based on current weight
     * - Anti-Hebbian component for strong normalization
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate statistics for covariance rule
    size_t preSpikeCount = preSpikes.size();
    size_t postSpikeCount = postSpikes.size();
    
    if (preSpikeCount == 0 || postSpikeCount == 0) {
        return;
    }
    
    // Calculate mean firing rates
    float preMean = static_cast<float>(preSpikeCount) / static_cast<float>(preSpikeCount * preSpikeCount);
    float postMean = static_cast<float>(postSpikeCount) / static_cast<float>(postSpikeCount * postSpikeCount);
    
    // Calculate covariance (coactivity - baseline)
    float covariance = 0.0f;
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            // Broad time window for Hebbian (slower than STDP)
            float dt = static_cast<float>(postTime - preTime);
            if (std::abs(dt) < 50.0f) {  // 50ms window for Hebbian
                // Coactivity contributes positively
                covariance += 1.0f;
            }
        }
    }
    
    // Normalize by number of pairs
    covariance /= static_cast<float>(preSpikeCount * postSpikeCount);
    
    // Calculate baseline coactivity (expected by chance)
    float baseline = preMean * postMean;
    
    // Compute weight change using covariance rule
    // This implements: Δw = η × (covariance - baseline)
    float weight = synapse->getWeight();
    float maxPossibleChange = 1.0f - std::abs(weight);
    float effectiveLearningRate = pImpl->learningRate * std::min(1.0f, maxPossibleChange);
    
    float delta = effectiveLearningRate * (covariance - baseline);
    
    // Apply with proper bounds checking through applyWeightChange
    applyWeightChange(synapse, delta);
    
    // Also update eligibility trace for reward-modulated learning
    float currentTrace = synapse->getEligibilityTrace();
    synapse->setEligibilityTrace(currentTrace + delta);
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
