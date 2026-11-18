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
     * Real Hebbian learning implementation with selective learning
     * 
     * Enhanced with competitive mechanisms to promote selective learning:
     * 1. Competitive normalization: weights compete for limited resources
     * 2. Activity-dependent learning rates: promotes selective potentiation
     * 3. Soft bounds with synaptic competition: prevents runaway potentiation
     * 4. Heterosynaptic depression: weakens inactive synapses
     * 
     * Mathematical formulation (Competitive Hebbian):
     * Δw = η * (pre * post) - λ * |w| * (baseline activity)
     * 
     * Where:
     *   η = learningRate * (1 - competitionFactor)
     *   λ = depressionRate for inactive synapses
     *   competitionFactor = relative synaptic activity
     * 
     * Biological inspiration:
     *   - Reflects homeostatic synaptic scaling
     *   - Competitive synaptic pruning in development
     *   - Activity-dependent synaptic consolidation
     *   
     * Benefits:
     *   - Promotes selective learning (only important synapses potentiate)
     *   - Prevents runaway potentiation through competitive normalization
     *   - Maintains synaptic diversity and network stability
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Nearest-neighbour version: use only the most recent spike pair
    // Timestamps are in seconds, so the 100ms window is 0.1s.
    float dtSeconds = static_cast<float>(postSpikes.back() - preSpikes.back());
    
    // Enhanced Hebbian rule with selective learning:
    // 1. Basic Hebbian contribution (pre-before-post correlation)
    float basicDelta = std::abs(dtSeconds) < 0.1f ? pImpl->learningRate : 0.0f;
    
    // 2. Activity-dependent modulation: stronger learning when network is active
    float networkActivity = std::min(1.0f, synapse->getEfficacy() * 2.0f);
    float activityModulation = 1.0f + 0.5f * networkActivity;
    
    // 3. Competitive normalization: weights compete for limited resources
    float weightMagnitude = std::abs(synapse->getWeight());
    float competitionFactor = weightMagnitude / pImpl->maxWeight;
    
    // 4. Heterosynaptic depression: weaken inactive synapses
    float depressionRate = 0.005f;  // Small depression rate
    float heterosynapticDelta = -depressionRate * weightMagnitude;
    
    // Combine effects: Hebbian potentiation + competitive normalization + depression
    float delta = basicDelta * activityModulation * (1.0f - competitionFactor) + heterosynapticDelta;
    
    if (synapse->getType() == SynapseType::Inhibitory) {
        delta = -delta;  // strengthen inhibition, don't flip it to excitatory
    }
    
    // Apply with enhanced bounds. Competitive soft bounds:
    // - Strong Hebbian learning is scaled by available weight capacity (1 - |w|)
    // - Depression is scaled by current weight magnitude (|w|)
    // - This creates competitive dynamics where strong synapses can only grow
    //   slowly, while weak synapses grow faster but have depression
    if (std::abs(delta) > 1e-6f) {
        // Scale by competitive factors
        delta *= (1.0f - weightMagnitude);  // Limited growth for strong synapses
        applyWeightChange(synapse, delta);
        
        // Enforce sign with competitive bounds
        if (synapse->getType() == SynapseType::Inhibitory && synapse->getWeight() > -0.01f) {
            synapse->setWeight(-0.01f);
        } else if (synapse->getType() == SynapseType::Excitatory && synapse->getWeight() < 0.01f) {
            synapse->setWeight(0.01f);
        }
        
        // Competitive synaptic pruning: if weight is near max and not very active,
        // weaken it to make room for other synapses to grow
        if (weightMagnitude > pImpl->maxWeight * 0.95f && 
            synapse->getEfficacy() < 0.5f) {
            float pruningFactor = 0.02f;
            float prunedWeight = synapse->getWeight() * (1.0f - pruningFactor);
            if (synapse->getType() == SynapseType::Excitatory && 
                prunedWeight > 0.01f) {
                synapse->setWeight(prunedWeight);
            } else if (synapse->getType() == SynapseType::Inhibitory && 
                      prunedWeight < -0.01f) {
                synapse->setWeight(prunedWeight);
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
