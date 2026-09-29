#pragma once

namespace nlm {

namespace constants {

// Main.cpp Phase 2 constants
namespace main_phase2 {
    // Neural dynamics
    constexpr float DEFAULT_MEMBRANE_THRESHOLD = -50.0f;
    constexpr float DEFAULT_RESTING_POTENTIAL = -70.0f;
    constexpr float DEFAULT_RESET_POTENTIAL = -75.0f;
    constexpr float DEFAULT_MEMBRANE_TIME_CONSTANT = 20.0f;
    constexpr float DEFAULT_REFECTORY_PERIOD = 2.0f;
    
    // Plasticity
    constexpr float DEFAULT_STDP_LTP_WEIGHT = 0.01f;
    constexpr float DEFAULT_STDP_LTD_WEIGHT = 0.012f;
    constexpr float DEFAULT_STDP_TIME_CONSTANT = 20.0f;
    constexpr float DEFAULT_HEBBIAN_LEARNING_RATE = 0.001f;
    
    // Structural plasticity
    constexpr float DEFAULT_SYNAPTOGENESIS_RATE = 0.0001f;
    constexpr float DEFAULT_PRUNING_RATE = 0.00001f;
    
    // Development stages (in simulation steps)
    constexpr double DEVELOPMENT_STAGE_INITIAL = 60.0;      // ~1 minute
    constexpr double DEVELOPMENT_STAGE_CRITICAL = 300.0;    // ~5 minutes
    constexpr double DEVELOPMENT_STAGE_MATURATION = 900.0;  // ~15 minutes
    
    // Test constants
    constexpr float EXCITATORY_CURRENT_STRENGTH = 50.0f;
    constexpr size_t BASIC_CONNECTIVITY_TEST_STEPS = 50;
    constexpr float SYNAPTIC_CURRENT_FOR_TESTING = 60.0f;
    constexpr size_t STDP_TEST_TRIALS = 50;
    constexpr size_t REPEATED_INPUT_PATTERN_STEPS = 1000;
    
    // Time constants
    constexpr double DEFAULT_TIMESTEP = 0.001;
    constexpr double TIME_CONVERSION_FACTOR = 0.001;
    
    // Neuron counts
    constexpr int64_t DEFAULT_NEURON_COUNT = 1000;
    constexpr int64_t TEST_NEURON_COUNT_SMALL = 100;
    constexpr int64_t TEST_NEURON_COUNT_TINY = 50;
    
    // Connectivity
    constexpr float DEFAULT_CONNECTION_PROBABILITY = 0.15f;
    constexpr float DEFAULT_SYNAPSE_DENSITY = 0.1f;
    
    // Random seeds
    constexpr uint64_t DEFAULT_RANDOM_SEED = 42;
}

// AgentBrain constants
namespace agentbrain {
    // Neuromodulation
    constexpr float NOVELTY_WEIGHT_FACTOR = 2.0f;
    constexpr float PREDICTION_ERROR_WEIGHT_FACTOR = 0.5f;
    constexpr float DEFAULT_NOVELTY_DECAY_RATE = 0.99f;
    
    // Curiosity thresholds
    constexpr float CURIOUSITY_THRESHOLD = 0.3f;
    constexpr float EXPLORATION_THRESHOLD = 0.5f;
    constexpr float MAX_EXPLORATION_CHANCE = 0.3f;
    
    // Development stages
    constexpr double DEVELOPMENT_STAGE_INITIAL = 60.0;     // ~1 minute
    constexpr double DEVELOPMENT_STAGE_CRITICAL = 300.0;   // ~5 minutes
    constexpr double DEVELOPMENT_STAGE_MATURATION = 900.0; // ~15 minutes
    
    // Reward modulation
    constexpr float REWARD_PREDICTION_DECAY = 0.95f;
    constexpr float REWARD_PREDICTION_UPDATE = 0.05f;
    constexpr float PLASTICITY_FACTOR_BASE = 0.5f;
    constexpr float PLASTICITY_FACTOR_MIN = 0.1f;
    constexpr float PLASTICITY_FACTOR_MAX = 2.0f;
    
    // Structural plasticity
    constexpr float DEFAULT_SYNAPTOGENESIS_RATE = 0.0001f;
    constexpr float DEFAULT_PRUNING_RATE = 0.00001f;
    
