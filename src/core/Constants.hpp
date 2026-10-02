#pragma once

namespace nlm {

// Configuration constants
namespace ConfigConstants {
    // Default values
    constexpr int64_t DEFAULT_RANDOM_SEED = 42;
    constexpr double DEFAULT_SIMULATION_TIMESTEP = 0.001;
    constexpr int64_t DEFAULT_NEURON_COUNT = 500;
    constexpr int64_t DEFAULT_REGION_COUNT = 1;
    constexpr float DEFAULT_CONNECTION_PROBABILITY = 0.15f;
    constexpr float DEFAULT_STDP_LTP_WEIGHT = 0.02f;
    constexpr float DEFAULT_STDP_LTD_WEIGHT = 0.015f;
    constexpr float DEFAULT_STDP_TAU = 20.0f;
    constexpr float DEFAULT_SYNAPTOGENESIS_RATE = 0.0001f;
    constexpr float DEFAULT_PRUNING_RATE = 0.00001f;
    
    // Simulation constants
    constexpr SimulationStep DEFAULT_CONNECTIVITY_TEST_STEPS = 50;
    constexpr SimulationStep PLASTICITY_EXPERIMENT_STEPS = 1000;
    constexpr int DEFAULT_STDP_TRIALS = 50;
    constexpr size_t DEFAULT_STDP_SYNAPSE_COUNT = 5;
    constexpr int DEFAULT_CONNECTIVITY_TEST_NEURONS = 10;
    constexpr int DEFAULT_SYNAPTIC_CHANGE_THRESHOLD = 10;
    constexpr float DEFAULT_SYNAPTIC_DELTA_THRESHOLD = 0.01f;
    
    // Network topology
    constexpr int64_t MIN_NEURONS = 10;
    constexpr int64_t MAX_NEURONS = 10000;
    constexpr float MIN_CONNECTION_PROBABILITY = 0.01f;
    constexpr float MAX_CONNECTION_PROBABILITY = 0.5f;
    
    // Input/output
    constexpr const char* DEFAULT_CONFIG_FILE = "configs/default.cfg";
    
    // Memory and performance
    constexpr int MAX_SYNAPTIC_HISTORY_SIZE = 1000;
    constexpr size_t MIN_SYNAPSE_COUNT_FOR_TESTING = 5;
}

} // namespace nlm