#include "BCM.hpp"
#include "../../brain/Synapse.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct BCMRule::Impl {
    float learningRate;
    float maxWeight;
    float minWeight;
    
    // BCM threshold variable (θ)
    float threshold;
    
    // Sliding window for activity integration
    float activityWindow;
    
    // Current synaptic activity (for BCM)
    float currentActivity;
    
    Impl() : learningRate(0.01f), maxWeight(1.0f), minWeight(-1.0f),
             threshold(0.0f), activityWindow(10.0f), currentActivity(0.0f) {}
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
     * Mathematical formulation:
     * 
     * The BCM learning rule implements sliding window difference learning:
     * 
     * For synaptic weight change:
     * Δw = η * [post(t) - θ(t)] * pre(t)
     * 
     * Where:
     *   η = learning rate
     *   post(t) = postsynaptic activity (firing rate)
     *   θ(t) = threshold variable (sliding threshold)
     *   pre(t) = presynaptic activity
     *   
     * Threshold dynamics:
     *   dθ/dt = ε * [post(t)^2 - θ] * (1 - θ)
     *   
     *   Where ε = adaptation rate
     * 
     * Biological inspiration:
     *   - Based on synaptic competition in visual cortex
     *   - Implements "associative memory with normalization"
     *   - Found in Hebbian/anti-Hebbian competition
     *   - Explains orientation selectivity in V1
     *   
     * Implementation details:
     *   - Compute average postsynaptic activity over time window
     *   - Update threshold based on average activity
     *   - Apply weight change only when post > θ
     *   - Ensure weights stay within biological bounds
     */
    
    if (!synapse) {
        return;
    }
    
    // Calculate average postsynaptic activity (firing rate)
    float postActivity = calculatePostActivity(postSpikes, dt);
    
    // Update threshold based on recent postsynaptic activity
    updateThreshold(postActivity, dt);
    
    // Calculate weight change using BCM rule
    float weightChange = calculateBCMWeightChange(synapse, preSpikes, postActivity, dt);
    
    // Apply weight change with bounds
    applyWeightChange(synapse, weightChange);
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

void BCMRule::setThreshold(float threshold) {
    pImpl->threshold = threshold;
}

float BCMRule::getThreshold() const {
    return pImpl->threshold;
}

void BCMRule::setActivityWindow(float window) {
    pImpl->activityWindow = std::max(window, 1.0f);
}

float BCMRule::getActivityWindow() const {
    return pImpl->activityWindow;
}

float BCMRule::calculatePostActivity(const std::vector<Timestamp>& spikes, TimestepDuration dt) {
    if (spikes.empty()) {
        return 0.0f;
    }
    
    // Calculate average firing rate over the activity window
    float windowDuration = static_cast<float>(pImpl->activityWindow);
    size_t recentSpikes = 0;
    
    for (Timestamp spikeTime : spikes) {
        if (spikeTime >= 0.0) {  // Valid spike time
            recentSpikes++;
        }
    }
    
    // Convert to firing rate (spikes per second)
    float firingRate = (recentSpikes / windowDuration) * 1000.0f;  // Convert to Hz
    
    // Update current activity for threshold calculation
    pImpl->currentActivity = firingRate;
    
    return firingRate;
}

void BCMRule::updateThreshold(float postActivity, TimestepDuration dt) {
    // BCM threshold dynamics: dθ/dt = ε * [post(t)^2 - θ] * (1 - θ)
    // Simplified Euler integration
    float epsilon = 0.1f;  // Adaptation rate
    float postSquared = postActivity * postActivity;
    
    float dTheta = epsilon * (postSquared - pImpl->threshold) * (1.0f - pImpl->threshold);
    
    // Update threshold
    pImpl->threshold += dTheta * static_cast<float>(dt) * 0.001f;  // Convert ms to s
    
    // Ensure threshold stays in valid range [0, 2]
    pImpl->threshold = std::clamp(pImpl->threshold, 0.0f, 2.0f);
}

float BCMRule::calculateBCMWeightChange(Synapse* synapse,
                                       const std::vector<Timestamp>& preSpikes,
                                       float postActivity,
                                       TimestepDuration dt) {
    // Calculate presynaptic activity (firing rate)
    float preActivity = calculatePreActivity(preSpikes, dt);
    
    // BCM weight change: Δw = η * [post(t) - θ(t)] * pre(t)
    float weightChange = pImpl->learningRate * 
                         (postActivity - pImpl->threshold) * 
                         preActivity;
    
    return weightChange;
}

float BCMRule::calculatePreActivity(const std::vector<Timestamp>& spikes, TimestepDuration dt) {
    if (spikes.empty()) {
        return 0.0f;
    }
    
    // Calculate presynaptic firing rate
    float windowDuration = static_cast<float>(pImpl->activityWindow);
    size_t recentSpikes = 0;
    
    for (Timestamp spikeTime : spikes) {
        if (spikeTime >= 0.0) {  // Valid spike time
            recentSpikes++;
        }
    }
    
    // Convert to firing rate (spikes per second)
    float firingRate = (recentSpikes / windowDuration) * 1000.0f;  // Convert to Hz
    
    return firingRate;
}

} // namespace nlm
