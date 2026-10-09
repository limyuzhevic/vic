#pragma once

namespace nlm {

namespace ConfigConstants {
    // Brain configuration constants
    constexpr int NEURON_COUNT_DEFAULT = 1000;
    constexpr double SYNAPSE_DENSITY_DEFAULT = 0.1;
    constexpr double CONNECTION_PROBABILITY_DEFAULT = 0.05;
    constexpr double INITIAL_WEIGHT_MEAN = 0.5;
    constexpr double INITIAL_WEIGHT_STD = 0.1;
    constexpr double V_THRESH_DEFAULT = -50.0;           // mV
    constexpr double V_REST_DEFAULT = -70.0;            // mV
    constexpr double V_RESET_DEFAULT = -75.0;           // mV
    constexpr double TAU_MEM_DEFAULT = 20.0;             // ms
    constexpr double TAU_REF_DEFAULT = 2.0;              // ms
    
    // Plasticity configuration constants
    constexpr bool STDP_ENABLE_DEFAULT = true;
    constexpr double STDP_LEARNING_RATE_DEFAULT = 0.001;
    constexpr double STDP_TAU_PLUS_DEFAULT = 20.0;       // ms
    constexpr double STDP_TAU_MINUS_DEFAULT = 20.0;      // ms
    constexpr bool HEBBIAN_ENABLE_DEFAULT = true;
    constexpr bool STRUCTURAL_PLASTICITY_ENABLE_DEFAULT = true;
    constexpr double HEBBIAN_LEARNING_RATE_DEFAULT = 0.01;
    constexpr double STRUCTURAL_PLASTICITY_RATE_DEFAULT = 0.0001f;
    
    // Neuromodulation configuration constants
    constexpr double DOPAMINE_SCALE_DEFAULT = 1.0;
    constexpr bool CURIOSITY_ENABLE_DEFAULT = true;
    constexpr bool NOVELTY_ENABLE_DEFAULT = true;
    constexpr double NOVELTY_SENSITIVITY_DEFAULT = 0.5;
    constexpr double PREDICTION_ERROR_SENSITIVITY_DEFAULT = 0.3;
    
    // Development configuration constants
    constexpr double DEVELOPMENT_STAGE_INITIAL = 60.0;     // seconds
    constexpr double DEVELOPMENT_STAGE_CRITICAL = 300.0;   // seconds
    constexpr double DEVELOPMENT_STAGE_MATURATION = 900.0; // seconds
    constexpr double DEVELOPMENT_STAGE_ADULT = 3600.0;     // seconds
    constexpr double PLASTICITY_INITIAL = 1.0;
    constexpr double PLASTICITY_CRITICAL = 0.8;
    constexpr double PLASTICITY_MATURATION = 0.5;
    constexpr double PLASTICITY_ADULT = 0.2;
    
    // Memory configuration constants
    constexpr size_t WORKING_MEMORY_SIZE_DEFAULT = 1000;
    constexpr size_t EPISODIC_MEMORY_CAPACITY_DEFAULT = 100;
    constexpr double EPISODIC_MEMORY_DECAY_RATE_DEFAULT = 0.95;
    constexpr double ASSOCIATIVE_MEMORY_HEBBIAN_STRENGTH_DEFAULT = 0.1;
    
    // Prediction system constants
    constexpr bool PREDICTION_ENABLE_DEFAULT = true;
    constexpr double PREDICTION_WINDOW_DEFAULT = 100.0;    // ms
    constexpr double PREDICTION_NOISE_DEFAULT = 0.01;
    constexpr double PREDICTION_CONFIDENCE_THRESHOLD = 0.7;
    
    // Concept formation constants
    constexpr bool CONCEPT_FORMATION_ENABLE_DEFAULT = true;
    constexpr size_t CONCEPT_MEMORY_SIZE_DEFAULT = 500;
    constexpr double CONCEPT_SIMILARITY_THRESHOLD_DEFAULT = 0.8;
    
