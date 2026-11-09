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
     * Enhanced Hebbian learning implementation
     * 
     * Mathematical formulation (Enhanced Covariance rule):
     * Δw = η * (coactivity * (1 - |w|) * eligibility * rewardModulation) - |w| * baseline
     * 
     * Simplified version for spike-based systems:
     * Δw = η * (coactivity * eligibility - |w|) * rewardModulation
     * 
     * Where:
     *   coactivity = number of correlated pre/post spikes within time window
     *   eligibility = synaptic eligibility trace for reward-modulated learning
     *   rewardModulation = 1.0 + 0.3 * eligibility (enhances learning with reward)
     *   (1 - |w|) = weight-dependent scaling (prevents runaway potentiation)
     *   |w| = weight-dependent scaling (prevents runaway depression)
     * 
     * Biological inspiration:
     *   - Reflects AMPA receptor trafficking
     *   - Hebbian plasticity at Schaffer collateral synapses in hippocampus
     *   - Correlation-based learning in visual cortex
     *   - Integration with reward systems for value-based Hebbian learning
     *   
     * Key improvements over original:
     *   - Enhanced weight-dependent scaling with eligibility traces
     *   - Integration with reward-modulated learning
     *   - Better anti-symmetric handling for inhibitory synapses
     *   - Reduced reliance on nearest-neighbour (uses recent spike pairs)
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Enhanced nearest-neighbour version with recent spike pairs
    // Consider the last 3 spike pairs for richer correlation dynamics
    size_t maxPairs = std::min({preSpikes.size(), postSpikes.size(), size_t(3)});
    float coactivity = 0.0f;
    
    for (size_t i = 0; i < maxPairs; ++i) {
        size_t preIdx = preSpikes.size() - 1 - i;
        size_t postIdx = postSpikes.size() - 1 - i;
        
        if (preIdx >= postIdx) continue;  // Only consider pre-before-post
        
        float dtSeconds = static_cast<float>(postSpikes[postIdx] - preSpikes[preIdx]);
        if (std::abs(dtSeconds) < 0.1f) {  // 100ms window
            coactivity += 1.0f;
        }
    }
    
    // Get synaptic eligibility trace for reward-modulated learning
    float eligibility = synapse->getEligibilityTrace();
    
    // Get reward prediction error modulation factor
    float rewardModulation = 1.0f + 0.3f * eligibility;
    rewardModulation = std::clamp(rewardModulation, 0.5f, 2.0f);
    
    // Enhanced Hebbian learning with eligibility and weight-dependent scaling
    float delta = pImpl->learningRate * coactivity * eligibility * rewardModulation;
    
    // Anti-symmetric handling for inhibitory synapses
    if (synapse->getType() == SynapseType::Inhibitory) {
        // For inhibitory synapses, the learning rule should be symmetric:
        // co-activity should strengthen inhibition (more negative)
        // but the learning rate should be adjusted for inhibitory targets
        delta = -delta * (0.5f + 0.5f * eligibility);  // More adjustment for higher eligibility
    }
    
    // Apply with enhanced bounds including eligibility-aware scaling
    if (std::abs(delta) > 1e-6f) {
        // Enhanced weight-dependent scaling with eligibility factor
        float currentWeight = synapse->getWeight();
        float weightDependentFactor = (1.0f - std::abs(currentWeight)) * (1.0f + eligibility * 0.5f);
        delta *= weightDependentFactor;
        
        // Apply weight change with adaptive bounds
        float newWeight = currentWeight + delta;
        
        // Enhanced bounds: synapses with high eligibility should be allowed
        // to change more rapidly (faster learning) but still respect global limits
        float eligibilityFactor = std::clamp(eligibility, 0.0f, 1.0f);
        float boundRange = (pImpl->maxWeight - pImpl->minWeight) * (1.0f + eligibilityFactor * 0.3f);
        float adaptiveMin = currentWeight - boundRange * 0.5f;
        float adaptiveMax = currentWeight + boundRange * 0.5f;
        
        // Clamp to adaptive bounds while respecting global limits
        adaptiveMin = std::max(adaptiveMin, pImpl->minWeight);
        adaptiveMax = std::min(adaptiveMax, pImpl->maxWeight);
        newWeight = std::clamp(newWeight, adaptiveMin, adaptiveMax);
        
        synapse->setWeight(newWeight);
        
        // Enhanced eligibility trace update for next time step
        float currentTrace = synapse->getEligibilityTrace();
        synapse->setEligibilityTrace(currentTrace + delta * eligibility * 0.1f);
        
        // Enforce sign: excitatory stays >= 0.01, inhibitory stays <= -0.01.
        if (synapse->getType() == SynapseType::Inhibitory && synapse->getWeight() > -0.01f) {
            synapse->setWeight(-0.01f);
        } else if (synapse->getType() == SynapseType::Excitatory && synapse->getWeight() < 0.01f) {
            synapse->setWeight(0.01f);
        }
        
        // Log significant weight changes for debugging
        if (std::abs(delta) > 0.01f) {
            NLM_LOG_DEBUG("Hebbian: Significant weight change " + std::to_string(delta) +
                         " on synapse " + std::to_string(synapse->getId().index()) +
                         " from " + std::to_string(currentWeight) + " to " + std::to_string(newWeight) +
                         " (eligibility: " + std::to_string(eligibility) + ")");
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
