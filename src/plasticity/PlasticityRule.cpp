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
    // TODO PHASE 2: Implement real Hebbian learning
    // PLACEHOLDER: Simple correlated firing increases weight
    
    if (preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Count coincident spikes (simplified)
    size_t coincidences = 0;
    for (Timestamp pre : preSpikes) {
        for (Timestamp post : postSpikes) {
            if (std::abs(pre - post) < 10.0) {  // 10ms window
                ++coincidences;
            }
        }
    }
    
    // Apply weight change proportional to coincidences
    if (coincidences > 0) {
        applyWeightChange(synapse, pImpl->learningRate * static_cast<float>(coincidences));
    }
}

void HebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) {
        NLM_LOG_WARNING("Attempting to apply weight change to null synapse");
        return;
    }
    
    float currentWeight = synapse->getWeight();
    float newWeight = currentWeight + delta;
    
    // Apply biophysical constraints
    newWeight = std::clamp(newWeight, -1.0f, 1.0f);  // Synaptic weight bounds
    
    synapse->setWeight(newWeight);
    
    // Log Hebbian learning for debugging
    if (std::abs(delta) > 1e-6f) {
        NLM_LOG_DEBUG("Hebbian: synapse weight " + std::to_string(currentWeight) +
                     " → " + std::to_string(newWeight) +
                     " (Δ=" + std::to_string(delta) + ")");
    }
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
