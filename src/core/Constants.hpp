#pragma once

#include <cfloat>
#include <limits>

namespace nlm {

namespace Constants {
    // Physical simulation constants
    static constexpr float DEFAULT_MEMBRANE_CAPACITANCE = 1.0f;     // nF
    static constexpr float DEFAULT_TIME_CONSTANT = 20.0f;          // ms
    static constexpr float DEFAULT_RESTING_POTENTIAL = -70.0f;      // mV
    static constexpr float DEFAULT_THRESHOLD_POTENTIAL = -55.0f;    // mV
    static constexpr float DEFAULT_RESET_POTENTIAL = -65.0f;        // mV
    static constexpr float DEFAULT_LEAK_CONDUCTANCE = 0.01f;        // nS
    static constexpr float DEFAULT_SPIKE_WIDTH = 1.0f;              // ms

    // Plasticity constants
    static constexpr float DEFAULT_STDP_LTP_WEIGHT = 0.01f;
    static constexpr float DEFAULT_STDP_LTD_WEIGHT = 0.012f;
    static constexpr float DEFAULT_STDP_FACILITATION_TAU = 100.0f; // ms
    static constexpr float DEFAULT_STDP_DEPRESSION_TAU = 200.0f;    // ms
    static constexpr float DEFAULT_STDP_U_MAX = 1.0f;

    // Structural plasticity constants
    static constexpr float DEFAULT_SYNAPTAGENESIS_RATE = 0.0001f;
    static constexpr float DEFAULT_PRUNING_RATE = 0.00001f;

    // Neuromodulation constants
    static constexpr float DEFAULT_DOPAMINE_LEVEL = 0.0f;
    static constexpr float DEFAULT_NOVELTY_DECAY = 0.99f;
    static constexpr float DEFAULT_CURIOUSY_FACTOR = 2.0f;

    // Memory constants
    static constexpr size_t DEFAULT_MAX_SPIKE_HISTORY = 100;
    static constexpr size_t DEFAULT_WORKING_MEMORY_CAPACITY = 100;
    static constexpr size_t DEFAULT_MAX_EPISODIC_EPISODES = 1000;

    // Development constants
    static constexpr double DEFAULT_DEVELOPMENT_TIMESTEP = 1000.0;   // ms
    static constexpr float INITIAL_PLASTICITY_MODIFIER = 1.0f;
    static constexpr float MATURATION_PLASTICITY_MODIFIER = 0.5f;
    static constexpr float ADULT_PLASTICITY_MODIFIER = 0.2f;

    // Simulation constants
    static constexpr float DEFAULT_TIMESTEP = 0.001f;                // seconds
    static constexpr float DEFAULT_SIMULATION_TIME = 60.0f;          // seconds
    static constexpr float DEFAULT_MAX_ACCELERATION = 10.0f;         // m/s²

    // Sensory constants
    static constexpr size_t DEFAULT_VISION_WIDTH = 16;
    static constexpr size_t DEFAULT_VISION_HEIGHT = 16;
    static constexpr float DEFAULT_VISION_SCALE = 5.0f;
    static constexpr float DEFAULT_TOUCH_SENSITIVITY = 8.0f;

    // Motor constants
    static constexpr size_t DEFAULT_MOTOR_GROUP_COUNT = 6;
    static constexpr float DEFAULT_MOTOR_ACTIVITY_THRESHOLD = 0.5f;

    // Environment constants
    static constexpr float DEFAULT_WORLD_WIDTH = 20.0f;
    static constexpr float DEFAULT_WORLD_HEIGHT = 20.0f;
    static constexpr float DEFAULT_AGENT_RADIUS = 0.5f;
    static constexpr float DEFAULT_MOVE_SPEED = 3.0f;
    static constexpr float DEFAULT_TURN_SPEED = 2.0f;

    // Reward and value constants
    static constexpr float DEFAULT_RESOURCE_VALUE = 10.0f;
    static constexpr float DEFAULT_HAZARD_DAMAGE = -15.0f;
    static constexpr float DEFAULT_MAX_ENERGY = 100.0f;
    static constexpr float DEFAULT_RESOURCE_ENERGY_GAIN = 30.0f;
    static constexpr float DEFAULT_HAZARD_PASSIVE_DAMAGE = 0.01f;

    // Checkpoint constants
    static constexpr size_t DEFAULT_MAX_CHECKPOINT_ENTRIES = 10000;
    static constexpr size_t DEFAULT_CHECKPOINT_VERSION = 1;

    // Attention and cognition constants
    static constexpr float DEFAULT_ATTENTION_INHIBITION_STRENGTH = 0.5f;
    static constexpr float DEFAULT_ATTENTION_EXCITATION_STRENGTH = 1.5f;
    static constexpr size_t DEFAULT_PLANNING_DEPTH = 5;
}

namespace Defaults {
    // Configuration defaults
    static constexpr size_t DEFAULT_NEURON_COUNT = 1000;
    static constexpr size_t DEFAULT_REGION_COUNT = 1;
    static constexpr float DEFAULT_CONNECTION_PROBABILITY = 0.1f;
    static constexpr float DEFAULT_SIMULATION_TIMESTEP = 0.001f;
    static constexpr uint64_t DEFAULT_RANDOM_SEED = 42;
    static constexpr std::string DEFAULT_CHECKPOINT_PATH = "checkpoint_test.bin";

    // Experiment defaults
    static constexpr uint64_t DEFAULT_MAX_STEPS = 10000;
    static constexpr std::string DEFAULT_EXPERIMENT_NAME = "nlm_experiment";

    // Memory defaults
    static constexpr size_t DEFAULT_MAX_WORKING_MEMORY_SIZE = 100;
    static constexpr size_t DEFAULT_MAX_EPISODIC_SIZE = 1000;
}

namespace Limits {
    // Safety limits
    static constexpr float MIN_WEIGHT = -1.0f;
    static constexpr float MAX_WEIGHT = 1.0f;
    static constexpr float MIN_DOPAMINE_LEVEL = -1.0f;
    static constexpr float MAX_DOPAMINE_LEVEL = 1.0f;
    static constexpr size_t MAX_NEURONS = 100000;
    static constexpr size_t MAX_SYNAPSES = 1000000;
}

namespace Validation {
    // Validation ranges
    static constexpr float ENERGY_MIN = 0.0f;
    static constexpr float ENERGY_MAX = 1.0f;
    static constexpr float HEALTH_MIN = 0.0f;
    static constexpr float HEALTH_MAX = 1.0f;
    static constexpr float POSITION_MIN = -1000.0f;
    static constexpr float POSITION_MAX = 1000.0f;
    static constexpr float ORIENTATION_MIN = -M_PI;
    static constexpr float ORIENTATION_MAX = M_PI;
}

} // namespace nlm

