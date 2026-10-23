// NLM (Neural Learning Machine) - Core Constants
// 
// Centralized definition of magic numbers and constants used throughout the NLM project.
// This improves code maintainability and reduces the risk of inconsistent values.

#pragma once

namespace nlm {

// Simulation constants
namespace Simulation {
    // Default timestep for neural simulations (1ms)
    constexpr double DEFAULT_TIMESTEP = 0.001;  // seconds
    
    // Maximum and minimum timestep values
    constexpr double MIN_TIMESTEP = 1e-6;      // 1 microsecond
    constexpr double MAX_TIMESTEP = 1.0;       // 1 second
    
    // Simulation step bounds
    constexpr SimulationStep MIN_STEP = 0;
    constexpr SimulationStep MAX_STEP = 1000000;
    
    // Time thresholds (in simulation steps)
    constexpr SimulationStep SHORT_EPISODE = 100;
    constexpr SimulationStep MEDIUM_EPISODE = 1000;
    constexpr SimulationStep LONG_EPISODE = 10000;
}

// Neural dynamics constants
namespace Neuron {
    // LIF neuron parameters (standard values)
    constexpr double DEFAULT_RESTING_POTENTIAL = -70.0;   // mV
    constexpr double DEFAULT_THRESHOLD = -55.0;          // mV
    constexpr double DEFAULT_RESET_POTENTIAL = -70.0;    // mV
    constexpr double DEFAULT_LEAK_CONDUCTANCE = 10.0;     // nS
    constexpr double DEFAULT_REFLECTORY_PERIOD = 5;      // simulation steps
    
    // Firing rate limits
    constexpr double MIN_FIRING_RATE = 0.0;              // Hz
    constexpr double MAX_FIRING_RATE = 1000.0;           // Hz
    
    // Membrane potential bounds
    constexpr double MIN_MEMBRANE_POTENTIAL = -100.0;   // mV
    constexpr double MAX_MEMBRANE_POTENTIAL = 50.0;     // mV
}

// Plasticity constants
namespace Plasticity {
    // STDP parameters
    constexpr double DEFAULT_STDP_LEARNING_RATE = 0.001;
    constexpr double DEFAULT_STDP_TIME_CONSTANT = 20.0;  // ms
    constexpr double DEFAULT_STDP_LTP_WEIGHT = 0.02;
    constexpr double DEFAULT_STDP_LTD_WEIGHT = 0.015;
    
    // Hebbian learning parameters
    constexpr double DEFAULT_HEBBIAN_LEARNING_RATE = 0.01;
    
    // Structural plasticity parameters
    constexpr double DEFAULT_SYNAPTOGENESIS_RATE = 0.0001;
    constexpr double DEFAULT_PRUNING_RATE = 0.00001;
    
    // Weight bounds
    constexpr double MIN_SYNAPTIC_WEIGHT = 0.0;
    constexpr double MAX_SYNAPTIC_WEIGHT = 10.0;
}

// Memory system constants
namespace Memory {
    // Working memory capacity limits
    constexpr size_t MAX_WORKING_MEMORY_ENTRIES = 100;
    constexpr size_t DEFAULT_WORKING_MEMORY_SIZE = 50;
    
    // Episodic memory parameters
    constexpr size_t MAX_EPISODIC_RECORDS = 1000;
    constexpr double EPISODE_RECALL_THRESHOLD = 0.5;
}

// Neuromodulation constants
namespace Neuromodulation {
    // Dopamine signal ranges
    constexpr float MIN_DOPAMINE_LEVEL = -1.0f;
    constexpr float MAX_DOPAMINE_LEVEL = 1.0f;
    
    // Curiosity and novelty ranges
    constexpr float MIN_CURIOUSITY_LEVEL = 0.0f;
    constexpr float MAX_CURIOUSITY_LEVEL = 1.0f;
    
    // Reward prediction error bounds
    constexpr float MIN_PREDICTION_ERROR = -1.0f;
    constexpr float MAX_PREDICTION_ERROR = 1.0f;
}

// Configuration constants
namespace Config {
    // Default configuration values
    constexpr int64_t DEFAULT_RANDOM_SEED = 42;
    constexpr int64_t DEFAULT_NEURON_COUNT = 1000;
    constexpr int64_t DEFAULT_REGION_COUNT = 1;
    constexpr float DEFAULT_CONNECTION_PROBABILITY = 0.1f;
    constexpr double DEFAULT_SIMULATION_TIMESTEP = 0.001;
    
