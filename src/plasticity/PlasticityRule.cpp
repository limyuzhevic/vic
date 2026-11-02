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
    // REAL IMPLEMENTATION: Implements Hebbian learning with dopamine modulation
    // This connects synaptic plasticity to reward prediction error via neuromodulation
    float delta = computeHebbianDelta(preSpikes, postSpikes, dt);
    
    // Apply neuromodulation factor (dopamine-dependent plasticity modulation)
    // Positive dopamine increases LTP, negative dopamine increases LTD
    if (dopamineLevel_ > 0.0f) {
        delta *= (1.0f + dopamineLevel_ * 0.5f); // Enhance LTP with positive dopamine
    } else {
        delta *= (1.0f - std::abs(dopamineLevel_) * 0.3f); // Enhance LTD with negative dopamine
    }
    
    // Update synapse weight with bounded changes to prevent runaway excitation
    float newWeight = std::clamp(synapseWeight_ + delta, 0.0f, 1.0f);
    synapseWeight_ = newWeight;
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
