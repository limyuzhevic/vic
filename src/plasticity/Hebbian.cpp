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
    bool normalizationEnabled;
    bool symmetric;
    float normalizationFactor;
    float activityHistoryWeight;
    float temporalWindow;
    
    // Statistics for debugging and analysis
    size_t totalUpdates;
    size_t normalizationCount;
    float lastCovariance;
    
    Impl() : learningRate(0.01f), maxWeight(1.0f), minWeight(-1.0f),
             normalizationEnabled(true), symmetric(false),
             normalizationFactor(0.1f), activityHistoryWeight(0.5f),
             temporalWindow(50.0f), totalUpdates(0), normalizationCount(0),
             lastCovariance(0.0f) {}
};

// Private helper functions for the Hebbian class
namespace {
    float CalculateSpikeTimingCovariance(
        const std::vector<Timestamp>& preSpikes,
        const std::vector<Timestamp>& postSpikes,
        TimestepDuration dt) {
        
        // Calculate cross-correlation of spike trains
        std::vector<float> preCounts(dt, 0.0f);
        std::vector<float> postCounts(dt, 0.0f);
        
        // Bin spikes into time windows
        for (Timestamp spike : preSpikes) {
            size_t bin = std::min(static_cast<size_t>(spike), dt - 1);
            preCounts[bin]++;
        }
        
        for (Timestamp spike : postSpikes) {
            size_t bin = std::min(static_cast<size_t>(spike), dt - 1);
            postCounts[bin]++;
        }
        
        // Calculate means
        float preMean = std::accumulate(preCounts.begin(), preCounts.end(), 0.0f) / dt;
        float postMean = std::accumulate(postCounts.begin(), postCounts.end(), 0.0f) / dt;
        
        // Calculate covariance
        float covariance = 0.0f;
        for (size_t i = 0; i < dt; ++i) {
            float preDev = preCounts[i] - preMean;
            float postDev = postCounts[i] - postMean;
            covariance += preDev * postDev;
        }
        
        covariance /= dt;
        
        return covariance;
    }
    
    float NormalizeWeightChange(Hebbian::Impl* impl, Synapse* synapse, float delta) {
        float currentWeight = synapse->getWeight();
        
        // Calculate target weight based on desired activity
        float targetWeight = 0.0f;
        if (impl->lastCovariance > 0.0f) {
            targetWeight = std::min(currentWeight + delta * 2.0f, impl->maxWeight);
        }
        
        // Normalize the change
        float normalizedDelta = delta * impl->normalizationFactor;
        
        if (++impl->normalizationCount % 10 == 0) {
            // Apply soft normalization periodically
            float normFactor = std::exp(-std::abs(currentWeight - targetWeight) * impl->activityHistoryWeight);
            normalizedDelta *= normFactor;
        }
        
        return normalizedDelta;
    }
}

Hebbian::Hebbian(float learningRate, float maxWeight, float minWeight,
                bool normalizationEnabled) : pImpl(new Impl) {
    setLearningRate(learningRate);
    setMaxWeight(maxWeight);
    pImpl->minWeight = minWeight;
    pImpl->normalizationEnabled = normalizationEnabled;
    pImpl->symmetric = false;
}

Hebbian::~Hebbian() = default;

void Hebbian::update(Synapse* synapse,
                     const std::vector<Timestamp>& preSpikes,
                     const std::vector<Timestamp>& postSpikes,
                     TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate spike statistics
    float preFiringRate = static_cast<float>(preSpikes.size()) / dt;
    float postFiringRate = static_cast<float>(postSpikes.size()) / dt;
    
    // Calculate covariance between pre- and post-synaptic activity
    float covariance = CalculateSpikeTimingCovariance(
        preSpikes, postSpikes, dt);
    
    // Store for statistics
    pImpl->lastCovariance = covariance;
    
    // Apply Hebbian learning rule with biological constraints
    float delta = pImpl->learningRate * covariance;
    
    // Apply synaptic normalization if enabled
    if (pImpl->normalizationEnabled) {
        delta = NormalizeWeightChange(pImpl.get(), synapse, delta);
    }
    
    // Apply asymmetric depression for anti-correlated activity if symmetric mode
    if (pImpl->symmetric && covariance < -1e-3f) {
        // Additional depression for anti-correlated activity
        delta += pImpl->learningRate * 0.5f * covariance;
    }
    
    // Apply the weight change
    if (std::abs(delta) > 1e-6f) {
        applyWeightChange(synapse, delta);
        ++pImpl->totalUpdates;
    }
}

void Hebbian::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float currentWeight = synapse->getWeight();
    float newWeight = currentWeight + delta;
    
    // Apply bounds checking
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    
    // Apply synaptic history modulation if available
    float efficacy = synapse->getEfficacy();
    newWeight *= (1.0f + efficacy * 0.1f);  // Modulate by efficacy
    
    // Re-clamp after efficacy modulation
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    
    synapse->setWeight(newWeight);
}

const char* Hebbian::getName() const {
    return "Hebbian (Covariance-based)";
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

void Hebbian::setNormalizationEnabled(bool enabled) {
    pImpl->normalizationEnabled = enabled;
}

bool Hebbian::isNormalizationEnabled() const {
    return pImpl->normalizationEnabled;
}

void Hebbian::setSymmetric(bool symmetric) {
    pImpl->symmetric = symmetric;
}

bool Hebbian::isSymmetric() const {
    return pImpl->symmetric;
}

} // namespace nlm