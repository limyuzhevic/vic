// Implementation of BCMRule (Bienenstock-Cooper-Munro)
// BCM plasticity: homeostatic scaling based on average postsynaptic activity

#include "BCMRule.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>

namespace nlm {

struct BCMRule::Impl {
    float learningRate;      // Learning rate
    float hebbianThreshold;  // BCM threshold for potentiation
    float homeostaticTarget; // Target synaptic weight
    float minWeight;         // Minimum synaptic weight
    float maxWeight;         // Maximum synaptic weight
    float eligibilityTrace;  // Current eligibility trace
    
    Impl() : learningRate(0.001f), hebbianThreshold(0.5f), homeostaticTarget(0.1f),
             minWeight(-0.5f), maxWeight(1.0f), eligibilityTrace(0.0f) {}
};

BCMRule::BCMRule() : pImpl(new Impl) {}

BCMRule::~BCMRule() = default;

void BCMRule::update(Synapse* synapse,
                    const std::vector<Timestamp>& preSpikes,
                    const std::vector<Timestamp>& postSpikes,
                    TimestepDuration dt) {
    /*
     * BCM (Bienenstock-Cooper-Munro) plasticity implementation
     * 
     * Mathematical formulation:
     * Δw = η * (x_post - θ) * x_pre * φ(w)
     * 
     * Where:
     *   η = learningRate (learning parameter)
     *   x_post = postsynaptic activity (average firing rate)
     *   θ = hebbianThreshold (BCM threshold - increases with x_post)
     *   x_pre = presynaptic activity
     *   φ(w) = sliding threshold function (usually w^2 / (w^2 + κ^2))
     * 
     * Biological inspiration:
     *   - BCM theory of synaptic modification (Bienenstock et al., 1982)
     *   - Homeostatic scaling to maintain stable firing rates
     *   - Synaptic competition and normalization
     *   - Memory allocation during development
     *   
     * Key features:
     *   - Threshold θ slides with average postsynaptic activity
     *   - Potentiation when x_post > θ
     *   - Depression when x_post < θ
     *   - Maintains synaptic balance
     *   
     * This implements homeostatic plasticity that stabilizes network activity
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate presynaptic and postsynaptic activity
    float preActivity = static_cast<float>(preSpikes.size());
    float postActivity = static_cast<float>(postSpikes.size());
    
    // Update sliding threshold θ = θ0 * x_post
    pImpl->hebbianThreshold = 0.1f * postActivity;  // Simplified: θ0 = 0.1
    
    // Calculate BCM weight change
    float activityDifference = postActivity - pImpl->hebbianThreshold;
    
    // Apply BCM rule with sliding threshold function
    float phi = pImpl->eligibilityTrace;  // Simplified: φ(w) = w
    float delta = pImpl->learningRate * activityDifference * preActivity * phi;
    
    // Apply with homeostatic target
    float currentWeight = synapse->getWeight();
    float targetEffect = pImpl->homeostaticTarget - currentWeight;
    delta += 0.1f * targetEffect;  // Homeostatic component
    
    // Apply weight change
    if (std::abs(delta) > 1e-6f) {
        applyWeightChange(synapse, delta);
    }
    
    // Update eligibility trace
    pImpl->eligibilityTrace = currentWeight;
}

void BCMRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Apply BCM weight change with bounds
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* BCMRule::getName() const {
    return "BCM";
}

void BCMRule::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0001f, 0.01f);
}

float BCMRule::getLearningRate() const {
    return pImpl->learningRate;
}

void BCMRule::setHebbianThreshold(float threshold) {
    pImpl->hebbianThreshold = std::clamp(threshold, 0.0f, 1.0f);
}

float BCMRule::getHebbianThreshold() const {
    return pImpl->hebbianThreshold;
}

void BCMRule::setHomeostaticTarget(float target) {
    pImpl->homeostaticTarget = std::clamp(target, -0.5f, 1.0f);
}

float BCMRule::getHomeostaticTarget() const {
    return pImpl->homeostaticTarget;
}

} // namespace nlm