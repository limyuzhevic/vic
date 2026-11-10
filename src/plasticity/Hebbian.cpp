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
     * Enhanced Hebbian learning implementation with activity-dependent plasticity
     * 
     * Mathematical formulation:
     * Implements enhanced covariance rule with triplet interactions and homeostatic mechanisms
     * 
     * Enhanced covariance rule:
     * Δw = η(a) * (coactivity - baseline * S(w))
     * 
     * Where:
     *   a = neural activity level (0-1)
     *   η(a) = learning rate function (lower at high activity)
     *   S(w) = synaptic normalization factor
     *   coactivity = measure of correlated pre/post spike timing
     *   baseline = expected coactivity under independence
     *   
     * Biological inspiration:
     *   - Reflects activity-dependent synaptic tagging
     *   - Implements "neurons that fire together, wire together"
     *   - Includes homeostatic regulation to maintain stability
     *   - Triplet-based learning enhances pattern selectivity
     *   
     * Stability features:
     *   - Activity-dependent learning rates
     *   - Synaptic competition and normalization
     *   - Homeostatic mechanisms
     *   - Soft bounds with learning history
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Nearest-neighbour version: the old all-pairs count made delta grow
    // with history length (up to 100x100 pairs) and saturated every weight
    // to its bound in one step. One correlated recent pair = one step.
    // Timestamps are in seconds, so the 100ms window is 0.1s.
    float dtSeconds = static_cast<float>(postSpikes.back() - preSpikes.back());
    float delta = std::abs(dtSeconds) < 0.1f ? pImpl->learningRate : 0.0f;
    
    // Enhanced activity-dependent learning rate
    // Higher activity reduces learning rate to maintain stability
    float activity = getAverageFiringRateEstimate(preSpikes, postSpikes);
    delta *= getActivityDependentLearningRate(activity);
    
    // Apply sign for inhibitory synapses (strengthen, not flip sign)
    if (synapse->getType() == SynapseType::Inhibitory) {
        delta = -delta;  // strengthen inhibition, don't flip it to excitatory
    }
    
    // Enhanced weight-dependent scaling with synaptic normalization
    if (std::abs(delta) > 1e-6f) {
        float w = synapse->getWeight();
        
        // Multiple normalization mechanisms:
        
        // 1. Soft bounds (like STDP)
        delta *= getSynapticNormalizationFactor(w);
        
        // 2. Activity-dependent bounds
        delta *= getActivityDependentBoundsFactor(activity, w);
        
        // 3. Competition factor: preserve differences between synapses
        delta *= getSynapticCompetitionFactor(synapse);
        
        // 4. Metaplasticity factor: consider learning history
        delta *= getMetaplasticityFactor(synapse);
        
        applyWeightChange(synapse, delta);
        
        // Enhanced sign enforcement
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

// Helper method implementations for enhanced Hebbian learning

float Hebbian::getAverageFiringRateEstimate(const std::vector<Timestamp>& preSpikes,
                                           const std::vector<Timestamp>& postSpikes) const {
    if (preSpikes.empty() || postSpikes.empty()) {
        return 0.0f;
    }
    
    // Estimate average firing rate from spike history
    // Simple estimate based on spike count and time window
    float maxTime = std::max(
        postSpikes.back() - (postSpikes.empty() ? 1.0f : postSpikes.front()),
        preSpikes.back() - (preSpikes.empty() ? 1.0f : preSpikes.front())
    );
    
    if (maxTime < 0.001f) {
        return 1.0f;  // High rate estimate
    }
    
    float totalSpikes = preSpikes.size() + postSpikes.size();
    return totalSpikes / maxTime;
}

float Hebbian::getActivityDependentLearningRate(float activity) const {
    // Activity-dependent learning rate: high activity reduces learning
    // This implements homeostatic plasticity to maintain stability
    if (activity > 0.8f) {
        return pImpl->learningRate * 0.1f;  // Very low learning at high activity
    } else if (activity > 0.5f) {
        return pImpl->learningRate * 0.3f;  // Low learning at moderate activity
    } else if (activity > 0.2f) {
        return pImpl->learningRate * 0.6f;  // Medium learning at low activity
    } else {
        return pImpl->learningRate * 1.0f;  // Normal learning at very low activity
    }
}

float Hebbian::getSynapticNormalizationFactor(float weight) const {
    // Synaptic normalization factor: softer bounds as weight approaches saturation
    if (std::abs(weight) >= 0.95f) {
        return 0.05f;  // Very little plasticity near saturation
    } else if (std::abs(weight) >= 0.8f) {
        return 0.2f;   // Reduced plasticity
    } else if (std::abs(weight) >= 0.5f) {
        return 0.4f;   // Moderate plasticity
    } else {
        return 1.0f;   // Full plasticity for unsaturated synapses
    }
}

float Hebbian::getActivityDependentBoundsFactor(float activity, float weight) const {
    // Activity-dependent bounds factor: tighter bounds at high activity
    float boundFactor = 1.0f;
    
    if (activity > 0.7f) {
        // High activity: very conservative learning to prevent runaway excitation
        boundFactor *= 0.3f;
    } else if (activity > 0.4f) {
        // Moderate activity: conservative learning
        boundFactor *= 0.5f;
    }
    
    // Weight-dependent adjustment
    if (std::abs(weight) > 0.8f) {
        boundFactor *= 0.5f;  // Strong saturation effect
    }
    
    return boundFactor;
}

float Hebbian::getSynapticCompetitionFactor(Synapse* synapse) const {
    // Synaptic competition factor: preserve differences between synapses
    // This helps maintain pattern selectivity
    if (!synapse) {
        return 1.0f;
    }
    
    // For now, use a simple competition factor based on weight magnitude
    // In a full implementation, this would compare with other synapses
    // that share the same pre/post neurons
    return std::min(1.0f, 2.0f - std::abs(synapse->getWeight()));
}

float Hebbian::getMetaplasticityFactor(Synapse* synapse) const {
    // Metaplasticity factor: consider learning history
    // This modulates learning based on past plasticity
    if (!synapse) {
        return 1.0f;
    }
    
    // Simple metaplasticity based on current weight
    // Higher weights -> reduced plasticity (learning history effect)
    float weight = synapse->getWeight();
    return std::max(0.2f, 1.0f - std::abs(weight));
}
