#pragma once

namespace nlm {

// Neural Learning Machine Constants
// Extracted hardcoded values to improve maintainability and configuration

// Sensory Input Sizes
const size_t DEFAULT_VISION_WIDTH = 16;
const size_t DEFAULT_VISION_HEIGHT = 16;
const size_t DEFAULT_TOUCH_SENSORS = 8;
const size_t DEFAULT_INTERNAL_SIGNALS = 4;
const size_t DEFAULT_PROPRIOCEPTION_CHANNELS = 6;
const size_t DEFAULT_AUDIO_CHANNELS = 0;

// Motor Output Sizes
const size_t DEFAULT_MOTOR_ACTIONS = 6;
const size_t DEFAULT_MOTOR_LOOK_ACTIONS = 2;
const size_t TOTAL_MOTOR_COMMANDS = 8;  // MoveForward, MoveBackward, TurnLeft, TurnRight, LookLeft, LookRight, Interact, Wait

// Scale Factors for Sensory Processing
const float VISION_SCALE_FACTOR = 5.0f;
const float TOUCH_SCALE_FACTOR = 8.0f;
const float INTERNAL_SCALE_FACTOR = 5.0f;
const float PROPRIOCEPTION_SCALE_FACTOR = 3.0f;
const float INTERNAL_CENTER_OFFSET = 1.0f;
const float PROPRIOCEPTION_CENTER_OFFSET = 1.0f;

// Activity Thresholds
const float MOTOR_ACTIVITY_THRESHOLD = 0.5f;

// Neuromodulation Parameters
const float NOVELTY_EXPLORATION_WEIGHT = 2.0f;
const float PREDICTION_ERROR_CURIOUSITY_WEIGHT = 0.5f;
const float NOVELTY_DECAY_RATE = 0.99f;
const float DOPAMINE_CLAMP_MIN = -1.0f;
const float DOPAMINE_CLAMP_MAX = 1.0f;
const float PLASTICITY_BASE_FACTOR = 0.5f;

// Curiosity and Exploration Parameters
const float CURIOSITY_HIGH_THRESHOLD = 0.5f;
const float EXPLORATION_PROBABILITY_MAX = 0.3f;

// Development Parameters
const double DEVELOPMENT_STAGE_INITIAL = 60.0;      // seconds
const double DEVELOPMENT_STAGE_CRITICAL = 300.0;    // seconds
const double DEVELOPMENT_STAGE_MATURATION = 900.0;   // seconds
const double DEVELOPMENT_STAGE_ADULT = 9999.9;      // end

const float PLASTICITY_INITIAL = 1.0f;
const float PLASTICITY_CRITICAL = 0.8f;
const float PLASTICITY_MATURATION = 0.5f;
const float PLASTICITY_ADULT = 0.2f;

const float SYNAPTOGENESIS_BASE_RATE = 0.0001f;
const float PRUNING_BASE_RATE = 0.00001f;

// STDP Parameters
const float STDP_BASE_LTP_WEIGHT = 0.01f;
const float STDP_BASE_LTD_WEIGHT = 0.012f;

// Learning and Plasticity
const float INITIAL_WEIGHT_SUM_SCALE = 1.0f;
const float WEIGHT_CHANGE_THRESHOLD = 0.01f;
const float SYNAPTIC_CHANGE_THRESHOLD = 0.001f;

// Checkpoint and Persistence
const char* DEFAULT_CHECKPOINT_FILENAME = "/tmp/nlm_checkpoint.bin";
const char* TEMP_CHECKPOINT_FILENAME = "/tmp/nlm_checkpoint_test.bin";

// Simulation Parameters
const double DEFAULT_TIMESTEP = 0.001;  // seconds
const size_t DEFAULT_SIMULATION_STEPS = 1000;

// Agent Configuration
const float DEFAULT_AGENT_SENSORY_NOVELTY_DECAY = 0.99f;
const float DEFAULT_AGENT_EXPLORATION_CHANCE = 0.3f;

// Memory Integration
const size_t MEMORY_ACTIVE_TRACES_THRESHOLD = 1;
const size_t MEMORY_EPISODES_REPLAY_COUNT = 3;

} // namespace nlm
