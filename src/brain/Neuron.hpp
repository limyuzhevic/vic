#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// ============================================================================
// NEURON-SPECIFIC CONSTANTS (Neuron.hpp)
// ============================================================================
// These constants are specific to neuron behavior, physiology, and dynamics.
// They include membrane potentials, thresholds, refractory periods, and other
// biophysical parameters that characterize individual neurons.

namespace NeuronConstants {

// ============================================================================
// PHYSIOLOGICAL PARAMETERS
// ============================================================================

// Resting membrane potential (mV)
static constexpr MembranePotential DEFAULT_RESTING_POTENTIAL = -70.0f;

// Spike threshold (mV)
static constexpr MembranePotential DEFAULT_THRESHOLD = -55.0f;

// Reset membrane potential (mV)
static constexpr MembranePotential DEFAULT_RESET_POTENTIAL = -70.0f;

// Maximum membrane potential (clamping upper bound) (mV)
static constexpr MembranePotential MAX_MEMBRANE_POTENTIAL = 50.0f;

// Minimum membrane potential (clamping lower bound) (mV)
static constexpr MembranePotential MIN_MEMBRANE_POTENTIAL = -100.0f;

// ============================================================================
// BIOPHYSICAL PARAMETERS
// ============================================================================

// Leak conductance (nS)
static constexpr MembranePotential DEFAULT_LEAK_CONDUCTANCE = 10.0f;

// Membrane capacitance (nF) - from Neuron.cpp:22
static constexpr float DEFAULT_MEMBRANE_CAPACITANCE = 1.0f;

// Membrane time constant (ms) - from Neuron.cpp:23
static constexpr float DEFAULT_TIME_CONSTANT = 20.0f;

// ============================================================================
// TEMPORAL DYNAMICS
// ============================================================================

// Time scaling factor: 1ms timestep * 1000 = seconds per millisecond
static constexpr float TIME_SCALE_FACTOR = 1000.0f;

// ============================================================================
// FUNCTIONAL PARAMETERS
// ============================================================================

// Refractory period (steps) - typical 2-10ms range converted to steps
static constexpr uint32_t DEFAULT_REFACTORY_PERIOD = 5; // from Neuron.cpp:38

// Spike-frequency adaptation variable decay factor
static constexpr float ADAPTATION_DECAY_FACTOR = 0.95f; // from Neuron.cpp:228

// Spike-frequency adaptation increment per spike
static constexpr float ADAPTATION_INCREMENT = 1.0f; // from Neuron.cpp:251

// Refractory period range for random initialization
static constexpr uint32_t MIN_REFACTORY_PERIOD = 2; // from Neuron.cpp:293
static constexpr uint32_t MAX_REFACTORY_PERIOD = 10; // from Neuron.cpp:293

// ============================================================================
// DETECTION AND STATE CONSTANTS
// ============================================================================

// Last spike time sentinel value (indicates no spike yet)
static constexpr float LAST_SPIKE_TIME_SENTINEL = -1.0f; // from Neuron.cpp:295

// ============================================================================
// TEMPORAL DYNAMICS
// ============================================================================

// Default simulation timestep (1ms) - from Neuron.cpp:264
static constexpr TimestepDuration DEFAULT_TIMESTEP = 0.001; // seconds

// ============================================================================
// PLACEHOLDER CONSTANTS (for future implementation)
// ============================================================================

static constexpr float PLACEHOLDER_1 = 0.0f;
static constexpr float PLACEHOLDER_2 = 1.0f;
static constexpr float PLACEHOLDER_3 = 0.5f;
static constexpr size_t PLACEHOLDER_4 = 10;
static constexpr int PLACEHOLDER_5 = 42;

} // namespace NeuronConstants

} // namespace nlm
