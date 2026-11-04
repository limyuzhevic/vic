// Configuration Constants for NLM (Neural Learning Machine)
// This file contains all magic numbers used in the NLM configuration system
// to improve maintainability, readability, and consistency.

#pragma once

namespace nlm {

// Default configuration values - used when no explicit value is provided
namespace Defaults {
    constexpr int64_t RANDOM_SEED = 42;                           // Default random seed for reproducible experiments
    constexpr double SIMULATION_TIMESTEP = 0.001;                 // Simulation timestep in seconds (1ms)
    constexpr int64_t NEURON_COUNT = 500;                          // Default number of neurons in the brain
    constexpr int64_t REGION_COUNT = 1;                           // Default number of brain regions
    constexpr double CONNECTION_PROBABILITY = 0.15f;              // Probability of forming connections between neurons
    constexpr double PLASTICITY_LEARNING_RATE = 0.01f;             // Default learning rate for plasticity
    constexpr double STDP_LTP_WEIGHT = 0.01f;                      // STDP Long-Term Potentiation weight
    constexpr double STDP_LTD_WEIGHT = 0.012f;                     // STDP Long-Term Depression weight
    constexpr double STDP_TIME_CONSTANT = 20.0f;                   // STDP time constant (tau) in ms
    constexpr double SYNAPTOGENESIS_RATE = 0.0001f;               // Rate of new synapse formation
    constexpr double PRUNING_RATE = 0.00001f;                     // Rate of synapse removal
    constexpr double DOPAMINE_BASELINE = 0.1f;                     // Baseline dopamine level
    constexpr double REWARD_DISCOUNT_FACTOR = 0.99f;              // Discount factor for reward prediction
    constexpr int64_t ENVIRONMENT_WIDTH = 10;                     // Grid world width
    constexpr int64_t ENVIRONMENT_HEIGHT = 10;                    // Grid world height
    constexpr int64_t MAX_SIMULATION_STEPS = 10000;               // Maximum simulation steps
    constexpr double SIMULATION_TIME_LIMIT = 0.0;                 // No time limit by default
    constexpr double VISUALIZATION_UPDATE_RATE = 30.0;            // Frame rate for visualization
    constexpr double SENSORY_NOVELTY_DECAY = 0.99f;               // Decay rate for novelty detection
    constexpr float REFLEX_BONUS_THRESHOLD = 0.5f;                // Threshold for vision-guided reflex bonus
    constexpr float HAZARD_SUPPRESSION_THRESHOLD = 0.35f;          // Threshold for hazard suppression
    constexpr float MAX_FORWARD_TOUCH = 1.0f;                     // Maximum touch signal (normalized)
    constexpr float FORWARD_OBJECT_VISION_THRESHOLD = 1.0f;       // Vision threshold for forward objects
    constexpr int64_t MAX_REPLAY_INTERVAL = 100;                  // Replay every 100 steps
    constexpr int64_t MAX_CONSOLIDATION_INTERVAL = 1000;          // Consolidate every 1000 steps
    constexpr float LEARNED_PREFERENCE_INITIAL = 0.0f;            // Initial learned preference trait
    constexpr float EPSILON = 0.001f;                             // Small value for floating-point comparisons
    constexpr float MAX_WEIGHT_CHANGE = 1.0f;                     // Maximum synaptic weight change
    constexpr int64_t MAX_CONCEPT_COUNT = 100;                    // Maximum number of learned concepts
    constexpr float MAX_SELF_MODEL_CONFIDENCE = 1.0f;             // Maximum self-model confidence
    constexpr size_t MAX_TEXT_NEURONS = 31;                       // Number of dedicated text neurons
    constexpr size_t MAX_TEXT_PATTERN_SIZE = 4;                  // Maximum text pattern size (words)
}

// Validation bounds - used to validate configuration values
namespace Validation {
    constexpr int64_t MIN_RANDOM_SEED = 0;                        // Minimum valid random seed
    constexpr int64_t MAX_RANDOM_SEED = 2147483647;               // Maximum valid random seed
    
    constexpr double MIN_SIMULATION_TIMESTEP = 0.0001f;          // Minimum timestep (100 microseconds)
    constexpr double MAX_SIMULATION_TIMESTEP = 0.1f;             // Maximum timestep (100 ms)
    
    constexpr int64_t MIN_NEURON_COUNT = 10;                      // Minimum number of neurons
    constexpr int64_t MAX_NEURON_COUNT = 10000;                   // Maximum number of neurons
    
    constexpr int64_t MIN_REGION_COUNT = 1;                       // Minimum number of regions
    constexpr int64_t MAX_REGION_COUNT = 10;                      // Maximum number of regions
    
    constexpr double MIN_CONNECTION_PROBABILITY = 0.0f;          // Minimum connection probability
    constexpr double MAX_CONNECTION_PROBABILITY = 1.0f;          // Maximum connection probability
    
