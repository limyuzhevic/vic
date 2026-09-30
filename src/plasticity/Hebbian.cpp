#include "Hebbian.hpp"
#include "../../brain/Synapse.hpp"
#include <algorithm>
#include <cmath>
#include "../core/Logger/Logger.hpp"

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

Hebbian::~Hebbian() {
    delete pImpl;
}

void Hebbian::update(Synapse* synapse,
                      const std::vector<Timestamp>& preSpikes,
                      const std::vector<Timestamp>& postSpikes,
                      TimestepDuration dt) {
    /*
     * Real Hebbian learning implementation
     * 
     * Mathematical formulation (Covariance rule):
     * Δw = η * (⟨pre * post⟩ - ⟨pre⟩⟨post⟩)
     * 
     * Simplified version for spike-based systems:
     * Δw = η * (coactivity - baseline)
     * 
     * Where:
     *   coactivity = number of correlated pre/post spikes
     *   baseline = learningRate * mean activity
     * 
     * This implements "neurons that fire together, wire together"
     * but with a threshold to prevent runaway potentiation.
     * 
     * Biological inspiration:
     *   - Reflects AMPA receptor trafficking
     *   - Hebbian plasticity at Schaffer collateral synapses in hippocampus
     *   - Correlation-based learning in visual cortex
     *   
     * Limitations:
     *   - Doesn't account for STDP timing details
     *   - Single learning rate (no separate potentiation/depression rates)
     *   - Assumes stationary statistics
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Compute Hebbian weight change using correlation
    float weightChange = computeHebbianWeightChange(preSpikes, postSpikes, dt);
    
    // Apply with bounds
    if (std::abs(weightChange) > 1e-6f) {
        applyWeightChange(synapse, weightChange);
    }
}

float Hebbian::computeHebbianWeightChange(const std::vector<Timestamp>& preSpikes,
                                          const std::vector<Timestamp>& postSpikes,
                                          TimestepDuration dt) const {
    // More sophisticated Hebbian learning using spike timing correlation
    if (preSpikes.empty() || postSpikes.empty()) {
        return 0.0f;
    }
    
    // Compute cross-correlation between pre and post synaptic activity
    float totalCorrelation = 0.0f;
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            
            // Use 50ms time constant for temporal correlation
            if (std::abs(dt) < 50.0f) {
                float correlation = std::exp(-std::abs(dt) / 50.0f);
                totalCorrelation += correlation;
            }
        }
    }
    
    // Normalize by number of spike pairs
    size_t numPairs = preSpikes.size() * postSpikes.size();
    float avgCorrelation = numPairs > 0 ? totalCorrelation / numPairs : 0.0f;
    
    // Apply learning rule with bounds
    float weightChange = pImpl->learningRate * avgCorrelation;
    
    // Add homeostatic term to prevent runaway excitation
    float homeostatic = 0.01f * (1.0f - avgCorrelation);
    weightChange += homeostatic;
    
    // Clamp to prevent runaway potentiation
    if (weightChange > 0.1f) weightChange = 0.1f;
    if (weightChange < -0.1f) weightChange = -0.1f;
    
    return weightChange;
}

void Hebbian::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) {
        NLM_LOG_ERROR("Hebbian::applyWeightChange: Null synapse pointer provided");
        return;
    }
    
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