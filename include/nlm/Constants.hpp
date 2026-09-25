#pragma once

#include <cstddef>
#include <cstdint>

namespace nlm {

// Neural Learning Machine Constants
// This header contains commonly used constants throughout the NLM project
// to eliminate magic numbers and improve code maintainability.

namespace Constants {
    // ======= TIMING AND SIMULATION =======
    
    // Base simulation timestep (1ms)
    constexpr double SIMULATION_TIMESTEP = 0.001;
    
    // Development stages timing (in simulation steps)
    constexpr double INITIAL_STAGE_DURATION = 60.0;      // ~1 minute
    constexpr double CRITICAL_PERIOD_DURATION = 240.0;   // ~4 minutes  
    constexpr double MATURATION_DURATION = 600.0;        // ~10 minutes
    
    // Memory system intervals
    constexpr size_t REPLAY_INTERVAL = 100;              // Replay every 100 steps
    constexpr size_t CONSOLIDATION_INTERVAL = 1000;      // Consolidate every 1000 steps
    constexpr size_t EPISODE_STORAGE_INTERVAL = 10;      // Store episode every 10 steps
    
    // Checkpoint system
    constexpr size_t DEFAULT_CHECKPOINT_INTERVAL = 10000;
    constexpr float CHECKPOINT_COMPRESSION_LEVEL = 0.5f; // Medium compression
    constexpr size_t MAX_CHECKPOINTS = 5;
    
    // ======= BRAIN TOPOLOGY =======
    
    // Default brain configuration
    constexpr size_t DEFAULT_NEURON_COUNT = 1000;
    constexpr size_t DEFAULT_REGION_COUNT = 1;
    constexpr float DEFAULT_CONNECTION_PROBABILITY = 0.1f;
    
    // Neuron and synapse counts
    constexpr size_t SENSORY_NEURON_GROUPS = 4;
    constexpr size_t MOTOR_NEIGHBOR_GROUPS = 6;
    constexpr size_t MOTOR_ACTIONS = 6;
    constexpr size_t VISION_PIXELS = 256;               // 16x16 vision grid
    constexpr size_t TOUCH_SENSORS = 8;
    constexpr size_t INTERNAL_SENSORS = 4;
    constexpr size_t PROPRIOCEPTION_SENSORS = 6;
    
    // ======= NEURAL DYNAMICS =======
    
    // Leaky Integrate-and-Fire (LIF) neuron parameters
    constexpr float LIF_RESTORING_MEMBRANE_POTENTIAL = -65.0f;  // mV
    constexpr float LIF_THRESHOLD_POTENTIAL = -50.0f;          // mV
    constexpr float LIF_RESET_POTENTIAL = -65.0f;             // mV
    constexpr float LIF_LEAK_CONDUCTANCE = 0.04f;              // ms^-1
    constexpr float LIF_REFACTORY_PERIOD = 2.0f;              // ms
    constexpr float LIF_SPIKE_AMPLITUDE = 20.0f;              // mV
    
    // ===== SYNAPTIC PROPERTIES =====
    
    // Synaptic transmission
    constexpr float SYNAPTIC_DELAY_BASE = 1.0f;             // ms
    constexpr float SYNAPTIC_DELAY_VARIATION = 2.0f;       // ms
    constexpr float SYNAPTIC_WEIGHT_MIN = -1.0f;
    constexpr float SYNAPTIC_WEIGHT_MAX = 1.0f;
    constexpr float SYNAPTIC_WEIGHT_INITIAL = 0.1f;
    
    // ===== PLASTICITY RULES =====
    
    // STDP parameters
    constexpr float STDP_LTP_WEIGHT = 0.01f;
    constexpr float STDP_LTD_WEIGHT = 0.012f;
    constexpr float STDP_TIME_CONSTANTS = 20.0f;           // ms
    constexpr float STDP_NORMALIZATION = 1.0f / 25.0f;     // Normalizes STDP contributions
    
