#pragma once

namespace nlm {

// Constants for sensory scaling factors
const float SENSORY_VISION_SCALE = 5.0f;
const float SENSORY_TOUCH_SCALE = 8.0f;
const float SENSORY_INTERNAL_SCALE = 5.0f;
const float SENSORY_PROPRIOCEPTION_SCALE = 3.0f;
const float SENSORY_NOVELTY_DECAY = 0.99f;

// Constants for curiosity calculation
const float CURIOUSITY_NOVELTY_WEIGHT = 2.0f;
const float CURIOUSITY_PREDICTION_ERROR_WEIGHT = 0.5f;
const float MAX_CURIOUSITY_LEVEL = 1.0f;

// Constants for motor command selection
const float MOTOR_ACTIVITY_THRESHOLD = 0.5f;
const float MAX_EXPLORATION_CHANCE = 0.3f;
const float MAX_CURIOUSITY_EXPLORATION = 0.5f;

// Constants for neuromodulation
const float NEUROMODULATION_CLAMP_MIN = -1.0f;
const float NEUROMODULATION_CLAMP_MAX = 1.0f;
const float PLASTICITY_MODIFIER_MIN = 0.1f;
const float PLASTICITY_MODIFIER_MAX = 2.0f;

// Constants for development stages
const double DEVELOPMENT_STAGE_INITIAL = 60.0;   // ~1 minute
const double DEVELOPMENT_STAGE_CRITICAL = 300.0; // ~5 minutes
const double DEVELOPMENT_STAGE_MATURATION = 900.0; // ~15 minutes

// Constants for structural plasticity
const float INITIAL_SYNAPTOGENESIS_RATE = 0.0001f;
const float INITIAL_PRUNING_RATE = 0.00001f;

// Configuration constants
const size_t DEFAULT_VISION_WIDTH = 16;
const size_t DEFAULT_VISION_HEIGHT = 16;
const size_t DEFAULT_MOTOR_COMMANDS = 6;
const size_t DEFAULT_TOUCH_SENSORS = 8;
const size_t DEFAULT_INTERNAL_SENSORS = 4;
const size_t DEFAULT_PROPRIOCEPTORS = 6;

// Motor command count
const size_t MOTOR_COMMAND_COUNT = 8; // Including LookLeft and LookRight

// Constants for signal processing and normalization
const float SIGNAL_CENTER = 2.0f;           // Center for signal normalization (values -1 to 1)
const float SIGNAL_OFFSET = 1.0f;          // Offset for signal normalization
const float SIGNAL_MIN = -1.0f;            // Minimum signal value
const float SIGNAL_MAX = 1.0f;             // Maximum signal value

// Constants for eligibility trace
const float ELIGIBILITY_THRESHOLD = 0.001f; // Minimum eligibility for plasticity
const float ELIGIBILITY_DECAY = 0.1f;       // Decay rate for eligibility traces

// Constants for learning rate modifiers
const float BASE_LEARNING_RATE = 0.5f;     // Base learning rate
const float LEARNING_RATE_MODIFIER = 0.5f;  // Learning rate modifier

// Constants for experimental parameters
const float LEARNING_EXPERIMENT_THRESHOLD = 0.01f; // Threshold for detecting learning
const float SYNAPTIC_CHANGE_THRESHOLD = 0.001f;   // Threshold for significant synaptic changes

// Add more constants as needed...
} // namespace nlm

// Add more constants as needed...
