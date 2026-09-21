#include "BCM.hpp"
#include "../../brain/Synapse.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct BCMRule::Impl {
    float learningRate;
    float threshold;        // BCM threshold (θ)
    float timeConstant;     // Threshold adaptation time constant
    float minWeight;        // Minimum synaptic weight
    float maxWeight;        // Maximum synaptic weight
    
    // For BCM dynamics: maintain running averages of post-synaptic activity
    float postActivityAverage;    // Average post-synaptic activity
    float activityTrace;          // Short-term trace of recent activity
    
    Impl() : learningRate(0.01f), threshold(1.5f), timeConstant(200.0f),
             minWeight(-1.0f), maxWeight(1.0f),
             postActivityAverage(0.0f), activityTrace(0.0f) {}
};

BCMRule::BCMRule() : pImpl(new Impl) {}

BCMRule::~BCMRule() = default;

void BCMRule::update(Synapse* synapse,
                    const std::vector<Timestamp>& preSpikes,
                    const std::vector<Timestamp>& postSpikes,
                    TimestepDuration dt) {
    /*
     * Real BCM (Bienenstock-Cooper-Munro) learning implementation
     * 
     * Mathematical formulation (BCM theory):
     * Δw = η × (V_post - θ) × pre_activity × eligibility
     * 
     * Where:
     * - η = learning rate parameter
     * - V_post = post-synaptic membrane potential (proxy: spike count)
     * - θ = dynamic threshold (adapts to mean post activity)
     * - pre_activity = pre-synaptic activity
     * - eligibility = synaptic eligibility trace
     * 
     * Key BCM principles:
     * 1. Dynamic threshold θ adapts to post-synaptic activity levels
     * 2. Synaptic change depends on whether activity is above or below θ
     * 3. Stable synaptic weights around θ (homeostasis)
     * 4. Self-organizing network dynamics
     * 
     * Biological inspiration:
     * - Synaptic homeostasis hypothesis
     * - BCM theory in hippocampal place cells
     * - Theta-neuron models in cortex
     * - Activity-regulated development
     * 
     * Implementation details:
     * - Maintain running average of post-synaptic activity
     * - Update threshold based on average activity
     * - Compute weight change based on difference from threshold
     * - Use pre-synaptic activity as driving factor
     * - Apply with bounds to prevent instability
     * 
     * Limitations:
     * - Simplified activity representation
     * - Doesn't capture detailed neuronal dynamics
     * - Assumes stationary input statistics
     */
    
    if (!synapse) {
        return;
    }
    
    // Estimate post-synaptic activity from spike count
    float postActivity = static_cast<float>(postSpikes.size());
    
    // Update running average of post-synaptic activity
    // Exponential moving average for adaptive threshold
    pImpl->activityTrace = pImpl->activityTrace * 0.9f + postActivity * 0.1f;
    pImpl->postActivityAverage = pImpl->activityTrace;
    
    // Update threshold based on average activity
    // BCM threshold adapts to mean post-synaptic activity
    pImpl->threshold = pImpl->postActivityAverage * 0.1f + 1.0f;  // Base θ = 1.0
    
    // Estimate pre-synaptic activity from spike count
    float preActivity = static_cast<float>(preSpikes.size());
    
    // Compute BCM weight change
    // Δw = η × (V_post - θ) × pre_activity
    float delta = pImpl->learningRate * (postActivity - pImpl->threshold) * preActivity;
    
    // Apply BCM weight change with bounds
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

void BCMRule::setThreshold(float threshold) {
    pImpl->threshold = std::clamp(threshold, 0.0f, 10.0f);
}

float BCMRule::getThreshold() const {
    return pImpl->threshold;
}

void BCMRule::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 0.1f);
}

float BCMRule::getLearningRate() const {
    return pImpl->learningRate;
}

void BCMRule::setTimeConstant(float tau) {
    pImpl->timeConstant = std::clamp(tau, 10.0f, 1000.0f);
}

float BCMRule::getTimeConstant() const {
    return pImpl->timeConstant;
}

} // namespace nlm
