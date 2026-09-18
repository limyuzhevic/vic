#include "STDPPlasticity.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct STDPPlasticity::Impl {
    float learning_rate;     // Learning rate (typically 0.001)
    float tau_plus;          // Time constant for LTP (ms)
    float tau_minus;         // Time constant for LTD (ms)
    
    // Spike history for STDP
    std::unordered_map<NeuronId, std::vector<std::pair<Timestamp, bool>>> spike_history;
    
    // Helper functions for STDP
    float computeLTP(float dt) {
        return learning_rate * std::exp(-dt / tau_plus);
    }
    
    float computeLTD(float dt) {
        return -learning_rate * std::exp(-dt / tau_minus);
    }
    
    Impl() : learning_rate(0.001f), tau_plus(20.0f), tau_minus(20.0f) {}
};

STDPPlasticity::STDPPlasticity(float learning_rate, float tau_plus, float tau_minus)
    : pImpl(new Impl) {
    if (learning_rate > 0) pImpl->learning_rate = learning_rate;
    if (tau_plus > 0) pImpl->tau_plus = tau_plus;
    if (tau_minus > 0) pImpl->tau_minus = tau_minus;
}

STDPPlasticity::~STDPPlasticity() = default;

void STDPPlasticity::applyPlasticity(Neuron* pre, Neuron* post, float dt) {
    if (!pre || !post) return;
    
    NeuronId preId = pre->getId();
    NeuronId postId = post->getId();
    
    // Get spike history for both neurons
    auto& preHistory = pImpl->spike_history[preId];
    auto& postHistory = pImpl->spike_history[postId];
    
    float weight_change = 0.0f;
    
    // Calculate LTP (post spike after pre spike)
    for (const auto& postSpike : postHistory) {
        for (const auto& preSpike : preHistory) {
            float delta_t = postSpike.first - preSpike.first;
            if (delta_t > 0 && delta_t < tau_plus * 5) {  // Within reasonable window
                weight_change += computeLTP(delta_t);
            }
        }
    }
    
    // Calculate LTD (pre spike after post spike)
    for (const auto& preSpike : preHistory) {
        for (const auto& postSpike : postHistory) {
            float delta_t = preSpike.first - postSpike.first;
            if (delta_t > 0 && delta_t < tau_minus * 5) {
                weight_change += computeLTD(delta_t);
            }
        }
    }
    
    // Apply weight change to the synapse
    // This is a simplification - real implementation would need to find the synapse
    // between pre and post neurons
    if (std::abs(weight_change) > 0.0001f) {
        // For now, just record the change
        // Real implementation would need to find the actual synapse
    }
}

void STDPPlasticity::recordPreSynapseSpike(Neuron* neuron) {
    if (!neuron) return;
    
    NeuronId neuronId = neuron->getId();
    Timestamp current_time = static_cast<Timestamp>(pImpl->spike_history[neuronId].size() * 10.0f); // Dummy timestamp
    
    // Add to spike history
    pImpl->spike_history[neuronId].push_back({current_time, true});
    
    // Keep history limited
    if (pImpl->spike_history[neuronId].size() > 1000) {
        pImpl->spike_history[neuronId].erase(pImpl->spike_history[neuronId].begin());
    }
}

void STDPPlasticity::recordPostSynapseSpike(Neuron* neuron) {
    recordPreSynapseSpike(neuron); // Same logic for recording
}

void STDPPlasticity::reset() {
    pImpl->spike_history.clear();
}

void STDPPlasticity::setLearningRate(float rate) {
    if (rate > 0) pImpl->learning_rate = rate;
}

float STDPPlasticity::getLearningRate() const {
    return pImpl->learning_rate;
}

void STDPPlasticity::setTauPlus(float tau) {
    if (tau > 0) pImpl->tau_plus = tau;
}

float STDPPlasticity::getTauPlus() const {
    return pImpl->tau_plus;
}

void STDPPlasticity::setTauMinus(float tau) {
    if (tau > 0) pImpl->tau_minus = tau;
}

float STDPPlasticity::getTauMinus() const {
    return pImpl->tau_minus;
}

} // namespace nlm