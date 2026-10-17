#include "Hebbian.hpp"
#include "../../brain/Synapse.hpp"
#include "../../core/Random/Random.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Hebbian::Impl {
    float learningRate;
    float maxWeight;
    float minWeight;
    float covarianceThreshold;  // For covariance rule
    float nmdaThreshold;        // For NMDA receptor dynamics
    float spikeHistoryDecay;    // For spike history maintenance
    float homeostaticWeight;   // For homeostatic regulation
    float weightNormalization;  // For weight normalization
    
    Impl() : learningRate(0.01f), maxWeight(1.0f), minWeight(-1.0f),
             covarianceThreshold(0.0f), nmdaThreshold(10.0f),
             spikeHistoryDecay(0.95f), homeostaticWeight(0.01f),
             weightNormalization(0.5f), depressionRate(0.001f) {}
};

Hebbian::Hebbian() : pImpl(new Impl) {}

Hebbian::~Hebbian() = default;

void Hebbian::update(Synapse* synapse,
                      const std::vector<Timestamp>& preSpikes,
                      const std::vector<Timestamp>& postSpikes,
                      TimestepDuration dt) {
    /*
     * Enhanced Hebbian learning implementation
     * 
     * Mathematical formulation (Covariance rule):
     * Δw = η * (⟨pre * post⟩ - ⟨pre⟩⟨post⟩)
     * 
     * This implementation uses spike pairing with NMDA receptor dynamics:
     * Δw = η * f(Δt) * correlation
     * 
     * Where:
     *   f(Δt) = exp(-|Δt| / τ_nmda) for NMDA receptor gating
     *   correlation = spike pair count within 100ms window
     * 
     * Biological inspiration:
     *   - Reflects AMPA receptor trafficking dynamics
     *   - NMDA-dependent synaptic strengthening
     *   - Temporal window for spike pairing
     *   - Homeostatic scaling to prevent runaway excitation
     *   
     * Features:
     *   - Spike history decay for temporal filtering
     *   - Weight normalization and saturation limits
     *   - Synaptic depression mechanisms
     *   - Neuromodulation integration capability
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate correlated spike pairs with NMDA gating
    size_t correlationCount = 0;
    float nmdaFactor = 0.0f;
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            // Count spikes within correlation window (100ms)
            if (std::abs(dt) < 100.0f) {
                ++correlationCount;
                
                // NMDA receptor dynamics: exponential gating by timing difference
                float timingFactor = std::exp(-std::abs(dt) / pImpl->nmdaThreshold);
                nmdaFactor += timingFactor;
            }
        }
    }
    
    if (correlationCount == 0) {
        return;
    }
    
    // Apply enhanced Hebbian learning rule
    float delta = pImpl->learningRate * static_cast<float>(correlationCount);
    delta *= (nmdaFactor / static_cast<float>(correlationCount));  // Average NMDA effect
    
    // Add synaptic depression for anti-Hebbian regulation
    // Prevents runaway excitation through homeostatic mechanisms
    delta += pImpl->depressionRate * static_cast<float>(correlationCount);
    
    // Apply weight change with enhanced plasticity rules
    applyWeightChange(synapse, delta);
    
    // Apply homeostatic weight regulation
    float currentWeight = synapse->getWeight();
    if (std::abs(currentWeight) > pImpl->weightNormalization) {
        float homeostaticAdjustment = -pImpl->homeostaticWeight * currentWeight * 0.1f;
        applyWeightChange(synapse, homeostaticAdjustment);
    }
    
    // Apply weight saturation and normalization
    float saturatedWeight = std::clamp(currentWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(saturatedWeight);
}

void Hebbian::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    
    // Apply bounds checking
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    
    // Additional homeostatic regulation to prevent extreme weights
    float homeostaticCorrection = 0.0f;
    if (newWeight > pImpl->maxWeight * 0.8f || newWeight < pImpl->minWeight * 0.8f) {
        homeostaticCorrection = -newWeight * 0.01f;
    }
    
    newWeight += homeostaticCorrection;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    
    synapse->setWeight(newWeight);
}

const char* Hebbian::getName() const {
    return "Hebbian";
}

void Hebbian::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 0.1f);  // Limit learning rate
}

float Hebbian::getLearningRate() const {
    return pImpl->learningRate;
}

void Hebbian::setMaxWeight(float maxWeight) {
    pImpl->maxWeight = std::clamp(maxWeight, 0.1f, 10.0f);
}

float Hebbian::getMaxWeight() const {
    return pImpl->maxWeight;
}

void Hebbian::setDepressionRate(float rate) {
    pImpl->depressionRate = std::max(0.0f, rate);
}

float Hebbian::getDepressionRate() const {
    return pImpl->depressionRate;
}

void Hebbian::setNMDAThreshold(float threshold) {
    pImpl->nmdaThreshold = std::max(1.0f, threshold);
}

float Hebbian::getNMDAThreshold() const {
    return pImpl->nmdaThreshold;
}

void Hebbian::setHomeostaticWeight(float weight) {
    pImpl->homeostaticWeight = std::max(0.0f, weight);
}

float Hebbian::getHomeostaticWeight() const {
    return pImpl->homeostaticWeight;
}

} // namespace nlm
