#include "STDP.hpp"
#include "../../brain/Synapse.hpp"
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
     * Real STDP implementation based on spike-timing correlation
     * 
     * Mathematical formulation:
     * For each pre-post spike pair with timing difference Δt = t_post - t_pre:
     * 
     * If Δt > 0 (pre before post): POTENTIATION
     *   Δw = A+ * exp(-Δt / τ+)
     *   
     * If Δt < 0 (post before pre): DEPRESSION
     *   Δw = A- * exp(Δt / τ-)
     * 
     * Where:
     *   A+ = ltpWeight (potentiation amplitude)
     *   A- = ltdWeight (depression amplitude)  
     *   τ+ = τ- = timeConstant (STDP time window)
     * 
     * Biological inspiration:
     *   - Reflects NMDA receptor-mediated calcium signaling
     *   - Pre-before-post activates NMDA receptors when postsynaptic spikes
     *   - Post-before-pre causes backpropagating action potentials
     *   
     * Optimizations applied:
     *   - Early termination for very close spike timing
     *   - Pre-computation of exponential decay factors
     *   - Efficient processing of spike timing differences
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    
    // Optimization: Early exit for very close spike timing (no significant effect)
    // Check if any pre-post spike pair has a significant time difference
    bool hasSignificantTiming = false;
    for (size_t i = 0; i < preSpikes.size() && !hasSignificantTiming; ++i) {
        for (size_t j = 0; j < postSpikes.size() && !hasSignificantTiming; ++j) {
            float timeDiff = std::abs(static_cast<float>(postSpikes[j] - preSpikes[i]));
            if (timeDiff > 0.001f) {  // Significant if > 1ms
                hasSignificantTiming = true;
            }
        }
    }
    
    if (!hasSignificantTiming) {
        return;  // No significant STDP effects possible
    }
    
    // Optimization: Process only recent spikes to reduce complexity
    // Limit to most recent 20 spikes from each set for O(1) operation
    const size_t MAX_PROCESSING = 20;
    size_t preCount = std::min(preSpikes.size(), MAX_PROCESSING);
    size_t postCount = std::min(postSpikes.size(), MAX_PROCESSING);
    
    // Pre-compute common constants for efficiency
    const float ltp = pImpl->ltpWeight;
    const float ltd = pImpl->ltdWeight;
    const float inv_tau = 1.0f / tau;
    
    // Process spike pairs with optimized calculations
    for (size_t i = 0; i < preCount; ++i) {
        Timestamp preTime = preSpikes[preSpikes.size() - 1 - i];  // Most recent first
        
        for (size_t j = 0; j < postCount; ++j) {
            Timestamp postTime = postSpikes[postSpikes.size() - 1 - j];  // Most recent first
            
            float dt = static_cast<float>(postTime - preTime);  // Δt in ms
            
            if (std::abs(dt) < 0.000001f) {
                continue;  // Skip simultaneous spikes (very small threshold)
            }
            
            float delta;
            if (dt > 0) {
                // Pre before post: POTENTIATION
                delta = ltp * std::exp(-dt * inv_tau);
            } else {
                // Post before pre: DEPRESSION
                delta = -ltd * std::exp(dt * inv_tau);
            }
            
            totalDelta += delta;
        }
    }
    
    // Apply weight change with bounds
    if (std::abs(totalDelta) > 1e-6f) {
        // Scale by synaptic efficacy if available
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;
        
        // Apply weight change
        synapse->addToWeight(totalDelta);
        
        // Update eligibility trace for reward-modulated learning
        float currentTrace = synapse->getEligibilityTrace();
        synapse->setEligibilityTrace(currentTrace + totalDelta);
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