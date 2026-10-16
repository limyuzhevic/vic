// NLM (熙然) - Neural Learning Machine
// Global Constants and Configuration
//
// This file defines all configurable constants used throughout the NLM project.
// It centralizes magic numbers and hardcoded values, making them easier to
// configure and modify. All constants should be used via the Config system
// for runtime configurability where needed.

#pragma once

#include <string>
#include <vector>
#include <limits>

namespace nlm {

// ==================== SIMULATION CONSTANTS ====================

namespace SimulationConstants {
    // Time management
    constexpr double DEFAULT_TIMESTEP = 0.001;           // seconds
    constexpr double MIN_TIMESTEP = 0.0001;             // seconds (100 microseconds)
    constexpr double MAX_TIMESTEP = 0.1;                // seconds
    constexpr Timestamp DEFAULT_START_TIME = 0.0;        // seconds
    constexpr SimulationStep DEFAULT_MAX_STEPS = 100000; // simulation steps
    
    // Time units conversion
    constexpr double SECONDS_TO_MILLISECONDS = 1000.0;
    constexpr double MILLISECONDS_TO_SECONDS = 0.001;
    constexpr double MINUTES_TO_SECONDS = 60.0;
    constexpr double HOURS_TO_SECONDS = 3600.0;
}

// ==================== NEURON CONSTANTS ====================

namespace NeuronConstants {
    // LIF neuron parameters
    constexpr float MEMBRANE_CAPACITANCE = 1.0f;           // nF - neuron membrane capacitance
    constexpr float TIME_CONSTANT = 20.0f;                // ms - membrane time constant
    constexpr float RESTING_POTENTIAL = -70.0f;          // mV - resting membrane potential
    constexpr float DEFAULT_THRESHOLD = -55.0f;           // mV - firing threshold
    constexpr float DEFAULT_RESET_POTENTIAL = -65.0f;     // mV - reset potential after spike
    constexpr float DEFAULT_LEAK_CONDUCTANCE = 0.1f;      // nS - leak conductance
    constexpr float DEFAULT_ADAPTATION_STRENGTH = 0.01f;  // Scaling factor for adaptation
    constexpr float DEFAULT_ADAPTATION_DECAY_RATE = 0.95f; // Fraction of adaptation remaining each step
    
    // Refractory period parameters
    constexpr uint32_t MIN_REFRACTORY_PERIOD = 2;         // ms - minimum refractory period
    constexpr uint32_t MAX_REFRACTORY_PERIOD = 10;        // ms - maximum refractory period
    
    // Spike history limits
    constexpr size_t MAX_SPIKE_HISTORY = 100;             // Number of spikes to track
    
    // Membrane potential bounds for stability
    constexpr float MIN_MEMBRANE_POTENTIAL = -100.0f;     // mV - prevent hyperpolarization too much
    constexpr float MAX_MEMBRANE_POTENTIAL = 50.0f;       // mV - prevent depolarization too much
    
    // Adaptation variable bounds
    constexpr float MAX_ADAPTATION_VARIABLE = 10.0f;      // Prevent runaway adaptation
    
    // Biological initialization ranges
    constexpr float RESTING_POTENTIAL_VARIATION = 3.0f;   // mV - ± variation from baseline
    constexpr float THRESHOLD_VARIATION = 2.0f;           // mV - ± variation from baseline
    constexpr float RESET_POTENTIAL_VARIATION = 5.0f;     // mV - ± variation from resting
    constexpr float REFRACTORY_VARIATION = 8;             // ms - variation from min to max
}

// ==================== SYNAPSE CONSTANTS ====================

namespace SynapseConstants {
    // Weight bounds for stability
    constexpr float MIN_WEIGHT = -1.0f;                   // Minimum synaptic weight
    constexpr float MAX_WEIGHT = 1.0f;                    // Maximum synaptic weight
    constexpr float WEIGHT_CHANGE_THRESHOLD = 0.01f;     // Minimum weight change to be considered significant
    
