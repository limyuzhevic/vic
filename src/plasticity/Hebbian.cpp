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
    float runningAveragePre;    // Running average of pre-synaptic activity
    float runningAveragePost;   // Running average of post-synaptic activity
    size_t sampleCount;         // Number of samples for averaging
    
    Impl() : learningRate(0.01f), maxWeight(1.0f), minWeight(-1.0f),
             covarianceThreshold(0.0f), runningAveragePre(0.0f),
             runningAveragePost(0.0f), sampleCount(0) {}
};

Hebbian::Hebbian() : pImpl(new Impl) {}

Hebbian::~Hebbian() = default;

void Hebbian::update(Synapse* synapse,
                      const std::vector<Timestamp>& preSpikes,
                      const std::vector<Timestamp>& postSpikes,
                      TimestepDuration dt) {
    /*
     * Real Hebbian learning implementation - Enhanced covariance rule
     * 
     * Mathematical formulation (Covariance rule):
     * Δw = η * (⟨pre * post⟩ - ⟨pre⟩⟨post⟩)
     * 
     * Where:
     *   ⟨pre⟩ = average pre-synaptic activity
     *   ⟨post⟩ = average post-synaptic activity
     *   ⟨pre * post⟩ = average of pre * post products
     *   η = learningRate
     * 
     * This implements "neurons that fire together, wire together"
     * with online learning to adapt to changing activity statistics.
     * 
     * Biological inspiration:
     *   - Reflects AMPA receptor trafficking at synapses
     *   - Hebbian plasticity at Schaffer collateral synapses in hippocampus
     *   - Correlation-based learning in visual cortex
     *   - Synaptic tagging and capture mechanisms
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Convert spike counts to firing rates (Hz)
    float preFiringRate = static_cast<float>(preSpikes.size()) / dt;
    float postFiringRate = static_cast<float>(postSpikes.size()) / dt;
    
    // Update running averages with exponential decay
    const float alpha = 0.1f;  // Learning rate for statistics adaptation
    pImpl->runningAveragePre = (1.0f - alpha) * pImpl->runningAveragePre + alpha * preFiringRate;
    pImpl->runningAveragePost = (1.0f - alpha) * pImpl->runningAveragePost + alpha * postFiringRate;
    pImpl->sampleCount++;
    
    // Compute covariance: cov(pre, post) = E[pre*post] - E[pre]*E[post]
    // For simplicity, we use the instantaneous product as approximation
    float instantaneousProduct = preFiringRate * postFiringRate;
    float expectedProduct = pImpl->runningAveragePre * pImpl->runningAveragePost;
    
    // Weight change based on covariance
    float delta = pImpl->learningRate * (instantaneousProduct - expectedProduct);
    
    // Apply with bounds and non-zero check
    if (std::abs(delta) > 1e-6f) {
        applyWeightChange(synapse, delta);
    }
}

void Hebbian::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    
    // Apply synaptic-specific constraints
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    
    // Apply homeostatic regulation if threshold exceeded
    if (std::abs(newWeight) > pImpl->covarianceThreshold) {
        float homeostaticCorrection = -0.1f * (newWeight - std::copysign(pImpl->covarianceThreshold, newWeight));
        newWeight += homeostaticCorrection;
        newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    }
    
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
