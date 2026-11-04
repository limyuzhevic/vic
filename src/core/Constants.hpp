#pragma once

namespace nlm {

// Neural network constants
namespace Constants {
    // Neuron parameters (LIF model)
    constexpr float V_REST = -70.0f;      // Resting potential (mV)
    constexpr float V_THRESH = -50.0f;    // Threshold potential (mV)
    constexpr float V_RESET = -75.0f;     // Reset potential after spike (mV)
    constexpr float TAU_MEM = 20.0f;      // Membrane time constant (ms)
    constexpr float TAU_REF = 2.0f;       // Refractory period (ms)

    // Simulation parameters
    constexpr float DEFAULT_TIMESTEP = 0.001f; // Default simulation timestep (s)
    constexpr float SIMULATION_DURATION = 10.0f; // Default simulation duration (s)

    // Connectivity and plasticity
    constexpr float DEFAULT_CONNECTION_PROB = 0.1f; // Default connection probability
    constexpr float DEFAULT_INITIAL_WEIGHT_MEAN = 0.5f;
    constexpr float DEFAULT_INITIAL_WEIGHT_STD = 0.1f;

    // Plasticity parameters
    constexpr float DEFAULT_STDP_LTP_WEIGHT = 0.02f; // STDP LTP weight
    constexpr float DEFAULT_STDP_LTD_WEIGHT = 0.015f; // STDP LTD weight
    constexpr float DEFAULT_STDP_TAU = 20.0f;         // STDP time constant
    constexpr float DEFAULT_HBC_THRESHOLD = 0.01f;    // Hebbian threshold

    // Structural plasticity
    constexpr float DEFAULT_SYNAPTONGENESIS_RATE = 0.0001f;
    constexpr float DEFAULT_PRUNING_RATE = 0.00001f;

    // Memory systems
    constexpr size_t DEFAULT_WORKING_MEMORY_CAPACITY = 100; // Default capacity
    constexpr size_t DEFAULT_MAX_EPISODES = 1000;

    // Neuromodulation
    constexpr float DEFAULT_DOPAMINE_SCALE = 1.0f;
    constexpr float DEFAULT_NOVELTY_DECAY = 0.99f;
    constexpr float DEFAULT_CURIOUSITY_THRESHOLD = 0.3f;

    // Cognition
    constexpr size_t DEFAULT_PLANNING_DEPTH = 5;
    constexpr float DEFAULT_ATTENTION_STRENGTH = 0.5f;
    constexpr float DEFAULT_EXCITATION_STRENGTH = 1.5f;

    // Motor system
    constexpr size_t MOTOR_ACTION_COUNT = 6; // MoveForward, MoveBackward, TurnLeft, TurnRight, Interact, Wait
    constexpr float DEFAULT_INTERACT_REFLEX_BONUS = 20.0f;
    constexpr float DEFAULT_HAZARD_TURN_BONUS = 12.0f;

    // Social learning
    constexpr float DEFAULT_SOCIAL_SIMILARITY_THRESHOLD = 0.8f;

    // Development
    constexpr float INITIAL_PLATIVITY = 1.0f;
    constexpr float CRITICAL_PLATIVITY = 0.8f;
    constexpr float MATURATION_PLATIVITY = 0.5f;
    constexpr float ADULT_PLATIVITY = 0.2f;

    // Checkpoint system
    constexpr const char* DEFAULT_CHECKPOINT_DIR = "./checkpoints";
    constexpr size_t DEFAULT_MAX_CHECKPOINTS = 10000;
    constexpr float DEFAULT_CHECKPOINT_INTERVAL = 100.0f; // steps

    // Performance
    constexpr size_t MAX_PARALLEL_REGIONS = 16; // For parallel processing
    constexpr float PERFORMANCE_SCALE_FACTOR = 1.0f;

    // Numerical tolerances
    constexpr float EPSILON = 1e-6f;
    constexpr float SMALL_VALUE = 1e-6f;
    constexpr float MEDIUM_VALUE = 0.01f;
    constexpr float LARGE_VALUE = 0.1f;

    // Debug and logging
    constexpr bool ENABLE_DETAILED_LOGGING = true;
    constexpr bool ENABLE_PERFORMANCE_PROFILING = false;

    // Visualization
    constexpr int DEFAULT_VISION_WIDTH = 8;
    constexpr int DEFAULT_VISION_HEIGHT = 8;
    constexpr size_t TEXT_NEuron_OFFSET = 16; // Text neurons start at index 16

    // Reward computation constants
    constexpr float NOVELTY_REWARD_SCALE = 2.0f;
    constexpr float PROGRESS_MAX_REWARD = 1.0f;
    constexpr float PROGRESS_CURVE_FACTOR = 10.0f;
    constexpr float PREDICTION_ERROR_REWARD_SCALE = 0.5f;
    constexpr float CURIOSITY_REWARD_SCALE = 1.5f;
    constexpr float VISION_REWARD_SCALE = 1.0f;
    constexpr float VISION_PUNISHMENT_SCALE = 2.0f;
    constexpr float TOUCH_REWARD_SCALE = 1.0f;
    constexpr float INTERNAL_REWARD_SCALE = 1.0f;
    constexpr float INTEGRATION_NOVELTY_WEIGHT = 0.4f;
    constexpr float INTEGRATION_CURIOUSITY_WEIGHT = 0.3f;
    constexpr float INTEGRATION_PREDICTION_WEIGHT = 0.3f;
    constexpr float INTEGRATION_BONUS_SCALE = 0.5f;
    constexpr float TEMPORAL_DECAY_RATE = 0.95f;
    constexpr float TEMPORAL_BASE_RATE = 0.1f;
    constexpr float ELIGIBILITY_DECAY = 0.9f;
    constexpr float MAX_ELIGIBILITY = 1.0f;

    // Neuromodulation constants
    constexpr float DA_CENTER_OPTIMUM = 0.3f;
    constexpr float DA_LOW_PLASTICITY = 0.5f;
    constexpr float DA_OPTIMAL_PLASTICITY = 1.2f;
    constexpr float DA_HIGH_PLASTICITY = 0.8f;
    constexpr float DA_SURPRISE_THRESHOLD = 0.4f;
    constexpr float DA_PREDICTION_ERROR_GAIN = 0.5f;
    constexpr float REWARD_PREDICTION_ERROR_THRESHOLD = 0.1f;
    constexpr float PHASIC_BURST_MAGNITUDE = 1.5f;
    constexpr float PHASIC_DIP_MAGNITUDE = 0.8f;
    constexpr float TONIC_MODULATION = 0.3f;
    constexpr float EXPLORATION_THRESHOLD = 0.2f;
    constexpr float EXPLORATION_BONUS = 0.5f;
}

} // namespace nlm