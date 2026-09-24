// NLM Neural Simulation Framework - Configuration Constants
// Common constants used throughout the NLM codebase to replace magic numbers
//
// These constants provide biologically-inspired values for neural simulation parameters
// and improve code readability and maintainability.

#pragma once

namespace nlm {

// ================================================
// NEURON DYNAMICS CONSTANTS
// ================================================

struct NeuronConstants {
    // Membrane properties (mV)
    static constexpr float MEMBRANE_CAPACITANCE = 1.0f;
    static constexpr float TIME_CONSTANT = 20.0f;
    static constexpr float MAX_SPIKE_HISTORY = 100.0f;
    static constexpr float MEMBRANE_RESISTANCE = 100.0f;
    
    // Electrical properties
    static constexpr float RESTING_POTENTIAL = -70.0f;
    static constexpr float THRESHOLD_POTENTIAL = -50.0f;
    static constexpr float RESET_POTENTIAL = -75.0f;
    
    // Ion channel properties
    static constexpr float LEAK_CONDUCTANCE = 0.3f;
    static constexpr float POTASSIUM_CONDUCTANCE = 36.0f;
    static constexpr float SODIUM_CONDUCTANCE = 120.0f;
    
    // Synaptic properties
    static constexpr float SYNAPTIC_DELAY_DEFAULT = 1.0f;
    static constexpr float SYNAPTIC_WEIGHT_MIN = 0.0f;
    static constexpr float SYNAPTIC_WEIGHT_MAX = 1.0f;
    static constexpr float SYNAPTIC_WEIGHT_DEFAULT = 0.5f;
    static constexpr float SYNAPTIC_CONDUCTANCE = 0.5f;
};

// ================================================
// SIMULATION CONSTANTS
// ================================================

struct SimulationConstants {
    // Time management
    static constexpr double DEFAULT_TIMESTEP = 0.001;  // 1 ms
    static constexpr double MIN_TIMESTEP = 1e-6;        // 1 μs
    static constexpr double MAX_TIMESTEP = 0.1;         // 100 ms
    static constexpr double SIMULATION_TIME_STEP = 0.001;  // ms
    
    // Simulation control
    static constexpr SimulationStep MAX_SIMULATION_STEPS = 1000000;
    static constexpr SimulationStep DEFAULT_MAX_STEPS = 10000;
    static constexpr size_t MAX_NEURON_COUNT = 10000;
    static constexpr size_t DEFAULT_NEURON_COUNT = 1000;
    static constexpr size_t MIN_NEURON_COUNT = 10;
    
    // Numerical precision
    static constexpr float FLOAT_EPSILON = 1e-6f;
    static constexpr float FLOAT_PRECISION = 1e-4f;
    static constexpr float DOUBLE_EPSILON = 1e-9;
};

// ================================================
// PLASTICITY CONSTANTS
// ================================================

struct PlasticityConstants {
    // STDP (Spike-Timing-Dependent Plasticity)
    static constexpr float STDP_LTP_WEIGHT = 0.01f;
    static constexpr float STDP_LTD_WEIGHT = 0.012f;
    static constexpr float STDP_TIME_CONSTANT = 20.0f;  // τ in ms
    static constexpr float STDP_TIME_WINDOW = 50.0f;    // Time window for STDP
    static constexpr float STDP_THRESHOLD = 0.001f;    // Weight change threshold
    
    // Hebbian plasticity
    static constexpr float HEBBIAN_LEARNING_RATE = 0.001f;
    static constexpr float HEBBIAN_WINDOW_SIZE = 10.0f;
    static constexpr float HEBBIAN_THRESHOLD = 0.1f;
    
    // Structural plasticity
    static constexpr float SYNAPTONGENESIS_BASE_RATE = 0.0001f;
    static constexpr float PRUNING_BASE_RATE = 0.00001f;
    static constexpr float SYNAPTIC_STABILITY_THRESHOLD = 0.01f;
    static constexpr float CONNECTION_STRENGTH_THRESHOLD = 0.1f;
    
