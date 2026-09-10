#pragma once

namespace nlm {

// AgentBrain configuration constants
// Scale factors for sensory input transduction
const float DEFAULT_VISION_SCALE = 5.0f;    // Scale factor for vision input
const float DEFAULT_TOUCH_SCALE = 8.0f;     // Scale factor for touch input
const float DEFAULT_INTERNAL_SCALE = 5.0f;  // Scale factor for internal signals
const float DEFAULT_PROPRIOCEPTION_SCALE = 3.0f; // Scale factor for proprioception

// Novelty and curiosity computation
const float DEFAULT_NOVELTY_DECAY = 0.99f;  // Decay factor for novelty
const float DEFAULT_NOVELTY_MULTIPLIER = 2.0f; // Multiplier for novelty->curiosity
const float DEFAULT_PREDICTION_ERROR_WEIGHT = 0.5f; // Weight for prediction error

// Exploration and curiosity
const float DEFAULT_EXPLORATION_CHANCE_MAX = 0.3f; // Maximum exploration chance
const float DEFAULT_CURIOUSITY_THRESHOLD = 0.3f;    // Threshold for curiosity exploration
const float DEFAULT_EXPLORATION_THRESHOLD = 0.5f;   // Threshold for curiosity-based exploration

// Motor command decoding
const float MOTOR_COMMAND_THRESHOLD = 0.5f;      // Threshold for meaningful motor activity

// Development
const float DEVELOPMENT_AGE_INITIAL = 0.0;      // Initial developmental age
const float DEVELOPMENT_INITIAL_PLAT = 1.0f;     // Initial plasticity modifier

// Neuromodulation
const float NEUROMODULATION_WEIGHT_SUM = 0.95f; // Weight for exponential moving average
const float NEUROMODULATION_NEW_REWARD_WEIGHT = 0.05f; // Weight for new reward

// Plasticity
const float STDP_LTP_WEIGHT_INITIAL = 0.01f;     // Initial LTP weight
const float STDP_LTD_WEIGHT_INITIAL = 0.012f;    // Initial LTD weight
const float STDP_PLASTICITY_FACTOR_BASE = 0.5f;   // Base plasticity factor
const float STDP_PLASTICITY_MODIFIER_MIN = 0.1f; // Minimum plasticity modifier
const float STDP_PLASTICITY_MODIFIER_MAX = 2.0f; // Maximum plasticity modifier

// Structural plasticity
const float SYNAPTOGENESIS_RATE_INITIAL = 0.0001f; // Initial synaptogenesis rate
const float PRUNING_RATE_INITIAL = 0.00001f;      // Initial pruning rate

} // namespace nlm
