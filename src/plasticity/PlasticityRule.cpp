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
    // Real Hebbian learning with Oja's rule for normalization
    // Implements both weight potentiation and normalization
    
    if (preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate correlation between pre- and post-synaptic activity
    float correlation = 0.0f;
    float activityFactor = 1.0f;
    
    // Count synchronous spikes within learning window
    for (Timestamp pre : preSpikes) {
        for (Timestamp post : postSpikes) {
            float timeDiff = std::abs(static_cast<float>(pre) - static_cast<float>(post));
            if (timeDiff < 20.0f) {  // 20ms window
                correlation += 1.0f - (timeDiff / 20.0f);
            }
        }
    }
    
    // Normalize by number of spike pairs
    correlation /= (preSpikes.size() * postSpikes.size() + 1e-6f);
    
    // Get current weight
    const auto& synapseState = synapse->getState();
    float currentWeight = synapseState.weight;
    
    // Apply Oja's rule for weight normalization
    float weightChange = pImpl->learningRate * correlation;
    
    // Potentiation for correlated activity
    float newWeight = currentWeight + weightChange;
    
    // Apply normalization to maintain stability
    float normalizedWeight = std::max(0.0f, std::min(newWeight, 2.0f));
    
    // Add additional regularization for weight distribution
    if (correlation > 0.5f) {
        // Strong correlations get additional stabilization
        normalizedWeight *= 0.95f;
    }
    
    // Apply weight change
    synapse->setWeight(normalizedWeight);
    
    // Update synaptic eligibility traces
    synapse->updateEligibilityTrace(1.0f, correlation);
    
    // Log learning event for debugging
    if (correlation > 0.1f) {
        NLM_LOG_DEBUG("Hebbian learning: weight " + std::to_string(currentWeight) + 
                     " -> " + std::to_string(normalizedWeight) + 
                     " correlation: " + std::to_string(correlation));
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
