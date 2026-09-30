#include "NeuronDynamics.hpp"
#include <cmath>

namespace nlm {

NeuronDynamics::NeuronDynamics() = default;

bool NeuronDynamics::stepLIF(MembranePotential& V, Timestamp currentTime, TimestepDuration dt,
                            const NeuronState& state, MembranePotential synapticInput,
                            FiringState& firingState, float& adaptationVariable,
                            uint32_t& refractoryRemaining, MembranePotential threshold,
                            MembranePotential resetPotential, float leakConductance,
                            float membraneCapacitance, float timeConstant) {
    bool fired = false;
    
    // Handle refractory period
    if (refractoryRemaining > 0) {
        --refractoryRemaining;
        // During refractory period, clear synaptic input but don't integrate
        synapticInput = 0.0f;
        if (refractoryRemaining == 0) {
            firingState = FiringState::Resting;
        }
        return false;
    }
    
    // Synaptic input contributes to membrane potential change
    float synapticContribution = synapticInput / membraneCapacitance;
    
    // Leak contribution
    float leakContribution = (state.restingPotential - V) / timeConstant;
    
    // Update membrane potential using exponential Euler integration
    V = V + static_cast<float>(dt) * 1000.0f * (leakContribution + synapticContribution);
    
    // Apply spike-frequency adaptation (slow hyperpolarization after spike)
    if (adaptationVariable > 0.0f) {
        V -= adaptationVariable * 0.01f;
        adaptationVariable *= 0.95f;  // Decay adaptation
    }
    
    // Clamp membrane potential to prevent instability
    V = clampPotential(V);
    
    // Check for spike
    if (V >= threshold) {
        fired = true;
        firingState = FiringState::Active;
        firingState = FiringState::Refractory;  // Note: should be Refractory in spike handler
        
        // Reset membrane potential
        V = resetPotential;
        
        // Enter refractory period
        refractoryRemaining = state.refractoryPeriod;
        
        // Update adaptation for spike-frequency adaptation
        adaptationVariable += 1.0f;
    } else {
        firingState = FiringState::Active;
    }
    
    return fired;
}

bool NeuronDynamics::checkThreshold(const MembranePotential& V, const MembranePotential& threshold) const {
    return V >= threshold;
}

void NeuronDynamics::updateAdaptation(float& adaptationVariable) const {
    if (adaptationVariable > 0.0f) {
        adaptationVariable *= 0.95f;  // Decay adaptation
    }
}

} // namespace nlm