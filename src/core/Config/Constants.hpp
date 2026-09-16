#ifndef NLM_CONSTANTS_H
#define NLM_CONSTANTS_H

namespace nlm {

// Neural Network Constants
constexpr float DEFAULT_SIMULATION_TIMESTEP = 0.001f;
constexpr double DEFAULT_DEVELOPMENT_TIMESTEP = 0.1;
constexpr int DEFAULT_RANDOM_SEED = 42;
constexpr int DEFAULT_NEURON_COUNT = 500;
constexpr int DEFAULT_REGION_COUNT = 1;
constexpr float DEFAULT_CONNECTION_PROBABILITY = 0.15f;

// Neuron Dynamics Constants
constexpr float LIF_THRESHOLD_POTENTIAL = -50.0f;
constexpr float LIF_RESTING_POTENTIAL = -70.0f;
constexpr float LIF_RESET_POTENTIAL = -75.0f;
constexpr float LIF_MEMBRANE_TIME_CONSTANT = 20.0f;
constexpr float LIF_REFECTORY_PERIOD = 2.0f;

// Synapse Constants
constexpr float SYNAPSE_INITIAL_WEIGHT_MEAN = 0.5f;
constexpr float SYNAPSE_INITIAL_WEIGHT_STD = 0.1f;

// Plasticity Constants
constexpr float STDP_LTP_WEIGHT = 0.02f;
constexpr float STDP_LTD_WEIGHT = 0.015f;
constexpr float STDP_TIME_CONSTANT = 20.0f;
constexpr float HEBBIAN_LEARNING_RATE = 0.001f;
constexpr float STRUCTURAL_PLASTICITY_SYNAPTOGENESIS_RATE = 0.0001f;
constexpr float STRUCTURAL_PLASTICITY_PRUNING_RATE = 0.00001f;

// Neuromodulation Constants
constexpr float NEUROMOD_DOPAMINE_SCALE = 1.0f;
constexpr float NEUROMOD_NOVELTY_DECAY = 0.99f;
constexpr float NEUROMOD_PREDICTION_ERROR_THRESHOLD = 0.01f;

// Curiosity Constants
constexpr float CURIOSITY_NOVELTY_MULTIPLIER = 2.0f;
constexpr float CURIOSITY_PREDICTION_ERROR_MULTIPLIER = 0.5f;
constexpr float CURIOSITY_THRESHOLD = 0.3f;
constexpr float CURIOSITY_EXPLORATION_THRESHOLD = 0.5f;
constexpr float CURIOSITY_MAX_EXPLORATION_CHANCE = 0.3f;

// Motor System Constants
constexpr int MOTOR_COMMAND_COUNT = 6;  // MoveForward, MoveBackward, TurnLeft, TurnRight, Interact, Wait
constexpr float MOTOR_ACTIVITY_THRESHOLD = 0.5f;

// Sensory Processing Constants
constexpr float VISION_SCALE_FACTOR = 5.0f;
constexpr float TOUCH_SCALE_FACTOR = 8.0f;
constexpr float INTERNAL_SCALE_FACTOR = 5.0f;
constexpr float PROPRIOCEPTION_SCALE_FACTOR = 3.0f;

// Vision Constants
constexpr size_t VISION_WIDTH = 16;
constexpr size_t VISION_HEIGHT = 16;
constexpr size_t VISION_CHANNELS = 3;
constexpr size_t VISION_INPUT_SIZE = VISION_WIDTH * VISION_HEIGHT;

// Touch Constants
constexpr size_t TOUCH_INPUT_SIZE = 8;

// Internal Signals Constants
constexpr size_t INTERNAL_SIGNALS_SIZE = 4;

// Proprioception Constants
constexpr size_t PROPRIOCEPTION_SIZE = 6;

// Developmental Stages
constexpr double DEVELOPMENT_INITIAL_PHASE = 60.0;      // ~1 minute
constexpr double DEVELOPMENT_CRITICAL_PERIOD = 300.0;   // ~5 minutes
constexpr double DEVELOPMENT_MATURATION = 900.0;        // ~15 minutes

// Plasticity Modifiers
constexpr float PLASTICITY_INITIAL_MODIFIER = 1.0f;
constexpr float PLASTICITY_CRITICAL_PERIOD_MODIFIER = 0.8f;
constexpr float PLASTICITY_MATURATION_MODIFIER = 0.5f;
constexpr float PLASTICITY_ADULT_MODIFIER = 0.2f;

// Reward Modulation
constexpr float REWARD_EXPONENTIAL_DECAY_ALPHA = 0.95f;
constexpr float REWARD_EXPONENTIAL_DECAY_BETA = 0.05f;
constexpr float REWARD_DOPAMINE_CLAMP_MIN = -1.0f;
constexpr float REWARD_DOPAMINE_CLAMP_MAX = 1.0f;
constexpr float REWARD_ELIGIBILITY_THRESHOLD = 0.001f;
constexpr float REWARD_ELIGIBILITY_DECAY = 0.1f;

// Learning Experiment
constexpr size_t LEARNING_EXPERIMENT_MOST_ACTIVE_NEIGHBORS = 10;
constexpr float LEARNING_WEIGHT_CHANGE_THRESHOLD = 0.01f;

// Test Parameters
constexpr int TEST_BRAIN_CONNECTIVITY_INJECTION_CURRENT = 50;
constexpr int TEST_BRAIN_CONNECTIVITY_INJECTION_COUNT = 10;
constexpr int TEST_BRAIN_CONNECTIVITY_STEPS = 50;
constexpr int TEST_BRAIN_PLASTICITY_STEPS = 1000;
constexpr int TEST_BRAIN_STDP_TRIALS = 50;
constexpr int TEST_BRAIN_STDP_PRE_CURRENT = 60;

// System Limits
constexpr size_t MAX_VISION_BUFFER_SIZE = 256;  // VISION_WIDTH * VISION_HEIGHT
constexpr size_t MAX_MOTOR_GROUPS = 6;
constexpr size_t MAX_SENSORY_GROUPS = 4;

// Logging
constexpr int LOG_BUFFER_SIZE = 1024;
constexpr size_t MAX_LOG_LEVELS = 5;

} // namespace nlm

#endif // NLM_CONSTANTS_H
