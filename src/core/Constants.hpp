#pragma once

namespace nlm {
namespace Constants {

namespace nlm {
namespace Constants {

// Neural simulation constants
constexpr float SIMULATION_TIMESTEP = 0.001f;  // 1 ms timestep
constexpr float DEFAULT_THRESHOLD = -55.0f;     // mV
constexpr float DEFAULT_RESTING_POTENTIAL = -70.0f; // mV
constexpr float DEFAULT_RESET_POTENTIAL = -70.0f;   // mV
constexpr float DEFAULT_LEAK_CONDUCTANCE = 10.0f;   // nS
constexpr float DEFAULT_REFACTORY_PERIOD = 9.0f;    // steps (Phase 6: 9 steps, not 5)
constexpr float DEFAULT_ADAPTATION_STRENGTH = 0.01f;
constexpr float ZERO = 0.0f;                      // Zero value constant

// Connectivity constants
constexpr float DEFAULT_CONNECTION_PROBABILITY = 0.1f;
constexpr float DEFAULT_EXCITATORY_WEIGHT = 0.2f;
constexpr float DEFAULT_INHIBITORY_WEIGHT = 0.1f;
constexpr float DEFAULT_DELAY = 1.0f;             // synaptic delay
constexpr float CONNECTIONS_PER_NEURON_CAP = 25.0f;  // homeostatic scaling cap
constexpr float SYNAPTIC_SCALING_FACTOR = 10.0f;      // Synaptic current scaling

// Plasticity constants
constexpr float STDP_LTP_WEIGHT = 0.01f;         // Long-term potentiation weight
constexpr float STDP_LTD_WEIGHT = 0.012f;        // Long-term depression weight
constexpr float STDP_TIME_CONSTANT = 20.0f;      // STDP time constant (tau)
constexpr float HEBBIAN_LEARNING_RATE = 0.01f;   // Hebbian learning rate
constexpr float STRUCTURE_PLACTICITY_RATE = 0.0001f; // Synaptogenesis rate
constexpr float PRUNING_RATE = 0.00001f;         // Pruning rate
constexpr float PLASTICITY_LEARNING_RATE = 0.001f; // General plasticity learning rate

// Memory system constants
constexpr float WORKING_MEMORY_CAPACITY_MULTIPLIER = 0.1f;  // 10% of neurons
constexpr size_t MAX_EPISODES = 1000;
constexpr float EPISODE_STORAGE_INTERVAL = 10.0f;  // steps
constexpr float CONSOLIDATION_THRESHOLD = 0.3f;
constexpr float REPLAY_INTERVAL = 100.0f;       // steps
constexpr float CONSOLIDATION_INTERVAL = 1000.0f;    // steps

// Neuromodulation constants
constexpr float DOPAMINE_NOISE_VARIANCE = 0.01f;
constexpr float NOVELTY_THRESHOLD = 0.01f;       // For change detection
constexpr float NOVELTY_DECAY = 0.99f;          // Habituation decay
constexpr float NOVELTY_PREDICTION_ALPHA = 0.05f; // Slow prediction adaptation

// Motor system constants
constexpr float MOTOR_GROUP_SIZE = 6.0f;        // 6 action groups
constexpr float EXCITATORY_BIAS = 1.5f;         // Motor excitation
constexpr float INHIBITORY_BIAS = 0.5f;         // Motor inhibition

// Sensor processing constants
constexpr float SENSORY_EXCITATORY_SCALE = 5.0f;   // Vision, touch, proprioception
constexpr float SENSORY_INTERNAL_SCALE = 2.0f;    // Internal signals
constexpr float SENSORY_PROPRIOCEPTION_SCALE = 3.0f; // Proprioception
constexpr float TEXT_EXCITATORY_SCALE = 8.0f;    // Text neuron encoding
constexpr float INTERACT_RANGE = 1.5f;          // Interaction distance
constexpr float INTERACT_REWARD = 10.0f;        // Resource reward
constexpr float HAZARD_PENALTY = -15.0f;       // Hazard penalty
constexpr float PROXIMITY_INTERACT_THRESHOLD = 0.05f;  // For touch-based interact

// Curiosity and learning constants
constexpr float EXPLORATION_PROBABILITY = 0.3f; // Max exploration probability
constexpr float CONCEPT_FORMATION_THRESHOLD = 0.1f; // Confidence threshold
constexpr float PREDICTION_ERROR_WEIGHT = 0.5f; // Prediction error scaling

// Development constants
constexpr double DEVELOPMENT_STAGE_DURATION_INITIAL = 60.0;  // seconds
constexpr double DEVELOPMENT_STAGE_DURATION_CRITICAL = 300.0; // seconds
constexpr double DEVELOPMENT_STAGE_DURATION_MATURATION = 900.0; // seconds
constexpr double DEVELOPMENT_STAGE_DURATION_ADULT = 2000.0;   // seconds
constexpr float PLASTICITY_MODIFIER_INITIAL = 1.0f;    // High plasticity
constexpr float PLASTICITY_MODIFIER_CRITICAL = 0.8f;
constexpr float PLASTICITY_MODIFIER_MATURATION = 0.5f;
constexpr float PLASTICITY_MODIFIER_ADULT = 0.2f;     // Stable

// Checkpoint system constants
constexpr size_t CHECKPOINT_MANAGER_MAX_VERSIONS = 10000;
constexpr size_t CHECKPOINT_MANAGER_MAX_FILES = 5;
constexpr bool CHECKPOINT_MANAGER_COMPRESSION_ENABLED = true;
constexpr size_t CHECKPOINT_BUFFER_SIZE = 5000;

// Statistical tracking constants
constexpr float SPIKE_COUNTING_INTERVAL = 1.0f;    // steps
constexpr float PERFORMANCE_MEASUREMENT_INTERVAL = 1000.0f; // steps
constexpr float MEMORY_CONSOLIDATION_INTERVAL = 1000.0f;    // steps

// Text encoding constants
constexpr size_t TEXT_ENCODER_OUTPUT_SIZE = 4;           // 4 neurons from 16 text neurons
constexpr size_t TEXT_ENCODER_MAX_INPUT_SIZE = 30;       // 31 neurons total (0-30)

// Social learning constants
constexpr float SOCIAL_SIMILARITY_THRESHOLD = 0.8f;    // Imitation threshold
constexpr float SOCIAL_OBSERVATION_DECAY = 0.95f;      // Memory decay

// Safety and debugging constants
constexpr float DEBUG_LOGGING_THRESHOLD = 0.01f;       // Log when activity exceeds this
constexpr float PERFORMANCE_WARNING_THRESHOLD = 0.9f;  // Warn when performance drops below this
constexpr size_t MAX_HISTORICAL_ENTRIES = 10000;        // Cap history size
constexpr float SPIKE_RATE_WARNING = 100.0f;           // Hz

// Performance optimization constants
constexpr size_t SYNAPTIC_DELIVERY_O1_BUFFER_SIZE = 1024;  // O(1) synaptic delivery buffer
constexpr float SPIKE_PROCESSING_BATCH_SIZE = 100.0f;       // Batch spike processing
constexpr float NEURON_LOOKUP_HASH_LOAD_FACTOR = 0.75f;     // Hash table load factor

// Error handling constants
constexpr int MAX_ERROR_RETRIES = 3;
constexpr float ERROR_CORRECTION_TIMEOUT = 1.0f;  // seconds
constexpr bool FAIL_FAST_ON_CRITICAL_ERROR = true;

// Command and configuration constants
constexpr int CONFIG_PARSER_MAX_LINE_LENGTH = 4096;
constexpr int CONFIG_COMMAND_MAX_ARGS = 10;
constexpr size_t MAX_WORKER_THREADS = 8;           // Thread pool size
constexpr size_t TASK_QUEUE_SIZE = 1000;           // Task queue capacity

// Sensory input thresholds and scaling
constexpr float FORWARD_TOUCH_THRESHOLD = 0.05f;    // For movement detection
constexpr float FORWARD_OBJECT_VISION_THRESHOLD = 0.5f; // Resource detection
constexpr float FORWARD_OBJECT_HAZARD_THRESHOLD = 0.35f; // Hazard detection
constexpr float MAX_TOUCH_RANGE = 0.05f;           // Interact range threshold

// Phase 5 and 6 specific constants
constexpr float BRILLIANT_APPROACH_THRESHOLD = 0.5f;    // Phototaxis trigger
constexpr float BRILLIANT_APPROACH_BONUS = 10.0f;      // Phototaxis bonus
constexpr float NOVELTY_SPIKE_THRESHOLD = 0.02f;       // Novelty spike trigger
constexpr float CONCEPT_BONUS = 8.0f;                 // Concept reward bonus
constexpr float CONCEPT_PENALTY = 8.0f;               // Concept hazard penalty
constexpr float PLANNER_BIAS_THRESHOLD = 0.05f;         // Planner bias activation
constexpr float PLANNER_BIAS_OVERRIDE = 15.0f;          // Planner override bonus
constexpr float LEARNED_PREFERENCE_MAX = 1.0f;          // Max learned preference
constexpr float LEARNED_PREFERENCE_MIN = -1.0f;        // Min learned preference
constexpr float LEARNED_PREFERENCE_UPDATE = 0.1f;      // Preference update rate
constexpr float SELF_MODEL_CONFIDENCE_THRESHOLD = 0.8f; // Self-model bias activation
constexpr float SELF_MODEL_CONFIDENCE_GAP = 0.05f;     // Self-model confidence gap
constexpr float SOCIAL_BIASED_SIMILARITY_THRESHOLD = 0.8f; // Social imitation threshold
constexpr float SOCIAL_BIAS_BONUS = 5.0f;              // Social imitation bonus
constexpr float TURN_DIVERSITY_THRESHOLD = 25.0f;      // Turn diversity trigger
constexpr float TURN_DIVERSITY_BONUS = 6.0f;           // Turn diversity bonus
constexpr float REPEAT_FATIGUE_THRESHOLD = 3.0f;       // Repetition fatigue trigger
constexpr float REPEAT_FATIGUE_PENALTY = 8.0f;         // Repetition fatigue penalty
constexpr float MOMENTUM_REVERSAL_PENALTY = 8.0f;      // Movement reversal penalty
constexpr float SPATIAL_TOUCH_RANGE = 1.5f;            // Physical touch range
constexpr float INTERACTION_RANGE = 1.0f;             // Interaction range

// Deprecated constants (to be phased out)
// LEAK_CONDUCTANCE: Deprecated - use DEFAULT_LEAK_CONDUCTANCE
// RESET_POTENTIAL: Deprecated - use DEFAULT_RESET_POTENTIAL

} // namespace Constants
} // namespace nlm

} // namespace Constants
} // namespace nlm