    // Configuration file constants
    constexpr const char* DEFAULT_CONFIG_FILE = "configs/default.cfg";
    constexpr const char* CONFIG_FILE_EXTENSION = ".cfg";
    constexpr const char* CONFIG_FILE_BACKUP_EXTENSION = ".backup";
    
    // Configuration validation bounds
    constexpr int64_t MIN_NEURON_COUNT = 1;
    constexpr int64_t MAX_NEURON_COUNT = 100000;
    constexpr float MIN_CONNECTION_PROBABILITY = 0.0f;
    constexpr float MAX_CONNECTION_PROBABILITY = 1.0f;
}

// Agent system constants
namespace Agent {
    // Motor command constants
    constexpr size_t NUM_MOTOR_COMMANDS = 8;
    constexpr size_t NUM_SOMATIC_MOVEMENTS = 3;
    constexpr size_t NUM_HEAD_MOVEMENTS = 2;
    
    // Sensory processing constants
    constexpr size_t VISION_WIDTH = 16;
    constexpr size_t VISION_HEIGHT = 16;
    constexpr size_t TOUCH_SENSORS = 8;
    constexpr size_t INTERNAL_SENSORS = 4;
    constexpr size_t PROPRIOCEPTION_SENSORS = 6;
    
    // Sensory input ranges
    constexpr float VISION_MIN_VALUE = 0.0f;
    constexpr float VISION_MAX_VALUE = 1.0f;
    constexpr float TOUCH_MIN_VALUE = 0.0f;
    constexpr float TOUCH_MAX_VALUE = 1.0f;
}

// World system constants
namespace World {
    // World dimensions
    constexpr int DEFAULT_WORLD_WIDTH = 20;
    constexpr int DEFAULT_WORLD_HEIGHT = 20;
    constexpr int MIN_WORLD_SIZE = 5;
    constexpr int MAX_WORLD_SIZE = 100;
    
    // Vision and sensing
    constexpr int DEFAULT_VISION_WIDTH = 8;
    constexpr int DEFAULT_VISION_HEIGHT = 8;
    constexpr float VISION_FOV_ANGLE = 90.0f;  // degrees
    
    // Physics constants
    constexpr float GRAVITY = 9.81f;
    constexpr float MAX_VELOCITY = 10.0f;
    constexpr float MAX_ANGULAR_VELOCITY = 3.14159f;  // rad/s
    
    // Energy and resources
    constexpr float MAX_ENERGY = 100.0f;
    constexpr float MIN_ENERGY = 0.0f;
    constexpr float ENERGY_DECAY_RATE = 0.01f;
}

// Performance constants
namespace Performance {
    // Benchmarking constants
    constexpr size_t BENCHMARK_STEPS = 1000;
    constexpr double BENCHMARK_TIME_THRESHOLD = 1.0;  // seconds
    
    // Parallel processing
    constexpr size_t MAX_PARALLEL_THREADS = 32;
    constexpr size_t SIMD_VECTOR_SIZE = 32;  // bytes
    
    // Memory management
    constexpr size_t MEMORY_POOL_ALIGNMENT = 64;  // bytes
    constexpr size_t DEFAULT_MEMORY_POOL_SIZE = 1024 * 1024;  // 1MB
}

// Error handling constants
namespace Error {
    // Error codes
    constexpr int ERROR_SUCCESS = 0;
    constexpr int ERROR_GENERAL = 1;
    constexpr int ERROR_INVALID_ARGUMENT = 2;
    constexpr int ERROR_OUT_OF_MEMORY = 3;
    constexpr int ERROR_FILE_NOT_FOUND = 4;
    constexpr int ERROR_INVALID_CONFIG = 5;
    constexpr int ERROR_SIMULATION_FAILED = 6;
    
    // Logging levels
    constexpr int LOG_LEVEL_TRACE = 0;
    constexpr int LOG_LEVEL_DEBUG = 1;
    constexpr int LOG_LEVEL_INFO = 2;
    constexpr int LOG_LEVEL_WARNING = 3;
    constexpr int LOG_LEVEL_ERROR = 4;
    constexpr int LOG_LEVEL_FATAL = 5;
}

// Validation functions
namespace Validation {
    template<typename T>
    bool isWithinBounds(const T& value, const T& min, const T& max) {
        return value >= min && value <= max;
    }
    
    template<typename T>
    bool isPositive(const T& value) {
        return value > T(0);
    }
    
    template<typename T>
    bool isNonNegative(const T& value) {
        return value >= T(0);
    }
    
    bool isValidConfiguration(const Config& config);
    bool isValidSimulationStep(SimulationStep step);
    bool isValidTimestep(double timestep);
}

} // namespace nlm