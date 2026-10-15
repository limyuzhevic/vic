// NLM (熙然) - Neural Learning Machine
// Constants and configuration values
// This file centralizes all magic numbers and configuration constants
// to improve code readability and maintainability.

#pragma once

#include <cstddef>
#include <string>

namespace nlm {

// Neuron parameters
namespace NeuronConstants {
    // Membrane capacitance (nF)
    constexpr float MEMBRANE_CAPACITANCE = 1.0f;
    
    // Membrane time constant (ms)
    constexpr float TIME_CONSTANT = 20.0f;
    
    // Maximum spike history entries
    constexpr size_t MAX_SPIKE_HISTORY = 100;
    
    // Default membrane potential values (mV)
    constexpr float DEFAULT_MEMBRANE_POTENTIAL = -70.0f;
    constexpr float DEFAULT_RESTING_POTENTIAL = -70.0f;
    constexpr float DEFAULT_THRESHOLD = -55.0f;
    constexpr float DEFAULT_RESET_POTENTIAL = -70.0f;
    constexpr float DEFAULT_LEAK_CONDUCTANCE = 10.0f;
    
    // Refractory period range (steps)
    constexpr uint32_t MIN_REFACTORY_PERIOD = 2;
    constexpr uint32_t MAX_REFACTORY_PERIOD = 10;
    
    // Adaptation variable scaling
    constexpr float ADAPTATION_DECAY_FACTOR = 0.95f;
    constexpr float ADAPTATION_INCREMENT = 1.0f;
    
    // Membrane potential clamping
    constexpr float MIN_MEMBRANE_POTENTIAL = -100.0f;
    constexpr float MAX_MEMBRANE_POTENTIAL = 50.0f;
}

// Synapse parameters
namespace SynapseConstants {
    // Synaptic weight ranges
    constexpr float MIN_WEIGHT = -1.0f;
    constexpr float MAX_WEIGHT = 1.0f;
    
    // STDP parameters
    constexpr float DEFAULT_STDP_LTP_WEIGHT = 0.01f;
    constexpr float DEFAULT_STDP_LTD_WEIGHT = 0.012f;
    constexpr float DEFAULT_STDP_TAU = 20.0f;
    
    // Structural plasticity rates
    constexpr float DEFAULT_SYNAPTOGENESIS_RATE = 0.0001f;
    constexpr float DEFAULT_PRUNING_RATE = 0.00001f;
    
    // Synaptic conductance
    constexpr float DEFAULT_SYNAPSE_CONDUCTANCE = 0.0f;
}

// Plasticity parameters
namespace PlasticityConstants {
    // Hebbian learning
    constexpr float HEBBIAN_LEARNING_RATE = 0.001f;
    
    // STDP time windows
    constexpr float PRE_POST_TIME_WINDOW = 20.0f;
    constexpr float POST_PRE_TIME_WINDOW = 20.0f;
    
    // Reward modulation factor
    constexpr float REWARD_MODULATION_FACTOR = 0.5f;
}

// Memory system constants
namespace MemoryConstants {
    // Working memory
    constexpr size_t DEFAULT_WORKING_MEMORY_CAPACITY = 100;
    constexpr float WORKING_MEMORY_DECAY_RATE = 0.01f;
    constexpr float MEMORY_TRACE_MAX_AGE = 1000;
    constexpr float MEMORY_ACTIVATION_THRESHOLD = 0.1f;
    
    // Episodic memory
    constexpr size_t DEFAULT_MAX_EPISODES = 1000;
    constexpr float EPISODE_CONSOLIDATION_THRESHOLD = 0.3f;
    
    // Memory competition
    constexpr float MEMORY_COMPETITION_THRESHOLD = 0.3f;
    constexpr float WINNER_EXCITATION_STRENGTH = 1.5f;
    constexpr float LOSER_INHIBITION_STRENGTH = 3.0f;
}

// Neuromodulation constants
namespace NeuromodulationConstants {
    // Dopamine dynamics
    constexpr float DOPAMINE_DECAY_RATE = 0.1f;
    constexpr float DOPAMINE_PEAK_LEVEL = 1.0f;
    constexpr float DOPAMINE_EXCITABILITY_GAIN = 0.5f;
    
    // Curiosity/dissatisfaction
    constexpr float CURIOSITY_THRESHOLD = 0.5f;
    constexpr float NOVELTY_DETECTION_SENSITIVITY = 0.1f;
    
    // Prediction error
    constexpr float PREDICTION_ERROR_DECAY = 0.05f;
}

// Cognitive constants
namespace CognitiveConstants {
    // Planning
    constexpr size_t DEFAULT_PLANNING_DEPTH = 5;
    
    // Attention
    constexpr float DEFAULT_INHIBITION_STRENGTH = 0.5f;
    constexpr float DEFAULT_EXCITATION_STRENGTH = 1.5f;
    constexpr float DEFAULT_COMPETITION_THRESHOLD = 0.3f;
    
    // Concept formation
    constexpr float CONCEPT_FORMATION_LEARNING_RATE = 0.01f;
    constexpr size_t CONCEPT_FORMATION_PATTERN_SIZE = 10;
}

// Development constants
namespace DevelopmentConstants {
    // Developmental stages
    constexpr size_t CRITICAL_PERIOD_MULTIPLIER = 2;
    constexpr size_t MATURATION_MULTIPLIER = 4;
    constexpr size_t ADULT_STABILITY_MULTIPLIER = 10;
    
