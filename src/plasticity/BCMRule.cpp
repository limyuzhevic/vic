#include "BCMRule.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>

namespace nlm {

struct BCMRule::Impl {
    float theta;           // Sliding threshold (BCM rule)
    float learningRate;    // Learning rate
    float tau;             // Adaptation time constant
    float averagePostActivity; // Running average of post-synaptic activity
    float maxTheta;        // Maximum threshold value
    float minTheta;        // Minimum threshold value
    
    Impl() : theta(0.5f), learningRate(0.01f), tau(1000.0f),
             averagePostActivity(0.0f), maxTheta(1.0f), minTheta(0.01f) {}
};

BCMRule::BCMRule() : pImpl(new Impl) {}

BCMRule::~BCMRule() = default;

void BCMRule::update(Synapse* synapse,
                    const std::vector<Timestamp>& preSpikes,
                    const std::vector<Timestamp>& postSpikes,
                    TimestepDuration dt) {
    // TODO PHASE 2: Implement real BCM rule
    // PLACEHOLDER: BCM implements sliding threshold plasticity
    // Learning depends on post-synaptic activity relative to theta
    
    if (preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate post-synaptic activity (simplified)
    float postActivity = std::min(1.0f, static_cast<float>(postSpikes.size()) / 10.0f);
    
    // Update running average of post-synaptic activity
    pImpl->averagePostActivity = 0.99f * pImpl->averagePostActivity + 0.01f * postActivity;
    
    // Update sliding threshold theta based on average activity
    float targetTheta = std::clamp(pImpl->averagePostActivity, pImpl->minTheta, pImpl->maxTheta);
    pImpl->theta += (targetTheta - pImpl->theta) * (dt / pImpl->tau);
    
    // Apply BCM weight change
    float weightChange = 0.0f;
    if (postActivity > pImpl->theta) {
        // When post-synaptic activity exceeds threshold, potentiate
        weightChange = pImpl->learningRate * (postActivity - pImpl->theta);
    } else {
        // When below threshold, depress
        weightChange = -pImpl->learningRate * std::abs(postActivity - pImpl->theta) * 0.5f;
    }
    
    // Apply weight change
    if (std::abs(weightChange) > 1e-6f) {
        applyWeightChange(synapse, weightChange);
    }
}

void BCMRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Clamp weight to reasonable bounds
    float newWeight = synapse->getWeight() + delta;
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
    pImpl->tau = std::clamp(tau, 10.0f, 10000.0f);
}

float BCMRule::getTau() const {
    return pImpl->tau;
}

} // namespace nlm
