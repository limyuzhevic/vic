#include "Synapse.hpp"
#include "../../brain/Neuron.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

// Implement real synaptic dynamics for Phase 2
void Synapse::step(Timestamp currentTime) {
    /*
     * Real synaptic dynamics based on biophysical models:
     * 
     * 1. Short-term plasticity (STP): Facilitation and depression
     * 2. Synaptic conductance dynamics
     * 3. Short-term depression through residual calcium
     * 4. Facilitation through vesicle depletion
     * 
     * Mathematical formulation:
     * - Facilitation: f(t) = 1 - exp(-t / τ_facil)
     * - Depression: d(t) = exp(-t / τ_depress)
     * - Combined effect: g(t) = A * f(t) * d(t)
     */
    
    TimestepDuration dt = static_cast<TimestepDuration>(0.001);  // 1ms timestep
    
    // Update short-term depression (utilization-based)
    if (pImpl->lastPreSpikeTime >= 0.0f) {
        float timeSincePre = currentTime - pImpl->lastPreSpikeTime;
        
        // Depression recovery toward baseline (1.0)
        pImpl->shortTermDepression = 1.0f - 
            pImpl->shortTermDepression * std::exp(-timeSincePre / pImpl->STP_DEPRESSION_TAU);
        
        // Clamp depression to reasonable bounds
        pImpl->shortTermDepression = std::clamp(pImpl->shortTermDepression, 0.1f, 1.0f);
    }
    
    // Update short-term facilitation
    if (pImpl->lastPreSpikeTime >= 0.0f) {
        float timeSincePre = currentTime - pImpl->lastPreSpikeTime;
        
        // Facilitation decay toward baseline (0.0)
        pImpl->shortTermFacilitation *= std::exp(-timeSincePre / pImpl->STP_FACILITATION_TAU);
        
        // Clamp facilitation
        pImpl->shortTermFacilitation = std::clamp(pImpl->shortTermFacilitation, 0.0f, 1.0f);
    }
    
    // Update residual calcium for plasticity
    if (pImpl->lastPostSpikeTime >= 0.0f) {
        float timeSincePost = currentTime - pImpl->lastPostSpikeTime;
        pImpl->efficacy *= std::exp(-timeSincePost / 50.0f);  // Calcium decay
    }
    
    // Decay eligibility trace for reward-modulated learning
    decayEligibilityTrace(0.001f * dt);  // Scale by timestep
    
    // Apply synaptic conductance changes
    float totalConductance = pImpl->shortTermFacilitation * pImpl->shortTermDepression;
    
    // Update synaptic weight with activity-dependent modulation
    if (pImpl->lastPreSpikeTime >= 0.0f && pImpl->lastPostSpikeTime >= 0.0f) {
        // Spike pairing modifies weight
        float delta = pImpl->efficacy * (pImpl->lastPostSpikeTime - pImpl->lastPreSpikeTime);
        addToWeight(delta * 0.0001f);  // Small change per spike pair
    }
    
    // Clamp weight to hard bounds
    pImpl->weight = std::clamp(pImpl->weight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
    
    // Ensure efficacy stays within reasonable bounds
    pImpl->efficacy = std::clamp(pImpl->efficacy, 0.1f, 2.0f);
}

} // namespace nlm