    // Plasticity modulation by development
    constexpr float PLASTICITY_INITIAL = 1.0f;      // High plasticity
    constexpr float PLASTICITY_CRITICAL = 0.8f;
    constexpr float PLASTICITY_MATURE = 0.5f;
    constexpr float PLASTICITY_ADULT = 0.2f;        // Low plasticity
}

// Simulation constants
namespace SimulationConstants {
    // Time management
    constexpr double DEFAULT_TIMESTEP = 0.001;     // seconds
    constexpr size_t DEFAULT_REPLAY_INTERVAL = 100;   // steps
    constexpr size_t DEFAULT_CONSOLIDATION_INTERVAL = 1000; // steps
    constexpr size_t DEVELOPMENT_UPDATE_INTERVAL = 1000; // steps
    constexpr size_t STRUCTURAL_PLACEMENT_INTERVAL = 100;   // steps
    
    // Input/output scaling
    constexpr float SENSORY_INPUT_SCALE = 10.0f;     // mV
    constexpr float SENSORY_INPUT_NORMALIZATION = 1.0f / 10.0f; // [-1, 1] -> [-10, 10] mV
    
    // Statistics and monitoring
    constexpr size_t STAT_HISTORY_SIZE = 100;
    constexpr double STAT_COLLECTION_INTERVAL = 0.1; // seconds
}

// Logging constants
namespace LogConstants {
    // Message formatting
    constexpr size_t MAX_LOG_MESSAGE_LENGTH = 1024;
    constexpr size_t MAX_LOG_SOURCE_LENGTH = 256;
    
    // Performance logging
    constexpr size_t PERFORMANCE_LOG_INTERVAL = 1000; // steps
    constexpr double PERFORMANCE_LOG_TIME_INTERVAL = 10.0; // seconds
}

// Configuration constants
namespace ConfigConstants {
    // File and path
    constexpr const char* DEFAULT_CONFIG_FILE = "configs/default.cfg";
    constexpr const char* DEFAULT_CHECKPOINT_DIR = "./checkpoints";
    constexpr size_t DEFAULT_CHECKPOINT_INTERVAL = 10000;
    constexpr size_t DEFAULT_MAX_CHECKPOINTS = 5;
    
    // Randomization
    constexpr uint64_t DEFAULT_RANDOM_SEED = 42;
    
    // Network topology
    constexpr size_t DEFAULT_NEURON_COUNT = 1000;
    constexpr size_t DEFAULT_REGION_COUNT = 1;
    constexpr float DEFAULT_CONNECTION_PROBABILITY = 0.1f;
    
    // Performance
    constexpr size_t BENCHMARK_STEPS_SMALL = 100;
    constexpr size_t BENCHMARK_STEPS_MEDIUM = 500;
    constexpr size_t BENCHMARK_STEPS_LARGE = 1000;
}

// Visualization constants
namespace VisualizationConstants {
    // Display parameters
    constexpr size_t DEFAULT_WINDOW_WIDTH = 1024;
    constexpr size_t DEFAULT_WINDOW_HEIGHT = 768;
    constexpr double DEFAULT_REFRESH_RATE = 60.0; // Hz
    
    // Neuron visualization
    constexpr float NEURON_RADIUS_ACTIVE = 5.0f;
    constexpr float NEURON_RADIUS_INACTIVE = 3.0f;
    constexpr float NEURON_RADIUS_REFACTORY = 4.0f;
    
    // Synapse visualization
    constexpr float SYNAPSE_WIDTH_ACTIVE = 2.0f;
    constexpr float SYNAPSE_WIDTH_INACTIVE = 1.0f;
}

// Error handling constants
namespace ErrorConstants {
    // Result status codes
    constexpr int SUCCESS = 0;
    constexpr int FAILURE = 1;
    constexpr int INVALID_ARGUMENT = 2;
    constexpr int OUT_OF_MEMORY = 3;
    constexpr int FILE_NOT_FOUND = 4;
    constexpr int VALIDATION_FAILED = 5;
    
    // Tolerance values
    constexpr float FLOAT_EQUALITY_EPSILON = 0.001f;
    constexpr double DOUBLE_EQUALITY_EPSILON = 1e-9;
    constexpr float WEIGHT_DELTA_THRESHOLD = 0.01f;
}

// Threading constants
namespace ThreadingConstants {
    // Synchronization
    constexpr size_t DEFAULT_THREAD_POOL_SIZE = 4;
    constexpr double THREAD_SYNC_TIMEOUT = 5.0; // seconds
    constexpr size_t TASK_QUEUE_SIZE = 1000;
    
    // Lock contention
    constexpr size_t LOCK_RETRY_ATTEMPTS = 10;
    constexpr double LOCK_RETRY_DELAY = 0.001; // seconds
}

// Memory management constants
namespace MemoryConstants {
    // Pool allocation
    constexpr size_t MEMORY_POOL_ALIGNMENT = 64;     // bytes (cache line)
    constexpr size_t NEURON_POOL_BLOCK_SIZE = 1024;
    constexpr size_t SYNAPSE_POOL_BLOCK_SIZE = 2048;
    
    // Garbage collection
    constexpr size_t GC_COLLECTION_INTERVAL = 10000;
    constexpr float GC_STRESS_THRESHOLD = 0.8f;     // 80% memory usage
}

} // namespace nlm
