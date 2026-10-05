#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace nlm {

// Enhanced neuron state with additional plasticity tracking
struct NeuronState {
    float membranePotential;    // Current membrane potential (mV)
    float restingPotential;    // Resting potential (mV)
    float threshold;           // Firing threshold (mV)
    float resetPotential;       // Post-spike reset value (mV)
    float leakConductance;      // Leak conductance (nS)
    float synapseConductance;   // Synaptic conductance (nS)
    float firingRate;          // Current firing rate (Hz)
    uint8_t firingState;       // Current firing state (enum compressed)
    uint32_t refractoryRemaining;  // Steps remaining in refractory period
    uint32_t refractoryPeriod;     // Total refractory period (steps)
    float adaptationVariable;     // For spike-frequency adaptation
    float lastSpikeTime;          // Timestamp of last spike (-1 if none)
    
    // Enhanced plasticity tracking
    float eligibilityTrace;      // For reward-modulated learning
    float synapticEfficacy;      // Use-dependent modulation
    float neuromodulationLevel;   // Recent neuromodulator concentration
    float plasticityModulator;   // Development/age-dependent plasticity factor
    
    NeuronState()
        : membranePotential(-70.0f)
        , restingPotential(-70.0f)
        , threshold(-55.0f)
        , resetPotential(-70.0f)
        , leakConductance(10.0f)
        , synapseConductance(0.0f)
        , firingRate(0.0f)
        , firingState(0)  // FiringState::Resting
        , refractoryRemaining(0)
        , refractoryPeriod(5)
        , adaptationVariable(0.0f)
        , lastSpikeTime(-1.0f)
        , eligibilityTrace(0.0f)
        , synapticEfficacy(1.0f)
        , neuromodulationLevel(0.0f)
        , plasticityModulator(1.0f) {}
};

} // namespace nlm
