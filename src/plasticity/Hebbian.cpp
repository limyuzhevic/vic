#include "Hebbian.hpp"
#include "../brain/Synapse.hpp"
#include <algorithm>

namespace nlm {

struct Hebbian::Impl {
    float learningRate;
    float maxWeight;
    float minWeight;
    float covarianceThreshold;  // For covariance rule
    
    Impl() : learningRate(0.015f), maxWeight(1.0f), minWeight(-1.0f),
             covarianceThreshold(0.1f) {}
};

Hebbian::Hebbian() : pImpl(new Impl) {}

Hebbian::~Hebbian() = default;

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
    
    // Enhanced Hebbian learning with better pattern selectivity
    // The original covariance rule (Δw = η * (⟨pre * post⟩ - ⟨pre⟩⟨post⟩))
    // encourages correlated firing, which is good for pattern learning.
    // However, we need to enhance it to prevent runaway saturation and
    // promote selective pattern development.
    
    float delta = 0.0f;
    
    // Multi-factor Hebbian learning with exploration-exploitation balance
    float dtSeconds = static_cast<float>(postSpikes.back() - preSpikes.back());
    
    // Timing-based Hebbian: only learn when spikes are well-timed
    if (std::abs(dtSeconds) < 0.1f) {
        // Strong Hebbian learning for well-timed spikes
        delta += pImpl->learningRate * 1.5f;  // Enhanced learning rate
    } else if (std::abs(dtSeconds) < 0.3f) {
        // Moderate Hebbian learning for moderately-timed spikes
        delta += pImpl->learningRate * 0.7f;  // Reduced learning rate
    } else {
        // Minimal learning for poorly-timed spikes
        delta += 0.0f;
    }
    
    // Pattern consistency bonus: strengthen synapses that show consistent patterns
    if (preSpikes.size() >= 3 && postSpikes.size() >= 3) {
        // Analyze spike timing consistency for pattern selectivity
        float preMean = 0.0f, postMean = 0.0f;
        float preVar = 0.0f, postVar = 0.0f;
        
        // Sample recent spike times for pattern analysis
        size_t sampleSize = std::min({preSpikes.size(), postSpikes.size(), size_t(3)});
        
        for (size_t i = 0; i < sampleSize; ++i) {
            size_t preIdx = preSpikes.size() - 1 - i;
            size_t postIdx = postSpikes.size() - 1 - i;
            
            if (preIdx < preSpikes.size() && postIdx < postSpikes.size()) {
                preMean += preSpikes[preIdx];
                postMean += postSpikes[postIdx];
            }
        }
        
        preMean /= sampleSize;
        postMean /= sampleSize;
        
        for (size_t i = 0; i < sampleSize; ++i) {
            size_t preIdx = preSpikes.size() - 1 - i;
            size_t postIdx = postSpikes.size() - 1 - i;
            
            if (preIdx < preSpikes.size() && postIdx < postSpikes.size()) {
                float preDiff = preSpikes[preIdx] - preMean;
                float postDiff = postSpikes[postIdx] - postMean;
                preVar += preDiff * preDiff;
                postVar += postDiff * postDiff;
            }
        }
        
        preVar /= sampleSize;
        postVar /= sampleSize;
        
        // Low variance indicates consistent timing - good pattern
        float preCV = (preMean > 0.0f) ? std::sqrt(preVar) / preMean : 1.0f;
        float postCV = (postMean > 0.0f) ? std::sqrt(postVar) / postMean : 1.0f;
        
        float patternConsistency = std::max(0.5f, 1.0f - (preCV + postCV) * 0.5f);
        delta *= patternConsistency;  // Enhance learning for consistent patterns
    }
    
    // Exploration bonus: encourage learning for underrepresented synapses
    // This prevents all synapses from converging to the same value
    float currentWeight = std::abs(synapse->getWeight());
    if (currentWeight < 0.1f) {
        // Underrepresented synapses get exploration bonus
        delta *= 1.2f;  // 20% bonus for exploration
    } else if (currentWeight > 0.8f) {
        // Overrepresented synapses get exploration penalty
        delta *= 0.8f;  // 20% penalty for exploitation
    }
    
    // Apply sign correction for inhibitory synapses
    if (synapse->getType() == SynapseType::Inhibitory) {
        delta = -delta;  // strengthen inhibition, don't flip it to excitatory
    }
    
    // Apply with bounds. Soft bound, Phase 4 (see STDP.cpp): Hebbian
    if (std::abs(delta) > 1e-6f) {
        delta *= (1.0f - std::abs(synapse->getWeight()));
        applyWeightChange(synapse, delta);
        
        // Enforce proper sign: excitatory stays positive, inhibitory stays negative
        if (synapse->getType() == SynapseType::Inhibitory && synapse->getWeight() > -0.01f) {
            synapse->setWeight(-0.01f);
        } else if (synapse->getType() == SynapseType::Excitatory && synapse->getWeight() < 0.01f) {
            synapse->setWeight(0.01f);
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
