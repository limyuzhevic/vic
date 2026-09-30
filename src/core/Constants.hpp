// NLM Constants and Configuration Defaults
// Centralized constants for Phase 2: Real Neural Computation
// Generated automatically from configuration system

#pragma once

#include <string>

namespace nlm {
namespace constants {

// ===== PROGRAM INFORMATION =====
const std::string PROGRAM_NAME = "NLM";
const std::string PROGRAM_DESCRIPTION = "Neural Learning Machine - Phase 2: Real Neural Computation";
const std::string PROGRAM_VERSION = "0.1.0";
const std::string PROGRAM_AUTHOR = "NLM Development Team";
const std::string PROGRAM_COPYRIGHT = "Copyright (c) 2026 NLM";
const std::string PROGRAM_WEBSITE = "https://github.com/nlm/NeuralLearningMachine";

// ===== SIMULATION CORE =====
const int64_t DEFAULT_RANDOM_SEED = 42;
const double DEFAULT_SIMULATION_TIMESTEP = 0.001;  // 1ms
const int64_t DEFAULT_NEURON_COUNT = 500;
const int64_t DEFAULT_REGION_COUNT = 1;
const double DEFAULT_CONNECTION_PROBABILITY = 0.15;

// ===== STDP PLASTICITY =====
const double DEFAULT_STDP_LTP_WEIGHT = 0.02;
const double DEFAULT_STDP_LTD_WEIGHT = 0.015;
const double DEFAULT_STDP_TIME_CONSTANT = 20.0;  // ms

// ===== STRUCTURAL PLASTICITY =====
const double DEFAULT_SYNAPTOGENESIS_RATE = 0.0001;
const double DEFAULT_PRUNING_RATE = 0.00001;

// ===== NEURON PARAMETERS =====
const double LIF_RESTING_POTENTIAL = -70.0;  // mV
const double LIF_THRESHOLD = -55.0;         // mV
const double LIF_RESET_POTENTIAL = -65.0;   // mV
const uint32_t DEFAULT_REFECTORY_PERIOD = 10;  // ms

// ===== SYNAPSE PARAMETERS =====
const double DEFAULT_SYNAPSE_WEIGHT = 0.5;
const double MAX_SYNAPSE_WEIGHT = 10.0;
const double MIN_SYNAPSE_WEIGHT = 0.01;
const double SYNAPSE_DELAY = 1.0;  // ms

// ===== SENSORY PARAMETERS =====
const double SENSORY_EXCITATORY_STRENGTH = 30.0;  // Current units
const double SENSORY_INHIBITORY_STRENGTH = 15.0;  // Current units

// ===== MOTOR PARAMETERS =====
const double MOTOR_ACTIVATION_THRESHOLD = 0.5;
const double MOTOR_MAX_OUTPUT = 1.0;

// ===== MEMORY SYSTEMS =====
const size_t WORKING_MEMORY_CAPACITY = 100;
const size_t EPISODIC_MEMORY_CAPACITY = 1000;
const double MEMORY_DECAY_RATE = 0.95;

// ===== PREDICTION SYSTEM =====
const double PREDICTION_ERROR_THRESHOLD = 0.1;
const double PREDICTION_LEARNING_RATE = 0.01;
const double PREDICTION_DISCOUNT_FACTOR = 0.9;

// ===== NEUROMODULATION =====
const double DOPAMINE_BASELINE = 0.1;
const double DOPAMINE_PEAK = 1.0;
const double CURIOUSY_EXPLORATION_RATE = 0.1;
const double NOVELTY_THRESHOLD = 0.5;

// ===== DEVELOPMENT SYSTEM =====
const double MATURATION_RATE = 0.01;
const double CONNECTION_PRUNING_THRESHOLD = 0.1;

// ===== TESTING AND DEBUGGING =====
const size_t BASIC_TEST_NEURONS = 10;
const size_t BASIC_TEST_STEPS = 50;
const size_t PLASTICITY_TEST_STEPS = 1000;
const size_t STDP_TEST_TRIALS = 50;
const int64_t TEST_RANDOM_SEED = 42;

// ===== LOGGING =====
const std::string DEFAULT_LOG_LEVEL = "INFO";
const bool DEFAULT_LOG_TO_FILE = false;
const std::string DEFAULT_LOG_FILENAME = "nlm.log";
const size_t MAX_LOG_HISTORY = 1000;

// ===== ENVIRONMENT =====
const std::string DEFAULT_ENVIRONMENT_NAME = "GridWorld";
const int64_t DEFAULT_ENVIRONMENT_WIDTH = 10;
const int64_t DEFAULT_ENVIRONMENT_HEIGHT = 10;

// ===== VISUALIZATION =====
const bool DEFAULT_VISUALIZATION_ENABLED = false;
const double DEFAULT_VISUALIZATION_UPDATE_RATE = 30.0;  // Hz

// ===== SIMULATION CONTROL =====
const int64_t DEFAULT_MAX_SIMULATION_STEPS = 10000;
const double DEFAULT_SIMULATION_TIME_LIMIT = 0.0;  // 0 = unlimited

// ===== ERROR CODES =====
const int ERROR_NONE = 0;
const int ERROR_FILE_NOT_FOUND = 1;
const int ERROR_INVALID_FORMAT = 2;
const int ERROR_MEMORY_ALLOCATION = 3;
const int ERROR_INVALID_ARGUMENTS = 4;
const int ERROR_SIMULATION_FAILED = 5;

// ===== LOG MESSAGES =====
const std::string LOG_MESSAGE_BANNER_START = "Starting NLM simulation...";
const std::string LOG_MESSAGE_BANNER_COMPLETE = "Simulation complete.";
const std::string LOG_MESSAGE_LEARNING_DETECTED = "Learning detected: synaptic weights changed through experience";
const std::string LOG_MESSAGE_NO_LEARNING = "No learning: weights did not change significantly";
const std::string LOG_MESSAGE_TEST_START = "Running test: ";
const std::string LOG_MESSAGE_TEST_COMPLETE = "Test completed.";

// ===== ADVANCED FEATURES =====
const double ADVANCED_FEATURE_THRESHOLD = 0.8;
const size_t ADVANCED_FEATURE_SAMPLE_SIZE = 100;

// ===== BACKWARD COMPATIBILITY =====
// Legacy parameter names (deprecated)
const std::string LEGACY_RANDOM_SEED = "random_seed";
const std::string LEGACY_NEURON_COUNT = "neuron_count";
const std::string LEGACY_CONNECTION_PROB = "connection_probability";

} // namespace constants
} // namespace nlm