#include "Hebbian.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>
#include <queue>
#include <random>

namespace nlm {

struct Hebbian::Impl {
    float learningRate;
    float maxWeight;
    float minWeight;
    float covarianceThreshold;  // For covariance rule
    float decayRate;             // Weight decay rate
    float homeostaticRate;      // Homeostatic scaling rate
    float noiseLevel;            // Biological noise level
    
    // Covariance statistics
    float preMean;               // Mean presynaptic activity
    float postMean;              // Mean postsynaptic activity
    float preVariance;           // Presynaptic activity variance
    float postVariance;          // Postsynaptic activity variance
    float covariance;            // Current covariance
    
    // Synaptic eligibility trace
    float eligibilityTrace;      // For reward-modulated Hebbian
    float traceDecayRate;        // Eligibility trace decay
    
    // Spike history for STDP-like interaction
    std::queue<Timestamp> recentPreSpikes;
    std::queue<Timestamp> recentPostSpikes;
    size_t spikeHistorySize;
    
    Impl() : learningRate(0.01f), maxWeight(1.0f), minWeight(-1.0f),
             covarianceThreshold(0.0f), decayRate(0.0001f), homeostaticRate(0.00005f),
             noiseLevel(0.01f), preMean(0.0f), postMean(0.0f), preVariance(0.0f),
             postVariance(0.0f), covariance(0.0f), eligibilityTrace(0.0f),
             traceDecayRate(0.1f), spikeHistorySize(10) {}
};

Hebbian::Hebbian() : pImpl(new Impl) {
    NLM_LOG_INFO("Hebbian plasticity rule initialized with advanced covariance-based learning");
}

Hebbian::~Hebbian() = default;

void Hebbian::update(Synapse* synapse,
                     const std::vector<Timestamp>& preSpikes,
                     const std::vector<Timestamp>& postSpikes,
                     TimestepDuration dt) {
    // Real Hebbian implementation with covariance-based learning rule
    // Δw = η * (⟨pre * post⟩ - ⟨pre⟩⟨post⟩) * homeostatic_factor
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Update spike history
    updateSpikeHistory(preSpikes, postSpikes, dt);
    
    // Compute covariance-based weight change
    float delta = computeCovarianceWeightChange(synapse, preSpikes, postSpikes, dt);
    
    // Apply homeostatic regulation
    float homeostaticFactor = computeHomeostaticFactor(synapse);
    delta *= homeostaticFactor;
    
    // Add biological noise for stochastic learning
    float noise = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * pImpl->noiseLevel;
    delta += noise * std::abs(delta);
    
    // Apply weight change with bounds
    if (std::abs(delta) > 1e-8f) {
        applyWeightChange(synapse, delta);
        
        // Update eligibility trace for reward-modulated learning
        pImpl->eligibilityTrace += delta * learningRateToTrace(pImpl->learningRate);
        
        // Log Hebbian activity
        if (std::abs(delta) > 0.001f) {
            NLM_LOG_DEBUG("Hebbian: delta=" + std::to_string(delta) + 
                         " homeostatic_factor=" + std::to_string(homeostaticFactor) +
                         " weight=" + std::to_string(synapse->getWeight()));
        }
    }
}

void Hebbian::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Compute new weight with biological constraints
    float newWeight = synapse->getWeight() + delta;
    
    // Apply hard bounds
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    
    // Apply soft bounds based on synaptic history
    float adaptiveBound = computeAdaptiveBound(synapse, newWeight);
    newWeight = std::clamp(newWeight, -adaptiveBound, adaptiveBound);
    
    synapse->setWeight(newWeight);
}

const char* Hebbian::getName() const {
    return "Hebbian_Covariance";
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
    pImpl->covarianceThreshold = std::clamp(threshold, 0.0f, 1.0f);
}

float Hebbian::getCovarianceThreshold() const {
    return pImpl->covarianceThreshold;
}

void Hebbian::setDecayRate(float decayRate) {
    pImpl->decayRate = std::clamp(decayRate, 0.0f, 0.001f);
}

float Hebbian::getDecayRate() const {
    return pImpl->decayRate;
}

void Hebbian::setHomeostaticRate(float rate) {
    pImpl->homeostaticRate = std::clamp(rate, 0.0f, 0.001f);
}

float Hebbian::getHomeostaticRate() const {
    return pImpl->homeostaticRate;
}

void Hebbian::setNoiseLevel(float noise) {
    pImpl->noiseLevel = std::clamp(noise, 0.0f, 0.1f);
}

float Hebbian::getNoiseLevel() const {
    return pImpl->noiseLevel;
}

void Hebbian::updateSpikeHistory(const std::vector<Timestamp>& preSpikes,
                                 const std::vector<Timestamp>& postSpikes,
                                 TimestepDuration dt) {
    // Maintain recent spike history for temporal correlation
    for (Timestamp preTime : preSpikes) {
        if (pImpl->recentPreSpikes.size() >= pImpl->spikeHistorySize) {
            pImpl->recentPreSpikes.pop();
        }
        pImpl->recentPreSpikes.push(preTime);
    }
    
    for (Timestamp postTime : postSpikes) {
        if (pImpl->recentPostSpikes.size() >= pImpl->spikeHistorySize) {
            pImpl->recentPostSpikes.pop();
        }
        pImpl->recentPostSpikes.push(postTime);
    }
    
    // Decay eligibility trace
    pImpl->eligibilityTrace *= (1.0f - pImpl->traceDecayRate * static_cast<float>(dt) * 0.001f);
}

