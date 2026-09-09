#include "PlasticityRule.hpp"
#include <cmath>

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
    // Implement real Hebbian learning based on spike timing
    // Delta w ∝ pre_spike × post_spike (correlational learning)
    
    if (preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate spike correlation with temporal precision
    float correlation = 0.0f;
    for (Timestamp pre : preSpikes) {
        for (Timestamp post : postSpikes) {
            // Exponential decay of correlation with time difference
            float timeDiff = static_cast<float>(std::abs(post - pre));
            float temporalFactor = std::exp(-timeDiff / 20.0f);  // 20ms time constant
            correlation += temporalFactor;
        }
    }
    
    // Normalize by total possible correlations
    float maxCorrelations = static_cast<float>(preSpikes.size()) * static_cast<float>(postSpikes.size());
    if (maxCorrelations > 0.0f) {
        correlation /= maxCorrelations;
    }
    
    // Apply weight change based on correlation
    if (correlation > 0.0f) {
        float weightChange = pImpl->learningRate * correlation * dt;
        applyWeightChange(synapse, weightChange);
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
