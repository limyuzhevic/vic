#include "HebbianPlasticity.hpp"
#include <algorithm>

namespace nlm {

struct HebbianPlasticity::Impl {
    float learning_rate;     // Learning rate (typically 0.001)
    float threshold;         // Activation threshold for Hebbian learning
    float max_weight_change; // Maximum weight change per step
    
    // Correlation history for Hebbian learning
    std::unordered_map<NeuronId, float> correlation_history;
    
    Impl() : learning_rate(0.001f), threshold(0.1f), max_weight_change(0.01f) {}
};

HebbianPlasticity::HebbianPlasticity(float learning_rate, float threshold)
    : pImpl(new Impl) {
    if (learning_rate > 0) pImpl->learning_rate = learning_rate;
    if (threshold > 0) pImpl->threshold = threshold;
}

HebbianPlasticity::~HebbianPlasticity() = default;

void HebbianPlasticity::applyPlasticity(Neuron* pre, Neuron* post, float dt) {
    if (!pre || !post || dt <= 0.0f) return;
    
    // Get neuron states
    float pre_activity = pre->getMembranePotential();
    float post_activity = post->getMembranePotential();
    
    // Simplified Hebbian rule: Δw = learning_rate * pre * post * dt
    float delta_w = pImpl->learning_rate * pre_activity * post_activity * static_cast<float>(dt);
    
    // Limit weight change
    delta_w = std::max(-pImpl->max_weight_change, std::min(delta_w, pImpl->max_weight_change));
    
    // Update correlation history
    NeuronId preId = pre->getId();
    NeuronId postId = post->getId();
    
    pImpl->correlation_history[preId] += pre_activity * post_activity * dt;
    
    // Keep history manageable
    if (pImpl->correlation_history[preId] > 10.0f) {
        pImpl->correlation_history[preId] = 10.0f;
    }
    
    // Apply learning if correlation exceeds threshold
    if (pImpl->correlation_history[preId] > pImpl->threshold) {
        // This would update the actual synapse in a full implementation
        // For now, we track the potential for learning
    }
}

void HebbianPlasticity::recordPreSynapseSpike(Neuron* neuron) {
    // Hebbian learning doesn't typically require spike history recording
    // but we track correlation
}

void HebbianPlasticity::recordPostSynapseSpike(Neuron* neuron) {
    // Hebbian learning doesn't typically require spike history recording
    // but we track correlation
}

void HebbianPlasticity::reset() {
    pImpl->correlation_history.clear();
}

void HebbianPlasticity::setLearningRate(float rate) {
    if (rate > 0) pImpl->learning_rate = rate;
}

float HebbianPlasticity::getLearningRate() const {
    return pImpl->learning_rate;
}

void HebbianPlasticity::setThreshold(float threshold) {
    if (threshold > 0) pImpl->threshold = threshold;
}

float HebbianPlasticity::getThreshold() const {
    return pImpl->threshold;
}

} // namespace nlm