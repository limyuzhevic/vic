#include "PlasticityRule.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>
#include <queue>
#include <limits>

namespace nlm {

struct BCMRule::Impl {
    float learningRate;      // Learning rate (η)
    float maxWeight;         // Maximum synaptic weight
    float minWeight;         // Minimum synaptic weight
    float kappa;             // BCM threshold variable (θ²)
    float mu;                // Sliding threshold rate
    float timeWindow;        // Time window for BCM learning (ms)
    
    // For temporal trace
    std::queue<Timestamp> preSpikeQueue;
    Timestamp lastPostSpike;
    
    Impl() : learningRate(0.01f), maxWeight(1.0f), minWeight(-1.0f),
             kappa(0.5f), mu(0.0001f), timeWindow(100.0f),
             lastPostSpike(-1) {}
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
    
    // BCM (Bienenstock-Cooper-Munro) rule implementation:
    // Δw = η × x_post² × (x_post - θ²)
    // Where: x_post = post-synaptic activity, θ² = threshold variable
    // The threshold variable slides with mean post-synaptic activity
    
    // Update threshold variable (θ² = μ × mean_activity)
    float meanPostActivity = 0.0f;
    for (Timestamp postTime : postSpikes) {
        meanPostActivity += 1.0f; // Each spike contributes 1.0 (simplified)
    }
    meanPostActivity /= static_cast<float>(postSpikes.size());
    
    pImpl->kappa = pImpl->mu * meanPostActivity;
    
    // Calculate BCM weight change
    float totalDelta = 0.0f;
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            // Post-synaptic activity squared (x_post²)
            float postActivitySquared = 1.0f; // Simplified: each spike = 1.0
            
            // Difference from threshold (x_post - θ²)
            float thresholdDifference = postActivitySquared - pImpl->kappa;
            
            // BCM rule: Δw = η × x_post² × (x_post - θ²)
            float delta = pImpl->learningRate * postActivitySquared * thresholdDifference;
            totalDelta += delta;
        }
    }
    
    // Apply weight change with bounds
    if (std::abs(totalDelta) > 1e-6f) {
        // Scale by synaptic efficacy if available
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;
        
        // Apply weight change using applyWeightChange to ensure bounds checking
        applyWeightChange(synapse, totalDelta);
        
        // Update eligibility trace for reward-modulated learning
        float currentTrace = synapse->getEligibilityTrace();
        synapse->setEligibilityTrace(currentTrace + totalDelta * 0.01f);
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
    pImpl->learningRate = std::max(0.0f, std::min(rate, 1.0f)); // Clamp to [0, 1]
}

float BCMRule::getLearningRate() const {
    return pImpl->learningRate;
}

void BCMRule::setMaxWeight(float weight) {
    pImpl->maxWeight = weight;
}

float BCMRule::getMaxWeight() const {
    return pImpl->maxWeight;
}

void BCMRule::setMinWeight(float weight) {
    pImpl->minWeight = weight;
}

float BCMRule::getMinWeight() const {
    return pImpl->minWeight;
}

void BCMRule::setKappa(float kappa) {
    pImpl->kappa = std::max(0.0f, std::min(kappa, 10.0f)); // Clamp to reasonable range
}

float BCMRule::getKappa() const {
    return pImpl->kappa;
}

void BCMRule::setMu(float mu) {
    pImpl->mu = std::max(0.0f, mu); // Non-negative
}

float BCMRule::getMu() const {
    return pImpl->mu;
}

void BCMRule::setTimeWindow(float window) {
    pImpl->timeWindow = std::max(1.0f, window); // Minimum 1ms
}

float BCMRule::getTimeWindow() const {
    return pImpl->timeWindow;
}

void BCMRule::configure(float learningRate, float maxWeight, float minWeight,
                       float kappa, float mu, float timeWindow) {
    setLearningRate(learningRate);
    setMaxWeight(maxWeight);
    setMinWeight(minWeight);
    setKappa(kappa);
    setMu(mu);
    setTimeWindow(timeWindow);
}

} // namespace nlm
