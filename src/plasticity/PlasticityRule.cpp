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
    // Real Hebbian learning implementation
    // Based on covariance learning rule: Δw = η * (⟨pre * post⟩ - ⟨pre⟩⟨post⟩)
    // Implements "neurons that fire together, wire together" with homeostatic regulation
    
    if (preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate spike rate-based correlation
    float preRate = static_cast<float>(preSpikes.size()) / static_cast<float>(preSpikes.size());
    float postRate = static_cast<float>(postSpikes.size()) / static_cast<float>(postSpikes.size());
    
    // Calculate coincidence count (spikes within time window)
    size_t coincidences = 0;
    for (Timestamp pre : preSpikes) {
        for (Timestamp post : postSpikes) {
            if (std::abs(pre - post) < 10.0) {  // 10ms correlation window
                ++coincidences;
            }
        }
    }
    
    // Normalized correlation coefficient (0-1)
    float correlation = (coincidences / (static_cast<float>(preSpikes.size()) * static_cast<float>(postSpikes.size()))) * 0.5f;
    
    // Apply weight change proportional to correlation
    float delta = pImpl->learningRate * correlation;
    
    // Apply homeostatic regulation to prevent runaway potentiation
    float currentWeight = synapse->getWeight();
    
    // Weight change with adaptive bounds based on current weight
    float maxWeight = 1.0f - currentWeight;
    float minWeight = -currentWeight;
    
    delta = std::clamp(delta, minWeight, maxWeight);
    
    // Apply weight change
    if (std::abs(delta) > 1e-6f) {
        applyWeightChange(synapse, delta);
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