    // Short-term plasticity parameters
    constexpr float STP_FACILITATION_TAU = 100.0f;       // ms - time constant for facilitation
    constexpr float STP_DEPRESSION_TAU = 200.0f;         // ms - time constant for depression
    constexpr float STP_U_MAX = 1.0f;                    // Maximum utilization factor
    constexpr float STP_RECOVERY_RATE = 0.001f;          // Recovery rate from depression
    
    // Eligibility trace parameters
    constexpr float ELIGIBILITY_TRACE_DECAY = 0.001f;    // Fast decay rate for reward-modulated learning
    constexpr float MAX_ELIGIBILITY_TRACE = 1.0f;        // Maximum eligibility trace magnitude
    
    // Synaptic efficacy bounds
    constexpr float MIN_EFFICIENCY = 0.0f;               // Minimum synaptic efficacy
    constexpr float MAX_EFFICIENCY = 2.0f;               // Maximum synaptic efficacy
    
    // Short-term plasticity spike history bounds
    constexpr size_t MAX_SPIKE_HISTORY = 100;             // Number of spikes to track for STP
    
    // Weight initialization bounds
    constexpr float EXCITATORY_WEIGHT_MIN = 0.1f;        // Minimum excitatory weight
    constexpr float EXCITATORY_WEIGHT_MAX = 0.4f;        // Maximum excitatory weight
    constexpr float INHIBITORY_WEIGHT_MIN = -0.1f;       // Minimum inhibitory weight magnitude
    constexpr float INHIBITORY_WEIGHT_MAX = -0.4f;       // Maximum inhibitory weight magnitude
    constexpr float OTHER_WEIGHT_MIN = -0.1f;            // Minimum other synapse weight
    constexpr float OTHER_WEIGHT_MAX = 0.1f;             // Maximum other synapse weight
    
    // Synaptic delay bounds
    constexpr size_t MIN_DELAY = 1;                      // Minimum synaptic delay (ms)
    constexpr size_t MAX_DELAY = 5;                      // Maximum synaptic delay (ms)
    
    // Short-term plasticity initial state
    constexpr float INITIAL_DEPRESSION = 1.0f;           // Fully recovered from depression
    constexpr float INITIAL_FACILITATION = 0.0f;         // No initial facilitation
}

// ==================== BRAIN CONSTANTS ====================

namespace BrainConstants {
    // Simulation defaults
    constexpr size_t DEFAULT_REGION_COUNT = 1;            // Number of brain regions
    constexpr size_t DEFAULT_NEURON_COUNT = 1000;         // Total neurons in brain
    constexpr size_t DEFAULT_POPULATION_GROWTH_STEPS = 100; // Steps for population growth
    
    // Memory system defaults
    constexpr size_t DEFAULT_WORKING_MEMORY_CAPACITY = 100;     // Working memory slots
    constexpr size_t DEFAULT_EPISODIC_MEMORY_MAX_EPISODES = 1000; // Max episodes to store
    constexpr size_t DEFAULT_ASSOCIATIVE_MEMORY_SIZE = 5000;    // Associative memory entries
    
    // Cognition system defaults
    constexpr size_t DEFAULT_PLANNING_DEPTH = 5;         // Maximum planning steps
    constexpr float DEFAULT_ATTENTION_INHIBITION_STRENGTH = 0.5f; // Attention inhibition
    constexpr float DEFAULT_ATTENTION_EXCITATION_STRENGTH = 1.5f; // Attention excitation
    
    // Neuromodulation defaults
    constexpr float DEFAULT_NOVELTY_THRESHOLD = 0.5f;      // Novelty detection threshold
    constexpr float DEFAULT_CURIOUSITY_STRENGTH = 0.1f;    // Curiosity modulation strength
    constexpr float DEFAULT_PREDICTION_ERROR_GAIN = 0.1f; // Prediction error learning rate
    constexpr float DEFAULT_DOPAMINE_RELEASE_RATE = 0.05f;  // Dopamine release rate
    constexpr float DEFAULT_DOPAMINE_DECAY_RATE = 0.02f;    // Dopamine decay rate
    
