#include "../plasticity/BCMRule.hpp"
#include "../../brain/Synapse.hpp"
#include <algorithm>

namespace nlm {

struct BCMRule::Impl {
    float theta;           // Sliding threshold
    float learningRate;
    float tau;             // Time constant for threshold adaptation
    float targetActivity;  // Target average activity
    float currentThreshold;
    
    Impl() : theta(0.5f), learningRate(0.01f), tau(1000.0f),
             targetActivity(0.1f), currentThreshold(0.5f) {}
};

BCMRule::BCMRule() : pImpl(new Impl) {}

BCMRule::~BCMRule() = default;

void BCMRule::update(Synapse* synapse,
                    const std::vector<Timestamp>& preSpikes,
                    const std::vector<Timestamp>& postSpikes,
                    TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate average postsynaptic activity
    size_t preSpikesCount = preSpikes.size();
    float activity = (preSpikesCount > 0) ? static_cast<float>(preSpikesCount) / (preSpikesCount + 1) : 0.0f;
    
    // Update threshold based on activity
    updateThreshold(activity);
    
    // Compute weight change based on product of activity and threshold difference
    // BCM rule: Δw = η * φ * (y - θ) where φ is post-synaptic activity, y is output
    float activityDiff = activity - pImpl->currentThreshold;
    
    if (std::abs(activityDiff) > 1e-6f) {
        // Apply weight change
        float delta = pImpl->learningRate * activity * activityDiff;
        applyWeightChange(synapse, delta);
    }
}

void BCMRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    // Apply bounds for BCM
    newWeight = std::clamp(newWeight, -1.0f, 1.0f);
    synapse->setWeight(newWeight);
}

const char* BCMRule::getName() const {
    return "BCM";
}

void BCMRule::setTheta(float theta) {
    pImpl->theta = std::clamp(theta, 0.01f, 1.0f);
}

float BCMRule::getTheta() const {
    return pImpl->theta;
}

void BCMRule::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 0.1f);
}

float BCMRule::getLearningRate() const {
    return pImpl->learningRate;
}

void BCMRule::setTau(float tau) {
    pImpl->tau = std::max(0.1f, tau);
}

float BCMRule::getTau() const {
    return pImpl->tau;
}

void BCMRule::updateThreshold(float activity) {
    // Update threshold based on activity with time constant
    float target = pImpl->theta * activity;
    pImpl->currentThreshold += (target - pImpl->currentThreshold) * 
                               (1.0f / pImpl->tau);
    
    // Keep threshold within reasonable bounds
    pImpl->currentThreshold = std::clamp(pImpl->currentThreshold, 0.01f, 1.0f);
}

float BCMRule::getCurrentThreshold() const {
    return pImpl->currentThreshold;
}

} // namespace nlm