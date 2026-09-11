#include "BCMRule.hpp"
#include "../../brain/Synapse.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct BCMRule::Impl {
    float learningRate;
    float maxWeight;
    float minWeight;
    float thetaPlus;      // Threshold for potentiation (BCM modulator)
    float thetaMinus;     // Threshold for depression
    float mu;             // BCM learning rate modulation
    
    Impl() : learningRate(0.01f), maxWeight(1.0f), minWeight(-1.0f),
             thetaPlus(0.5f), thetaMinus(0.2f), mu(0.1f) {}
};

BCMRule::BCMRule() : pImpl(new Impl) {}

BCMRule::~BCMRule() = default;

void BCMRule::update(Synapse* synapse,
                    const std::vector<Timestamp>& preSpikes,
                    const std::vector<Timestamp>& postSpikes,
                    TimestepDuration dt) {
    /*
     * Bienenstock-Cooper-Munro (BCM) learning rule
     * 
     * BCM implements a sliding threshold for synaptic modification:
     * - Potentiation occurs when post-synaptic activity exceeds threshold θ+
     * - Depression occurs when post-synaptic activity is below threshold θ-
     * - Threshold adapts based on recent post-synaptic activity (ξ+)
     * 
     * Mathematical formulation:
     * Δw = η * x * (y - θ(y))
     * 
     * Where:
     *   x = presynaptic activity
     *   y = postsynaptic activity (firing rate)
     *   θ(y) = sliding threshold = θ- + (ξ+ - θ-) * (y / (y + μ))
     *   η = learning rate
     * 
     * Biological inspiration:
     * - Cortical synaptic learning
     * - False synaptic elimination
     * - Adaptive threshold for synaptic change
     * - Reflects NMDA receptor dynamics with calcium-dependent plasticity
     * 
     * Key properties:
     * - Hebbian: Learning is cooperative (pre * post)
     * - Non-Hebbian: Threshold prevents unlimited potentiation
     * - Stabilizing: Self-regulation prevents runaway excitation
     */
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate postsynaptic activity (firing rate proxy)
    float postActivity = static_cast<float>(postSpikes.size()) / 100.0f;  // Normalize
    
    // Calculate sliding threshold (BCM rule)
    float xiPlus = pImpl->thetaPlus;  // Could be updated based on recent activity
    float theta = pImpl->thetaMinus + (xiPlus - pImpl->thetaMinus) * (postActivity / (postActivity + pImpl->mu));
    
    // Calculate weight change based on BCM rule
    float delta = pImpl->learningRate * postActivity * (postActivity - theta);
    
    // Apply with bounds
    if (std::abs(delta) > 1e-6f) {
        applyWeightChange(synapse, delta);
    }
}

void BCMRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* BCMRule::getName() const {
    return "BCM";
}

void BCMRule::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

float BCMRule::getLearningRate() const {
    return pImpl->learningRate;
}

void BCMRule::setThetaPlus(float theta) {
    pImpl->thetaPlus = std::clamp(theta, 0.0f, 1.0f);
}

float BCMRule::getThetaPlus() const {
    return pImpl->thetaPlus;
}

void BCMRule::setThetaMinus(float theta) {
    pImpl->thetaMinus = std::clamp(theta, 0.0f, 1.0f);
}

float BCMRule::getThetaMinus() const {
    return pImpl->thetaMinus;
}

} // namespace nlm
