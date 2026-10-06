#pragma once

#include "Neuron.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

// LIF neuron dynamics calculations
// Contains helper functions for Leaky Integrate-and-Fire neuron simulations
// Separated from Neuron class to improve modularity and reduce header bloat

// Constants for LIF neuron model
namespace NeuronLIFConstants {
    static constexpr float MEMBRANE_CAPACITANCE = 1.0f;  // nF
    static constexpr float TIME_CONSTANT = 20.0f;      // ms
    static constexpr float TIME_STEP_MS = 0.001f;       // 1ms
    static constexpr float MAX_POTENTIAL = 50.0f;       // Upper clamp (mV)
    static constexpr float MIN_POTENTIAL = -100.0f;     // Lower clamp (mV)
    static constexpr float MAX_SPIKE_HISTORY = 100;     // Maximum recorded spikes
    static constexpr float ADAPTATION_DECAY_RATE = 0.95f;  // Adaptation variable decay
    static constexpr float ADAPTATION_STRENGTH = 0.01f;    // Adaptation influence on V
    static constexpr float INITIAL_ADAPTATION = 1.0f;      // Initial adaptation after spike
}

// Calculate leak contribution to membrane potential change
// dV_leak/dt = (V_rest - V) / tau
inline MembranePotential calculateLeakContribution(
    MembranePotential V,
    MembranePotential V_rest,
    float tau = NeuronLIFConstants::TIME_CONSTANT) {
    return (V_rest - V) / tau;
}

// Calculate synaptic contribution to membrane potential change
// dV_syn/dt = I_syn / C where I_syn is synaptic input current
inline MembranePotential calculateSynapticContribution(
    MembranePotential synapticInput,
    float capacitance = NeuronLIFConstants::MEMBRANE_CAPACITANCE) {
    return synapticInput / capacitance;
}

// Apply spike-frequency adaptation
// Reduces membrane potential based on adaptation variable
inline void applyAdaptation(
    MembranePotential& V,
    float& adaptationVariable,
    float dt = NeuronLIFConstants::TIME_STEP_MS) {
    if (adaptationVariable > 0.0f) {
        V -= adaptationVariable * NeuronLIFConstants::ADAPTATION_STRENGTH;
        adaptationVariable *= NeuronLIFConstants::ADAPTATION_DECAY_RATE;
    }
}

// Clamp membrane potential to prevent instability
inline MembranePotential clampMembranePotential(MembranePotential V) {
    return std::clamp(V, NeuronLIFConstants::MIN_POTENTIAL, 
                      NeuronLIFConstants::MAX_POTENTIAL);
}

// Update adaptation variable after spike
inline void updateAdaptationAfterSpike(float& adaptationVariable) {
    adaptationVariable += NeuronLIFConstants::INITIAL_ADAPTATION;
}

// Check if neuron should spike based on membrane potential
inline bool shouldSpike(MembranePotential V, MembranePotential threshold) {
    return V >= threshold;
}

// Calculate updated membrane potential using exponential Euler integration
inline MembranePotential updateMembranePotential(
    MembranePotential V,
    MembranePotential V_rest,
    MembranePotential synapticInput,
    TimestepDuration dt,
    float tau = NeuronLIFConstants::TIME_CONSTANT,
    float C = NeuronLIFConstants::MEMBRANE_CAPACITANCE) {
    
    // Calculate contributions
    float leakContribution = calculateLeakContribution(V, V_rest, tau);
    float synapticContribution = calculateSynapticContribution(synapticInput, C);
    
    // Update using LIF equation: dV/dt = (V_rest - V)/tau + I/C
    V = V + static_cast<float>(dt) * 1000.0f * (leakContribution + synapticContribution);
    return V;
}

} // namespace nlm