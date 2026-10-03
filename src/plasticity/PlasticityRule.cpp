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
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Real Hebbian learning: strengthen synapses when pre and post fire together
    // Calculate spike timing correlations
    float totalWeightChange = 0.0f;
    
    for (Timestamp pre : preSpikes) {
        for (Timestamp post : postSpikes) {
            float dt = static_cast<float>(post - pre);  // Δt in ms
            
            if (std::abs(dt) < 20.0f) {  // Within 20ms window
                // Hebbian plasticity: Δw = η * post * pre
                // where η is learning rate, post is postsynaptic spike, pre is presynaptic spike
                float weightChange = pImpl->learningRate * 1.0f * 1.0f;  // Binary spikes
                
                if (dt > 0) {
                    // Post after pre: LTP (long-term potentiation)
                    weightChange *= 1.5f;  // Strengthen more for causal order
                } else if (dt < 0) {
                    // Post before pre: LTD (long-term depression) or minimal change
                    weightChange *= 0.5f;  // Weaken for anti-causal order
                }
                
                totalWeightChange += weightChange;
            }
        }
    }
    
    // Apply accumulated weight change
    if (std::abs(totalWeightChange) > 1e-6f) {
        applyWeightChange(synapse, totalWeightChange);
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
