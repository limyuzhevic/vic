// Constants for NLM (Neural Learning Machine) Phase 2 Tests
// Centralized configuration for magic numbers and thresholds

#pragma once

namespace nlm {
namespace constants {

// Weight change thresholds
constexpr float WEIGHT_STRENGTH_THRESHOLD = 0.01f;
constexpr float WEIGHT_WEAKNESS_THRESHOLD = -0.01f;
constexpr float WEIGHT_CHANGE_THRESHOLD = 0.001f;

// Synapse recording limits
constexpr size_t MAX_SYNAPSES_TO_RECORD = 1000;

// Neuron injection parameters
constexpr float NEURON_INJECTION_CURRENT = 50.0f;  // Strong excitatory input
constexpr float MODERATE_INJECTION_CURRENT = 30.0f;  // Moderate input
constexpr float WEAK_INJECTION_CURRENT = 5.0f;  // Weak input

// Neuron parameter ranges for debugging
constexpr size_t DEBUG_NEIGHBOR_COUNT = 3;

// Activity thresholds
constexpr float ACTIVITY_THRESHOLD = 0.5f;  // Minimum activity to consider for action

// Memory input processing
constexpr float VISION_INPUT_SCALE = 5.0f;
constexpr float TOUCH_INPUT_SCALE = 8.0f;
constexpr float INTERNAL_INPUT_SCALE = 5.0f;
constexpr float PROPRIOCEPTION_INPUT_SCALE = 3.0f;

// Novelty detection
constexpr float NOVELTY_DECAY_FACTOR = 0.99f;
constexpr float CURIOSITY_NOVELTY_MULTIPLIER = 2.0f;
constexpr float CURIOSITY_PREDICTION_ERROR_MULTIPLIER = 0.5f;

// Neuromodulation
constexpr float MAX_DOPAMINE_LEVEL = 1.0f;
constexpr float MIN_DOPAMINE_LEVEL = -1.0f;
constexpr float PLasticity_MODIFIER_BASE = 0.5f;
constexpr float PLasticity_MODIFIER_RANGE = 0.5f;

// Development stages
constexpr double INITIAL_DEVELOPMENT_AGE = 60.0;      // Initial stage ends at ~1 min
constexpr double CRITICAL_PERIOD_END = 300.0;        // Critical period ends at ~5 min
constexpr double MATURATION_END = 900.0;             // Maturation ends at ~15 min

// Synaptic plasticity rates
constexpr float INITIAL_SYNAPTOGENESIS_RATE = 0.0001f;
constexpr float INITIAL_PRUNING_RATE = 0.00001f;

// STP parameters for experiment
constexpr float STP_LTP_WEIGHT_BASE = 0.01f;
constexpr float STP_LTD_WEIGHT_BASE = 0.012f;
constexpr float STP_TAU_BASE = 20.0f;

// Action selection with curiosity
constexpr float HIGH_CURIOUSITY_THRESHOLD = 0.5f;
constexpr float MODERATE_EXPLORATION_CHANCE = 0.3f;

// Reward prediction
constexpr float REWARD_PREDICTION_DECAY = 0.95f;
constexpr float REWARD_PREDICTION_UPDATE = 0.05f;

// Motor neuron activity calculation
constexpr float MEMBRANE_POTENTIAL_ACTIVITY_SCALE = 1.0f;

// Motor command enumeration
constexpr int NUM_MOTOR_COMMANDS = 8;  // Wait is index 7
constexpr int NUM_DIRECTIONAL_COMMANDS = 4;  // Forward, Backward, Left, Right
constexpr int NUM_LOOK_COMMANDS = 2;  // LookLeft, LookRight

// Curiosity exploration range
constexpr int CURIOSITY_EXPLORATION_CHOICES = 8;  // 0-7 inclusive

// Main executable defaults
constexpr uint64_t DEFAULT_RANDOM_SEED = 42;
constexpr double DEFAULT_TIMESTEP = 0.001;
constexpr int64_t DEFAULT_NEURON_COUNT = 500;
constexpr int64_t DEFAULT_REGION_COUNT = 1;
constexpr float DEFAULT_CONNECTION_PROBABILITY = 0.15f;

} // namespace constants
} // namespace nlm