    // Plasticity defaults
    constexpr float DEFAULT_STDP_LTP_WEIGHT = 0.01f;      // STDP LTP weight change
    constexpr float DEFAULT_STDP_LTD_WEIGHT = 0.012f;     // STDP LTD weight change
    constexpr float DEFAULT_STDP_TAU = 20.0f;             // STDP time constant
    constexpr float DEFAULT_SYNAPTIC_GROWTH_RATE = 0.0001f; // Synaptogenesis rate
    constexpr float DEFAULT_SYNAPTIC_PRUNING_RATE = 0.00001f; // Pruning rate
    constexpr float DEFAULT_HEBBIAN_LEARNING_RATE = 0.001f; // Hebbian learning rate
    
    // Checkpoint defaults
    constexpr size_t DEFAULT_MAX_CHECKPOINTS = 10000;     // Maximum checkpoints to keep
    constexpr size_t DEFAULT_CHECKPOINT_INTERVAL = 5000; // Checkpoint frequency
    constexpr const char* DEFAULT_CHECKPOINT_DIR = "./checkpoints"; // Checkpoint directory
    
    // Performance limits
    constexpr size_t MAX_REGIONS = 100;                   // Maximum number of regions
    constexpr size_t MAX_NEURONS_PER_REGION = 10000;      // Maximum neurons per region
    constexpr size_t MAX_SYNAPSES_PER_REGION = 100000;    // Maximum synapses per region
    constexpr size_t MAX_POPULATIONS_PER_REGION = 20;     // Maximum populations per region
    
    // Input/output defaults
    constexpr float DEFAULT_INPUT_CURRENT_AMPLITUDE = 50.0f; // Default sensory input current
    constexpr float DEFAULT_PLASTICITY_INPUT_CURRENT = 30.0f; // Default plasticity input current
    constexpr float DEFAULT_STDP_CURRENT_AMPLITUDE = 60.0f;   // Default STDP activation current
    
    // Test experiment defaults
    constexpr size_t DEFAULT_TEST_REGION_ID = 1;         // Default test region
    constexpr size_t DEFAULT_TEST_NEURONS_COUNT = 10;     // Neurons to test
    constexpr size_t DEFAULT_TEST_STEPS = 50;            // Test simulation steps
    constexpr size_t DEFAULT_PLASTICITY_STEPS = 1000;     // Plasticity experiment steps
    constexpr size_t DEFAULT_PLASTICITY_LOG_INTERVAL = 100; // Logging interval
    constexpr size_t DEFAULT_STDP_TRIALS = 50;            // STDP test trials
    constexpr size_t DEFAULT_STDP_TEST_SYNAPSES = 5;     // Synapses to test STDP on
    
    // Memory and learning defaults
    constexpr float DEFAULT_WEIGHT_CHANGE_THRESHOLD = 0.01f; // Significant weight change
    constexpr float DEFAULT_LEARNING_THRESHOLD = 0.001f;     // Learning detection threshold
    constexpr float DEFAULT_STDP_SIGNIFICANCE_THRESHOLD = 0.001f; // STDP significance
    
    // Reward and motivation defaults
    constexpr float DEFAULT_REWARD_SCALE = 1.0f;           // Reward value scaling
    constexpr float DEFAULT_REINFORCEMENT_STRENGTH = 0.1f; // Reinforcement learning rate
    constexpr float DEFAULT_PENALTY_STRENGTH = 0.05f;      // Penalty learning rate
}

// ==================== DEVELOPMENT CONSTANTS ====================

namespace DevelopmentConstants {
    // Synaptogenesis (development of new synapses)
    constexpr float DEFAULT_SYNAPTAGENESIS_RATE = 0.0001f; // Rate of new synapse formation
    constexpr float DEFAULT_SYNAPSE_MATURATION_TIME = 100.0f; // Time for new synapses to mature
    constexpr size_t DEFAULT_MIN_SYNAPSES_PER_NEURON = 5;    // Minimum synapses per neuron
    constexpr size_t DEFAULT_MAX_SYNAPSES_PER_NEURON = 1000; // Maximum synapses per neuron
    
