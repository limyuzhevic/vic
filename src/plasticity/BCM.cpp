// BCM.cpp - Implementation of BCM (Bayesian Cognitive Model) plasticity rule

#include "BCM.hpp"
#include "Synapse.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <queue>

namespace nlm {

struct BCM::Impl {
    // BCM parameters
    float learningRate = 0.01f;
    float bLayer = 0.5f;           // BCM threshold parameter (b)
    float learningThreshold = 0.5f; // Activity threshold for plasticity
    float maxWeight = 1.0f;
    float minWeight = -1.0f;
    
    // BCM state variables
    std::unordered_map<NeuronId, float> slidingAverage;  // Sliding average of pre-synaptic activity
    std::unordered_map<NeuronId, float> postSynapticActivity;  // Post-synaptic activity
    std::queue<Timestamp> activityHistory;  // Recent activity history
    static constexpr size_t MAX_ACTIVITY_HISTORY = 1000;
    
    // BCM learning variables
    float globalThreshold = 0.0f;
    float thresholdAdaptationRate = 0.001f;
    
    Impl() {
        slidingAverage.reserve(10000);
        postSynapticActivity.reserve(10000);
        activityHistory.reserve(MAX_ACTIVITY_HISTORY);
    }
};

BCM::BCM() : pImpl(new Impl) {}

BCM::~BCM() {
    delete pImpl;
}

void BCM::update(Synapse* synapse, 
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate pre-synaptic sliding average
    NeuronId preNeuron = synapse->getSourceNeuron().index();
    float preActivity = calculatePreSynapticActivity(preNeuron, preSpikes);
    
    // Calculate post-synaptic activity
    float postActivity = calculatePostSynapticActivity(postSpikes);
    
    // Apply BCM learning rule
    calculateBCMLearning(synapse, preActivity, postActivity, dt);
}

void BCM::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float currentWeight = synapse->getWeight();
    float newWeight = currentWeight + delta;
    
    // Apply weight bounds
    newWeight = std::max(pImpl->minWeight, std::min(pImpl->maxWeight, newWeight));
    
    synapse->setWeight(newWeight);
}

const char* BCM::getName() const {
    return "BCM";
}

void BCM::setLearningRate(float rate) {
    pImpl->learningRate = std::max(0.0f, rate);
}

float BCM::getLearningRate() const {
    return pImpl->learningRate;
}

void BCM::setBLayer(float b) {
    pImpl->bLayer = std::max(0.0f, b);
}

float BCM::getBLayer() const {
    return pImpl->bLayer;
}

void BCM::setLearningThreshold(float threshold) {
    pImpl->learningThreshold = std::max(0.0f, threshold);
}

float BCM::getLearningThreshold() const {
    return pImpl->learningThreshold;
}

void BCM::calculateBCMLearning(Synapse* synapse, 
                              float preActivity, 
                              float postActivity, 
                              TimestepDuration dt) {
    // BCM learning rule: Δw = η * x_pre * (x_post - θ_b)
    // where θ_b is the BCM threshold parameter
    
    // Calculate BCM threshold adaptation
    updateBCMLearningThreshold(preActivity, postActivity);
    
    // Compute weight change based on BCM rule
    float activityDifference = postActivity - pImpl->bLayer;
    
    // Apply activity-dependent learning gate
    float learningGate = calculateLearningGate(postActivity);
    
    float weightChange = pImpl->learningRate * preActivity * activityDifference * learningGate;
    
    // Apply synaptic eligibility trace
    float eligibilityTrace = synapse->getEligibilityTrace();
    weightChange *= eligibilityTrace;
    
    applyWeightChange(synapse, weightChange);
}

void BCM::updateBCMLearningThreshold(float preActivity, float postActivity) {
    // Adaptive threshold based on recent network activity
    float targetActivity = 0.5f; // Target post-synaptic activity level
    
    // Adjust threshold towards target activity
    float thresholdChange = pImpl->thresholdAdaptationRate * 
                           (targetActivity - postActivity);
    
    pImpl->globalThreshold += thresholdChange;
    pImpl->globalThreshold = std::max(0.0f, std::min(1.0f, pImpl->globalThreshold));
}

float BCM::calculateLearningGate(float postActivity) const {
    // BCM learning gate: learning occurs when postActivity > θ_b
    // This implements the BCM "fire together, wire together" principle
    
    if (postActivity > pImpl->learningThreshold) {
        // Strong post-synaptic activity promotes learning
        return std::min(1.0f, postActivity * 2.0f);
    } else {
        // Weak post-synaptic activity inhibits learning
        return 0.0f;
    }
}

float BCM::calculatePreSynapticActivity(NeuronId neuronId, 
                                       const std::vector<Timestamp>& spikes) const {
    if (spikes.empty()) return 0.0f;
    
    // Calculate sliding average of pre-synaptic activity
    float totalActivity = 0.0f;
    for (Timestamp spikeTime : spikes) {
        // Weight recent spikes more heavily
        float weight = 1.0f / (1.0f + spikeTime * 0.001f);
        totalActivity += weight;
    }
    
    return totalActivity / static_cast<float>(spikes.size());
}

float BCM::calculatePostSynapticActivity(const std::vector<Timestamp>& spikes) const {
    if (spikes.empty()) return 0.0f;
    
    // Calculate average post-synaptic activity
    float totalActivity = 0.0f;
    for (Timestamp spikeTime : spikes) {
        totalActivity += 1.0f / (1.0f + spikeTime * 0.001f);
    }
    
    return totalActivity / static_cast<float>(spikes.size());
}

} // namespace nlm