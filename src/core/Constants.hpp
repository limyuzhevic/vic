// NLM Phase 2 Constants Header
// Centralized definition of all magic numbers and constants

#pragma once

namespace nlm {
namespace constants {

// Simulation constants
constexpr double DEFAULT_TIMESTEP = 0.001;      // 1ms timestep
constexpr size_t DEFAULT_NEURON_COUNT = 500;    // Default number of neurons
constexpr size_t DEFAULT_REGION_COUNT = 1;     // Default number of regions
constexpr double DEFAULT_CONNECTION_PROB = 0.15; // Connection probability

// STDP parameters
constexpr double STDP_LTP_WEIGHT = 0.02;        // Long-term potentiation weight
constexpr double STDP_LTD_WEIGHT = 0.015;       // Long-term depression weight
constexpr double STDP_TIME_CONSTANT = 20.0;     // STDP time constant (ms)

// Structural plasticity
constexpr double SYNAPTOGENESIS_RATE = 0.0001;  // Synaptogenesis rate
constexpr double PRUNING_RATE = 0.00001;        // Pruning rate

// Test configuration
constexpr size_t CONNECTIVITY_TEST_NEUTRONS = 10; // Neurons to inject current into
constexpr size_t CONNECTIVITY_TEST_STEPS = 50;   // Steps to run connectivity test
constexpr size_t PLASTICITY_TEST_STEPS = 1000;   // Steps for plasticity experiment
constexpr size_t STDP_TEST_TRIALS = 50;           // STDP verification trials

// Output and logging
constexpr const char* DEFAULT_CONFIG_FILE = "configs/default.cfg"; // Default config file
constexpr const char* PROGRAM_NAME = "nlm";                    // Program name

// Performance
constexpr size_t MAX_SPIKE_HISTORY = 100;      // Maximum spikes to track per neuron
constexpr size_t MAX_TEST_RESULTS = 1000;     // Maximum test results to store

// Brain parameters
constexpr double LIF_MEMBRANE_CAPACITANCE = 1.0;  // Membrane capacitance (nF)
constexpr double LIF_TIME_CONSTANT = 20.0;       // Membrane time constant (ms)
constexpr double LIF_RESTING_POTENTIAL = -70.0;  // Resting potential (mV)
constexpr double LIF_THRESHOLD = -55.0;           // Firing threshold (mV)
constexpr double LIF_RESET_POTENTIAL = -70.0;    // Reset potential (mV)

// Neuron initialization ranges (for random initialization)
constexpr double NEURON_POTENTIAL_VARIATION = 3.0;  // mV variation around resting
constexpr double NEURON_THRESHOLD_VARIATION = 2.0;  // mV variation around threshold
constexpr double NEURON_RESTING_VARIATION = 2.0;    // mV variation around resting
constexpr double NEURON_RESET_VARIATION = 5.0;      // mV variation around reset
constexpr uint32_t REFRACTORY_PERIOD_MIN = 2;       // Minimum refractory period (ms)
constexpr uint32_t REFRACTORY_PERIOD_MAX = 10;      // Maximum refractory period (ms)

// Testing thresholds
constexpr float LEARNING_THRESHOLD = 0.01f;        // Minimum weight change to consider learning
constexpr float STDP_THRESHOLD = 0.001f;           // Minimum STDP weight change to consider significant
constexpr float CONNECTION_THRESHOLD = 0.1f;       // Minimum current to attempt spike

} // namespace constants
} // namespace nlm
