#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <array>

namespace nlm {

// Forward declarations
class Synapse;
class RandomGenerator;

// Neuron state structure for efficient storage
struct NeuronState {
    MembranePotential membranePotential;    // Current membrane potential (mV)
    MembranePotential restingPotential;    // Resting potential (mV)
    MembranePotential threshold;           // Firing threshold (mV)
    MembranePotential resetPotential;       // Post-spike reset value (mV)
    MembranePotential leakConductance;      // Leak conductance (nS)
    MembranePotential synapseConductance;   // Synaptic conductance (nS)
    FiringRate firingRate;                 // Current firing rate (Hz)
    FiringState firingState;               // Current firing state
    uint32_t refractoryRemaining;          // Steps remaining in refractory period
    uint32_t refractoryPeriod;             // Total refractory period (steps)
    float adaptationVariable;              // For spike-frequency adaptation
    float lastSpikeTime;                   // Timestamp of last spike (-1 if none)
    
    NeuronState()
        : membranePotential(-70.0f)
        , restingPotential(-70.0f)
        , threshold(-55.0f)
        , resetPotential(-70.0f)
        , leakConductance(10.0f)
        , synapseConductance(0.0f)
        , firingRate(0.0f)
        , firingState(FiringState::Resting)
        , refractoryRemaining(0)
        , refractoryPeriod(5)
        , adaptationVariable(0.0f)
        , lastSpikeTime(-1.0f) {}
};

// Neuron class representing a single neuron
// Implements Leaky Integrate-and-Fire (LIF) dynamics with advanced features
// 
// This class models individual neurons in the NLM (Neural Learning Machine) framework.
// It implements biologically plausible neural dynamics including:
// - Leaky Integrate-and-Fire (LIF) membrane potential dynamics
// - Spike-frequency adaptation
// - Refractory period with adaptive length
// - Synaptic input integration with temporal filtering
// - Post-spike afterhyperpolarization (AHP)
// - Plasticity state management
// 
// The neuron follows the LIF equation:
// dV/dt = (V_rest - V)/tau + I/C + adaptation + noise
// where:
//   V = membrane potential (mV)
//   V_rest = resting potential (typically -70mV)
//   tau = membrane time constant (typically 20ms)
//   I = total synaptic current input (nA)
//   C = membrane capacitance (typically 1nF)
//   adaptation = spike-frequency adaptation current
//   noise = stochastic channel noise (future extension)
// 
// Neuron behavior is governed by four main states:
//   Resting: Stable subthreshold activity
//   Active: Above threshold, integrating inputs
//   Refractory: Cannot spike, recovering membrane potential
//   Threshold: Critical transition state
// 
// The implementation uses exponential Euler integration for numerical stability
// and includes adaptive time-stepping for better accuracy near spikes.

// Configuration Parameters:
//   - Resting potential: -70mV (default)
//   - Threshold: -55mV (default)
//   - Reset potential: -70mV (default)
//   - Membrane time constant: 20ms (configurable)
//   - Membrane capacitance: 1nF (configurable)
//   - Refractory period: 5ms (configurable, adaptive)
//   - Adaptation time constant: 1000ms (configurable)

} // namespace nlm
