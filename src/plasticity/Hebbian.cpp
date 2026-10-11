#include "Hebbian.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>
#include <numeric>

namespace nlm {

struct Hebbian::Impl {
    // Learning parameters
    float learningRate;
    float maxWeight;
    float minWeight;
    
    // Covariance learning parameters
    float learningWindowMs;      // Window for spike timing correlation (ms)
    float covarianceThreshold;   // Minimum covariance to trigger learning
    bool useCovarianceLearning;  // Enable/disable covariance-based learning
    bool rateBasedLearning;      // Use rate-based instead of spike-timing-based
    
    // Homeostatic regulation parameters
    float homeostaticTarget;    // Target average weight (for normalization)
    float homeostaticRate;      // Speed of homeostatic adjustment
    bool weightNormalization;    // Enable weight normalization
    
    // Statistics and state
    float runningPreRate;        // Running estimate of pre-synaptic firing rate
    float runningPostRate;       // Running estimate of post-synaptic firing rate
    float runningCovariance;     // Running estimate of covariance
    float learningAccumulator;   // Accumulator for learning signal
    
    Impl() : 
        learningRate(0.01f),
        maxWeight(1.0f),
        minWeight(-1.0f),
        learningWindowMs(50.0f),
        covarianceThreshold(0.001f),
        useCovarianceLearning(true),
        rateBasedLearning(false),
        homeostaticTarget(0.0f),
        homeostaticRate(0.001f),
        weightNormalization(true),
        runningPreRate(0.0f),
        runningPostRate(0.0f),
        runningCovariance(0.0f),
        learningAccumulator(0.0f) {}
};

Hebbian::Hebbian() : pImpl(new Impl) {}

Hebbian::~Hebbian() = default;

void Hebbian::update(Synapse* synapse,
                       const std::vector<Timestamp>& preSpikes,
                       const std::vector<Timestamp>& postSpikes,
                       TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate total time window from spikes for rate calculations
    Timestamp minTime = std::min(
        preSpikes.front(),
        std::min(postSpikes.front(), 
                std::max(preSpikes.back(), postSpikes.back()))
    );
    Timestamp maxTime = std::max(preSpikes.back(), postSpikes.back());
    TimestepDuration totalWindow = maxTime - minTime;
    
    if (totalWindow <= 0.0) {
        return;
    }
    
    // Get current weight for homeostatic calculations
    float currentWeight = synapse->getWeight();
    
    // Calculate covariance using spike timing
    float covariance = getCovariance(preSpikes, postSpikes, pImpl->learningWindowMs);
    
    // Calculate firing rates
    float preRate = getPreSpikeRate(preSpikes, totalWindow);
    float postRate = getPostSpikeRate(postSpikes, totalWindow);
    
    // Update running statistics with exponential decay
    const float alpha = 0.1f;  // Learning rate for running stats
    pImpl->runningPreRate = alpha * preRate + (1.0f - alpha) * pImpl->runningPreRate;
    pImpl->runningPostRate = alpha * postRate + (1.0f - alpha) * pImpl->runningPostRate;
    pImpl->runningCovariance = alpha * covariance + (1.0f - alpha) * pImpl->runningCovariance;
    
    // Apply learning rule based on configuration
    float delta = 0.0f;
    
    if (pImpl->useCovarianceLearning) {
        if (pImpl->rateBasedLearning) {
            // Rate-based Hebbian learning
            float expectedCov = pImpl->runningPreRate * pImpl->runningPostRate;
            delta = pImpl->learningRate * (pImpl->runningCovariance - expectedCov);
        } else {
            // Spike-timing-based Hebbian learning with covariance rule
            delta = pImpl->learningRate * pImpl->runningCovariance;
        }
        
        // Apply threshold
        if (std::abs(delta) < pImpl->covarianceThreshold) {
            delta = 0.0f;
        }
    } else {
        // Simple Hebbian learning (original implementation)
        size_t correlationCount = 0;
        for (Timestamp preTime : preSpikes) {
            for (Timestamp postTime : postSpikes) {
                float spikeDt = static_cast<float>(postTime - preTime);
                if (std::abs(spikeDt) < pImpl->learningWindowMs) {
                    ++correlationCount;
                }
            }
        }
        delta = pImpl->learningRate * static_cast<float>(correlationCount);
    }
    
    // Apply adaptive learning rate based on correlation strength
    float adaptiveFactor = 1.0f;
    if (pImpl->runningCovariance > 0.0f) {
        adaptiveFactor = std::min(2.0f, 1.0f + 5.0f * pImpl->runningCovariance);
    } else if (pImpl->runningCovariance < 0.0f) {
        adaptiveFactor = std::max(0.1f, 1.0f - 5.0f * pImpl->runningCovariance);
    }
    delta *= adaptiveFactor;
    
    // Apply homeostatic regulation if enabled
    if (pImpl->weightNormalization && pImpl->homeostaticTarget != 0.0f) {
        float homeostaticDelta = pImpl->homeostaticRate * (pImpl->homeostaticTarget - currentWeight);
        delta += homeostaticDelta;
    }
    
    // Apply weight change
    if (std::abs(delta) > 1e-6f) {
        applyWeightChange(synapse, delta);
    }
}

void Hebbian::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    
    // Apply homeostatic normalization if enabled
    if (pImpl->weightNormalization && pImpl->homeostaticTarget != 0.0f) {
        // Normalize weight towards homeostatic target
        float homeostaticFactor = pImpl->homeostaticRate;
        newWeight = pImpl->homeostaticTarget + (newWeight - pImpl->homeostaticTarget) * (1.0f - homeostaticFactor);
    }
    
