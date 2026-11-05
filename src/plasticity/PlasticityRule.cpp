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
    // Real Hebbian learning: correlated pre-post spiking strengthens synapses
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate spike correlation with temporal precision
    float correlation = 0.0f;
    size_t correlationCount = 0;
    
    // For each pre-synaptic spike, look for post-synaptic spikes within time window
    for (Timestamp preSpike : preSpikes) {
        for (Timestamp postSpike : postSpikes) {
            float timeDiff = std::abs(preSpike - postSpike);
            
            // Consider spikes correlated if within Hebbian time window (e.g., 10ms)
            if (timeDiff < 10.0) {  // 10ms Hebbian window
                correlation += 1.0f - (timeDiff / 10.0f);  // Stronger correlation for closer spikes
                ++correlationCount;
            }
        }
    }
    
    // Apply weight change based on correlation
    if (correlationCount > 0) {
        float learningEffect = correlation / correlationCount;  // Average correlation
        applyWeightChange(synapse, pImpl->learningRate * learningEffect);
    }
}

void HebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    // Real Hebbian weight change with synaptic constraints
    if (!synapse) return;
    
    // Get current weight and apply Hebbian update
    float currentWeight = synapse->getWeight();
    
    // Hebbian learning can potentiate (strengthen) synapses
    // Weight change is proportional to synaptic activity correlation
    float newWeight = currentWeight + delta;
    
    // Apply synaptic constraints:
    // - Maximum weight limit
    // - Minimum weight limit
    // - Soft bound to prevent runaway excitation
    const float maxWeight = 1.0f;
    const float minWeight = 0.0f;
    
    // Soft bound: prevent individual weights from saturating
    // Creates competition between synapses for limited resources
    if (newWeight > maxWeight) {
        float excess = newWeight - maxWeight;
        newWeight = maxWeight - excess * 0.1f;  // Soft bound
    }
    
    // Ensure weight is within bounds
    newWeight = std::clamp(newWeight, minWeight, maxWeight);
    
    // Apply homeostatic scaling based on overall network activity
    // This prevents runaway excitation in Hebbian networks
    newWeight = synapse->applyHomeostaticScaling(newWeight);
    
    // Update synapse weight
    synapse->setWeight(newWeight);
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