    // Motor decoding
    constexpr float MOTOR_ACTIVITY_THRESHOLD = 0.5f;
    constexpr int MAX_MOTOR_COMMAND_CHOICES = 8;  // Forward, backward, left, right, look left, look right, interact, wait
    
    // Sensory processing scaling
    constexpr float VISION_INPUT_SCALE_FACTOR = 5.0f;
    constexpr float TOUCH_INPUT_SCALE_FACTOR = 8.0f;
    constexpr float INTERNAL_INPUT_SCALE_FACTOR = 5.0f;
    constexpr float PROPRIOCEPTION_INPUT_SCALE_FACTOR = 3.0f;
    
    // Memory and novelty detection
    constexpr size_t VISION_INPUT_SIZE = 256;  // 16x16 grid
    constexpr size_t TOUCH_INPUT_SIZE = 8;
    constexpr size_t INTERNAL_INPUT_SIZE = 4;
    constexpr size_t PROPRIOCEPTION_INPUT_SIZE = 6;
    
    // Dopamine modulation
    constexpr float DOPAMINE_CLAMP_MIN = -1.0f;
    constexpr float DOPAMINE_CLAMP_MAX = 1.0f;
    
    // Configuration flags
    constexpr bool DEFAULT_REWARD_MODULATION_ENABLED = true;
    constexpr bool DEFAULT_STRUCTURAL_PLASTICITY_ENABLED = true;
    constexpr bool DEFAULT_DEVELOPMENT_ENABLED = true;
    constexpr bool DEFAULT_CURIOUSITY_ENABLED = true;
}

// Experiment constants
namespace experiments {
    // Experiment configuration
    constexpr size_t EXPERIMENT_STEPS = 1000;
    constexpr size_t PLOT_INTERVAL = 100;
    constexpr float REWARD_SCALE = 1.0f;
    
    // Learning experiment parameters
    constexpr float LEARNING_THRESHOLD = 0.001f;
    constexpr float WEIGHT_CHANGE_THRESHOLD = 0.01f;
    
    // Plasticity experiment parameters
    constexpr size_t PLASTICITY_INPUT_DURATION = 100;
    constexpr float PLASTICITY_CURRENT_STRENGTH = 30.0f;
    
    // STDP experiment parameters
    constexpr size_t STDP_CORRELATED_TRIALS = 50;
    constexpr float STDP_PRE_POST_CURRENT = 60.0f;
    constexpr float STDP_CORRELATION_DELAY = 10.0f;  // ms
}

// Logging and debugging constants
namespace logging {
    constexpr const char* LOG_FILE_EXTENSION = ".log";
    constexpr size_t MAX_LOG_LINE_LENGTH = 1024;
    constexpr int LOG_BUFFER_SIZE = 4096;
    constexpr double LOG_FLUSH_INTERVAL = 0.1;  // seconds
    
    // Console logging levels
    constexpr int CONSOLE_LOG_LEVEL_DEFAULT = 2;  // Info
    constexpr int CONSOLE_LOG_LEVEL_MIN = 0;       // Critical
    constexpr int CONSOLE_LOG_LEVEL_MAX = 3;       // Debug
}

// Performance constants
namespace performance {
    constexpr size_t DEFAULT_WORKER_THREADS = 4;
    constexpr size_t MAX_CONCURRENT_SIMULATIONS = 8;
    constexpr double TARGET_SIMULATION_FPS = 60.0;
    constexpr size_t CHUNK_SIZE_FOR_PROCESSING = 1024;
}

// Validation constants
namespace validation {
    constexpr float INPUT_RANGE_MIN = -1000.0f;
    constexpr float INPUT_RANGE_MAX = 1000.0f;
    constexpr float WEIGHT_RANGE_MIN = 0.0f;
    constexpr float WEIGHT_RANGE_MAX = 10.0f;
    constexpr double SIMULATION_TIME_STEP_MIN = 0.0001;
    constexpr double SIMULATION_TIME_STEP_MAX = 0.1;
    constexpr size_t NEURON_COUNT_MIN = 1;
    constexpr size_t NEURON_COUNT_MAX = 1000000;
}

// Type conversion constants
namespace conversion {
    constexpr size_t MAX_STRING_LENGTH = 1024;
    constexpr size_t MAX_ARRAY_SIZE = 1000000;
    constexpr double EPSILON = 1e-9;
    constexpr float FLOAT_EPSILON = 1e-6f;
}

} // namespace constants

} // namespace nlm
