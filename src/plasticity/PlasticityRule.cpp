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
    if (!synapse) {
        NLM_LOG_ERROR("HebbianRule::update: synapse is null");
        return;
    }
    
    if (preSpikes.empty() || postSpikes.empty()) {
        return;  // No activity, no learning
    }
    
    if (!enabled_) {
        return;  // Learning disabled
    }
    
    // Real Hebbian learning based on spike timing
    // Implements the classic Hebb's rule: "Neurons that fire together wire together"
    // Weight change: Δw ∝ x_pre * y_post
    
    // Get synaptic weight bounds
    float currentWeight = synapse->getWeight();
    float minWeight = currentWeight > 0 ? -1.0f : -0.5f;
    float maxWeight = currentWeight > 0 ? 1.0f : 0.5f;
    
    // Calculate spike timing differences
    float totalPositiveDelta = 0.0f;
    float totalNegativeDelta = 0.0f;
    
    for (Timestamp preSpike : preSpikes) {
        for (Timestamp postSpike : postSpikes) {
            float delta_t = postSpike - preSpike;  // Post minus pre spike time
            
            if (delta_t < 0 && delta_t > -20.0f) {
                // Pre before post (causal)
                // Potentiation: weight increases
                totalPositiveDelta += std::exp(-std::abs(delta_t) / 10.0f);  // Gaussian decay
            } else if (delta_t > 0 && delta_t < 20.0f) {
                // Post before pre (anti-causal)
                // Depression: weight decreases
                totalNegativeDelta += std::exp(-std::abs(delta_t) / 10.0f);  // Gaussian decay
            }
        }
    }
    
    // Apply Hebbian weight changes
    float learningRate = pImpl->learningRate;
    
    // Potentiation from causal spikes
    float positiveChange = learningRate * totalPositiveDelta;
    float newWeight = currentWeight + positiveChange;
    
    // Depression from anti-causal spikes
    float negativeChange = -learningRate * totalNegativeDelta;
    newWeight += negativeChange;
    
    // Clamp weight to biologically realistic bounds
    newWeight = std::clamp(newWeight, minWeight, maxWeight);
    
    // Apply weight change
    synapse->setWeight(newWeight);
    
    // Record learning event for statistics
    NLM_LOG_DEBUG("HebbianRule: Synapse " + std::to_string(synapse->getId()) + 
                 " weight changed from " + std::to_string(currentWeight) + " to " + 
                 std::to_string(newWeight) + " (Δ=" + std::to_string(newWeight - currentWeight) + ")");
}

void HebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) {
        NLM_LOG_ERROR("HebbianRule::applyWeightChange: synapse is null");
        return;
    }
    
    // Apply weight change with bounds checking
    float currentWeight = synapse->getWeight();
    float newWeight = currentWeight + delta;
    
    // Determine appropriate bounds based on current weight sign
    float minWeight = currentWeight > 0 ? -1.0f : -0.5f;
    float maxWeight = currentWeight > 0 ? 1.0f : 0.5f;
    
    // Clamp to biologically realistic bounds
    newWeight = std::clamp(newWeight, minWeight, maxWeight);
    
    synapse->setWeight(newWeight);
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