    // Hebbian learning
    constexpr float HEBBIAN_LEARNING_RATE = 0.01f;
    
    // Structural plasticity rates
    constexpr float SYNAPTOGENESIS_RATE = 0.0001f;
    constexpr float PRUNING_RATE = 0.00001f;
    
    // ===== NEUROMODULATION =====
    
    // Dopamine effects
    constexpr float DOPAMINE_EXCITABILITY_FACTOR = 0.5f;
    constexpr float DOPAMINE_PLASTICITY_FACTOR = 0.5f;
    constexpr float DOPAMINE_CLAMP_MIN = -1.0f;
    constexpr float DOPAMINE_CLAMP_MAX = 1.0f;
    
    // Curiosity system
    constexpr float CURIOSITY_EXPLORATION_THRESHOLD = 0.3f;
    constexpr float CURIOSITY_EXPLORATION_MAX_LEVEL = 0.5f;
    constexpr float CURIOSITY_NOISE_THRESHOLD = 0.5f;
    constexpr float CURIOSITY_NOVELTY_WEIGHT = 2.0f;
    constexpr float CURIOSITY_PREDICTION_ERROR_WEIGHT = 0.5f;
    
    // Novelty detection
    constexpr float NOVELTY_DECAY_RATE = 0.99f;
    constexpr float NOVELTY_SENSITIVITY = 1.0f;
    
    // ===== SENSORY PROCESSING =====
    
    // Sensory input scaling
    constexpr float VISION_SCALE_FACTOR = 5.0f;
    constexpr float TOUCH_SCALE_FACTOR = 8.0f;
    constexpr float INTERNAL_SCALE_FACTOR = 5.0f;
    constexpr float PROPRIOCEPTION_SCALE_FACTOR = 3.0f;
    
    // Vision processing
    constexpr float VISION_THRESHOLD = 0.5f;
    constexpr float VISION_NOISE_LEVEL = 0.01f;
    
    // ===== MOTOR OUTPUT =====
    
    // Movement parameters
    constexpr float MOVEMENT_COST = 0.01f;
    constexpr float TURN_COST = 0.005f;
    constexpr float WALL_COLLISION_COST = 0.05f;
    constexpr float MAX_MOVEMENT_SPEED = 3.0f;
    constexpr float MAX_TURN_SPEED = 2.0f;
    
    // Action costs and rewards
    constexpr float ACTION_SUCCESS_REWARD = 1.0f;
    constexpr float ACTION_FAILURE_COST = -1.0f;
    constexpr float NOISE_THRESHOLD = 0.5f;
    
    // ===== WORLD INTERACTION =====
    
    // Simple world parameters
    constexpr float WORLD_WIDTH = 20.0f;
    constexpr float WORLD_HEIGHT = 20.0f;
    constexpr float WORLD_MAX_ENERGY = 100.0f;
    constexpr float WORLD_ENERGY_DECAY_RATE = 0.01f;
    constexpr float WORLD_RESOURCE_ENERGY_GAIN = 30.0f;
    constexpr float WORLD_HAZARD_DAMAGE = 0.2f;
    
    // Vision and perception
    constexpr float VISION_FOV = 1.57079632679f;         // 90 degrees in radians
    constexpr float VISION_MAX_RANGE = 8.0f;
    constexpr size_t VISION_RAY_COUNT = 16;
    constexpr float VISION_SENSOR_PICKUP_RANGE = 1.5f;
    
    // ===== COGNITION =====
    
    // Attention system
    constexpr float ATTENTION_INHIBITION_STRENGTH = 0.5f;
    constexpr float ATTENTION_EXCITATION_STRENGTH = 1.5f;
    constexpr float ATTENTION_CONCENTRATION_THRESHOLD = 0.7f;
    
    // Working memory
    constexpr size_t WORKING_MEMORY_CAPACITY_FRACTION = 0.1f;  // 10% of neurons
    constexpr float WORKING_MEMORY_DECAY_RATE = 0.9f;
    
