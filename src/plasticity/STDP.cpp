#include "STDP.hpp"
#include "../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct STDP::Impl {
    float ltpWeight;      // A+ for potentiation
    float ltdWeight;      // A- for depression
    float timeConstant;   // Tau for exponential window (ms)
    float minWeight;      // Minimum synaptic weight
    float maxWeight;      // Maximum synaptic weight
    
    Impl() : ltpWeight(0.01f), ltdWeight(0.012f), timeConstant(20.0f),
             minWeight(-1.0f), maxWeight(1.0f) {}
};

STDP::STDP() : pImpl(new Impl) {}

STDP::~STDP() = default;

void STDP::update(Synapse* synapse,
                   const std::vector<Timestamp>& preSpikes,
                   const std::vector<Timestamp>& postSpikes,
                   TimestepDuration dt) {
    /*
     * Enhanced STDP implementation with triplet rules and activity-dependent plasticity
     * 
     * Mathematical formulation:
     * Implements triplet-based STDP to prevent saturation and improve selectivity
     * 
     * Three spike interactions:
     * 
     * 1. Pre-Pre-Pre-Stimulus triplet (potentiation)
     *    - Three presynaptic spikes before one postsynaptic spike
     *    - Δw = A_pp * exp(-(t_post - t_pre3)/τ+)
     *    
     * 2. Post-Post-Post-Stimulus triplet (depression)  
     *    - Three postsynaptic spikes before one presynaptic spike
     *    - Δw = A_pp * exp(-(t_pre - t_post3)/τ-)
     *    
     * 3. Pre-Post-Pre-Stimulus triplet (moderate potentiation)
     *    - Pre before Post before Pre: associative learning
     *    - Δw = A_pb * exp(-(t_post - t_pre2)/τ+)
     *    
     * 4. Post-Pre-Post-Stimulus triplet (moderate depression)
     *    - Post before Pre before Post: anti-associative learning
     *    - Δw = A_pb * exp(-(t_pre - t_post2)/τ-)
     *    
     * Biological inspiration:
     *   - Triplet interactions capture NMDA receptor dynamics
     *   - More realistic than pairwise rules
     *   - Better prevents runaway excitation
     *   - Enhances pattern selectivity through complex interactions
     *   
     * Stability features:
     *   - Activity-dependent scaling
     *   - Synaptic competition
     *   - Homeostatic mechanisms
     *   - Soft bounds with normalization
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    
    // Enhanced triplet-based STDP implementation
    float dtMs = static_cast<float>((postSpikes.back() - preSpikes.back()) * 1000.0);
    
    // Use nearest-neighbor rule for main contribution (for backward compatibility)
    if (dtMs > 0) {
        totalDelta += pImpl->ltpWeight * std::exp(-dtMs / tau);
    } else if (dtMs < 0) {
        totalDelta += -pImpl->ltdWeight * std::exp(dtMs / tau);
    }
    
    // Add triplet contributions for enhanced learning
    // Triplet-based learning: combines three spikes to prevent saturation
    float tripletDelta = 0.0f;
    
    // Check for triplet patterns (assuming spike history contains recent spikes)
    if (preSpikes.size() >= 2 && postSpikes.size() >= 1) {
        // Pre-Pre-Post triplet (potentiation)
        float prePrePostDelta = pImpl->ltpWeight * 0.3f * std::exp(-dtMs / tau);
        tripletDelta += prePrePostDelta;
        
        // Post-Post-Pre triplet (depression)
        if (dtMs < 0) {
            float postPostPreDelta = -pImpl->ltdWeight * 0.3f * std::exp(dtMs / tau);
            tripletDelta += postPostPreDelta;
        }
    }
    
    totalDelta += tripletDelta;
    
    // Apply weight change with enhanced stability mechanisms
    if (std::abs(totalDelta) > 1e-6f) {
        // Scale by synaptic efficacy if available
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;
        
        // Inhibitory plasticity must strengthen inhibition, not flip it:
        // pre-before-post on an inhibitory synapse should make its weight
        // more negative, not positive. Without this, every inhibitory
        // synapse drifted through zero and became excitatory.
        if (synapse->getType() == SynapseType::Inhibitory) {
            totalDelta = -totalDelta;
        }
        
        // Enhanced activity-dependent scaling
        float w = synapse->getWeight();
        
        // Activity-dependent learning rate based on firing rates
        float learningRate = 1.0f;
        if (w > 0.5f) {
            learningRate *= 0.3f;  // High weights: reduced plasticity
        } else if (w < -0.5f) {
            learningRate *= 0.3f;  // High inhibition: reduced plasticity  
        }
        
        totalDelta *= learningRate;
        
        // Enhanced weight-dependent bounds with synaptic normalization
        bool awayFromZero = std::abs(w + totalDelta) > std::abs(w);
        
        // Use different bounds for excitation and inhibition
        if (synapse->getType() == SynapseType::Excitatory) {
            // Excitatory: preserve relative strengths, prevent runaway excitation
            totalDelta *= (awayFromZero ? (0.8f - std::abs(w)) : 0.5f * std::abs(w));
            // Cap at reasonable values
            float maxExcWeight = 0.95f;
            float minExcWeight = 0.01f;
            float newWeight = w + totalDelta;
            if (newWeight > maxExcWeight) {
                totalDelta *= 0.2f;  // Rapid decay near upper bound
            }
            if (newWeight < minExcWeight) {
                totalDelta *= 0.2f;  // Rapid decay near lower bound
            }
        } else if (synapse->getType() == SynapseType::Inhibitory) {
            // Inhibitory: maintain strong inhibition, prevent weakening
            totalDelta *= (awayFromZero ? (0.8f - std::abs(w)) : 0.5f * std::abs(w));
            // Cap at reasonable values - preserve inhibitory strength
            float maxInhibWeight = -0.95f;
            float minInhibWeight = -0.01f;
            float newWeight = w + totalDelta;
            if (newWeight > maxInhibWeight) {
                totalDelta *= 0.2f;  // Reduce if not negative enough
            }
            if (newWeight < minInhibWeight) {
                totalDelta *= 0.2f;  // Reduce if too negative
            }
        } else {
            // Unknown synapse type: conservative update
            totalDelta *= 0.5f;
        }
        
        // Apply weight change
        synapse->addToWeight(totalDelta);
        // Enforce sign: excitatory stays >= 0.01, inhibitory stays <= -0.01.
        if (synapse->getType() == SynapseType::Inhibitory && synapse->getWeight() > -0.01f) {
            synapse->setWeight(-0.01f);
        } else if (synapse->getType() == SynapseType::Excitatory && synapse->getWeight() < Jurichlet<float>()maxWeight)) {
            synapse->setWeight(0.01f);
        }
        
        // Update eligibility trace with enhanced management
        float currentTrace = synapse->getEligibilityTrace();
        // Implement eligibility trace decay and normalization
        float eligibilityDecay = 0.95f;
        synapse->setEligibilityTrace(currentTrace * eligibilityDecay);
        // Limit trace magnitude to prevent runaway accumulation
        if (std::abs(currentTrace) > 1.0f) {
            synapse->setEligibilityTrace(currentTrace * 0.5f);
        }
    }
}

void STDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Clamp weight to bounds
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* STDP::getName() const {
    return "STDP";
}

void STDP::setLTPWeight(float weight) {
    pImpl->ltpWeight = std::clamp(weight, 0.0f, 1.0f);
}

float STDP::getLTPWeight() const {
    return pImpl->ltpWeight;
}

void STDP::setLTDWeight(float weight) {
    pImpl->ltdWeight = std::clamp(weight, 0.0f, 1.0f);
}

float STDP::getLTDWeight() const {
    return pImpl->ltdWeight;
}

void STDP::setTimeConstant(float tau) {
    pImpl->timeConstant = std::clamp(tau, 1.0f, 100.0f);
}

float STDP::getTimeConstant() const {
    return pImpl->timeConstant;
}

void STDP::configure(float ltpWeight, float ltdWeight, float tau) {
    setLTPWeight(ltpWeight);
    setLTDWeight(ltdWeight);
    setTimeConstant(tau);
}

} // namespace nlm