    // Eligibility traces
    static constexpr float ELIGIBILITY_TRACE_DECAY = 0.1f;
    static constexpr float ELIGIBILITY_TRACE_MAX = 1.0f;
    static constexpr float ELIGIBILITY_TRACE_MIN = 0.001f;
};

// ================================================
// NEUROMODULATION CONSTANTS
// ================================================

struct NeuromodulationConstants {
    // Dopamine system
    static constexpr float DOPAMINE_PEAK_LEVEL = 1.0f;
    static constexpr float DOPAMINE_BASELINE = 0.1f;
    static constexpr float DOPAMINE_DECAY_RATE = 0.1f;
    static constexpr float DOPAMINE_RELEASE_RATE = 1.0f;
    static constexpr float DOPAMINE_PLASTICITY_MODIFIER_STRENGTH = 0.5f;
    
    // Serotonin (simplified)
    static constexpr float SEROTONIN_PEAK_LEVEL = 1.0f;
    static constexpr float SEROTONIN_DECAY_RATE = 0.05f;
    
    // Noradrenaline (simplified)
    static constexpr float NORADRENALINE_PEAK_LEVEL = 1.0f;
    static constexpr float NORADRENALINE_DECAY_RATE = 0.2f;
    
    // Acetylcholine (simplified)
    static constexpr float ACHOLINE_PEAK_LEVEL = 1.0f;
    static constexpr float ACHOLINE_DECAY_RATE = 0.15f;
};

// ================================================
// DEVELOPMENT CONSTANTS
// ================================================

struct DevelopmentConstants {
    // Developmental stages
    static constexpr double DEVELOPMENT_INITIAL = 60.0;      // ~1 minute
    static constexpr double DEVELOPMENT_CRITICAL = 300.0;    // ~5 minutes
    static constexpr double DEVELOPMENT_MATURATION = 900.0;  // ~15 minutes
    static constexpr double DEVELOPMENT_ADULT = 3600.0;      // ~1 hour
    static constexpr double DEVELOPMENT_AGING = 86400.0;     // ~1 day
    
    // Plasticity modulation through development
    static constexpr float PLASTICITY_INITIAL = 1.0f;
    static constexpr float PLASTICITY_CRITICAL = 0.8f;
    static constexpr float PLASTICITY_MATURATION = 0.5f;
    static constexpr float PLASTICITY_ADULT = 0.2f;
    static constexpr float PLASTICITY_AGING = 0.1f;
    
    // Structural development
    static constexpr float SYNAPTONGENESIS_RATE_INITIAL = 0.001f;
    static constexpr float PRUNING_RATE_INITIAL = 0.0001f;
    static constexpr float MYELINATION_RATE = 0.00005f;
};

// ================================================
// SENSOR/MOTOR CONSTANTS
// ================================================

struct SensoryMotorConstants {
    // Sensory processing
    static constexpr float VISION_SCALE_FACTOR = 5.0f;
    static constexpr float TOUCH_SCALE_FACTOR = 8.0f;
    static constexpr float INTERNAL_SCALE_FACTOR = 5.0f;
    static constexpr float PROPRIOCEPTION_SCALE_FACTOR = 3.0f;
    
    // Vision processing
    static constexpr size_t VISION_WIDTH = 16;
    static constexpr size_t VISION_HEIGHT = 16;
    static constexpr size_t VISION_NEUTRAL_POINT = 128;
    
    // Touch processing
    static constexpr size_t TOUCH_SENSORS = 8;
    static constexpr float TOUCH_THRESHOLD = 0.1f;
    
    // Internal signals
    static constexpr size_t INTERNAL_SIGNALS = 4;
    static constexpr size_t PROPRIOCEPTION_SENSORS = 6;
    
    // Motor control
    static constexpr size_t MOTOR_ACTIONS = 6;
    static constexpr float MOTOR_ACTIVITY_THRESHOLD = 0.5f;
    static constexpr float MOTOR_OUTPUT_RANGE = 1.0f;
    
    // Motor neuron distribution
    static constexpr size_t NEURONS_PER_ACTION = 1;  // At least one neuron per action
};

// ================================================
// ENVIRONMENT CONSTANTS
// ================================================

struct EnvironmentConstants {
    // Simple world
    static constexpr int WORLD_DEFAULT_WIDTH = 20;
    static constexpr int WORLD_DEFAULT_HEIGHT = 20;
    static constexpr float WORLD_DEFAULT_VISION_WIDTH = 8.0f;
    static constexpr float WORLD_DEFAULT_VISION_HEIGHT = 8.0f;
    static constexpr float WORLD_DEFAULT_MAX_ENERGY = 100.0f;
    static constexpr float WORLD_DEFAULT_TIME_STEP = 0.1f;
    
