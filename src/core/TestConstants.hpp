// Phase 2 Constants for NLM Neural Simulation Tests
#pragma once

namespace nlm {
namespace phase2 {

// Time constants (milliseconds)
constexpr double SIMULATION_TIMESTEP = 0.001;  // 1ms timestep
constexpr double SIMULATION_DURATION = 1.0;   // 1 second total simulation

// Neural network parameters
constexpr uint64_t NEURON_COUNT = 500;          // Number of neurons in test network
constexpr uint64_t REGION_COUNT = 1;           // Number of brain regions
constexpr double CONNECTION_PROBABILITY = 0.15; // Probability of synapse formation

// Input current values (synaptic input)
constexpr double CURRENT_STRENGTH = 30.0;      // Current for input neurons
constexpr double INJECT_CURRENT = 50.0;       // Current for connectivity test
constexpr double LTP_CURRENT = 60.0;          // Current for STDP potentiation test

// Plasticity parameters
constexpr float STDP_LTP_WEIGHT = 0.02f;       // STDP Long-Term Potentiation weight
constexpr float STDP_LTD_WEIGHT = 0.015f;      // STDP Long-Term Depression weight
constexpr float STDP_TAU = 20.0f;              // STDP time constant

// Structural plasticity
constexpr float SYNAPTOGENESIS_RATE = 0.0001f; // Rate of new synapse formation
constexpr float PRUNING_RATE = 0.00001f;        // Rate of synapse removal

// Learning thresholds
constexpr float LEARNING_THRESHOLD = 0.01f;     // Minimum weight change to consider learning
constexpr float STDP_THRESHOLD = 0.001f;        // Minimum STDP weight change

// Test parameters
constexpr uint32_t TEST_STEPS = 1000;          // Steps for plasticity experiment
constexpr uint32_t CONNECTIVITY_TEST_STEPS = 50; // Steps for connectivity test
constexpr uint32_t STDP_TEST_TRIALS = 50;       // STDP trials
constexpr size_t MOST_ACTIVE_NEURONS = 10;      // Number of top neurons to track

// Neuron parameters
constexpr double NEURON_THRESHOLD = -55.0;     // Firing threshold (mV)
constexpr double NEURON_RESTING_POTENTIAL = -70.0; // Resting potential (mV)
constexpr double NEURON_RESET_POTENTIAL = -70.0;    // Reset potential (mV)
constexpr uint32_t NEURON_REFECTORY_PERIOD = 5;    // Refractory period (steps)

// Output formatting
constexpr int OUTPUT_PRECISION = 3;            // Decimal precision for output
constexpr size_t MAX_LOG_LENGTH = 1024;         // Maximum log message length

// File paths
constexpr const char* DEFAULT_CONFIG_FILE = "configs/default.cfg";
constexpr const char* CHECKPOINT_FILE = "./checkpoint_test.bin";

} // namespace phase2
} // namespace nlm