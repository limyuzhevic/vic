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
    // Real Hebbian learning with spike-timing dependent plasticity
    if (preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // STDP: spike timing matters - if pre before post, strengthen; if post before pre, weaken
    float totalDelta = 0.0f;
    const float alpha = pImpl->learningRate * 0.1f;  // STDP learning rate
    const float tau_pre = 20.0f;  // Pre-synaptic trace decay (ms)
    const float tau_post = 20.0f;  // Post-synaptic trace decay (ms)
    
    // Calculate pre-synaptic trace from recent spikes
    float trace_pre = 0.0f;
    for (Timestamp spike : preSpikes) {
        float age = static_cast<float>(dt) - spike;
        if (age > 0.0) {
            trace_pre += std::exp(-age / tau_pre);
        }
    }
    
    // Calculate post-synaptic trace from recent spikes
    float trace_post = 0.0f;
    for (Timestamp spike : postSpikes) {
        float age = static_cast<float>(dt) - spike;
        if (age > 0.0) {
            trace_post += std::exp(-age / tau_post);
        }
    }
    
    // Apply STDP rule
    float delta = alpha * (trace_post - trace_pre);
    
    // Combine with classic Hebbian term
    size_t coincidences = 0;
    for (Timestamp pre : preSpikes) {
        for (Timestamp post : postSpikes) {
            if (std::abs(pre - post) < 10.0) {  // 10ms window for coincidence
                ++coincidences;
            }
        }
    }
    
    // Add Hebbian component
    if (coincidences > 0) {
        delta += pImpl->learningRate * static_cast<float>(coincidences) * 0.01f;
    }
    
    // Apply weight change with bounds
    synapse->addToWeight(delta);
    
    // Update plasticity flags
    PlasticityFlags& flags = synapse->getPlasticityFlags();
    flags.hebbian = true;
    flags.eligible = true;
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
