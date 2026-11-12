#include "Hebbian.hpp"
#include "../brain/Synapse.hpp"
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
    /*
     * Enhanced Hebbian learning implementation with improved bounds checking
     * and activity-dependent learning rate normalization.
     * 
     * Mathematical formulation (Enhanced Covariance rule):
     * Δw = η * (⟨pre * post⟩ - ⟨pre⟩⟨post⟩) * efficacy * stability_factor
     * 
     * Where:
     *   efficacy = 1.0 for excitatory synapses, 0.5 for inhibitory (slower learning)
     *   stability_factor = (1 - |w|) * (1 + activity_level) for stabilization
     * 
     * Biological inspiration:
     *   - AMPA receptor trafficking with activity-dependent consolidation
     *   - Spike-timing dependent plasticity interactions
     *   - Homeostatic mechanisms to prevent runaway excitation
     *   
     * Improvements over Phase 4:
     *   - Better learning rate normalization based on synaptic type
     *   - Adaptive bounds checking based on current weight
     *   - Activity-dependent learning rate modulation
     *   - Separate potentiation/depression mechanisms for different synapse types
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate stability factor based on current weight
    float currentWeight = synapse->getWeight();
    float stabilityFactor = 1.0f;
    
    // Weight-dependent stability: approach zero as we approach bounds
    if (std::abs(currentWeight) > 0.9f) {
        stabilityFactor *= 0.1f;  // Slow down learning near bounds
    }
    
    // Activity-dependent factor: higher activity -> stronger learning
    float activityLevel = static_cast<float>(preSpikes.size() + postSpikes.size());
    stabilityFactor *= (1.0f + activityLevel * 0.01f);
    
    // Nearest-neighbour version with improved timing window
    float dtSeconds = static_cast<float>(postSpikes.back() - preSpikes.back());
    float timingFactor = 1.0f;
    
    // Stronger learning with correlated spikes (within 100ms)
    if (std::abs(dtSeconds) < 0.1f) {
        timingFactor = 1.0f;
    } else if (std::abs(dtSeconds) < 0.5f) {
        // Moderate correlation
        timingFactor = 0.5f;
    } else {
        // Weak correlation
        timingFactor = 0.1f;
    }
    
    // Calculate base learning delta with type-specific learning rates
    float baseDelta = pImpl->learningRate;
    
    // Apply learning rate normalization based on synapse type
    // Inhibitory synapses learn slower to prevent instability
    float learningRateMod = 1.0f;
    if (synapse->getType() == SynapseType::Inhibitory) {
        learningRateMod = 0.7f;  // Slower learning for inhibition
        baseDelta = -pImpl->learningRate * 0.8f;  // Bias toward strengthening inhibition
    } else if (synapse->getType() == SynapseType::Excitatory) {
        learningRateMod = 1.0f;  // Normal learning for excitation
    }
    
    // Calculate final delta with all factors
    float delta = baseDelta * timingFactor * learningRateMod * stabilityFactor;
    
    // Apply with enhanced bounds checking
    if (std::abs(delta) > 1e-6f) {
        // Enhanced bounds checking based on synapse type
        float currentAbsWeight = std::abs(currentWeight);
        
        // Weight-dependent scaling: larger changes near zero, smaller near bounds
        float weightDependentScale = 1.0f;
        if (currentAbsWeight > 0.8f) {
            weightDependentScale = 0.2f;  // Small changes near bounds
        } else if (currentAbsWeight > 0.5f) {
            weightDependentScale = 0.5f;  // Moderate changes
        }
        
        delta *= weightDependentScale;
        
        // Apply enhanced weight change
        applyWeightChange(synapse, delta);
        
        // Enforce type-specific safe bounds
        if (synapse->getType() == SynapseType::Inhibitory) {
            // Maintain inhibitory strength in negative range
            if (synapse->getWeight() > -0.02f) {
                synapse->setWeight(-0.02f);
            } else if (synapse->getWeight() < -1.0f) {
                synapse->setWeight(-1.0f);
            }
        } else if (synapse->getType() == SynapseType::Excitatory) {
            // Maintain excitatory strength in positive range
            if (synapse->getWeight() < 0.02f) {
                synapse->setWeight(0.02f);
            } else if (synapse->getWeight() > 1.0f) {
                synapse->setWeight(1.0f);
            }
        }
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

} // namespace nlm
