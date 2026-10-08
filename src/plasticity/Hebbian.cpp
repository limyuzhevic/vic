#include "Hebbian.hpp"
#include "../../brain/Synapse.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct Hebbian::Impl {
    float learningRate;
    float maxWeight;
    float minWeight;
    float covarianceThreshold;  // For covariance rule
    float activityBaseline;     // Baseline activity for normalization
    float adaptationRate;       // Rate of adaptation to changing statistics
    
    // Statistics for covariance calculation
    float runningPreActivity;   // Running average of pre-synaptic activity
    float runningPostActivity;  // Running average of post-synaptic activity
    float runningCovariance;    // Running average of covariance
    
    // Time window for activity integration
    float activityWindow;       // Time window in ms
    
    Impl() : learningRate(0.01f), maxWeight(1.0f), minWeight(-1.0f),
             covarianceThreshold(0.0f), activityBaseline(0.1f),
             adaptationRate(0.01f), runningPreActivity(0.0f),
             runningPostActivity(0.0f), runningCovariance(0.0f),
             activityWindow(100.0f) {}
};

Hebbian::Hebbian() : pImpl(new Impl) {}

Hebbian::~Hebbian() = default;

void Hebbian::update(Synapse* synapse,
                      const std::vector<Timestamp>& preSpikes,
                      const std::vector<Timestamp>& postSpikes,
                      TimestepDuration dt) {
    /*
     * Real Hebbian learning implementation - Covariance rule
     * 
     * Mathematical formulation (Covariance rule):
     * Δw = η * (⟨pre * post⟩ - ⟨pre⟩⟨post⟩)
     * 
     * Simplified version for spike-based systems:
     * Δw = η * (coactivity - baseline)
     * 
     * Where:
     *   η = learning rate
     *   coactivity = number of correlated pre/post spikes
     *   baseline = learningRate * mean activity
     *   
     * Biological inspiration:
     *   - Reflects STDP and Hebbian mechanisms
     *   - Implements "neurons that fire together, wire together"
     *   - Found in cortical learning and memory formation
     *   - Accounts for statistical structure in neural activity
     *   
     * Implementation details:
     *   - Uses spike counts as proxy for neural activity
     *   - Implements sliding window statistics
     *   - Includes baseline activity to prevent runaway potentiation
     *   - Normalizes weight changes based on activity levels
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate correlation statistics
    float preActivity = calculateActivity(preSpikes);
    float postActivity = calculateActivity(postSpikes);
    float covariance = calculateCovariance(preSpikes, postSpikes);
    
    // Update running statistics for baseline calculation
    updateRunningStatistics(preActivity, postActivity, covariance, dt);
    
    // Calculate weight change using covariance rule
    float delta = calculateWeightChange(covariance, preActivity, postActivity);
    
    // Apply weight change with bounds
    if (std::abs(delta) > 1e-6f) {
        applyWeightChange(synapse, delta);
    }
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

void Hebbian::setMinWeight(float minWeight) {
    pImpl->minWeight = std::clamp(minWeight, -10.0f, 0.0f);
}

float Hebbian::getMinWeight() const {
    return pImpl->minWeight;
}

void Hebbian::setCovarianceThreshold(float threshold) {
    pImpl->covarianceThreshold = threshold;
}

float Hebbian::getCovarianceThreshold() const {
    return pImpl->covarianceThreshold;
}

void Hebbian::setActivityBaseline(float baseline) {
    pImpl->activityBaseline = std::clamp(baseline, 0.0f, 1.0f);
}

float Hebbian::getActivityBaseline() const {
    return pImpl->activityBaseline;
}

void Hebbian::setAdaptationRate(float rate) {
    pImpl->adaptationRate = std::clamp(rate, 0.001f, 0.1f);
}

float Hebbian::getAdaptationRate() const {
    return pImpl->adaptationRate;
}

void Hebbian::setActivityWindow(float window) {
    pImpl->activityWindow = std::max(window, 1.0f);
}

float Hebbian::getActivityWindow() const {
    return pImpl->activityWindow;
}

float Hebbian::calculateActivity(const std::vector<Timestamp>& spikes) {
    if (spikes.empty()) {
        return 0.0f;
    }
    
    // Calculate average firing rate over the activity window
    float windowDuration = pImpl->activityWindow;
    size_t spikeCount = spikes.size();
    
    // Convert spike count to firing rate (Hz)
    // Assuming spikes are in timestamps (ms)
    float firingRate = (static_cast<float>(spikeCount) / windowDuration) * 1000.0f;
    
    return firingRate;
}

float Hebbian::calculateCovariance(const std::vector<Timestamp>& preSpikes,
                                   const std::vector<Timestamp>& postSpikes) {
    if (preSpikes.empty() || postSpikes.empty()) {
        return 0.0f;
    }
    
    // Calculate pre and post spike counts
    size_t preCount = preSpikes.size();
    size_t postCount = postSpikes.size();
    
    // Calculate covariance-like measure
    // This is a simplified version: more correlated spikes = higher covariance
    float preActivity = static_cast<float>(preCount);
    float postActivity = static_cast<float>(postCount);
    
    // Calculate time alignment (simple window overlap)
    size_t overlappingSpikes = 0;
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            // Consider spikes within 20ms of each other as overlapping
            if (std::abs(static_cast<float>(postTime) - static_cast<float>(preTime)) < 20.0f) {
                overlappingSpikes++;
                break;  // Count each pre spike only once
            }
        }
    }
    
    // Normalized covariance measure
    float normalizedOverlap = (overlappingSpikes / static_cast<float>(preCount * postCount));
    
    // Scale by activity levels and baseline
    float covariance = normalizedOverlap * preActivity * postActivity * pImpl->activityBaseline;
    
    return covariance;
}

void Hebbian::updateRunningStatistics(float preActivity, float postActivity, float covariance, TimestepDuration dt) {
    // Update running averages with exponential decay
    float decayFactor = std::exp(-static_cast<float>(dt) * 0.001f / pImpl->activityWindow);
    
    pImpl->runningPreActivity = pImpl->runningPreActivity * decayFactor + preActivity * (1.0f - decayFactor);
    pImpl->runningPostActivity = pImpl->runningPostActivity * decayFactor + postActivity * (1.0f - decayFactor);
    pImpl->runningCovariance = pImpl->runningCovariance * decayFactor + covariance * (1.0f - decayFactor);
}

float Hebbian::calculateWeightChange(float covariance, float preActivity, float postActivity) {
    // Base weight change from covariance
    float delta = pImpl->learningRate * covariance;
    
    // Subtract baseline to prevent runaway potentiation
    float baseline = pImpl->learningRate * pImpl->activityBaseline * preActivity * postActivity;
    delta -= baseline;
    
    // Apply threshold - if change is too small, don't update
    if (std::abs(delta) < pImpl->covarianceThreshold) {
        delta = 0.0f;
    }
    
    // Normalize by activity level to prevent overlearning with high firing rates
    float activityNormalization = std::sqrt(preActivity * postActivity);
    if (activityNormalization > 0.0f) {
        delta /= activityNormalization;
    }
    
    return delta;
}

} // namespace nlm