    constexpr double MIN_PLASTICITY_LEARNING_RATE = 0.0f;        // Minimum learning rate
    constexpr double MAX_PLASTICITY_LEARNING_RATE = 1.0f;        // Maximum learning rate
    
    constexpr double MIN_STDP_WEIGHT = 0.0f;                      // Minimum STDP weight
    constexpr double MAX_STDP_WEIGHT = 1.0f;                      // Maximum STDP weight
    
    constexpr double MIN_STDP_TIME_CONSTANT = 1.0f;               // Minimum STDP time constant (ms)
    constexpr double MAX_STDP_TIME_CONSTANT = 1000.0f;            // Maximum STDP time constant (ms)
    
    constexpr double MIN_SYNAPTOGENESIS_RATE = 0.0f;              // Minimum synaptogenesis rate
    constexpr double MAX_SYNAPTOGENESIS_RATE = 0.01f;             // Maximum synaptogenesis rate
    
    constexpr double MIN_PRUNING_RATE = 0.0f;                     // Minimum pruning rate
    constexpr double MAX_PRUNING_RATE = 0.001f;                   // Maximum pruning rate
    
    constexpr double MIN_DOPAMINE_BASELINE = -1.0f;               // Minimum dopamine baseline
    constexpr double MAX_DOPAMINE_BASELINE = 1.0f;                // Maximum dopamine baseline
    
    constexpr double MIN_REWARD_DISCOUNT_FACTOR = 0.0f;          // Minimum discount factor
    constexpr double MAX_REWARD_DISCOUNT_FACTOR = 1.0f;          // Maximum discount factor
    
    constexpr int64_t MIN_ENVIRONMENT_DIMENSION = 5;              // Minimum environment dimension
    constexpr int64_t MAX_ENVIRONMENT_DIMENSION = 100;           // Maximum environment dimension
    
    constexpr double MIN_VISUALIZATION_UPDATE_RATE = 1.0f;        // Minimum update rate (Hz)
    constexpr double MAX_VISUALIZATION_UPDATE_RATE = 1000.0f;     // Maximum update rate (Hz)
    
    constexpr float MIN_REFLEX_BONUS_THRESHOLD = 0.0f;           // Minimum reflex bonus threshold
    constexpr float MAX_REFLEX_BONUS_THRESHOLD = 1.0f;           // Maximum reflex bonus threshold
    
    constexpr float MIN_HAZARD_SUPPRESSION_THRESHOLD = 0.0f;      // Minimum hazard suppression threshold
    constexpr float MAX_HAZARD_SUPPRESSION_THRESHOLD = 1.0f;      // Maximum hazard suppression threshold
}

// Simulation constants - used for simulation control and timing
namespace Simulation {
    constexpr double EPSILON = 0.001f;                            // Small value for floating-point comparisons in simulation
    constexpr double NANOS_PER_SECOND = 1000000000.0f;            // Number of nanoseconds in a second
    constexpr double MILLIS_PER_SECOND = 1000.0f;                 // Number of milliseconds in a second
    constexpr double MICROSECONDS_PER_SECOND = 1000000.0f;        // Number of microseconds in a second
    
    constexpr int64_t TICK_SPREAD_LIMIT = 1000;                    // Maximum tick spread for event processing
    constexpr double TIME_TOLERANCE = 0.0001f;                    // Time tolerance for event scheduling
    
    constexpr size_t MAX_PENDING_SPIKE_EVENTS = 1000;             // Maximum pending spike events
    constexpr size_t MAX_DELAYED_SPIKE_EVENTS = 1000;            // Maximum delayed spike events
    constexpr double MAX_DELAY_SECONDS = 1.0f;                    // Maximum synaptic delay (1 second)
    
    constexpr double MIN_MEMBRANE_POTENTIAL = -100.0f;           // Minimum neuron membrane potential
    constexpr double MAX_MEMBRANE_POTENTIAL = 50.0f;              // Maximum neuron membrane potential
    constexpr double RESTING_POTENTIAL = -65.0f;                  // Resting membrane potential
    constexpr double THRESHOLD_POTENTIAL = -55.0f;                // Spike threshold potential
    
    constexpr double MIN_SYNAPTIC_WEIGHT = 0.0f;                  // Minimum synaptic weight
    constexpr double MAX_SYNAPTIC_WEIGHT = 2.0f;                  // Maximum synaptic weight
    constexpr double DEFAULT_SYNAPTIC_WEIGHT = 0.5f;              // Default synaptic weight
    
    constexpr double MIN_CURRENT_INJECTION = -100.0f;            // Minimum current injection
    constexpr double MAX_CURRENT_INJECTION = 100.0f;             // Maximum current injection
}

// Neural system constants - used for neural computation and connectivity
namespace Neural {
    constexpr size_t MIN_POPULATION_SIZE = 1;                    // Minimum neuron population size
    constexpr size_t MAX_POPULATION_SIZE = 1000;                 // Maximum neuron population size
    