    // Clamp to bounds
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

void Hebbian::setLearningWindow(float windowMs) {
    pImpl->learningWindowMs = std::clamp(windowMs, 1.0f, 200.0f);
}

float Hebbian::getLearningWindow() const {
    return pImpl->learningWindowMs;
}

void Hebbian::setCovarianceThreshold(float threshold) {
    pImpl->covarianceThreshold = std::abs(threshold);
}

float Hebbian::getCovarianceThreshold() const {
    return pImpl->covarianceThreshold;
}

void Hebbian::setHomeostaticTarget(float target) {
    pImpl->homeostaticTarget = std::clamp(target, -1.0f, 1.0f);
}

float Hebbian::getHomeostaticTarget() const {
    return pImpl->homeostaticTarget;
}

void Hebbian::setHomeostaticRate(float rate) {
    pImpl->homeostaticRate = std::clamp(rate, 0.0001f, 0.1f);
}

float Hebbian::getHomeostaticRate() const {
    return pImpl->homeostaticRate;
}

void Hebbian::setWeightNormalization(bool enable) {
    pImpl->weightNormalization = enable;
}

bool Hebbian::getWeightNormalization() const {
    return pImpl->weightNormalization;
}

void Hebbian::setUseCovarianceLearning(bool enable) {
    pImpl->useCovarianceLearning = enable;
}

bool Hebbian::getUseCovarianceLearning() const {
    return pImpl->useCovarianceLearning;
}

void Hebbian::setRateBasedLearning(bool enable) {
    pImpl->rateBasedLearning = enable;
}

bool Hebbian::getRateBasedLearning() const {
    return pImpl->rateBasedLearning;
}

float Hebbian::getCurrentWeight(Synapse* synapse) const {
    if (!synapse) return 0.0f;
    return synapse->getWeight();
}

float Hebbian::getPreSpikeRate(const std::vector<Timestamp>& spikes, TimestepDuration totalTime) const {
    if (spikes.empty() || totalTime <= 0.0) return 0.0f;
    return static_cast<float>(spikes.size()) / static_cast<float>(totalTime);
}

float Hebbian::getPostSpikeRate(const std::vector<Timestamp>& spikes, TimestepDuration totalTime) const {
    if (spikes.empty() || totalTime <= 0.0) return 0.0f;
    return static_cast<float>(spikes.size()) / static_cast<float>(totalTime);
}

float Hebbian::getCovariance(const std::vector<Timestamp>& preSpikes, 
                           const std::vector<Timestamp>& postSpikes, 
                           TimestepDuration windowMs) const {
    if (preSpikes.empty() || postSpikes.empty() || windowMs <= 0.0) return 0.0f;
    
    // Calculate covariance using spike timing window
    float sumPre = 0.0f;
    float sumPost = 0.0f;
    float sumPrePost = 0.0f;
    size_t count = 0;
    
    // For each pre spike, count post spikes within window and vice versa
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = std::abs(static_cast<float>(postTime - preTime));
            if (dt < windowMs) {
                // Create binned activity indicators
                float preBin = 1.0f;  // Pre spike detected
                float postBin = 1.0f; // Post spike detected
                
                sumPre += preBin;
                sumPost += postBin;
                sumPrePost += preBin * postBin;
                count++;
            }
        }
    }
    
    if (count < 2) return 0.0f;
    
    float meanPre = sumPre / static_cast<float>(count);
    float meanPost = sumPost / static_cast<float>(count);
    float meanPrePost = sumPrePost / static_cast<float>(count);
    
    return meanPrePost - (meanPre * meanPost);
}

} // namespace nlm
