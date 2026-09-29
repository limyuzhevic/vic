#include "PlasticityRule.hpp"
#include "../brain/Synapse.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>
#include <numeric>

namespace nlm {

bool PlasticityRule::isEnabled() const {
    return enabled_;
}

void PlasticityRule::setEnabled(bool enabled) {
    enabled_ = enabled;
}

struct HebbianRule::Impl {
    float learningRate;
    float maxWeight;
    float minWeight;
    float normalizationFactor;  // For weight normalization
    
    // Advanced parameters for biological realism
    float ojaNormalization;      // Oja's rule normalization parameter
    float bcmTheta;              // BCM theory threshold
    float bcmLearningRate;       // BCM learning rate
    
    Impl() : learningRate(0.01f), maxWeight(1.0f), minWeight(-1.0f),
             normalizationFactor(1.0f), ojaNormalization(0.01f),
             bcmTheta(0.5f), bcmLearningRate(0.01f) {}
};

HebbianRule::HebbianRule() : pImpl(new Impl) {}

HebbianRule::~HebbianRule() = default;

void HebbianRule::update(Synapse* synapse,
                         const std::vector<Timestamp>& preSpikes,
                         const std::vector<Timestamp>& postSpikes,
                         TimestepDuration dt) {
    /*
     * Real Hebbian learning implementation with advanced features
     * 
     * Mathematical formulations:
     * 
     * 1. Classical Hebbian (Raw coactivity):
     * Δw = η * Σ(pre_post)
     * 
     * 2. Oja's rule (Weight-normalized learning):
     * Δw = η * (x * y - w * y²) / sqrt(w² + ε)
     * 
     * 3. BCM theory (Threshold-based learning):
     * Δw = η * y * (y - θ(y)) where θ(y) = y² / (y² + κ²)
     * 
     * 4. Anti-Hebbian (Decreased connection):
     * Δw = -η * Σ(pre_post)  for negative weights
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float currentWeight = synapse->getWeight();
    float preRate = static_cast<float>(preSpikes.size()) / 1000.0f;
    float postRate = static_cast<float>(postSpikes.size()) / 1000.0f;
    
    // Calculate classical Hebbian contribution
    float classicalHebbian = 0.0f;
    if (synapse->isExcitatory() && currentWeight > 0) {
        // For excitatory synapses with positive weights
        size_t coincidences = 0;
        for (Timestamp pre : preSpikes) {
            for (Timestamp post : postSpikes) {
                if (std::abs(pre - post) < 50.0f) {  // 50ms window
                    ++coincidences;
                }
            }
        }
        classicalHebbian = pImpl->learningRate * static_cast<float>(coincidences);
    } else if (synapse->isInhibitory() && currentWeight < 0) {
        // For inhibitory synapses with negative weights, apply anti-Hebbian
        size_t coincidences = 0;
        for (Timestamp pre : preSpikes) {
            for (Timestamp post : postSpikes) {
                if (std::abs(pre - post) < 50.0f) {
                    ++coincidences;
                }
            }
        }
        classicalHebbian = -pImpl->learningRate * static_cast<float>(coincidences);
    }
    
    // Calculate Oja's rule contribution (weight normalization)
    float ojaContribution = 0.0f;
    if (std::abs(currentWeight) > 0.01f) {
        float normalization = std::sqrt(currentWeight * currentWeight + pImpl->ojaNormalization);
        ojaContribution = pImpl->learningRate * (preRate * postRate - 
                currentWeight * postRate * postRate) / normalization;
    }
    
    // Calculate BCM theory contribution
    float bcmContribution = 0.0f;
    float bcmTheta = pImpl->bcmTheta > 0.0f ? pImpl->bcmTheta : 0.5f;
    float bcmThetaFunc = postRate * postRate / (postRate * postRate + bcmTheta * bcmTheta);
    bcmContribution = pImpl->bcmLearningRate * postRate * (postRate - bcmThetaFunc);
    
    // Combine contributions with adaptive weighting
    float totalDelta = classicalHebbian * 0.5f + ojaContribution * 0.3f + bcmContribution * 0.2f;
    
    // Apply with bounds and constraints
    if (std::abs(totalDelta) > 1e-6f) {
        // Prevent runaway weight growth
        float targetWeight = currentWeight + totalDelta;
        
        if (synapse->isExcitatory()) {
            // For excitatory synapses, keep weights in reasonable range
            targetWeight = std::clamp(targetWeight, pImpl->minWeight, pImpl->maxWeight);
        } else if (synapse->isInhibitory()) {
            // For inhibitory synapses, keep weights negative but not too large
            targetWeight = std::clamp(targetWeight, -1.0f, 0.0f);
        }
        
        applyWeightChange(synapse, targetWeight - currentWeight);
    }
}

void HebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Apply weight change with biological constraints
    float newWeight = synapse->getWeight() + delta;
    
    // Biological constraints on synaptic weights
    if (synapse->isExcitatory()) {
        newWeight = std::clamp(newWeight, -0.1f, 1.0f);  // Excitatory: small negative to positive
    } else if (synapse->isInhibitory()) {
        newWeight = std::clamp(newWeight, -1.0f, -0.01f);  // Inhibitory: small negative
    } else {
        newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    }
    
    synapse->addToWeight(newWeight - synapse->getWeight());
}

const char* HebbianRule::getName() const {
    return "Hebbian";
}

void HebbianRule::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

float HebbianRule::getLearningRate() const {
    return pImpl->learningRate;
}

} // namespace nlm