    // Pruning (elimination of synapses)
    constexpr float DEFAULT_PRUNING_RATE = 0.00001f;      // Rate of synapse elimination
    constexpr float DEFAULT_PRUNING_THRESHOLD = 0.01f;     // Weight threshold for pruning
    
    // Maturation (synaptic strengthening)
    constexpr float DEFAULT_MATURATION_RATE = 0.001f;      // Rate of synaptic strengthening
    constexpr float DEFAULT_MATURATION_THRESHOLD = 0.1f;   // Strength threshold for maturation
    
    // Competition (synapse selection)
    constexpr float DEFAULT_COMPETITION_STRENGTH = 0.3f;   // Strength of synaptic competition
    constexpr float DEFAULT_SUCCESS_THRESHOLD = 0.5f;     // Success threshold for competition
}

// ==================== MEMORY CONSTANTS ====================

namespace MemoryConstants {
    // Working memory
    constexpr size_t DEFAULT_WORKING_MEMORY_SLOTS = 50;    // Number of working memory slots
    constexpr float DEFAULT_FORGETTING_RATE = 0.01f;        // Rate of memory decay
    constexpr float DEFAULT_CONSOLIDATION_THRESHOLD = 0.8f; // Consolidation threshold
    constexpr size_t DEFAULT_MAX_ACTIVE_EPISODES = 10;      // Maximum active episodes
    
    // Episodic memory
    constexpr size_t DEFAULT_EPISODIC_MEMORY_MAX_ENTRIES = 1000; // Max episodic memory entries
    constexpr float DEFAULT_EPISODIC_RECALL_PROBABILITY = 0.7f;   // Recall probability
    constexpr float DEFAULT_EPISODIC_CONSOLIDATION_STRENGTH = 0.5f; // Consolidation strength
    
    // Associative memory
    constexpr size_t DEFAULT_ASSOCIATIVE_MEMORY_SIZE = 5000;    // Associative memory capacity
    constexpr float DEFAULT_ASSOCIATIVE_STRENGTH_THRESHOLD = 0.3f; // Threshold for association
    constexpr float DEFAULT_ASSOCIATIVE_DECAY_RATE = 0.001f;     // Association decay rate
    
    // Semantic memory (concept formation)
    constexpr size_t DEFAULT_SEMANTIC_MEMORY_MAX_CONCEPTS = 100; // Maximum concepts
    constexpr float DEFAULT_CONCEPT_FORMATION_THRESHOLD = 0.6f;   // Concept formation threshold
    constexpr float DEFAULT_CONCEPT_GENERALIZATION_STRENGTH = 0.3f; // Concept generalization
}

// ==================== COGNITION CONSTANTS ====================

namespace CognitionConstants {
    // Planning
    constexpr size_t DEFAULT_MAX_PLANNING_STEPS = 10;     // Maximum planning steps
    constexpr float DEFAULT_PLANNING_COST = 0.1f;         // Planning cost factor
    constexpr float DEFAULT_PLANNING_REWARD_WEIGHT = 1.0f; // Reward weighting in planning
    
    // Attention
    constexpr size_t DEFAULT_ATTENTION_POOL_SIZE = 100;    // Attention selection pool size
    constexpr float DEFAULT_ATTENTION_INHIBITION = 0.5f;   // Attention inhibition strength
    constexpr float DEFAULT_ATTENTION_EXCITATION = 1.5f;   // Attention excitation strength
    constexpr float DEFAULT_ATTENTION_DECAY = 0.01f;       // Attention decay rate
    
    // Concept formation
    constexpr size_t DEFAULT_CONCEPT_POOL_SIZE = 50;       // Concept formation pool size
    constexpr float DEFAULT_CONCEPT_ACTIVATION_THRESHOLD = 0.7f; // Concept activation threshold
    constexpr float DEFAULT_CONCEPT_STABILITY = 0.9f;      // Concept stability
    constexpr float DEFAULT_CONCEPT_GENERALIZATION = 0.3f;  // Concept generalization strength
    
