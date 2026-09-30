#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

class NeuronDynamics {
public:
    NeuronDynamics();
    
    // Core LIF dynamics: Leaky Integrate-and-Fire
    // dV/dt = (V_rest - V)/tau + I/C
    // Discrete approximation: V_new = V + dt * ((V_rest - V)/tau + I/C)
    bool stepLIF(MembranePotential& V, Timestamp currentTime, TimestepDuration dt,
                 const NeuronState& state, MembranePotential synapticInput,
                 FiringState& firingState, float& adaptationVariable,
                 uint32_t& refractoryRemaining, MembranePotential threshold,
                 MembranePotential resetPotential, float leakConductance,
                 float membraneCapacitance, float timeConstant);
    
    // Check if neuron fires based on membrane potential
    bool checkThreshold(const MembranePotential& V, const MembranePotential& threshold) const;
    
    // Update adaptation variable (slow hyperpolarization after spike)
    void updateAdaptation(float& adaptationVariable) const;
    
    // Get LIF parameters
    static constexpr float getMembraneCapacitance() { return 1.0f; }  // nF
    static constexpr float getTimeConstant() { return 20.0f; }  // ms
    
    // Clamp membrane potential to prevent instability
    static MembranePotential clampPotential(MembranePotential V) {
        return std::clamp(V, -100.0f, 50.0f);
    }
};

} // namespace nlm