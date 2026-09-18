#pragma once

namespace nlm {

// Constants for configuration
// Simulation timing constants
static constexpr double DEFAULT_TIMESTEP = 0.001;  // seconds
static constexpr double DEFAULT_REPLAY_INTERVAL = 100.0;    // steps
static constexpr double DEFAULT_CONSOLIDATION_INTERVAL = 1000.0;  // steps

// Network topology constants
static constexpr size_t DEFAULT_NEURON_COUNT = 1000;
static constexpr size_t DEFAULT_REGION_COUNT = 1;
static constexpr float DEFAULT_CONNECTION_PROBABILITY = 0.1f;

// Plasticity constants
static constexpr float DEFAULT_STDP_LTP_WEIGHT = 0.01f;
static constexpr float DEFAULT_STDP_LTD_WEIGHT = 0.012f;
static constexpr float DEFAULT_STDP_TAU = 20.0f;

// Structural plasticity constants
static constexpr float DEFAULT_SYNAPTGENESIS_RATE = 0.0001f;
static constexpr float DEFAULT_PRUNING_RATE = 0.00001f;

// Neuromodulation constants
static constexpr float DEFAULT_NOVELTY_DECAY = 0.99f;
static constexpr float DEFAULT_DOPAMINE_CLAMP_MIN = -1.0f;
static constexpr float DEFAULT_DOPAMINE_CLAMP_MAX = 1.0f;

// Memory system constants
static constexpr size_t DEFAULT_WORKING_MEMORY_CAPACITY = 100;
static constexpr size_t DEFAULT_MAX_EPISODES = 1000;

// Curiosity and exploration constants
static constexpr float DEFAULT_CURIOUSITY_NOVELTY_WEIGHT = 2.0f;
static constexpr float DEFAULT_CURIOUSITY_ERROR_WEIGHT = 0.5f;
static constexpr float DEFAULT_CURIOUSITY_EXPLORATION_THRESHOLD = 0.3f;
static constexpr float DEFAULT_CURIOUSITY_MAX_EXPLORATION_CHANCE = 0.3f;

// Development system constants
static constexpr double DEVELOPMENT_STAGE_1_MIN_AGE = 60.0;    // ~1 minute
static constexpr double DEVELOPMENT_STAGE_2_MIN_AGE = 300.0;   // ~5 minutes
static constexpr double DEVELOPMENT_STAGE_3_MIN_AGE = 900.0;   // ~15 minutes

// Neuron type grouping constants
static constexpr size_t SENSORY_NEURON_FRACTION = 4;      // 25% vision, 25% touch, 25% internal, 25% proprioception
static constexpr size_t MOTOR_NEURON_FRACTION = 6;        // 6 action groups

// Synapse scaling constants
static constexpr float SYNAPSE_CURRENT_SCALE = 10.0f;
static constexpr float SYNAPSE_WEIGHT_SCALE = 100.0f;

// Activity threshold constants
static constexpr float NEURON_ACTIVITY_THRESHOLD = 0.5f;
static constexpr float NEURON_ACTIVATION_MEASUREMENT_SCALE = 10.0f;

} // namespace nlm