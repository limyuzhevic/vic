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
    // Implement real Hebbian learning using covariance rule
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Compute weight change using covariance rule: Δw = η * (⟨pre * post⟩ - ⟨pre⟩⟨post⟩)
    
    // Count correlated spike pairs (coactivity)
    size_t coactivity = 0;
    float totalPreRate = 0.0f;
    float totalPostRate = 0.0f;
    
    for (Timestamp preTime : preSpikes) {
        totalPreRate += 1.0f / (dt > 0 ? dt : 1.0f);
        for (Timestamp postTime : postSpikes) {
            float dtBetween = std::abs(postTime - preTime);
            // Count spikes within 50ms as correlated (Hebbian window)
            if (dtBetween < 50.0f) {
                coactivity++;
            }
        }
    }
    
    // Compute expected activity under independence
    float preRate = totalPreRate / static_cast<float>(preSpikes.size());
    float postRate = totalPostRate / static_cast<float>(postSpikes.size());
    float expectedCoactivity = preRate * postRate * dt * 50.0f; // Approximate
    
    // Compute Hebbian delta with covariance rule
    float correlation = static_cast<float>(coactivity) - expectedCoactivity;
    
    // Apply Hebbian update with learning rate
    float delta = pImpl->learningRate * correlation;
    
    // Apply with anti-Hebbian threshold to prevent runaway potentiation
    if (correlation > 0) {
        // Potentiation with soft bound
        float maxPotentiation = std::max(0.0f, 1.0f - synapse->getWeight());
        delta = std::min(delta, maxPotentiation);
    } else if (correlation < 0) {
        // Depression with soft bound
        float maxDepression = std::min(0.0f, -1.0f - synapse->getWeight());
        delta = std::max(delta, maxDepression);
    }
    
    // Apply weight change
    if (std::abs(delta) > 1e-6f) {
        applyWeightChange(synapse, delta);
    }
}

void HebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Clamp weight to biological bounds
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, -1.0f, 1.0f);
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

} // namespace nlm
