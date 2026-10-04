#include "Hebbian.hpp"
#include "../../brain/Synapse.hpp"
#include <algorithm>

namespace nlm {

struct Hebbian::Impl {
    float learningRate;
    float maxWeight;
    float minWeight;
    float covarianceThreshold;  // For covariance rule
    
    Impl() : learningRate(0.01f), maxWeight(1.0f), minWeight(-1.0f),
             covarianceThreshold(0.0f) {}
};

Hebbian::Hebbian() : pImpl(new Impl) {}

Hebbian::~Hebbian() = default;

void Hebbian::update(Synapse* synapse,
                      const std::vector<Timestamp>& preSpikes,
                      const std::vector<Timestamp>& postSpikes,
                      TimestepDuration dt) {
    // Enhanced Hebbian learning with covariance calculation and synaptic tagging
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // STEP 1: Calculate covariance between pre and post synaptic activity
    // Covariance = E[pre * post] - E[pre] * E[post]
    // This captures the statistical relationship between pre and post firing
    
    float preMean = 0.0f;
    float postMean = 0.0f;
    float prePostProductMean = 0.0f;
    
    // Convert spike times to binary activity (1 if spiked, 0 otherwise)
    // In real implementation, would use sliding window correlation
    size_t numTimeBins = std::min(size_t(10), std::max(preSpikes.size(), postSpikes.size()));
    
    for (size_t t = 0; t < numTimeBins; ++t) {
        float preActivity = 0.0f;
        float postActivity = 0.0f;
        
        // Check for spikes in current time bin (using timestamps in ms)
        float binStart = static_cast<float>(t) * 10.0f;  // 10ms bins
        float binEnd = binStart + 10.0f;
        
        for (auto preTime : preSpikes) {
            if (preTime >= binStart && preTime < binEnd) {
                preActivity += 1.0f;
            }
        }
        
        for (auto postTime : postSpikes) {
            if (postTime >= binStart && postTime < binEnd) {
                postActivity += 1.0f;
            }
        }
        
        preMean += preActivity / numTimeBins;
        postMean += postActivity / numTimeBins;
        prePostProductMean += preActivity * postActivity / numTimeBins;
    }
    
    // Calculate covariance
    float covariance = prePostProductMean - (preMean * postMean);
    
    // STEP 2: Apply synaptic tagging and capture
    // Only synapses that are "tagged" by postsynaptic activation can undergo plastic changes
    // This prevents unwanted weakening of synapses during unrelated activity
    
    bool isSynapseTagged = synapse->getPlasticityFlags().eligible;
    
    // STEP 3: Calculate weight change based on covariance and learning rate
    // Δw = η * Covariance
    // With bounds to prevent runaway potentiation
    
    float delta = 0.0f;
    if (isSynapseTagged) {
        // Apply Hebbian learning with covariance rule
        delta = pImpl->learningRate * covariance;
        
        // Add synaptic capture effect: postsynaptic spikes "capture" presynaptic activity
        // This strengthens synapses that contribute to postsynaptic firing
        float captureFactor = calculateCaptureEffect(preSpikes, postSpikes);
        delta *= (1.0f + captureFactor);
    }
    
    // STEP 4: Apply weight change with constraints
    // Constrain the change rate and enforce homeostatic bounds
    delta = std::clamp(delta, -pImpl->maxWeight * 0.1f, pImpl->maxWeight * 0.1f);  // Limit rate of change
    
    if (std::abs(delta) > 1e-6f) {
        applyWeightChange(synapse, delta);
        
        // Update synaptic eligibility trace for reward-modulated combinations
        float currentTrace = synapse->getEligibilityTrace();
        synapse->setEligibilityTrace(currentTrace + delta * 0.5f);  // Stronger trace for Hebbian
    }
}

float Hebbian::calculateCaptureEffect(const std::vector<Timestamp>& preSpikes,
                                     const std::vector<Timestamp>& postSpikes) const {
    // Calculate synaptic capture effect based on timing relationships
    // Capture is strongest when pre-synaptic spikes consistently precede post-synaptic spikes
    
    if (preSpikes.empty() || postSpikes.empty()) {
        return 0.0f;
    }
    
    size_t captureCount = 0;
    size_t totalPairs = 0;
    
    // Look for consistent pre-before-post patterns within biologically relevant time window
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            totalPairs++;
            
            // Pre-before-post within 50ms is considered capturing
            if (dt > 0.0f && dt < 50.0f) {
                captureCount++;
            }
        }
    }
    
    if (totalPairs == 0) return 0.0f;
    
    // Capture strength is proportional to fraction of pre-before-post pairs
    float captureStrength = static_cast<float>(captureCount) / static_cast<float>(totalPairs);
    
    // Apply sigmoid function to map to [0, 1] with sharper transition
    return 2.0f / (1.0f + std::exp(-10.0f * (captureStrength - 0.5f))) - 1.0f;
}

void Hebbian::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* Hebbian::getName() const {
    return "Hebbian";
}

void Hebbian::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

float Hebbian::getLearningRate() const {
    return pImpl->learningRate;
}

void Hebbian::setMaxWeight(float maxWeight) {
    pImpl->maxWeight = std::clamp(maxWeight, 0.0f, 10.0f);
}

float Hebbian::getMaxWeight() const {
    return pImpl->maxWeight;
}

} // namespace nlm
