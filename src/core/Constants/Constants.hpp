// NLM Configuration Constants
// Centralized magic numbers and configuration parameters for better maintainability

#pragma once

namespace nlm {
namespace ConfigConstants {

// General simulation constants
constexpr double DEFAULT_SIMULATION_TIMESTEP = 0.001;
constexpr double MIN_SIMULATION_TIMESTEP = 0.0001;
constexpr double MAX_SIMULATION_TIMESTEP = 1.0;

// Neural network constants
constexpr int DEFAULT_NEURON_COUNT = 500;
constexpr int MIN_NEURON_COUNT = 10;
constexpr int MAX_NEURON_COUNT = 10000;
constexpr int DEFAULT_REGION_COUNT = 1;
constexpr int MAX_REGION_COUNT = 100;

// Connectivity constants
constexpr float DEFAULT_CONNECTION_PROBABILITY = 0.15f;
constexpr float MIN_CONNECTION_PROBABILITY = 0.001f;
constexpr float MAX_CONNECTION_PROBABILITY = 1.0f;

// Plasticity constants
constexpr float DEFAULT_STDP_LTP_WEIGHT = 0.02f;
constexpr float DEFAULT_STDP_LTD_WEIGHT = 0.015f;
constexpr float DEFAULT_STDP_TAU = 20.0f;
constexpr float MIN_STDP_WEIGHT = 0.0f;
constexpr float MAX_STDP_WEIGHT = 1.0f;

constexpr float DEFAULT_SYNAPTOGENESIS_RATE = 0.0001f;
constexpr float DEFAULT_PRUNING_RATE = 0.00001f;
constexpr float MIN_PLATONICITY_RATE = 0.000001f;
constexpr float MAX_PLATONICITY_RATE = 0.001f;

// Neuromodulation constants
constexpr float DEFAULT_DOPAMINE_LEARNING_RATE = 0.01f;
constexpr float DEFAULT_NOVELTY_SENSITIVITY = 1.0f;
constexpr float DEFAULT_CURIOUSY_SENSITIVITY = 0.5f;
constexpr float MIN_MODULATION_LEVEL = -1.0f;
constexpr float MAX_MODULATION_LEVEL = 1.0f;

// Sensory constants
constexpr int DEFAULT_VISION_WIDTH = 16;
constexpr int DEFAULT_VISION_HEIGHT = 16;
constexpr int MIN_VISION_DIMENSION = 4;
constexpr int MAX_VISION_DIMENSION = 64;

constexpr int DEFAULT_TOUCH_SENSORS = 8;
constexpr int DEFAULT_INTERNAL_SIGNALS = 4;
constexpr int DEFAULT_PROPRIOCEPTION_SENSORS = 6;

// Motor constants
constexpr int DEFAULT_MOTOR_ACTIONS = 6;
constexpr int MIN_MOTOR_ACTIONS = 3;
constexpr int MAX_MOTOR_ACTIONS = 20;

// Development constants
constexpr double INITIAL_DEVELOPMENT_AGE = 0.0;
constexpr double CRITICAL_PERIOD_AGE = 60.0; // ~1 minute
constexpr double MATURATION_STAGE_AGE = 300.0; // ~5 minutes
constexpr double ADULT_STAGE_AGE = 900.0; // ~15 minutes
constexpr double AGING_STAGE_AGE = 3600.0; // ~1 hour

constexpr float INITIAL_PLASTICITY_MODIFIER = 1.0f;
constexpr float MATURED_PLASTICITY_MODIFIER = 0.2f;
constexpr float MIN_PLASTICITY_MODIFIER = 0.1f;
constexpr float MAX_PLASTICITY_MODIFIER = 2.0f;

// World constants
constexpr float DEFAULT_WORLD_WIDTH = 20.0f;
constexpr float DEFAULT_WORLD_HEIGHT = 20.0f;
constexpr float MIN_WORLD_SIZE = 5.0f;
constexpr float MAX_WORLD_SIZE = 100.0f;

constexpr float DEFAULT_MAX_ENERGY = 100.0f;
constexpr float DEFAULT_ENERGY_DECAY_RATE = 0.01f;
constexpr float MIN_ENERGY = 0.0f;
constexpr float MAX_ENERGY = 1000.0f;

constexpr float DEFAULT_RESOURCE_ENERGY_GAIN = 30.0f;
constexpr float DEFAULT_HAZARD_DAMAGE = 15.0f;

// Vision constants
constexpr int DEFAULT_VISION_RAY_COUNT = 16;
constexpr float DEFAULT_FOV_ANGLE = M_PI / 2.0f; // 90 degrees
constexpr float DEFAULT_VISION_MAX_RANGE = 8.0f;
constexpr float MIN_VISION_RANGE = 1.0f;
constexpr float MAX_VISION_RANGE = 50.0f;

constexpr float DEFAULT_TOUCH_RANGE = 1.5f;
constexpr int DEFAULT_TOUCH_DIRECTIONS = 8;

// Behavioral constants
constexpr float DEFAULT_MOVE_SPEED = 3.0f;
constexpr float DEFAULT_TURN_SPEED = 2.0f;
constexpr float DEFAULT_MOVE_ACCELERATION = 5.0f;
constexpr float DEFAULT_DECELERATION_RATE = 0.9f;

constexpr float DEFAULT_ACTION_COOLDOWN = 0.1f; // seconds
constexpr float MIN_ACTION_COOLDOWN = 0.0f;
constexpr float MAX_ACTION_COOLDOWN = 10.0f;

// Experiment constants
constexpr int DEFAULT_NUM_TRIALS = 50;
constexpr int DEFAULT_NUM_EPISODES = 5;
constexpr int DEFAULT_STEPS_PER_EPISODE = 200;
constexpr int MAX_TRIALS = 10000;
constexpr int MAX_EPISODES = 1000;

constexpr float DEFAULT_REWARD_RANGE = 100.0f;
constexpr float DEFAULT_PUNISHMENT_RANGE = -100.0f;
constexpr float MIN_REWARD = -1000.0f;
constexpr float MAX_REWARD = 1000.0f;

// Logging constants
constexpr int DEFAULT_LOG_BUFFER_SIZE = 1024;
constexpr int MAX_LOG_MESSAGE_LENGTH = 4096;

constexpr LogLevel DEFAULT_LOG_LEVEL = LogLevel::Info;
constexpr LogLevel MIN_LOG_LEVEL = LogLevel::Debug;
constexpr LogLevel MAX_LOG_LEVEL = LogLevel::Critical;

// Performance constants
constexpr size_t DEFAULT_PERFORMANCE_SAMPLE_INTERVAL = 100;
constexpr double MIN_PERFORMANCE_SAMPLE_INTERVAL = 1.0;
constexpr double MAX_PERFORMANCE_SAMPLE_INTERVAL = 10000.0;

constexpr float MIN_PERFORMANCE_METRIC = 0.0f;
constexpr float MAX_PERFORMANCE_METRIC = 10000.0f;

// File and I/O constants
constexpr const char* DEFAULT_CONFIG_FILE = "configs/default.cfg";
constexpr const char* DEFAULT_EXPERIMENT_LOG_FILE = "experiments.log";
constexpr const char* DEFAULT_RESULTS_FILE = "results.csv";
constexpr const char* DEFAULT_CHECKPOINT_FILE = "checkpoint.nlm";

constexpr size_t DEFAULT_MAX_FILE_PATH_LENGTH = 512;
constexpr size_t MAX_CONFIG_FILE_SIZE = 1024 * 1024; // 1MB
constexpr size_t MAX_LOG_FILE_SIZE = 100 * 1024 * 1024; // 100MB

// Random number generation constants
constexpr uint64_t DEFAULT_RANDOM_SEED = 42;
constexpr uint64_t MIN_RANDOM_SEED = 0;
constexpr uint64_t MAX_RANDOM_SEED = UINT64_MAX;

// Safety and validation constants
constexpr float MIN_WEIGHT_VALUE = 0.0f;
constexpr float MAX_WEIGHT_VALUE = 10.0f;
constexpr float MIN_MEMBRANE_POTENTIAL = -100.0f;
constexpr float MAX_MEMBRANE_POTENTIAL = 100.0f;
constexpr float MIN_THRESHOLD = -50.0f;
constexpr float MAX_THRESHOLD = 50.0f;

// Time constants
constexpr double MIN_WALL_CLOCK_TIME = 0.0;
constexpr double MAX_WALL_CLOCK_TIME = 24.0 * 3600.0 * 365.0; // 1 year

// Scaling constants
constexpr float DEFAULT_SPEED_FACTOR = 1.0f;
constexpr float MIN_SPEED_FACTOR = 0.1f;
constexpr float MAX_SPEED_FACTOR = 10.0f;

} // namespace ConfigConstants
} // namespace nlm