    // World dynamics
    static constexpr float GRAVITY_STRENGTH = 9.8f;
    static constexpr float FRICTION_COEFFICIENT = 0.5f;
    static constexpr float MAX_WALKING_SPEED = 2.0f;
    
    // Resource dynamics
    static constexpr float RESOURCE_DECAY_RATE = 0.01f;
    static constexpr float RESOURCE_REGROWTH_RATE = 0.001f;
    static constexpr float RESOURCE_CONSUMPTION_RATE = 0.5f;
};

// ================================================
// EXPERIMENT CONSTANTS
// ================================================

struct ExperimentConstants {
    // Learning experiments
    static constexpr SimulationStep LEARNING_EXPERIMENT_STEPS = 1000;
    static constexpr SimulationStep STDP_VERIFICATION_TRIALS = 50;
    static constexpr size_t STDP_VERIFICATION_SYNAPSES = 5;
    static constexpr float STDP_POTENTIATION_THRESHOLD = 0.001f;
    static constexpr float STDP_DEPRESSION_THRESHOLD = 0.001f;
    
    // Performance metrics
    static constexpr float PERFORMANCE_CONVERGENCE_THRESHOLD = 0.95f;
    static constexpr float PERFORMANCE_STABILITY_THRESHOLD = 0.99f;
    static constexpr size_t STAT_SAMPLING_INTERVAL = 100;
};

// ================================================
// LOGGING CONSTANTS
// ================================================

struct LoggingConstants {
    // Log levels
    static constexpr int DEBUG_LEVEL = 0;
    static constexpr int INFO_LEVEL = 1;
    static constexpr int WARNING_LEVEL = 2;
    static constexpr int ERROR_LEVEL = 3;
    static constexpr int CRITICAL_LEVEL = 4;
    
    // Log formatting
    static constexpr size_t MAX_LOG_MESSAGE_LENGTH = 1024;
    static constexpr size_t MAX_LOG_FILE_SIZE = 10 * 1024 * 1024;  // 10 MB
    static constexpr size_t LOG_ROTATION_COUNT = 5;
    
    // Performance
    static constexpr size_t LOG_BATCH_SIZE = 100;
    static constexpr double LOG_FLUSH_INTERVAL = 1.0;  // seconds
};

// ================================================
// MISCELLANEOUS CONSTANTS
// ================================================

struct MiscConstants {
    // Mathematical constants
    static constexpr float PI = 3.141592653589793f;
    static constexpr float E = 2.718281828459045f;
    static constexpr float SQRT2 = 1.414213562373095f;
    
    // Probability
    static constexpr float RANDOM_UNIFORM_MIN = 0.0f;
    static constexpr float RANDOM_UNIFORM_MAX = 1.0f;
    static constexpr float RANDOM_NORMAL_MEAN = 0.0f;
    static constexpr float RANDOM_NORMAL_STDDEV = 1.0f;
    
    // Array bounds
    static constexpr size_t MAX_VECTOR_SIZE = 10000;
    static constexpr size_t DEFAULT_VECTOR_CAPACITY = 100;
    
    // Configuration
    static constexpr const char* DEFAULT_CONFIG_FILE = "configs/default.cfg";
    static constexpr const char* DEFAULT_LOG_FILE = "nlm.log";
    static constexpr const char* BACKUP_EXTENSION = ".bak";
    static constexpr const char* CHECKPOINT_EXTENSION = ".nlm";
};

// ================================================
// ALIASES FOR COMMON CONSTANTS
// ================================================

// Convenience aliases for frequently used constants
using NeuronMembraneCapacitance = NeuronConstants::MEMBRANE_CAPACITANCE;
using NeuronTimeConstant = NeuronConstants::TIME_CONSTANT;
using SimulationTimeStep = SimulationConstants::DEFAULT_TIMESTEP;
using STDPWeight = PlasticityConstants::STDP_LTP_WEIGHT;
using DevelopmentInitial = DevelopmentConstants::DEVELOPMENT_INITIAL;
using VisionScale = SensoryMotorConstants::VISION_SCALE_FACTOR;
using MotorThreshold = SensoryMotorConstants::MOTOR_ACTIVITY_THRESHOLD;
using LogLevelDebug = LoggingConstants::DEBUG_LEVEL;

} // namespace nlm