#include "Oja_Hebbian.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct OjaHebbian::Impl {
    float learningRate;
    float maxWeight;
    float minWeight;
    float stability;      // Stability parameter (lambda) for Oja's rule
    
    Impl() : learningRate(0.01f), maxWeight(1.0f), minWeight(-1.0f),
             stability(0.01f) {}
};

OjaHebbian::OjaHebbian() : pImpl(new Impl) {}

OjaHebbian::~OjaHebbian() = default;

void OjaHebbian::update(Synapse* synapse,
                       const std::vector<Timestamp>& preSpikes,
                       const std::vector<Timestamp>& postSpikes,
                       TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Oja's rule: Δw = η * (y * x - λ * w * y^2)
    // Where y is post-synaptic activity, x is pre-synaptic activity
    
    // Estimate activities from spike counts
    float preActivity = static_cast<float>(preSpikes.size());
    float postActivity = static_cast<float>(postSpikes.size());
    
    if (postActivity < 1e-6f) {
        return; // No post-synaptic activity
    }
    
    // Hebbian term (correlation)
    float hebbianTerm = learningRate * postActivity * preActivity;
    
    // Oja normalization term (prevents runaway weights)
    float postSquared = postActivity * postActivity;
    float normalizationTerm = pImpl->stability * synapse->getWeight() * postSquared;
    
    // Oja's rule update
    float delta = hebbianTerm - normalizationTerm;
    
    // Apply with bounds
    if (std::abs(delta) > 1e-6f) {
        applyWeightChange(synapse, delta);
    }
}

void OjaHebbian::setStability(float stability) {
    pImpl->stability = std::clamp(stability, 0.001f, 0.1f);
}

float OjaHebbian::getStability() const {
    return pImpl->stability;
}

} // namespace nlm
