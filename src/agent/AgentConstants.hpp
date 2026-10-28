// Agent Brain System Constants
// Configuration parameters for agent behavior and neural dynamics

#pragma once

namespace nlm {

namespace AgentConstants {

// Motor system configuration
const size_t MOTOR_GROUP_COUNT = 6;           // Number of motor action groups (forward, backward, turn left/right, interact, wait)
const size_t MOTOR_RANDOM_CHOICES = 7;       // Number of random motor commands for exploration

// Sensory system configuration
const size_t SENSORY_GROUP_COUNT = 4;        // Number of sensory neuron groups (vision, touch, internal, proprioception)

// Sensor input dimensions
const size_t SENSE_VISUAL_WIDTH = 16;
const size_t SENSE_VISUAL_HEIGHT = 16;
const size_t SENSE_VISUAL_CHANNELS = 1;
const size_t VISION_INPUT_SIZE = SENSE_VISUAL_WIDTH * SENSE_VISUAL_HEIGHT;  // 256

const size_t TOUCH_INPUT_SIZE = 8;           // Touch sensor resolution
const size_t INTERNAL_INPUT_SIZE = 4;       // Internal signal sensors
const size_t PROPRIOCEPTION_INPUT_SIZE = 6;  // Proprioception sensors

// Current scaling factors for sensory input
const float VISION_CURRENT_SCALE = 5.0f;     // Scale factor for vision input
const float TOUCH_CURRENT_SCALE = 8.0f;     // Scale factor for touch input
const float INTERNAL_CURRENT_SCALE = 5.0f;  // Scale factor for internal signals
const float PROPRIOCEPTION_CURRENT_SCALE = 3.0f;  // Scale factor for proprioception

// Neuromodulation configuration
const float NOVELTY_DECAY_RATE = 0.99f;      // Exponential decay rate for novelty detection
const float INITIAL_NOVELTY = 0.0f;         // Initial novelty level

// Curiosity and exploration configuration
const float CURIOUSY_MULTIPLIER = 2.0f;      // Multiplier for novelty in curiosity calculation
const float PREDICTION_ERROR_WEIGHT = 0.5f;  // Weight of prediction error in curiosity
const float MAX_CURIOUSY_LEVEL = 1.0f;       // Maximum possible curiosity level

const float CURIOSITY_THRESHOLD = 0.3f;      // Threshold for curiosity-based exploration
const float ACTIVITY_THRESHOLD = 0.5f;      // Minimum activity required for motor action
const float EXPLORATION_CHANCE = 0.3f;      // Maximum probability of random exploration

// Reward modulation configuration
const float EXPECTED_REWARD_ALPHA = 0.95f;   // Exponential moving average alpha for expected reward
const float EXPECTED_REWARD_BETA = 0.05f;   // Exponential moving average beta for expected reward
const float DOPAMINE_CLAMP_MIN = -1.0f;      // Minimum dopamine level
const float DOPAMINE_CLAMP_MAX = 1.0f;       // Maximum dopamine level

// Reward eligibility and plasticity
const float ELIGIBILITY_THRESHOLD = 0.001f;  // Minimum eligibility trace magnitude
const float ELIGIBILITY_DECAY = 0.1f;        // Eligibility trace decay rate
const float PLASTICITY_BASE = 0.5f;          // Base plasticity factor
const float PLASTICITY_MIN = 0.1f;           // Minimum plasticity factor
const float PLASTICITY_MAX = 2.0f;           // Maximum plasticity factor

// STDP learning parameters
const float STDP_LTP_BASE = 0.01f;           // Base STDP long-term potentiation weight
const float STDP_LTD_BASE = 0.012f;          // Base STDP long-term depression weight

// Development system configuration
const double DEVELOPMENT_STAGE_1_END = 60.0;    // End of Initial stage (~1 minute)
const double DEVELOPMENT_STAGE_2_END = 300.0;   // End of CriticalPeriod stage (~5 minutes)
const double DEVELOPMENT_STAGE_3_END = 900.0;   // End of Maturation stage (~15 minutes)

const float PLASTICITY_INITIAL = 1.0f;         // Plasticity modifier in Initial stage
const float PLASTICITY_CRITICAL = 0.8f;       // Plasticity modifier in CriticalPeriod stage
const float PLASTICITY_MATURE = 0.5f;          // Plasticity modifier in Maturation stage
const float PLASTICITY_ADULT = 0.2f;           // Plasticity modifier in Adult stage

// Structural plasticity rates
const float SYNAPTOGENESIS_BASE_RATE = 0.0001f;    // Base synaptogenesis rate
const float PRUNING_BASE_RATE = 0.00001f;         // Base pruning rate
const float PRUNING_FACTOR = 2.0f;                // Multiplier for pruning rate in later stages

// Neural dynamics parameters (from main.cpp)
const float CURRENT_INJECTION_STRONG = 60.0f;      // Strong current injection for neuron activation
const float CURRENT_INJECTION_WEAK = 30.0f;        // Weak current injection for sensory neurons
const float CURRENT_INJECTION_EXPERIMENT = 50.0f; // Current injection for connectivity test

// Statistical thresholds
const float WEIGHT_CHANGE_SIGNIFICANCE = 0.01f;    // Threshold for significant weight change
const float MEAN_WEIGHT_DIFFERENCE_THRESHOLD = 0.001f; // Threshold for detecting learning

} // namespace AgentConstants

} // namespace nlm