    // Concept formation
    constexpr float CONCEPT_FORMATION_THRESHOLD = 0.8f;
    constexpr float CONCEPT_SIMILARITY_THRESHOLD = 0.6f;
    
    // ===== MEMORY SYSTEMS =====
    
    // Episodic memory
    constexpr size_t MAX_EPISODES = 1000;
    constexpr float EPISODE_REPLAY_COUNT = 3.0f;
    constexpr float CONSOLIDATION_THRESHOLD = 0.3f;
    constexpr float EPISODE_REWARD_IMPORTANCE = 1.0f;
    
    // Working memory
    constexpr float WORKING_MEMORY_STABILITY = 0.8f;
    constexpr float WORKING_MEMORY_TRANSIENCE = 0.2f;
    
    // ===== DEVELOPMENT =====
    
    // Developmental stages
    constexpr float DEVELOPMENT_PLASTICITY_INITIAL = 1.0f;
    constexpr float DEVELOPMENT_PLASTICITY_CRITICAL = 0.8f;
    constexpr float DEVELOPMENT_PLASTICITY_MATURE = 0.5f;
    constexpr float DEVELOPMENT_PLASTICITY_ADULT = 0.2f;
    
    // ===== MISCELLANEOUS =====
    
    // Random number generation
    constexpr uint64_t DEFAULT_RANDOM_SEED = 42;
    constexpr uint64_t LCG_MULTIPLIER = 6364136223846793005ULL;
    constexpr uint64_t LCG_INCREMENT = 1442695040888963407ULL;
    
    // Logging and debugging
    constexpr const char* DEFAULT_LOG_LEVEL = "INFO";
    constexpr size_t MAX_LOG_MESSAGE_LENGTH = 1024;
    
    // File and directory names
    constexpr const char* CHECKPOINT_DIRECTORY = "./checkpoints";
    constexpr const char* DEFAULT_CONFIG_FILE = "config.yaml";
    
    // Version and build
    constexpr const char* PROJECT_VERSION = "0.1.0";
    constexpr const char* PROJECT_NAME = "NLM - Neural Learning Machine";
} // namespace Constants

// ===== ALIASES FOR COMMON TYPES =====

using TimeStep = double;
using SimulationStep = size_t;
using Timestamp = double;
using NeuronId = size_t;
using RegionId = size_t;
using PopulationId = size_t;
using SynapseId = size_t;
using MembranePotential = float;
using SynapticWeight = float;

// ===== ENUMS =====

enum class NeuronType {
    Sensory,
    Internal,
    Motor
};

enum class DevelopmentalStage {
    Initial,
    CriticalPeriod,
    Maturation,
    Adult
};

enum class MotorCommand {
    MoveForward,
    MoveBackward,
    TurnLeft,
    TurnRight,
    LookLeft,
    LookRight,
    Interact,
    Wait
};

enum class ActionType {
    MoveForward,
    MoveBackward,
    TurnLeft,
    TurnRight,
    LookLeft,
    LookRight,
    Interact,
    Wait
};

enum class WorldObjectType {
    Resource,
    Hazard,
    Empty
};

// ===== HELPER FUNCTIONS =====

// Math constants and helper functions
namespace Math {
    constexpr float PI = 3.14159265358979323846f;
    constexpr float HALF_PI = PI * 0.5f;
    constexpr float TWO_PI = PI * 2.0f;
    
    inline float clamp(float value, float min, float max) {
        return value < min ? min : (value > max ? max : value);
    }
    
    inline float normalize(float value, float min, float max) {
        if (max == min) return 0.5f;
        return (value - min) / (max - min);
    }
    
    inline float lerp(float a, float b, float t) {
        return a + (b - a) * t;
    }
    
    inline float distance(float x1, float y1, float x2, float y2) {
        float dx = x2 - x1;
        float dy = y2 - y1;
        return std::sqrt(dx * dx + dy * dy);
    }
} // namespace Math

} // namespace nlm