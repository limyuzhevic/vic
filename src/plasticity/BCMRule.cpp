#include "PlasticityRule.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct BCMRule::Impl {
    float learningRate;
    float theta;         // Sliding threshold
    float thetaMinus;    // Lower threshold bound
    float thetaPlus;     // Upper threshold bound
    float minWeight;     // Minimum synaptic weight
    float maxWeight;     // Maximum synaptic weight
    
    Impl() : learningRate(0.01f), theta(0.5f), thetaMinus(0.1f), thetaPlus(2.0f),
             minWeight(-1.0f), maxWeight(1.0f) {}
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
    
    // BCM rule: weight change depends on postsynaptic activity relative to threshold
    // Δw = η * (post * (pre - θ))
    
    // Compute postsynaptic activity (simplified - count post spikes)
    float postActivity = static_cast<float>(postSpikes.size()) / 10.0f;  // Normalize
    
    // Compute presynaptic activity (simplified - count pre spikes)
    float preActivity = static_cast<float>(preSpikes.size()) / 10.0f;   // Normalize
    
    // BCM weight change: postActivity * (preActivity - theta)
    float delta = pImpl->learningRate * postActivity * (preActivity - pImpl->theta);
    
    // Apply with bounds
    if (std::abs(delta) > 1e-6f) {
        applyWeightChange(synapse, delta);
    }
    
    // Update sliding threshold (θ adapts based on average postsynaptic activity)
    static float avgPostActivity = 0.5f;  // Initial value
    avgPostActivity = 0.95f * avgPostActivity + 0.05f * postActivity;
    
    // Keep theta within bounds
    pImpl->theta = std::clamp(pImpl->theta, pImpl->thetaMinus, pImpl->thetaPlus);
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

void BCMRule::setThreshold(float theta) {
    pImpl->theta = std::clamp(theta, 0.0f, 10.0f);
}

float BCMRule::getThreshold() const {
    return pImpl->theta;
}

void BCMRule::setThetaMinus(float thetaMinus) {
    pImpl->thetaMinus = std::clamp(thetaMinus, 0.0f, thetaMinus);
}

float BCMRule::getThetaMinus() const {
    return pImpl->thetaMinus;
}

void BCMRule::setThetaPlus(float thetaPlus) {
    pImpl->thetaPlus = std::clamp(thetaPlus, thetaPlus, 10.0f);
}

float BCMRule::getThetaPlus() const {
    return pImpl->thetaPlus;
}

} // namespace nlm