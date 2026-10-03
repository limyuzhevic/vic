#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <limits>

namespace nlm {

// Scaling factors (from AgentBrain.cpp)
constexpr float SCALING_FACTOR_VISION = 5.0f;
constexpr float SCALING_FACTOR_TOUCH = 8.0f;
constexpr float SCALING_FACTOR_INTERNAL = 5.0f;
constexpr float SCALING_FACTOR_PROPRIOCEPTION = 3.0f;

// Thresholds (from AgentBrain.cpp)
constexpr float MAX_NOVELTY_FACTOR = 0.99f;
constexpr float DEFAULT_EXPLORATION_CHANCE = 0.3f;
constexpr float MIN_ACTIVITY_THRESHOLD = 0.5f;
constexpr float ELIGIBILITY_THRESHOLD = 0.001f;
constexpr float MIN_VALID_VALUE = std::numeric_limits<float>::lowest() * 10.0f;
constexpr float MAX_VALID_VALUE = std::numeric_limits<float>::max() * 0.1f;

// Development stages (from AgentBrain.cpp)
constexpr float MAX_DEVELOPMENT_AGE_1 = 60.0;
constexpr float MAX_DEVELOPMENT_AGE_2 = 300.0;
constexpr float MAX_DEVELOPMENT_AGE_3 = 900.0;

// Plasticity modifiers (from AgentBrain.cpp)
constexpr float MAX_PLATIVITY_MODIFIER = 2.0f;
constexpr float MIN_PLATIVITY_MODIFIER = 0.1f;
constexpr float PLASTICITY_BASE_FACTOR = 0.5f;
constexpr float PLASTICITY_MODIFIER_INITIAL = 1.0f;
constexpr float PLASTICITY_MODIFIER_CRITICAL = 0.8f;
constexpr float PLASTICITY_MODIFIER_MATURE = 0.5f;
constexpr float PLASTICITY_MODIFIER_ADULT = 0.2f;

// Neuromodulation parameters (from AgentBrain.cpp)
constexpr float NEUROMODULATION_RANGE = 0.05f;
constexpr float STANAP_LTP_FACTOR = 0.01f;
constexpr float STANAP_LTD_FACTOR = 0.012f;
constexpr float SYNAPTOGENESIS_RATE_BASE = 0.0001f;
constexpr float PRUNING_RATE_BASE = 0.00001f;

// STDP parameters (from STDP.cpp)
constexpr float STDP_LTP_WEIGHT = 0.02f;
constexpr float STDP_LTD_WEIGHT = 0.015f;
constexpr float STDP_TIME_CONSTANT = 20.0f;
constexpr float STDP_MIN_WEIGHT = -1.0f;
constexpr float STDP_MAX_WEIGHT = 1.0f;

// Structural plasticity (from main.cpp)
constexpr float SYNAPTOGENESIS_RATE = 0.0001f;
constexpr float PRUNING_RATE = 0.00001f;

// Size limits for sensory systems (from SensoryPercept.cpp)
constexpr size_t MAX_VISION_WIDTH = 256;
constexpr size_t MAX_VISION_HEIGHT = 256;
constexpr size_t MAX_VISION_SIZE = MAX_VISION_WIDTH * MAX_VISION_HEIGHT;
constexpr size_t MAX_TOUCH_SENSORS = 64;
constexpr size_t MAX_INTERNAL_SENSORS = 32;
constexpr size_t MAX_PROPRIOCEPTION_SENSORS = 20;
constexpr size_t MAX_AUDIO_SENSORS = 128;

// Motor action counts (from AgentBrain.cpp)
constexpr size_t MOTOR_ACTION_COUNT_FORWARD = 6;
constexpr size_t MOTOR_ACTION_COUNT_RANDOM = 7;
constexpr size_t VISION_DIMENSIONS = 256;
constexpr size_t TOUCH_SENSORS = 8;
constexpr size_t INTERNAL_SENSORS = 4;
constexpr size_t PROPRIOCEPTION_SENSORS = 6;
constexpr size_t MOTOR_OUTPUT_SIZE = 6;

// Other magic numbers (from various files)
constexpr float LEARNING_THRESHOLD = 0.01f;
constexpr float TOTAL_DELTA_THRESHOLD = 0.001f;
constexpr float WEIGHT_CHANGE_THRESHOLD = 0.001f;
constexpr float ACTIVITY_THRESHOLD = 0.5f;
constexpr float CURIOUS_ACTIVITY_MULTIPLIER = 0.5f;
constexpr float EXPLORATION_CHANCE_MULTIPLIER = 0.3f;
constexpr float NEUROMODULATION_CLAMP_MIN = -1.0f;
constexpr float NEUROMODULATION_CLAMP_MAX = 1.0f;
constexpr float PLASTICITY_FACTOR_BASE = 0.5f;
constexpr float PLASTICITY_FACTOR_OFFSET = 0.5f;
constexpr float SYNAPTOGENESIS_RATE_MULTIPLIER = 1.0f;
constexpr float PRUNING_RATE_MULTIPLIER = 1.0f;
constexpr size_t MAX_SPIKES_FOR_TEST = 10;
constexpr size_t MIN_SPIKES_FOR_TEST = 5;
constexpr size_t MAX_RANDOM_ACTION_CHOICE = 6;
constexpr size_t MAX_RANDOM_ACTION_TRIAL = 50;
constexpr size_t MAX_RANDOM_ACTION_PRE_NEUTRONS = 2;
constexpr size_t MAX_RANDOM_ACTION_POST_NEUTRONS = 2;
constexpr size_t MAX_ACTION_HISTORY_SIZE = 20;
constexpr size_t MAX_INJECTED_CURRENT = 60;
constexpr size_t MAX_TESTED_SYNAPSES = 5;
constexpr float SPIN_EXCITATION_CURRENT = 50.0f;
constexpr float STDP_EXCITATION_CURRENT = 60.0f;
constexpr float MIN_WEIGHT_DELTA_FOR_ELIGIBILITY = 1e-6f;
constexpr float MAX_TOTAL_DELTA_THRESHOLD = 0.001f;
constexpr float MIN_TOTAL_DELTA_THRESHOLD = -0.001f;
constexpr float SYNAPSE_EFFICIENCY_MULTIPLIER = 1.0f;
constexpr float WEIGHT_CHANGE_FOR_LEARNING = 0.001f;

} // namespace nlm

#endif // CONSTANTS_HPP