float Hebbian::computeCovarianceWeightChange(Synapse* synapse,
                                             const std::vector<Timestamp>& preSpikes,
                                             const std::vector<Timestamp>& postSpikes,
                                             TimestepDuration dt) {
    // Compute covariance-based weight change: Δw = η * cov(pre, post)
    
    // Calculate mean activities
    float preActivity = computeAverageActivity(preSpikes);
    float postActivity = computeAverageActivity(postSpikes);
    
    // Update statistics
    updateStatistics(preActivity, postActivity);
    
    // Compute covariance
    float covariance = pImpl->covariance;
    
    // Normalize and scale with learning rate
    float delta = pImpl->learningRate * covariance;
    
    // Apply covariance threshold
    if (std::abs(covariance) < pImpl->covarianceThreshold) {
        delta *= 0.1f;  // Suppress weak correlations
    }
    
    // Time scaling
    delta *= static_cast<float>(dt) * 1000.0f;  // Convert to milliseconds
    
    return delta;
}

float Hebbian::computeAverageActivity(const std::vector<Timestamp>& spikes) {
    if (spikes.empty()) return 0.0f;
    
    // Simple firing rate estimate
    float timeSpan = static_cast<float>(spikes.back() - spikes.front());
    if (timeSpan <= 0.0f) timeSpan = 1.0f;
    
    return static_cast<float>(spikes.size()) / (timeSpan / 1000.0f);  // Hz
}

void Hebbian::updateStatistics(float preActivity, float postActivity) {
    // Update running statistics for covariance calculation
    const float alpha = 0.1f;  // Learning rate for statistics
    
    // Update means
    pImpl->preMean += alpha * (preActivity - pImpl->preMean);
    pImpl->postMean += alpha * (postActivity - pImpl->postMean);
    
    // Update variances and covariance
    pImpl->preVariance += alpha * (preActivity - pImpl->preMean) * (preActivity - pImpl->preMean);
    pImpl->postVariance += alpha * (postActivity - pImpl->postMean) * (postActivity - pImpl->postMean);
    
    // Simplified covariance calculation
    float preDev = preActivity - pImpl->preMean;
    float postDev = postActivity - pImpl->postMean;
    pImpl->covariance += alpha * (preDev * postDev - pImpl->covariance);
}

float Hebbian::computeHomeostaticFactor(Synapse* synapse) {
    // Homeostatic scaling to maintain synaptic stability
    if (!synapse) return 1.0f;
    
    float weight = synapse->getWeight();
    
    // Scale factor based on deviation from optimal weight
    float optimalWeight = 0.0f;  // Assuming zero is optimal (balanced excitation/inhibition)
    float deviation = std::abs(weight - optimalWeight);
    
    // Apply homeostatic regulation
    float factor = 1.0f - pImpl->homeostaticRate * deviation;
    factor = std::clamp(factor, 0.5f, 2.0f);
    
    // Decay factor for spike history
    size_t historySize = std::min(pImpl->recentPreSpikes.size(), pImpl->recentPostSpikes.size());
    float historyFactor = 1.0f + std::exp(-static_cast<float>(historySize) / 5.0f);
    
    return factor * historyFactor;
}

float Hebbian::computeAdaptiveBound(Synapse* synapse, float proposedWeight) {
    if (!synapse) return pImpl->maxWeight;
    
    float currentWeight = synapse->getWeight();
    
    // Adaptive bound based on current weight and learning history
    float adaptiveBound = pImpl->maxWeight;
    
    // Reduce bound if weight is already high (to prevent runaway potentiation)
    if (std::abs(currentWeight) > 0.5f) {
        adaptiveBound *= (1.0f - std::abs(currentWeight) * 0.3f);
    }
    
    // Increase bound if weight is near zero (allowing more growth)
    if (std::abs(currentWeight) < 0.1f) {
        adaptiveBound *= 1.5f;
    }
    
    return adaptiveBound;
}

float Hebbian::learningRateToTrace(float learningRate) {
    // Convert learning rate to eligibility trace scaling factor
    return learningRate * 10.0f;  // Trace integrates over longer time
}

void Hebbian::reset() {
    pImpl->eligibilityTrace = 0.0f;
    pImpl->spikeCount = 0.0f;
    while (!pImpl->recentPreSpikes.empty()) pImpl->recentPreSpikes.pop();
    while (!pImpl->recentPostSpikes.empty()) pImpl->recentPostSpikes.pop();
}

float Hebbian::getEligibilityTrace() const {
    return pImpl->eligibilityTrace;
}

void Hebbian::setEligibilityTrace(float trace) {
    pImpl->eligibilityTrace = std::clamp(trace, 0.0f, 1.0f);
}

} // namespace nlm
