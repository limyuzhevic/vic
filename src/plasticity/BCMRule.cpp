#include "BCMRule.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct BCMRule::Impl {
    float learningRate;
    float threshold;
    float modulationStrength;
    float minWeight;
    float maxWeight;
    
    // BCM sliding threshold mechanism
    // θ(y) = θ₀ + ℳ * y² where y is post-synaptic activity
    float baseThreshold;
    
    Impl() : learningRate(0.01f), threshold(0.5f), modulationStrength(0.1f),
             minWeight(-1.0f), maxWeight(1.0f), baseThreshold(0.5f) {}
};

BCMRule::BCMRule() : pImpl(new Impl) {}

BCMRule::~BCMRule() = default;

void BCMRule::update(Synapse* synapse,
                     const std::vector<Timestamp>& preSpikes,
                     const std::vector<Timestamp>& postSpikes,
                     TimestepDuration dt) {
    /*
     * Bienenstock-Cooper-Munro (BCM) learning implementation
     * 
     * Mathematical formulation:
     * Δw = ℳ * (y - θ(y)) * (y - θ₀)
     * 
     * Where:
     *   y = post-synaptic activity (firing rate)
     *   θ(y) = activity-dependent threshold = θ₀ + ℳ * y²
     *   θ₀ = base threshold (θ₀)
     *   ℳ = modulation strength
     *   ℳ = learning rate
     * 
     * Biological inspiration:
     *   - Sliding threshold mechanism for synaptic stability
     *   - Normalizes firing rates across neurons
     *   - Prevents runaway excitation through homeostatic control
     *   - Used for maintaining E/I balance
     *   - Found in cortical circuits and memory consolidation
     *   
     * Implementation details:
     *   - Calculates post-synaptic firing rate from spike history
     *   - Computes activity-dependent threshold
     *   - Applies BCM rule with sliding threshold
     *   - Includes bounds to prevent numerical instability
     */
    
    if (!synapse || postSpikes.empty()) {
        return;
    }
    
    // Calculate post-synaptic activity (firing rate)
    float postActivity = static_cast<float>(postSpikes.size()) / 1000.0f;  // spikes per ms
    
    // Calculate activity-dependent threshold: θ(y) = θ₀ + ℳ * y²
    float activityDependentThreshold = pImpl->baseThreshold + 
                                      pImpl->modulationStrength * postActivity * postActivity;
    
    // Calculate BCM learning signal: ℳ * (y - θ(y)) * (y - θ₀)
    float bcmSignal = pImpl->modulationStrength * 
                     (postActivity - activityDependentThreshold) * 
                     (postActivity - pImpl->baseThreshold);
    
    // Apply BCM learning rule
    float delta = pImpl->learningRate * bcmSignal;
    
    // Apply with bounds and minimum threshold
    if (std::abs(delta) > 1e-8f) {
        applyWeightChange(synapse, delta);
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
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

float BCMRule::getLearningRate() const {
    return pImpl->learningRate;
}

void BCMRule::setThreshold(float threshold) {
    pImpl->baseThreshold = std::clamp(threshold, 0.0f, 1.0f);
    pImpl->threshold = threshold;
}

float BCMRule::getThreshold() const {
    return pImpl->threshold;
}

void BCMRule::setModulationStrength(float strength) {
    pImpl->modulationStrength = std::clamp(strength, 0.0f, 1.0f);
}

float BCMRule::getModulationStrength() const {
    return pImpl->modulationStrength;
}

} // namespace nlm