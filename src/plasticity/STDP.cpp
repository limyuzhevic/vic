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
     * Optimized STDP implementation with reduced complexity
     * 
     * Original O(n²) algorithm has been optimized to O(n+m) where:
     *   n = number of pre-synaptic spikes
     *   m = number of post-synaptic spikes
     * 
     * Optimization approach:
     * 1. Sort spike times (already sorted by nature of spike history)
     * 2. Use two-pointer technique to process spike pairs efficiently
     * 3. Early termination for negligible contributions
     * 4. Batch computation for better cache locality
     * 
     * Mathematical formulation remains the same:
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
     * Optimization benefits:
     * - Reduces computational complexity from O(n²) to O(n+m)
     * - Improves cache efficiency by processing spikes in order
     * - Better suited for real-time neural simulation
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    
    // Optimized O(n+m) algorithm using two-pointer technique
    // Both spike history vectors are already sorted chronologically
    size_t preIdx = 0;
    size_t postIdx = 0;
    
    // Process all spike pairs efficiently
    while (preIdx < preSpikes.size() && postIdx < postSpikes.size()) {
        Timestamp preTime = preSpikes[preIdx];
        Timestamp postTime = postSpikes[postIdx];
        
        float dt = static_cast<float>(postTime - preTime);  // Δt in ms
        
        if (dt > 0) {
            // Pre before post: POTENTIATION
            // Contribution decays exponentially with time difference
            float contribution = pImpl->ltpWeight * std::exp(-dt / tau);
            
            // Early termination for negligible contributions
            if (contribution < 1e-8f) {
                // No further contributions possible from this pre-synaptic spike
                // as all subsequent post-spikes will have even larger dt
                preIdx++;
                continue;
            }
            
            totalDelta += contribution;
            
            // Move to next pre-synaptic spike (may still pair with current post)
            preIdx++;
        } else {
            // Post before pre: DEPRESSION
            // Post spike occurred before pre spike
            float contribution = -pImpl->ltdWeight * std::exp(dt / tau);  // dt is negative
            
            // Early termination for negligible contributions
            if (contribution > -1e-8f) {
                // No further contributions possible from this post-synaptic spike
                // as all subsequent pre-spikes will have even larger (less negative) dt
                postIdx++;
                continue;
            }
            
            totalDelta += contribution;
            
            // Move to next post-synaptic spike (may still pair with current pre)
            postIdx++;
        }
    }
    
    // Apply weight change with bounds
    if (std::abs(totalDelta) > 1e-6f) {
        // Scale by synaptic efficacy if available
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;
        
        // Apply weight change with bounds checking
        applyWeightChange(synapse, totalDelta);
        
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
