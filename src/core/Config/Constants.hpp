// NLM Configuration Constants
// Centralized configuration for magic numbers and limits

#pragma once

#include <cstdint>
#include <limits>

namespace nlm {
namespace config {

// Neural network parameters
constexpr float DEFAULT_CURRENT_INJECTION = 50.0f;        // pA, typical current injection for stimulation
constexpr float DEFAULT_STDP_POTENTIATION = 60.0f;        // pA, current for LTP in STDP
constexpr float DEFAULT_SYNAPTIC_CHANGE_THRESHOLD = 0.01f; // mV, threshold for significant synaptic change
constexpr float DEFAULT_WEIGHT_CHANGE_THRESHOLD = 0.001f;   // mV, threshold for learning detection
constexpr float DEFAULT_PLASTICITY_WEIGHT = 0.02f;         // STDP LTP weight factor
constexpr float DEFAULT_DEPRESSION_WEIGHT = 0.015f;        // STDP LTD weight factor
constexpr float DEFAULT_STDP_TIME_CONST = 20.0f;           // ms, STDP time constant
constexpr float DEFAULT_SYNAPTOGENESIS_RATE = 0.0001f;     // per ms, rate of new synapse formation
constexpr float DEFAULT_PRUNING_RATE = 0.00001f;           // per ms, rate of synapse elimination

// Simulation parameters
constexpr float DEFAULT_SIMULATION_TIMESTEP = 0.001f;      // s, 1 ms timestep
constexpr size_t DEFAULT_SIMULATION_STEPS = 50;            // steps for basic tests
constexpr size_t DEFAULT_PLAPACITY_EXPERIMENT_STEPS = 1000; // steps for plasticity experiments
constexpr size_t DEFAULT_STDP_VERIFICATION_STEPS = 1000;  // steps for STDP testing
constexpr double DEFAULT_STDP_VERIFICATION_PREPOST_FREQUENCY = 1.0/0.002; // Hz, pre-post firing

// Network architecture parameters
constexpr size_t DEFAULT_NEURON_COUNT = 500;               // neurons in Phase 2 demo
constexpr size_t PHASE_6_DEMO_NEURON_COUNT = 500;          // neurons in Phase 6 demo
constexpr size_t MIN_NEURONS_FOR_TEST = 10;                // minimum neurons for connectivity tests
constexpr size_t MIN_SYNAPSES_FOR_STDP = 5;                // minimum synapses for STDP testing
constexpr float DEFAULT_CONNECTION_PROBABILITY = 0.15f;    // probability of connection between neurons

// Memory and logging parameters
constexpr size_t DEFAULT_SPIKE_HISTORY_SIZE = 1000;        // spikes to keep in history
constexpr size_t DEFAULT_WEIGHT_RECORDING_SIZE = 1000;     // weights to record in experiments
constexpr size_t DEFAULT_SPIKE_LOGGING_INTERVAL = 100;     // log interval for spikes

// File and path parameters
constexpr const char* DEFAULT_CONFIG_FILE = "configs/default.cfg";  // default config file path
constexpr const char* CHECKPOINT_FILE_EXTENSION = ".nlm_checkpoint";  // checkpoint file extension
constexpr size_t MAX_CHECKPOINT_SIZE_MB = 100;              // maximum checkpoint size in MB

// Performance parameters
constexpr size_t MAX_PARALLEL_SIMULATIONS = 4;              // maximum concurrent simulations
constexpr double MAX_SIMULATION_WALL_TIME = 300.0;          // maximum wall clock time in seconds
constexpr size_t MAX_SPIKE_RATE_HZ = 1000;                  // maximum firing rate in Hz

// Experimental parameters
constexpr float LEARNING_DETECTION_THRESHOLD = 0.001f;      // minimum weight change to consider learning
constexpr size_t MOST_ACTIVE_NEURONS_COUNT = 10;            // number of top active neurons to record
constexpr double EXPERIMENT_WALL_CLOCK_WARNING = 60.0;      // wall clock warning threshold

// Development parameters
constexpr size_t DEVELOPMENT_CRITICAL_PERIOD_STEPS = 1000; // steps for critical period
constexpr float DEVELOPMENT_MATURATION_RATE = 0.0005f;      // maturation rate per step
constexpr float DEVELOPMENT_PRUNING_EFFICIENCY = 0.8f;      // pruning efficiency factor

// Memory parameters
constexpr size_t WORKING_MEMORY_MAX_TRACES = 100;          // maximum working memory traces
constexpr size_t EPISODIC_MEMORY_MAX_EPISODES = 1000;      // maximum episodic memory episodes
constexpr size_t SEMANTIC_MEMORY_MAX_CONCEPTS = 500;        // maximum semantic memory concepts
constexpr size_t PROCEDURAL_MEMORY_MAX_SKILLS = 100;        // maximum procedural memory skills

// Neuromodulation parameters
constexpr float NEUROMODULATION_BASE_LEVEL = 0.1f;         // baseline neuromodulation level
constexpr float NEUROMODULATION_MAX_LEVEL = 1.0f;          // maximum neuromodulation level
constexpr float NEUROMODULATION_DECAY_RATE = 0.01f;        // neuromodulation decay per step
constexpr float NOVELTY_THRESHOLD = 0.05f;                  // novelty detection threshold
constexpr float CURIOSITY_GAIN = 0.5f;                      // curiosity modulation gain

// Prediction parameters
constexpr float PREDICTION_ERROR_THRESHOLD = 0.1f;          // prediction error threshold
constexpr size_t PREDICTION_WINDOW_SIZE = 10;               // prediction window size
constexpr float PREDICTION_LEARNING_RATE = 0.01f;           // prediction learning rate

// Cognitive parameters
constexpr size_t PLANNER_MAX_ACTIONS = 10;                  // maximum actions in plan
constexpr float PLANNER_ACTION_COST = 1.0f;                  // action cost factor
constexpr size_t CONCEPT_FORMATION_MIN_SAMPLES = 50;       // minimum samples for concept formation
constexpr float CONCEPT_FORMATION_THRESHOLD = 0.1f;         // concept formation threshold
constexpr size_t ATTENTION_POOL_SIZE = 50;                  // attentional selection pool size

// Error and exception parameters
constexpr int64_t ERROR_INVALID_PARAMETER = -1001;        // invalid parameter error code
constexpr int64_t ERROR_FILE_NOT_FOUND = -1002;             // file not found error code
constexpr int64_t ERROR_INVALID_STATE = -1003;             // invalid state error code
constexpr int64_t ERROR_OUT_OF_MEMORY = -1004;              // out of memory error code

// Logging parameters
constexpr size_t MAX_LOG_MESSAGE_LENGTH = 1024;            // maximum log message length
constexpr size_t LOG_BUFFER_SIZE = 10000;                  // log buffer size
constexpr size_t LOG_FLUSH_INTERVAL_MS = 100;               // log flush interval

// Python bindings parameters
constexpr size_t PYTHON_MAX_SAMPLES_FOR_VALIDATION = 1000; // maximum samples for Python validation
constexpr size_t PYTHON_MAX_ARRAY_DIMENSION = 3;           // maximum array dimension for Python bindings

// Phase 2 experimental parameters
constexpr size_t PHASE_2_TEST_STEPS = 50;
constexpr size_t PHASE_2_PLASTICITY_STEPS = 1000;
constexpr size_t PHASE_2_STDP_STEPS = 1000;
constexpr float PHASE_2_CURRENT_INJECTION = 50.0f;
constexpr float PHASE_2_STDP_CURRENT = 60.0f;
constexpr double PHASE_2_TIMESTEP = 0.001;

// Path constants
constexpr const char* DEFAULT_CONFIG_PATH = "configs/default.cfg";
constexpr const char* CHECKPOINT_DIRECTORY = "checkpoints";
constexpr const char* LOG_DIRECTORY = "logs";
constexpr const char* TEMP_DIRECTORY = "/tmp";

} // namespace config
} // namespace nlm