    // Prediction
    constexpr float DEFAULT_PREDICTION_ERROR_THRESHOLD = 0.1f; // Prediction error threshold
    constexpr float DEFAULT_PREDICTION_CONFIDENCE_THRESHOLD = 0.8f; // Prediction confidence
    constexpr size_t DEFAULT_PREDICTION_HISTORY_SIZE = 100; // Prediction history size
}

// ==================== NEUROMODULATION CONSTANTS ====================

namespace NeuromodulationConstants {
    // Dopamine system
    constexpr float DEFAULT_DOPAMINE_BASELINE = 0.1f;      // Baseline dopamine level
    constexpr float DEFAULT_DOPAMINE_PEAK = 1.0f;          // Peak dopamine level
    constexpr float DEFAULT_DOPAMINE_DECAY = 0.02f;        // Dopamine decay rate
    constexpr float DEFAULT_DOPAMINE_RECEPTOR_SENSITIVITY = 1.0f; // Dopamine receptor sensitivity
    
    // Curiosity system
    constexpr float DEFAULT_CURIOUSITY_THRESHOLD = 0.5f;   // Curiosity activation threshold
    constexpr float DEFAULT_CURIOUSITY_GROWTH_RATE = 0.01f; // Curiosity growth rate
    constexpr float DEFAULT_CURIOUSITY_DECAY_RATE = 0.005f; // Curiosity decay rate
    constexpr float DEFAULT_CURIOUSITY_REWARD_WEIGHT = 0.3f; // Curiosity reward weighting
    
    // Novelty detection
    constexpr float DEFAULT_NOVELTY_SENSITIVITY = 0.8f;    // Novelty detection sensitivity
    constexpr float DEFAULT_NOVELTY_THRESHOLD = 0.5f;      // Novelty threshold
    constexpr float DEFAULT_NOVELTY_DECAY = 0.01f;         // Novelty decay rate
    
    // Reward prediction error
    constexpr float DEFAULT_RPE_LEARNING_RATE = 0.1f;      // RPE learning rate
    constexpr float DEFAULT_RPE_DECAY = 0.01f;             // RPE decay rate
    constexpr float DEFAULT_RPE_MIN_VALUE = -1.0f;        // Minimum RPE value
    constexpr float DEFAULT_RPE_MAX_VALUE = 1.0f;         // Maximum RPE value
}

// ==================== PERFORMANCE CONSTANTS ====================

namespace PerformanceConstants {
    // Memory management
    constexpr size_t DEFAULT_MEMORY_POOL_SIZE = 1024;     // Memory pool size
    constexpr size_t DEFAULT_EVENT_QUEUE_SIZE = 10000;    // Event queue size
    constexpr size_t DEFAULT_PERF_MON_TIME_WINDOW = 1000;  // Performance monitoring time window
    
    // I/O operations
    constexpr size_t DEFAULT_LOG_BUFFER_SIZE = 4096;      // Log buffer size
    constexpr size_t DEFAULT_MAX_LOG_LINES = 100000;      // Maximum log lines to keep
    constexpr const char* DEFAULT_LOG_FILE = "./nlm.log"; // Default log file
    
    // Parallel processing
    constexpr size_t DEFAULT_MAX_THREADS = 8;             // Maximum parallel threads
    constexpr float DEFAULT_PARALLEL_EFFICIENCY = 0.8f;   // Expected parallel efficiency
    
    // Visualization
    constexpr int DEFAULT_SCREEN_WIDTH = 1920;            // Default screen width
    constexpr int DEFAULT_SCREEN_HEIGHT = 1080;           // Default screen height
    constexpr float DEFAULT_WINDOW_ASPECT_RATIO = 16.0f / 9.0f; // Default window aspect ratio
    constexpr size_t DEFAULT_FPS_LIMIT = 60;              // Default frame rate limit
}

// ==================== EXPERIMENT CONSTANTS ====================

namespace ExperimentConstants {
    // Experiment phases
    constexpr const char* PHASE1_DESCRIPTION = "Basic neural computation"; // Phase 1 description
    constexpr const char* PHASE2_DESCRIPTION = "Real neural computation with plasticity"; // Phase 2 description
    constexpr const char* PHASE3_DESCRIPTION = "World interaction with agents"; // Phase 3 description
    constexpr const char* PHASE4_DESCRIPTION = "Emerging cognition and memory"; // Phase 4 description
    constexpr const char* PHASE5_DESCRIPTION = "Performance optimizations"; // Phase 5 description
    constexpr const char* PHASE6_DESCRIPTION = "Full integration and deployment"; // Phase 6 description
    
