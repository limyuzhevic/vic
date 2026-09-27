// NLM (熙然) - Neural Learning Machine
// Phase 6: Final Integration
//
// Common constants and configuration values used throughout the NLM project
// to eliminate magic numbers and improve code maintainability.

#pragma once

namespace nlm {

// Time and Simulation
constexpr double DEFAULT_TIMESTEP = 0.001;        // Default simulation timestep (seconds)
constexpr double SIMULATION_TIMESTEP_PRECISION = 0.001f;  // Precision for timestep calculations
constexpr double DEVELOPMENT_TIMESTEP = 0.001;    // Time per development update
constexpr double WORLD_TIMESTEP = 0.01;           // World update timestep (10ms)

// Neural Configuration
constexpr size_t DEFAULT_NEURON_COUNT = 500;      // Default number of neurons
constexpr size_t DEMO_NEURON_COUNT = 100;         // Neuron count for demos
constexpr size_t EXPERIMENT_NEURON_COUNT = 1000;  // Neuron count for experiments
constexpr size_t MIN_NEURON_COUNT = 10;           // Minimum viable neuron count
constexpr float DEFAULT_CONNECTION_PROBABILITY = 0.1f;  // Probability of synaptic connections

// Memory Systems
constexpr size_t VISION_INPUT_SIZE = 256;         // Vision input size (16x16)
constexpr size_t TOUCH_INPUT_SIZE = 8;            // Touch input size
constexpr size_t INTERNAL_INPUT_SIZE = 4;        // Internal signals input size
constexpr size_t PROPRIOCEPTION_INPUT_SIZE = 6;  // Proprioception input size
constexpr size_t SENSEORY_INPUT_TOTAL = VISION_INPUT_SIZE + TOUCH_INPUT_SIZE + 
                                      INTERNAL_INPUT_SIZE + PROPRIOCEPTION_INPUT_SIZE;
constexpr size_t MOTOR_OUTPUT_SIZE = 6;           // Number of motor commands
constexpr size_t MAX_WORKING_MEMORY_TRACES = 100;  // Maximum working memory traces

// Plasticity Parameters
constexpr float DEFAULT_STDP_LTP_WEIGHT = 0.01f;  // STDP Long-Term Potentiation weight
constexpr float DEFAULT_STDP_LTD_WEIGHT = 0.012f; // STDP Long-Term Depression weight
constexpr float DEFAULT_STDP_TAU = 20.0f;         // STDP time constant
constexpr float DEFAULT_HEBBIAN_WEIGHT = 0.02f;   // Hebbian plasticity weight
constexpr float STD_HEBBIAN_WEIGHT = 0.015f;      // Alternative Hebbian weight
constexpr float DEFAULT_SYNAPTOGENESIS_RATE = 0.0001f;  // Rate of new synapse formation
constexpr float DEFAULT_PRUNING_RATE = 0.00001f;  // Rate of synapse elimination

// Neuromodulation
constexpr float DEFAULT_DOPAMINE_RANGE = 1.0f;    // Dopamine signal range [-1, 1]
constexpr float DEFAULT_NOVELTY_THRESHOLD = 0.3f; // Threshold for novelty detection
constexpr float DEFAULT_CURIOIVITY_FACTOR = 2.0f; // Curiosity scaling factor
constexpr float DEFAULT_PREDICTION_ERROR_WEIGHT = 0.5f; // Weight for prediction error

// Development
constexpr double DEVELOPMENT_STAGE_INITIAL = 0.0;     // Initial developmental stage
constexpr double DEVELOPMENT_STAGE_CRITICAL = 60.0;   // Critical period end
constexpr double DEVELOPMENT_STAGE_MATURATION = 300.0; // Maturation end
constexpr double DEVELOPMENT_STAGE_ADULT = 900.0;     // Adult stage end
constexpr double DEVELOPMENT_STAGE_AGING = 3600.0;    // Aging stage end
constexpr float PLASTICITY_INITIAL = 1.0f;          // Initial plasticity factor
constexpr float PLASTICITY_CRITICAL = 0.8f;         // Plasticity during critical period
constexpr float PLASTICITY_MATURATION = 0.5f;       // Plasticity during maturation
constexpr float PLASTICITY_ADULT = 0.2f;            // Plasticity in adult stage

// Attention and Selection
constexpr float ATTENTION_THRESHOLD = 0.5f;       // Threshold for attention selection
constexpr float ATTENTION_COMPETITION_STRENGTH = 1.0f; // Strength of competitive selection
constexpr float ATTENTION_DECAY_RATE = 0.99f;      // Decay rate for attention signals

// Prediction
constexpr float PREDICTION_THRESHOLD = 0.7f;      // Confidence threshold for predictions
constexpr float PREDICTION_ERROR_DECAY = 0.95f;    // Decay rate for prediction error
constexpr size_t PREDICTION_HORIZON = 10;         // Number of steps ahead for predictions

// Memory and Consolidation
constexpr float MEMORY_CONSOLIDATION_THRESHOLD = 0.8f; // Threshold for memory consolidation
constexpr size_t EPISODIC_MEMORY_MAX_EPISODES = 1000; // Maximum episodic memories
constexpr float EPISODIC_MEMORY_REPLAY_FRACTION = 0.1f; // Fraction of episodes to replay

// Motor Commands
constexpr size_t MOTOR_COMMAND_COUNT = 6;         // Number of motor commands
constexpr float MOTOR_COMMAND_THRESHOLD = 0.5f;   // Activity threshold for motor commands
constexpr float MOTOR_COMMAND_EXPLORATION_CHANCE = 0.3f; // Chance of random exploration

// Reward and Learning
constexpr float DEFAULT_REWARD_RANGE = 1.0f;       // Range of reward values
constexpr float REWARD_DISCOUNT_FACTOR = 0.95f;   // Discount factor for future rewards
constexpr float REWARD_NOISE_LEVEL = 0.1f;         // Level of noise in reward signals

// Exploration and Curiosity
constexpr float CURIOSITY_HIGH_THRESHOLD = 0.5f;   // High curiosity threshold
constexpr float CURIOSITY_EXPLORATION_BOOST = 0.3f; // Exploration boost from curiosity
constexpr float CURIOSITY_DECAY_RATE = 0.99f;      // Decay rate for curiosity signals

// Experiment Parameters
constexpr size_t DEFAULT_DEMO_EPISODES = 5;       // Default episodes for demos
constexpr size_t DEFAULT_DEMO_STEPS_PER_EPISODE = 200; // Default steps per episode
constexpr size_t DEFAULT_EXPERIMENT_TRIALS = 50;  // Default trials for experiments
constexpr uint64_t DEFAULT_MAX_STEPS = 10000;     // Default maximum simulation steps
constexpr uint64_t DEFAULT_SYNAPSE_FORMATION_STEPS = 10000; // Synapse formation duration
constexpr uint64_t DEFAULT_SENSORY_DEVELOPMENT_STEPS = 40000; // Sensory development duration
constexpr uint64_t DEFAULT_MOTOR_DEVELOPMENT_STEPS = 50000;  // Motor development duration
constexpr uint64_t DEFAULT_ASSOCIATIVE_DEVELOPMENT_STEPS = 100000; // Associative development duration
constexpr uint64_t DEFAULT_MEMORY_CONSOLIDATION_STEPS = 100000; // Memory consolidation duration
constexpr uint64_t DEFAULT_SOCIAL_DEVELOPMENT_STEPS = 100000;   // Social development duration

// Output and Logging
constexpr size_t STATUS_LOG_FREQUENCY = 1000;     // Frequency of status log messages
constexpr size_t PROGRESS_LOG_FREQUENCY = 100;    // Frequency of progress log messages
constexpr float METRICS_PRECISION = 0.001f;       // Precision for metric reporting

// Physical and Biological
constexpr float LIF_RESTING_POTENTIAL = -65.0f;   // LIF neuron resting potential (mV)
constexpr float LIF_THRESHOLD = -50.0f;          // LIF neuron spike threshold (mV)
constexpr float LIF_RESET_POTENTIAL = -40.0f;    // LIF neuron reset potential (mV)
constexpr float LIF_TIME_CONSTANT = 20.0f;        // LIF membrane time constant (ms)
constexpr float LIF_MEMBRANE_RESISTANCE = 100.0f; // LIF membrane resistance (MOhm)
constexpr float LIF_CAPACITANCE = 1.0f;           // LIF membrane capacitance (nF)

// Checkpoint and Persistence
constexpr const char* DEFAULT_CHECKPOINT_FILENAME = "./checkpoint_test.bin"; // Default checkpoint filename
constexpr const char* CHECKPOINT_BACKUP_SUFFIX = ".backup";                 // Checkpoint backup suffix

// Scaling and Benchmarking
constexpr size_t BENCHMARK_SCALE_FACTOR = 10;     // Factor for scaling benchmarks
constexpr float BENCHMARK_CONVERGENCE_THRESHOLD = 0.95f; // Convergence threshold for benchmarks

// Social and Communication
constexpr size_t MAX_OBSERVATION_HISTORY = 100;   // Maximum observations to store for social learning
constexpr float SOCIAL_LEARNING_WEIGHT = 0.5f;    // Weight for social learning
constexpr float IMITATION_THRESHOLD = 0.7f;      // Threshold for imitation learning

// Validation and Error Handling
constexpr float VALIDATION_TOLERANCE = 1e-6f;     // Tolerance for numerical validation
constexpr size_t VALIDATION_MAX_ITERATIONS = 1000; // Maximum iterations for validation
constexpr float ERROR_RECOVERY_ATTEMPTS = 3.0f;   // Number of recovery attempts for errors

} // namespace nlm
