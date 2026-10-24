# BCMRule.cpp implementation

#include "BCMRule.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct BCMRule::Impl {
    float learningRate;
    float theta;                   // Sliding threshold
    float tauTheta;                // Threshold dynamics time constant
    float ltpThreshold;            // Threshold for LTP
    float ltdThreshold;            // Threshold for LTD
    float maxWeight;
    float minWeight;
    
    Impl() : learningRate(0.01f), theta(0.5f), tauTheta(100.0f),
             ltpThreshold(0.8f), ltdThreshold(0.2f), maxWeight(1.0f), minWeight(-1.0f) {}
};

BCMRule::BCMRule() : pImpl(new Impl) {}

BCMRule::~BCMRule() {
    delete pImpl;
}

void BCMRule::update(Synapse* synapse,
                    const std::vector<Timestamp>& preSpikes,
                    const std::vector<Timestamp>& postSpikes,
                    TimestepDuration dt) {
    /*
     * Bienenstock-Cooper-Munro (BCM) Learning Rule Implementation
     * 
     * BCM theory: Synaptic modification depends on postsynaptic activity
     * through a sliding threshold theta(t)
     * 
     * Mathematical formulation:
     * 
     * For each presynaptic spike at time t_pre:
     *   Calculate postsynaptic trace Z(t) = ∫post(τ) exp(-(t-τ)/τθ) dτ
     *   theta(t) = Z(t) (sliding threshold)
     *   
     * Weight change rule:
     *   Δw = -η × x_pre × (post × theta - theta²)
     *   
     * Where:
     *   x_pre = presynaptic spike indicator
     *   post = postsynaptic firing rate
     *   theta = sliding threshold (proportional to postsynaptic trace)
     *   η = learning rate
     *   
     * Interpretation:
     *   - When post > theta: LTP (weight increases)
     *   - When post < theta: LTD (weight decreases)
     *   - When post = theta: no change
     *   
     * Biological inspiration:
     *   - Calcium-dependent synaptic modification
     *   - Sliding threshold in cortical synapses
     *   - Normalizes synaptic changes to postsynaptic activity
     *   - Prevents runaway excitation
     *   
     * Applications:
     *   - Orientation selectivity development in visual cortex
     *   - Cooperative Hebbian learning with stability
     *   - Model of receptive field development
     *   
     * Advantages over standard Hebbian:
     *   - Activity-dependent regulation
     *   - Automatic stabilization
     *   - Prevents synaptic saturation
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate postsynaptic activity (average firing rate)
    float postActivity = 0.0f;
    for (Timestamp postTime : postSpikes) {
        postActivity += 1.0f;  // Each spike contributes 1.0
    }
    postActivity /= static_cast<float>(postSpikes.size());
    
    // Update sliding threshold (simplified trace calculation)
    pImpl->theta += (postActivity - pImpl->theta) * (dt / pImpl->tauTheta);
    
    // Calculate weight change based on BCM rule
    for (Timestamp preTime : preSpikes) {
        // BCM weight change formula: Δw = -η × x_pre × (post × theta - theta²)
        // For binary spikes, x_pre = 1
        float delta = -pImpl->learningRate * (postActivity * pImpl->theta - pImpl->theta * pImpl->theta);
        
        // Apply with bounds
        if (std::abs(delta) > 1e-6f) {
            applyWeightChange(synapse, delta);
        }
    }
}

void BCMRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Clamp weight to bounds
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* BCMRule::getName() const {
    return "BCM";
}

void BCMRule::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 0.1f);
}

float BCMRule::getLearningRate() const {
    return pImpl->learningRate;
}

void BCMRule::setTheta(float theta) {
    pImpl->theta = std::clamp(theta, 0.0f, 2.0f);
}

float BCMRule::getTheta() const {
    return pImpl->theta;
}

void BCMRule::setTauTheta(float tau) {
    pImpl->tauTheta = std::clamp(tau, 10.0f, 1000.0f);
}

float BCMRule::getTauTheta() const {
    return pImpl->tauTheta;
}

void BCMRule::setLTPThreshold(float threshold) {
    pImpl->ltpThreshold = std::clamp(threshold, 0.0f, 1.0f);
}

float BCMRule::getLTPThreshold() const {
    return pImpl->ltpThreshold;
}

void BCMRule::setLTDThreshold(float threshold) {
    pImpl->ltdThreshold = std::clamp(threshold, 0.0f, 1.0f);
}

float BCMRule::getLTDThreshold() const {
    return pImpl->ltdThreshold;
}

} // namespace nlm