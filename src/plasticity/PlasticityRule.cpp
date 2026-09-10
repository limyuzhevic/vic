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
    // Real Hebbian learning implementation with covariance rule
    // Δw = η * (⟨pre * post⟩ - ⟨pre⟩⟨post⟩)
    
    if (preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate firing rates from spike history
    float preRate = static_cast<float>(preSpikes.size()) / 100.0f;
    float postRate = static_cast<float>(postSpikes.size()) / 100.0f;
    
    // Calculate co-activity (correlation) - spikes within 5ms window
    size_t coactivity = 0;
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            if (std::abs(static_cast<float>(preTime - postTime)) < 5.0f) {
                ++coactivity;
            }
        }
    }
    
    // Normalized co-activity rate
    float prePostRate = coactivity / 100.0f;
    
    // Covariance rule: Δw = η * (coactivity_rate - pre_rate * post_rate)
    float delta = pImpl->learningRate * (prePostRate - preRate * postRate);
    
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
