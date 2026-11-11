// NLM (熙然) Neural Learning Machine - Core Constants
// Physical and simulation parameters extracted from magic numbers
// This file contains constexpr constants for better maintainability and code readability

#pragma once

namespace nlm {

// ==============================
// HIGH PRIORITY CONSTANTS (Priority 1)
// ==============================

// Category 1: Current Injection Values
constexpr float CURRENT_STRONG_EXCITATORY = 50.0f;    // Strong input for connectivity tests
constexpr float CURRENT_MODERATE = 30.0f;              // Standard input for learning experiments
constexpr float CURRENT_HIGH = 60.0f;                 // Potent input for STDP verification

// Category 2: Neural Simulation Parameters
constexpr float NEURON_RESTING_POTENTIAL = -70.0f;    // Typical resting membrane potential (mV)
constexpr float NEURON_THRESHOLD = -55.0f;           // Spike threshold potential (mV)
constexpr float NEURON_CLAMP_MIN = -100.0f;          // Minimum membrane potential
constexpr float NEURON_CLAMP_MAX = 50.0f;            // Maximum membrane potential
constexpr float NEURON_TIME_CONSTANT = 20.0f;        // Membrane time constant (ms)

// Category 3: STDP Learning Parameters
constexpr float STDP_LTP_WEIGHT = 0.01f;              // Long-term potentiation weight
constexpr float STDP_LTD_WEIGHT = 0.012f;             // Long-term depression weight
constexpr float STDP_LEARNING_THRESHOLD = 0.001f;    // Minimum weight change to trigger plasticity
constexpr float STDP_TIME_CONSTANT = 20.0f;          // STDP exponential window (ms)

// Category 4: Synaptic Plasticity Rates
constexpr float SYNAPTOGENESIS_RATE = 0.0001f;       // Rate of new synapse formation
constexpr float PRUNING_RATE = 0.00001f;              // Rate of synapse elimination
constexpr float CONNECTION_PROBABILITY = 0.1f;       // Initial connection probability

// Category 5: Experiment Configuration Values
constexpr uint64_t PHASE6_MAX_STEPS = 10000;          // Maximum simulation steps
constexpr size_t PHASE6_NEURON_COUNT = 1000;         // Default neuron count
constexpr size_t DEFAULT_TRIALS = 50;                // Standard number of experimental trials
constexpr uint64_t PHASE6_DEMO_MAX_STEPS = 2000;      // Demo simulation steps

// ==============================
// MEDIUM PRIORITY CONSTANTS (Priority 2)
// ==============================

// Category 6: Simulation and Timing Parameters
constexpr double SIMULATION_TIMESTEP = 0.001;         // Core simulation timestep (1ms)
constexpr float DEFAULT_TIMESTEP = 1.0f;              // Default timestep in seconds
constexpr float ADAPTATION_DECAY_RATE = 0.9f;        // Spike-frequency adaptation decay
constexpr float ADAPTATION_INCREMENT = 0.1f;         // Adaptation variable scaling

// Category 7: Benchmark and Performance Values
constexpr size_t BENCHMARK_CURRENT_INJECTION_NEURONS = 10;  // Neurons receiving current in benchmark
constexpr size_t BENCHMARK_NEUTRAL_SIZE = 100;              // Small benchmark size
constexpr size_t BENCHMARK_MEDIUM_SIZE = 500;                // Medium benchmark size
constexpr size_t BENCHMARK_LARGE_SIZE = 1000;                 // Large benchmark size
constexpr size_t BENCHMARK_HUGE_SIZE = 2000;                  // Huge benchmark size
constexpr uint64_t MAX_TOTAL_SYNAPSES = 1000000;            // Maximum synapse limit

// ==============================
// OTHER CONSTANTS
// ==============================

// Visualization constants
constexpr int VIZ_DEFAULT_WIDTH = 800;
constexpr int VIZ_DEFAULT_HEIGHT = 600;
constexpr float VIZ_PARTICLE_SIZE = 2.0f;

// Debug and logging constants
constexpr size_t MAX_LOG_MESSAGE_LENGTH = 1024;
constexpr size_t MAX_ERROR_MESSAGE_LENGTH = 2048;

// Random number generation constants
constexpr uint64_t DEFAULT_RANDOM_SEED = 42;

} // namespace nlm