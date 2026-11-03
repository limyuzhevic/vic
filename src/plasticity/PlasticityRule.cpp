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
    // Implement real Hebbian learning with STDP (Spike-Timing Dependent Plasticity)
    if (preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate STDP weight change based on spike timing
    float weightChange = 0.0f;
    
    // For each pre-post spike pair, calculate contribution
    for (Timestamp pre : preSpikes) {
        for (Timestamp post : postSpikes) {
            float timeDiff = post - pre;  // Post minus pre spike time
            
            if (std::abs(timeDiff) < 20.0) {  // Within 20ms window
                if (timeDiff > 0) {
                    // Post fires after pre: LTP (Long-Term Potentiation)
                    weightChange += pImpl->learningRate * std::exp(-std::abs(timeDiff) / 10.0f);
                } else {
                    // Pre fires after post: LTD (Long-Term Depression)
                    weightChange -= pImpl->learningRate * std::exp(-std::abs(timeDiff) / 10.0f);
                }
            }
        }
    }
    
    // Apply weight change with bounds
    applyWeightChange(synapse, weightChange);
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