    constexpr double NEURON_RESTING_TIME = 0.002f;                // Resting time before next spike (2ms)
    constexpr double REFRACTORY_PERIOD = 0.002f;                 // Refractory period after spike (2ms)
    constexpr double DELAY_CONSTANT = 0.001f;                    // Delay constant for synaptic transmission
    
    constexpr float MIN_SYNAPSES_PER_NEURON = 1.0f;               // Minimum synapses per neuron
    constexpr float MAX_SYNAPSES_PER_NEURON = 1000.0f;           // Maximum synapses per neuron
    
    constexpr double MIN_SPIKE_RATE = 0.0f;                       // Minimum firing rate (Hz)
    constexpr double MAX_SPIKE_RATE = 1000.0f;                    // Maximum firing rate (Hz)
    
    constexpr size_t MAX_NEURON_ID = 1000000;                    // Maximum neuron ID value
    constexpr size_t MAX_SYNAPSE_ID = 1000000;                   // Maximum synapse ID value
    constexpr size_t MAX_REGION_ID = 1000;                       // Maximum region ID value
    constexpr size_t MAX_POPULATION_ID = 1000;                   // Maximum population ID value
    
    constexpr float MIN_PLASTICITY_STRENGTH = 0.0f;               // Minimum plasticity strength
    constexpr float MAX_PLASTICITY_STRENGTH = 1.0f;               // Maximum plasticity strength
    
    constexpr double MIN_TEMPORAL_WINDOW = 0.001f;                // Minimum temporal window for plasticity
    constexpr double MAX_TEMPORAL_WINDOW = 100.0f;                // Maximum temporal window for plasticity (100ms)
}

// Memory system constants - used for memory allocation and management
namespace Memory {
    constexpr size_t DEFAULT_WORKING_MEMORY_SIZE = 1000;          // Default working memory capacity
    constexpr size_t DEFAULT_EPISODIC_MEMORY_SIZE = 10000;        // Default episodic memory capacity
    constexpr size_t DEFAULT_ASSOCIATIVE_MEMORY_SIZE = 1000;     // Default associative memory capacity
    
    constexpr double MEMORY_DECAY_RATE = 0.99f;                   // Memory decay rate per timestep
    constexpr double MEMORY_CONSOLIDATION_THRESHOLD = 0.8f;        // Memory consolidation threshold
    constexpr double EPISODIC_RECALL_PROBABILITY = 0.1f;           // Probability of episodic recall
    
    constexpr size_t MAX_CONCEPT_PATTERNS = 100;                  // Maximum number of concept patterns
    constexpr size_t MAX_ASSOCIATIVE_PATTERNS = 1000;             // Maximum number of associative patterns
    constexpr double PATTERN_VECTOR_DIMENSION = 16.0f;            // Dimension of concept and associative pattern vectors
}

// Performance and optimization constants
namespace Performance {
    constexpr size_t MAX_WORKER_THREADS = 8;                      // Maximum number of worker threads
    constexpr size_t MIN_QUEUE_SIZE = 100;                        // Minimum event queue size
    constexpr size_t MAX_QUEUE_SIZE = 10000;                      // Maximum event queue size
    
    constexpr double TARGET_FRAME_TIME = 0.0333f;                 // Target frame time for 30 FPS
    constexpr double MAX_FRAME_TIME = 0.1f;                      // Maximum frame time
    
    constexpr size_t CACHE_LINE_SIZE = 64;                        // Cache line size in bytes
    constexpr size_t VECTORIZATION_ALIGNMENT = 32;                // Vectorization alignment for SIMD
    
    constexpr double CPU_FREQUENCY_GHZ = 3.0f;                    // Assumed CPU frequency for performance calculations
    constexpr double MEMORY_BANDWIDTH_GBPS = 25.6f;               // Assumed memory bandwidth (GB/s)
}

// Educational and debugging constants
namespace Educational {
    constexpr const char* VERSION = "Phase 2.1 (Educational Edition)";  // NLM version string
    constexpr const char* AUTHOR = "NLM Authors";                   // Project authors
    constexpr const char* DESCRIPTION = "Neural Learning Machine - Educational Neural Simulation Framework";  // Project description
    constexpr const char* LICENSE = "MIT";                        // Software license
    
    constexpr const char* USAGE_HEADER = 
        "Usage: nlm [options]\n"
        "\n"
        "Options:\n"
        "  --config=file     Load configuration from file\n"
        "  --key=value       Set configuration value\n"
        "  --help            Show this help message\n"
        "  --version         Show version information\n"
        "\n"
        "For more information, visit: https://github.com/nlm-project/nlm\n";
    
    constexpr size_t MAX_LOG_MESSAGE_LENGTH = 1024;              // Maximum log message length
    constexpr size_t MAX_ERROR_MESSAGE_LENGTH = 2048;             // Maximum error message length
    constexpr size_t MAX_DEBUG_INFO_LENGTH = 4096;                // Maximum debug information length
}

} // namespace nlm