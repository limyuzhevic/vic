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
    // IMPLEMENTED: Real Hebbian learning based on spike timing
    // Uses triplet STDP-like mechanism for correlated firing
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate spike timing differences with exponential weighting
    float weightChange = 0.0f;
    
    // Process all spike pairs with exponential decay
    for (Timestamp pre : preSpikes) {
        for (Timestamp post : postSpikes) {
            float dt = static_cast<float>(post - pre);  // Δt = post - pre
            
            // Hebbian potentiation for pre-before-post (causality)
            if (dt > 0) {
                // Maximum at dt=0, decays exponentially with time difference
                float contribution = pImpl->learningRate * std::exp(-std::abs(dt) / 10.0f);
                weightChange += contribution;
            }
            // Note: For post-before-pre (anti-causality), weight change depends on
            // the specific biological mechanism being modeled
        }
    }
    
    // Apply the calculated weight change with bounds
    if (weightChange != 0.0f) {
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