    // Experiment parameters
    constexpr size_t DEFAULT_EXPERIMENT_RUNS = 5;         // Default number of experiment runs
    constexpr float DEFAULT_EXPERIMENT_TIMEOUT = 3600.0f; // Default experiment timeout (seconds)
    constexpr size_t DEFAULT_EXPERIMENT_MAX_STEPS = 10000; // Maximum experiment steps
    constexpr float DEFAULT_EXPERIMENT_SUCCESS_THRESHOLD = 0.7f; // Success threshold
    
    // Metrics and statistics
    constexpr size_t DEFAULT_METRIC_HISTORY_SIZE = 1000;   // Metric history size
    constexpr float DEFAULT_METRIC_SMOOTHING_FACTOR = 0.1f; // Metric smoothing factor
    constexpr size_t DEFAULT_METRIC_SAMPLE_INTERVAL = 10;   // Metric sampling interval
}

// ==================== WORLD CONSTANTS ====================

namespace WorldConstants {
    // Environment dimensions
    constexpr float DEFAULT_WORLD_WIDTH = 100.0f;         // World width
    constexpr float DEFAULT_WORLD_HEIGHT = 100.0f;        // World height
    constexpr float DEFAULT_WORLD_DEPTH = 100.0f;         // World depth
    
    // Physics simulation
    constexpr float DEFAULT_GRAVITY = 9.8f;               // Gravity acceleration
    constexpr float DEFAULT_FRICTION = 0.5f;              // Surface friction
    constexpr float DEFAULT_ELASTICITY = 0.7f;            // Collision elasticity
    constexpr float DEFAULT_MAX_VELOCITY = 10.0f;         // Maximum agent velocity
    
    // Resources and hazards
    constexpr float DEFAULT_RESOURCE_VALUE = 10.0f;      // Resource value
    constexpr float DEFAULT_HAZARD_DAMAGE = 5.0f;         // Hazard damage amount
    constexpr float DEFAULT_REWARD_DISTANCE = 5.0f;       // Reward distance threshold
    
    // Agent behavior
    constexpr float DEFAULT_MAX_ENERGY = 100.0f;          // Maximum agent energy
    constexpr float DEFAULT_ENERGY_CONSUMPTION = 0.1f;    // Energy consumption rate
    constexpr float DEFAULT_ENERGY_REGEN_RATE = 0.05f;    // Energy regeneration rate
    constexpr float DEFAULT_SENSORY_RANGE = 20.0f;        // Sensory detection range
}

// ==================== UTILITY CONSTANTS ====================

namespace UtilityConstants {
    // String and text
    constexpr size_t MAX_STRING_LENGTH = 1024;            // Maximum string length
    constexpr size_t MAX_LOG_MESSAGE_LENGTH = 2048;       // Maximum log message length
    constexpr size_t MAX_PATH_LENGTH = 512;               // Maximum file path length
    constexpr size_t MAX_FILENAME_LENGTH = 256;           // Maximum filename length
    
    // Mathematical constants
    constexpr float PI = 3.141592653589793f;               // Mathematical constant
    constexpr float E = 2.718281828459045f;                // Euler's number
    constexpr float EPSILON = 1e-6f;                      // Small value for comparisons
    constexpr float GOLDEN_RATIO = 1.618033988749895f;     // Golden ratio
    
