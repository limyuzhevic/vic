#pragma once

namespace nlm {

// Sensory input dimensions (AgentBrain.cpp)
const size_t VISION_INPUT_SIZE = 256;    // 16x16 vision grid
const size_t TOUCH_INPUT_SIZE = 8;      // Touch sensor array
const size_t INTERNAL_INPUT_SIZE = 4;   // Internal signals
const size_t PROPRIOCEPTION_INPUT_SIZE = 6; // Proprioceptive sensors
const size_t TOTAL_SENSORY_INPUT_SIZE = VISION_INPUT_SIZE + TOUCH_INPUT_SIZE + 
                                        INTERNAL_INPUT_SIZE + PROPRIOCEPTION_INPUT_SIZE;

const size_t MOTOR_OUTPUT_SIZE = 6;     // Forward, backward, left, right, interact, wait

// Vision encoding parameters (AgentBrain.cpp)
const float VISION_SCALE_FACTOR = 5.0f;        // Scale vision intensity to current
const float TOUCH_SCALE_FACTOR = 8.0f;        // Scale touch to collision signal
const float INTERNAL_SCALE_FACTOR = 5.0f;     // Center and scale internal signals
const float PROPRIOCEPTION_SCALE_FACTOR = 3.0f; // Center and scale proprioception

// Novelty detection parameters (AgentBrain.cpp)
const float SENSORY_NOVELTY_DECAY = 0.99f;     // Decay factor for novelty computation
const float NOVELTY_NORMALIZATION_DIVISOR = 1;  // Prevent division by zero

// Neuromodulation parameters (AgentBrain.cpp)
const float CURIOUSITY_THRESHOLD = 0.3f;       // Minimum curiosity for exploration
const float EXPLORATION_CHANCE_MULTIPLIER = 0.3f; // Max exploration chance (30%)
const float RANDOM_MOTOR_COMMAND_CHOICES = 8; // LookLeft, LookRight in addition to Move commands
const float PREDICTION_ERROR_COEFFICIENT = 0.5f; // Weight for prediction error in curiosity
const float DOUBLED_NOVELTY_CONTRIBUTION = 2.0f;  // Novelty weight in curiosity calculation
const float EXPECTED_REWARD_ALPHA = 0.05f;   // EMA smoothing factor
const float EXPECTED_REWARD_BETA = 0.95f;    // EMA smoothing factor
const float DOPAMINE_CLAMP_MIN = -1.0f;      // Minimum dopamine level
const float DOPAMINE_CLAMP_MAX = 1.0f;       // Maximum dopamine level
const float PLASTICITY_FACTOR_BASE = 0.5f;   // Base plasticity factor
const float PLASTICITY_FACTOR_CLAMP_MIN = 0.1f; // Minimum plasticity factor
const float PLASTICITY_FACTOR_CLAMP_MAX = 2.0f;  // Maximum plasticity factor

// Developmental stages (AgentBrain.cpp)
const double DEVELOPMENTAL_AGE_INITIAL = 60.0;      // ~1 minute in simulation time
const double DEVELOPMENTAL_AGE_CRITICAL_PERIOD_END = 300.0; // ~5 minutes
const double DEVELOPMENTAL_AGE_MATURATION_END = 900.0; // ~15 minutes
const float PLASTICITY_INITIAL = 1.0f;             // High plasticity in early stages
const float PLASTICITY_CRITICAL_PERIOD = 0.8f;     // Moderate plasticity
const float PLASTICITY_MATURATION = 0.5f;          // Reduced plasticity
const float PLASTICITY_ADULT = 0.2f;               // Stable adult plasticity

// Structural plasticity rates (AgentBrain.cpp)
const float SYNAPTONGENESIS_RATE_BASE = 0.0001f;   // Base synaptogenesis rate
const float PRUNING_RATE_BASE = 0.00001f;         // Base pruning rate
const float PLASTICITY_MODIFIER_MIN = 0.1f;       // Minimum plasticity modifier
const float PLASTICITY_MODIFIER_MAX = 2.0f;       // Maximum plasticity modifier

// Motor decoding parameters (AgentBrain.cpp)
const float MOTOR_ACTIVITY_THRESHOLD = 0.5f;       // Minimum activity to act

// Motor command enumeration offset (for LookLeft/LookRight vs standard 6 actions)
const int MOTOR_COMMAND_OFFSET = 6;               // Index where LookLeft/LookRight begin
const int RANDOM_COMMAND_RANGE_MAX = 7;           // 0-7 inclusive

// Simulation parameters (Brain.cpp)
const TimestepDuration DEFAULT_TIMESTEP = 0.001;  // 1ms timestep
const SimulationStep DEFAULT_REPLAY_INTERVAL = 100; // Replay every 100 steps
const SimulationStep DEFAULT_CONSOLIDATION_INTERVAL = 1000; // Consolidate every 1000 steps
const size_t DEFAULT_CHECKPOINT_INTERVAL = 10000; // Checkpoint every 10000 steps

// Neural population ratios (Brain.cpp)
const float SENSORY_NEURON_RATIO = 0.25f;         // 25% of neurons are sensory
const float INTERNAL_NEURON_RATIO = 0.5f;        // 50% are internal
const float MOTOR_NEURON_RATIO = 0.25f;          // 25% are motor

// Connectivity parameters (Brain.cpp)
const float CONNECTION_PROBABILITY_DEFAULT = 0.1f; // Default connection probability
const float DEFAULT_EXCITATION_WEIGHT = 0.2f;     // Default excitatory weight
const float DEFAULT_INHIBITION_WEIGHT = 0.1f;    // Default inhibitory weight

// Plasticity system parameters (Brain.cpp)
const float DEFAULT_STDP_LTP_WEIGHT = 0.01f;      // Long-term potentiation weight
const float DEFAULT_STDP_LTD_WEIGHT = 0.012f;     // Long-term depression weight
const float DEFAULT_STDP_TIME_CONSTANT = 20.0f;   // STDP time constant

// Structural plasticity parameters (Brain.cpp)
const float STRUCTURED_PLASTICITY_SYNAPTONGENESIS_RATE = 0.0001f; // Synaptogenesis rate
const float STRUCTURED_PLASTICITY_PRUNING_RATE = 0.00001f;       // Pruning rate

// Memory system parameters (Brain.cpp)
const size_t WORKING_MEMORY_CAPACITY_RATIO = 10;   // Capacity = neuronCount / 10
const size_t EPISODIC_MEMORY_MAX_EPISODES = 1000; // Maximum stored episodes

// Planning parameters (Brain.cpp)
const int PLANNING_DEPTH_DEFAULT = 5;             // Default planning depth

// Neuromodulation system parameters (Brain.cpp)
const float ATTENTION_INHIBITION_STRENGTH_DEFAULT = 0.5f; // Default inhibition strength
const float ATTENTION_EXCITATION_STRENGTH_DEFAULT = 1.5f; // Default excitation strength

// Checkpoint system parameters (Brain.cpp)
const CompressionLevel CHECKPOINT_COMPRESSION_DEFAULT = CompressionLevel::Balanced; // Default compression

// Episode storage parameters (Brain.cpp)
const size_t EPISODE_STORE_INTERVAL = 10;         // Store episode every 10 steps

// Developmental update parameters (Brain.cpp)
const SimulationStep DEVELOPMENT_UPDATE_INTERVAL = 1000; // Update development every 1000 steps

// Memory consolidation parameters (Brain.cpp)
const float EPISODIC_CONSOLIDATION_THRESHOLD = 0.3f; // Threshold for consolidation

// Action selection parameters (Brain.cpp)
const float MOTOR_ACTIVATION_THRESHOLD = 0.5f;    // Minimum firing to trigger action

// Learning rate parameters
const float PLASTICITY_LEARNING_RATE_DEFAULT = 0.01f;    // Default learning rate
const float MAX_SYNAPSE_WEIGHT = 1.0f;                    // Maximum synapse weight
const float MIN_SYNAPSE_WEIGHT = -1.0f;                   // Minimum synapse weight

// Reward and error parameters
const float REWARD_SCALE_FACTOR = 1.0f;                   // Scale reward values
const float ERROR_SCALE_FACTOR = 1.0f;                    // Scale error values

// Threshold parameters
const float ACTIVATION_THRESHOLD = 0.5f;                  // Neural activation threshold
const float NOISE_THRESHOLD = 0.3f;                        // Noise threshold for plasticity

} // namespace nlm
