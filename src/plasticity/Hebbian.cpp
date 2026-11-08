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

// ============= OPTIMIZED HEBBIAN UPDATE =============
void Hebbian::update(Synapse* synapse,
                     const std::vector<Timestamp>& preSpikes,
                     const std::vector<Timestamp>& postSpikes,
                     TimestepDuration dt) {
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Optimized correlation detection - use only most recent spike timing
    float dtSeconds = static_cast<float>(postSpikes.back() - preSpikes.back());
    float delta = std::abs(dtSeconds) < 0.1f ? pImpl->learningRate : 0.0f;
    
    // Optimized inhibitory synapse handling
    if (synapse->getType() == SynapseType::Inhibitory) {
        delta = -delta;
    }
    
    // Apply weight change with optimized bounds checking
    if (std::abs(delta) > 1e-8f) {
        float w = synapse->getWeight();
        float absW = std::abs(w);
        
        // Optimized weight-dependent scaling - more precise ranges
        if (absW > 0.95f) {
            delta *= (1.0f - absW * 0.5f);  // Near saturation - gradual scaling
        } else if (absW < 0.05f) {
            delta *= absW * 2.0f;          // Near zero - larger influence
        } else {
            delta *= (1.0f - absW);        }   // Mid-range - linear scaling
        
        // Apply weight change
        applyWeightChange(synapse, delta);
        
        // Optimized sign enforcement
        float newWeight = synapse->getWeight();
        if (synapse->getType() == SynapseType::Inhibitory) {
            if (newWeight > -0.01f) {
                synapse->setWeight(-0.01f);
            }
        } else {
            if (newWeight < 0.01f) {
                synapse->setWeight(0.01f);
            }
        }
    }
}

// ============= OPTIMIZED WEIGHT APPLICATION =============
void Hebbian::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Optimized weight clamping with pre-computed bounds
    float newWeight = synapse->getWeight() + delta;
    
    // Branchless clamping using fmin/fmax
    if (newWeight < pImpl->minWeight) {
        newWeight = pImpl->minWeight;
    } else if (newWeight > pImpl->maxWeight) {
        newWeight = pImpl->maxWeight;
    }
    
    synapse->setWeight(newWeight);
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