    // Numerical limits
    constexpr float FLOAT_EPSILON = 1e-6f;               // Float precision epsilon
    constexpr float DOUBLE_EPSILON = 1e-15f;             // Double precision epsilon
    constexpr size_t SIZE_T_MAX = std::numeric_limits<size_t>::max(); // Maximum size_t value
    
    // Time constants
    constexpr Timestamp SECOND_TO_MILLISECOND = 1000.0;  // Milliseconds per second
    constexpr Timestamp MILLISECOND_TO_SECOND = 0.001;    // Seconds per millisecond
    constexpr Timestamp MINUTE_TO_SECOND = 60.0;          // Seconds per minute
    constexpr Timestamp HOUR_TO_SECOND = 3600.0;          // Seconds per hour
    
    // File system
    constexpr const char* DIRECTORY_SEPARATOR = "/";     // Directory separator
    constexpr const char* PATH_SEPARATOR = "/";          // Path separator
    constexpr const char* HOME_DIRECTORY = "~/";          // Home directory
    
    // Color codes (for terminal output)
    constexpr const char* COLOR_RED = "\033[31m";        // Red text
    constexpr const char* COLOR_GREEN = "\033[32m";      // Green text
    constexpr const char* COLOR_YELLOW = "\033[33m";     // Yellow text
    constexpr const char* COLOR_BLUE = "\033[34m";       // Blue text
    constexpr const char* COLOR_MAGENTA = "\033[35m";    // Magenta text
    constexpr const char* COLOR_CYAN = "\033[36m";       // Cyan text
    constexpr const char* COLOR_RESET = "\033[0m";        // Reset formatting
    constexpr const char* COLOR_BOLD = "\033[1m";        // Bold text
    constexpr const char* COLOR_ITALIC = "\033[3m";      // Italic text
    constexpr const char* COLOR_UNDERLINE = "\033[4m";   // Underlined text
}

// ==================== TYPE ALIASES FOR CONSTANTS ====================

// Convenience type aliases for common constant patterns
using Timestamp_ms = std::chrono::duration<double, std::milli>;
using Timestamp_s = std::chrono::duration<double>;
using Weight_delta = float;
using Current_input = float;
using Potential_mV = float;
using Conductance_nS = float;
using TimeConstant_ms = float;
using Delay_steps = size_t;

} // namespace nlm

// ==================== BACKWARD COMPATIBILITY MACROS ====================

// For backward compatibility with existing code, provide macros that map to the new constants
// These can be gradually replaced with direct constant usage

#define OLD_MEMBRANE_CAPACITANCE nlm::NeuronConstants::MEMBRANE_CAPACITANCE
#define OLD_TIME_CONSTANT nlm::NeuronConstants::TIME_CONSTANT
#define OLD_RESTING_POTENTIAL nlm::NeuronConstants::RESTING_POTENTIAL
#define OLD_DEFAULT_THRESHOLD nlm::NeuronConstants::DEFAULT_THRESHOLD
#define OLD_MIN_WEIGHT nlm::SynapseConstants::MIN_WEIGHT
#define OLD_MAX_WEIGHT nlm::SynapseConstants::MAX_WEIGHT
#define OLD_STDP_LTP_WEIGHT nlm::BrainConstants::DEFAULT_STDP_LTP_WEIGHT
#define OLD_STDP_LTD_WEIGHT nlm::BrainConstants::DEFAULT_STDP_LTD_WEIGHT
#define OLD_DEFAULT_NEURON_COUNT nlm::BrainConstants::DEFAULT_NEURON_COUNT
#define OLD_DEFAULT_REGION_COUNT nlm::BrainConstants::DEFAULT_REGION_COUNT
#define OLD_DEFAULT_CHECKPOINT_DIR nlm::BrainConstants::DEFAULT_CHECKPOINT_DIR

// Include the updated configuration headers
#include "Config.hpp"

// Utility function to get constants from config with fallback to defaults
namespace nlm {
    template<typename T>
    T getConfigWithDefault(const std::string& key, const T& defaultValue) {
        // This would need to be implemented based on the specific Config system
        // For now, this is a placeholder
        return defaultValue;
    }
}

} // namespace nlm