    // Attention constants
    constexpr bool ATTENTION_ENABLE_DEFAULT = true;
    constexpr double ATTENTION_FOCUS_STRENGTH_DEFAULT = 1.0;
    constexpr double ATTENTION_DECAY_RATE_DEFAULT = 0.9;
    
    // World configuration constants
    constexpr int WORLD_WIDTH_DEFAULT = 20;
    constexpr int WORLD_HEIGHT_DEFAULT = 20;
    constexpr int VISION_WIDTH_DEFAULT = 8;
    constexpr int VISION_HEIGHT_DEFAULT = 8;
    constexpr double SIMULATION_TIMESTEP_DEFAULT = 0.1;   // seconds
    constexpr double WORLD_FRICTION_DEFAULT = 0.1;
    constexpr double WORLD_GRAVITY_DEFAULT = 9.8;
    
    // Motor system constants
    constexpr double MOTOR_VELOCITY_DEFAULT = 5.0;
    constexpr double MOTOR_TURN_RATE_DEFAULT = 45.0;      // degrees per second
    constexpr double MOTOR_INTERACTION_RANGE_DEFAULT = 1.0;
    constexpr double MOTOR_WAIT_COST_DEFAULT = 0.1;
    
    // Reward system constants
    constexpr double REWARD_EXPLORATION_BONUS_DEFAULT = 0.5;
    constexpr double REWARD_STEADY_STATE_DEFAULT = 0.0;
    constexpr double REWARD_DECAY_RATE_DEFAULT = 0.99;
    constexpr double REWARD_PREDICTION_ERROR_WEIGHT = 0.5;
    
    // Logging constants
    constexpr bool LOG_LEVEL_DEBUG_DEFAULT = false;
    constexpr size_t LOG_MAX_HISTORY_SIZE_DEFAULT = 1000;
    constexpr std::string LOG_FILE_DEFAULT = "nlm_simulation.log";
    
    // Performance constants
    constexpr size_t NEURON_POOL_SIZE_DEFAULT = 10000;
    constexpr size_t SYNAPSE_POOL_SIZE_DEFAULT = 50000;
    constexpr size_t MEMORY_POOL_ALIGNMENT = 16;
    constexpr double PERFORMANCE_TARGET_FPS = 1000.0;   // frames per second
    
    // Checkpoint constants
    constexpr bool CHECKPOINT_ENABLE_DEFAULT = true;
    constexpr double CHECKPOINT_INTERVAL_DEFAULT = 1000.0; // seconds
    constexpr std::string CHECKPOINT_FILE_PREFIX = "nlm_checkpoint_";
    constexpr std::string CHECKPOINT_FILE_EXTENSION = ".bin";
    
    // Simulation constants
    constexpr double SIMULATION_MAX_TIME_DEFAULT = 3600.0;  // seconds
    constexpr size_t SIMULATION_MAX_STEPS_DEFAULT = 100000;
    constexpr double SIMULATION_TIME_EPSILON = 1e-6;
    constexpr size_t SIMULATION_THREAD_COUNT_DEFAULT = 4;
    
    // Random seed constants
    constexpr uint64_t RANDOM_SEED_DEFAULT = 42;
    constexpr uint64_t RANDOM_SEED_AUTO = 0;
    
    // File I/O constants
    constexpr size_t FILE_READ_BUFFER_SIZE = 4096;
    constexpr size_t FILE_WRITE_BUFFER_SIZE = 4096;
    constexpr std::string CONFIG_FILE_EXTENSION = ".json";
    constexpr std::string RESULTS_FILE_EXTENSION = ".csv";
    
    // Validation constants
    constexpr double VALIDATION_EPSILON = 1e-6;
    constexpr double VALIDATION_PERCENTAGE_MIN = 0.0;
    constexpr double VALIDATION_PERCENTAGE_MAX = 1.0;
    
} // namespace ConfigConstants

} // namespace nlm