#pragma once

// NLM Project Constants
// Named constants to replace magic numbers throughout the codebase

namespace nlm {

// Physics and simulation constants
constexpr float SIMULATION_TIMESTEP = 0.001f;        // 1ms timestep
constexpr float MAX_MEMBRANE_POTENTIAL = -20.0f;     // Maximum membrane potential
constexpr float RESTING_MEMBRANE_POTENTIAL = -70.0f; // Resting membrane potential
constexpr float RESET_MEMBRANE_POTENTIAL = -75.0f;  // Reset potential after spike
constexpr float THRESHOLD_MEMBRANE_POTENTIAL = -50.0f; // Spike threshold
constexpr float REFRACTORY_PERIOD = 2.0f;            // Refractory period (ms)
constexpr float MEMBRANE_TIME_CONSTANT = 20.0f;      // Membrane time constant (ms)

// Weight range constants
constexpr float MIN_SYNAPTIC_WEIGHT = -1.0f;        // Minimum synaptic weight
constexpr float MAX_SYNAPTIC_WEIGHT = 1.0f;         // Maximum synaptic weight
constexpr float DEFAULT_SYNAPTIC_WEIGHT = 0.5f;     // Default initial weight

// Plasticity constants
constexpr float DEFAULT_PLATONIC_LEARNING_RATE = 0.01f;
constexpr float STDP_LTP_WEIGHT_DEFAULT = 0.02f;    // STDP LTP weight
constexpr float STDP_LTD_WEIGHT_DEFAULT = 0.015f;   // STDP LTD weight
constexpr float STDP_TIME_CONSTANT = 20.0f;         // STDP time constant (ms)

// Development constants
constexpr float DEVELOPMENT_PROGRESS_INCREMENT = 0.01f;
constexpr float CRITICAL_PERIOD_START = 60.0f;      // Age when critical period starts (simulation steps)
constexpr float CRITICAL_PERIOD_END = 300.0f;       // Age when critical period ends
constexpr float MATURATION_STAGE_START = 300.0f;    // Age when maturation starts
constexpr float ADULT_STAGE_START = 900.0f;         // Age when adult stage starts

// Neuromodulation constants
constexpr float DOPAMINE_MAX_LEVEL = 1.0f;           // Maximum dopamine level
constexpr float DOPAMINE_MIN_LEVEL = -1.0f;          // Minimum dopamine level
constexpr float DEFAULT_NOVELTY_DECAY = 0.99f;      // Novelty decay rate
constexpr float CURVIOUSITY_THRESHOLD = 0.3f;       // Threshold for curiosity-based exploration
constexpr float EXPLORATION_THRESHOLD = 0.5f;       // Threshold for random exploration

// Memory constants
constexpr size_t MAX_WORKING_MEMORY_TRACES = 1000;  // Maximum working memory traces
constexpr size_t MAX_EPISODIC_MEMORY_EPISODES = 1000; // Maximum episodic memory episodes

// Environment constants
constexpr float DEFAULT_WORLD_WIDTH = 20.0f;        // Default world width
constexpr float DEFAULT_WORLD_HEIGHT = 20.0f;       // Default world height
constexpr float DEFAULT_VISION_WIDTH = 16;          // Default vision width
constexpr float DEFAULT_VISION_HEIGHT = 16;         // Default vision height
constexpr float AGENT_MOVEMENT_SPEED = 3.0f;        // Agent movement speed
constexpr float AGENT_TURN_SPEED = 2.0f;            // Agent turn speed
constexpr float INTERACTION_RANGE = 1.0f;           // Range for object interaction
constexpr float RESOURCE_ENERGY_GAIN = 30.0f;       // Energy gained from resources
constexpr float HAZARD_ENERGY_LOSS = -15.0f;        // Energy loss from hazards

// Energy constants
constexpr float MAX_ENERGY = 100.0f;                // Maximum energy
constexpr float ENERGY_DECAY_RATE = 0.01f;          // Energy decay per timestep
constexpr float MIN_ENERGY_THRESHOLD = 0.0f;       // Minimum energy (death threshold)

// Simulation constants
constexpr uint64_t DEFAULT_SIMULATION_STEPS = 10000; // Default maximum simulation steps
constexpr float SIMULATION_TIME_LIMIT = 0.0f;       // No time limit by default

// Visualization constants
constexpr bool VISUALIZATION_DEFAULT_ENABLED = false;
constexpr float VISUALIZATION_UPDATE_RATE = 30.0f;  // Frames per second

// Reward constants
constexpr float MOVEMENT_COST = 0.01f;              // Cost for movement
constexpr float TURNING_COST = 0.005f;              // Cost for turning
constexpr float INTERACTION_REWARD = 1.0f;         // Reward for successful interaction

// Pattern detection constants
constexpr float PATTERN_MATCHING_THRESHOLD = 0.7f;  // Threshold for pattern matching
constexpr size_t PATTERN_MATCHING_WINDOW = 10;     // Window size for pattern matching

// Attention constants
constexpr size_t MAX_ATTENTION_FOCUS = 5;          // Maximum attention focus items
constexpr float ATTENTION_DECAY_RATE = 0.9f;       // Attention decay rate

} // namespace nlm
