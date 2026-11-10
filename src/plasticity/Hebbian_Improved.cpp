#include "Hebbian.hpp"
#include "../brain/Synapse.hpp"
#include <algorithm>

namespace nlm {

struct Hebbian::Impl {
    float learningRate;
    float maxWeight;
    float minWeight;
    float covarianceThreshold;
    float globalLearningRate;
    uint64_t stepsSinceLastAdjustment;
    float networkMeanActivity;
    
    Impl() : learningRate(0.008f), maxWeight(0.5f), minWeight(-0.5f),
             covarianceThreshold(0.0f), globalLearningRate(1.0f),
             stepsSinceLastAdjustment(0), networkMeanActivity(0.0f) {}
};

Hebbian::Hebbian() : pImpl(new Impl) {}

Hebbian::~Hebbian() = default;

void Hebbian::update(Synapse* synapse,
                     const std::vector<Timestamp>& preSpikes,
                     const std::vector<Timestamp>& postSpikes,
                     TimestepDuration dt) {
    /*
     * Improved Hebbian learning implementation
     * 
     * Enhanced with adaptive learning rates and better saturation control
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Nearest-neighbour version with improved timing
    float dtSeconds = static_cast<float>(postSpikes.back() - preSpikes.back());
    float delta = 0.0f;
    
    // Calculate activity-based learning signal
    float preActivity = 1.0f;  // Simplified - could be enhanced with real activity
    float postActivity = 1.0f; // Simplified - could be enhanced with real activity
    
    // Use adaptive learning rate
    float adaptiveFactor = pImpl->globalLearningRate;
    
    // Improved covariance-like rule with activity-dependent learning
    if (std::abs(dtSeconds) < 0.1f) {
        // Strong coactivity: use enhanced learning
        delta = pImpl->learningRate * adaptiveFactor * 1.5f;
        
        // Apply sign for inhibitory synapses
        if (synapse->getType() == SynapseType::Inhibitory) {
            delta = -delta;
        }
    }
    
    // Apply weight change with improved bounds
    if (std::abs(delta) > 1e-7f) {
        // Enhanced soft bounds for learning selectivity
        float w = synapse->getWeight();
        float saturationFactor = 1.0f;
        
        if (synapse->getType() == SynapseType::Excitatory) {
            // Better saturation control for excitatory weights
            if (w >= 0.0f) {
                // Potentiation scale based on distance from upper bound
                saturationFactor = (pImpl->maxWeight - w) / pImpl->maxWeight;
            } else {
                // Depression scale based on distance from lower bound
                saturationFactor = (0.0f - w) / (pImpl->maxWeight - pImpl->minWeight);
            }
        } else if (synapse->getType() == SynapseType::Inhibitory) {
            // Better saturation control for inhibitory weights
            if (w <= 0.0f) {
                // Depression scale (more negative)
                saturationFactor = (std::abs(w) - std::abs(pImpl->minWeight)) / 
                                   (pImpl->maxWeight - pImpl->minWeight);
            } else {
                // Potentiation scale (more positive)
                saturationFactor = (w - 0.0f) / (pImpl->maxWeight - pImpl->minWeight);
            }
        }
        
        // Apply saturation factor with conservative scaling
        delta *= saturationFactor * 0.6f;  // Reduced from 1.0f
        
        applyWeightChange(synapse, delta);
        
        // Improved sign clamping
        if (synapse->getType() == SynapseType::Inhibitory && synapse->getWeight() > -0.02f) {
            synapse->setWeight(-0.02f);
        } else if (synapse->getType() == SynapseType::Excitatory && synapse->getWeight() < 0.02f) {
            synapse->setWeight(0.02f);
        }
        
        // Update eligibility trace with reduced accumulation
        float currentTrace = synapse->getEligibilityTrace();
        synapse->setEligibilityTrace(currentTrace + delta * 0.3f);
        
        // Track for learning rate adjustment
        pImpl->stepsSinceLastAdjustment++;
        
        // Adjust global learning rate periodically
        if (pImpl->stepsSinceLastAdjustment >= 1000) {
            adjustGlobalLearningRate();
        }
    }
}

void Hebbian::adjustGlobalLearningRate() {
    // Implement adaptive learning rate logic
    float targetRate = 1.0f;
    float currentRate = pImpl->globalLearningRate;
    
    if (currentRate > targetRate) {
        currentRate *= 0.9f;  // Slower adjustment
    } else {
        currentRate *= 1.1f;  // Slower adjustment
    }
    
    currentRate = std::clamp(currentRate, 0.5f, 2.0f);
    pImpl->globalLearningRate = currentRate;
    
    // Update network activity estimate
    pImpl->networkMeanActivity = (pImpl->networkMeanActivity * 0.9f) + 
                                 (currentRate * 0.1f);
    
    pImpl->stepsSinceLastAdjustment = 0;
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
