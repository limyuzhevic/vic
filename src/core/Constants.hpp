// NLM Project - Core Constants
// Centralized definition of constants and configuration parameters
// Improves code maintainability and reduces magic numbers

#pragma once

namespace nlm {
    namespace Constants {
        // Neuron physiology constants (mV, ms, Hz)
        constexpr float DEFAULT_RESTING_POTENTIAL = -70.0f;        // mV, typical neuron resting potential
        constexpr float DEFAULT_THRESHOLD = -55.0f;                // mV, typical firing threshold
        constexpr float DEFAULT_RESET_POTENTIAL = -70.0f;          // mV, after-spike reset value
        constexpr float DEFAULT_LEAK_CONDUCTANCE = 10.0f;          // nS, membrane leak conductance
        constexpr float DEFAULT_MEMBRANE_TIME_CONSTANT = 20.0f;    // ms, tau_m (membrane time constant)
        constexpr float DEFAULT_REFACTORY_PERIOD_MS = 2.0f;        // ms, absolute refractory period
        constexpr float DEFAULT_REFACTORY_STEPS = 5;               // simulation steps
        constexpr float DEFAULT_ADAPTATION_STRENGTH = 0.01f;       // spike-frequency adaptation

        // Synaptic constants
        constexpr float DEFAULT_SYNAPSE_WEIGHT = 1.0f;              // Normalized synaptic weight
        constexpr float DEFAULT_SYNAPSE_DELAY_STEPS = 1;            // Transmission delay in steps
        constexpr float DEFAULT_INITIAL_WEIGHT_MEAN = 0.5f;        // Mean for weight initialization
        constexpr float DEFAULT_INITIAL_WEIGHT_STD = 0.1f;         // Std dev for weight initialization
        constexpr float DEFAULT_EXCITATORY_WEIGHT_FACTOR = 1.0f;    // Excitatory synapse multiplier
        constexpr float DEFAULT_INHIBITORY_WEIGHT_FACTOR = -1.0f;   // Inhibitory synapse multiplier

        // Plasticity constants
        constexpr float DEFAULT_STDP_LTP_WEIGHT = 0.02f;           // STDP long-term potentiation
        constexpr float DEFAULT_STDP_LTD_WEIGHT = 0.015f;          // STDP long-term depression
        constexpr float DEFAULT_STDP_TIME_CONST = 20.0f;           // STDP time constant (ms)
        constexpr float DEFAULT_HEBBIAN_LEARNING_RATE = 0.001f;    // Hebbian learning rate
        constexpr float DEFAULT_STDP_MIN_WEIGHT = 0.01f;           // Minimum synaptic weight
        constexpr float DEFAULT_STDP_MAX_WEIGHT = 10.0f;           // Maximum synaptic weight
        constexpr float DEFAULT_PLASTICITY_THRESHOLD = 0.01f;      // Threshold for plastic changes

        // Structural plasticity constants
        constexpr float DEFAULT_SYNAPTOGENESIS_RATE = 0.0001f;     // New synapse formation rate
        constexpr float DEFAULT_PRUNING_RATE = 0.00001f;           // Synapse elimination rate
        constexpr float DEFAULT_MAX_SYNAPSE_DENSITY = 0.5f;        // Maximum connection probability

        // Neuromodulation constants
        constexpr float DEFAULT_DOPAMINE_SCALE = 1.0f;              // Dopamine modulation strength
        constexpr float DEFAULT_NOVITY_THRESHOLD = 0.5f;           // Novelty detection threshold
        constexpr float DEFAULT_CURVOSITY_SCALE = 0.1f;             // Curiosity exploration strength
        constexpr float DEFAULT_PREDICTION_ERROR_WEIGHT = 1.0f;     // Prediction error learning rate

        // Development constants
        constexpr float DEFAULT_DEVELOPMENT_TIMESTEP = 0.1f;       // Developmental update interval
        constexpr float CRITICAL_PERIOD_START = 0.0f;              // Start of critical period
        constexpr float CRITICAL_PERIOD_END = 10.0f;               // End of critical period
        constexpr float MATURATION_RATE = 0.05f;                   // Rate of adult development
        constexpr float AGING_START = 50.0f;                       // Start of aging process

        // Memory constants
        constexpr size_t DEFAULT_WORKING_MEMORY_SIZE = 100;        // Working memory capacity
        constexpr size_t DEFAULT_EPISODIC_MEMORY_CAPACITY = 1000;  // Episodic memory slots
        constexpr size_t DEFAULT_ASSOCIATIVE_MEMORY_SIZE = 500;   // Associative memory capacity
        constexpr float MEMORY_CONSOLIDATION_RATE = 0.1f;           // Memory consolidation strength
        constexpr size_t DEFAULT_SPIKE_HISTORY_LENGTH = 100;       // Recent spike history length

        // Prediction constants
        constexpr float DEFAULT_PREDICTION_DECAY = 0.99f;          // Prediction memory decay
        constexpr float DEFAULT_PREDICTION_ERROR_DECAY = 0.95f;    // Error signal decay
        constexpr size_t DEFAULT_PREDICTION_WINDOW = 50;           // Lookahead prediction steps
        constexpr float DEFAULT_PREDICTION_ACCURACY_THRESHOLD = 0.8f; // Success threshold

        // Cognitive constants
        constexpr float DEFAULT_ATTENTION_FOCUS_WIDTH = 0.3f;      // Attention selection width
        constexpr float DEFAULT_ATTENTION_INHIBITION = 0.1f;       // Lateral inhibition strength
        constexpr size_t DEFAULT_PLANNING_HORIZON = 20;            // Action planning depth
        constexpr float DEFAULT_PLANNING_COST_FACTOR = 0.01f;      // Planning cost sensitivity
        constexpr float DEFAULT_SELF_MODEL_STABILITY = 0.5f;       // Self-model learning rate

        // Environment constants
        constexpr float DEFAULT_WORLD_WIDTH = 20.0f;                // World simulation dimensions
        constexpr float DEFAULT_WORLD_HEIGHT = 20.0f;               // World simulation dimensions
        constexpr float DEFAULT_VISION_WIDTH = 8;                  // Sensory field dimensions
        constexpr float DEFAULT_VISION_HEIGHT = 8;                 // Sensory field dimensions
        constexpr float DEFAULT_MAX_AGENT_ENERGY = 100.0f;          // Agent energy capacity
        constexpr float DEFAULT_ENERGY_DECAY_RATE = 0.01f;         // Energy consumption rate
        constexpr float DEFAULT_ACTION_RADIUS = 0.5f;              // Action influence radius

        // Simulation constants
        constexpr float DEFAULT_TIMESTEP = 0.001f;                 // Base simulation timestep (s)
        constexpr size_t DEFAULT_SIMULATION_STEPS = 1000;          // Default simulation length
        constexpr float DEFAULT_BRAIN_UPDATE_FREQUENCY = 1.0f;      // Updates per second
        constexpr size_t DEFAULT_PERF_MEASUREMENT_INTERVAL = 100;   // Performance measurement frequency

        // Learning and exploration constants
        constexpr float DEFAULT_EXPLORATION_RATE = 0.1f;           // Random action exploration
        constexpr float DEFAULT_EXPLORATION_DECAY = 0.999f;        // Exploration rate decay
        constexpr float DEFAULT_GOAL_REWARD = 10.0f;                // Reward for goal completion
        constexpr float DEFAULT_PUNISHMENT = -5.0f;                // Penalty for failure
        constexpr size_t DEFAULT_SUCCESSFUL_TRIALS_FOR_CONCEPT = 5; // Concept formation success count

        // Logging and debugging constants
        constexpr bool DEFAULT_ENABLE_LOGGING = true;              // Global logging enable/disable
        constexpr const char* DEFAULT_LOG_FILE = "nlm.log";        // Default log file
        constexpr size_t DEFAULT_LOG_BUFFER_SIZE = 1024;           // Log message buffer size
        constexpr float DEFAULT_PERFORMANCE_LOG_INTERVAL = 1.0f;    // Performance logging frequency

        // Performance and optimization constants
        constexpr size_t DEFAULT_MEMORY_POOL_SIZE = 4096;          // Memory pool allocation size
        constexpr size_t DEFAULT_EVENT_QUEUE_SIZE = 8192;          // Event queue capacity
        constexpr size_t DEFAULT_MAX_THREADS = 8;                  // Maximum parallel threads
        constexpr bool DEFAULT_ENABLE_SIMD = true;                 // SIMD optimization enable
        constexpr bool DEFAULT_ENABLE_PARALLEL = false;            // Parallel processing enable
        constexpr float DEFAULT_PARALLEL_SCHEDULING_THRESHOLD = 0.5f; // Parallel task threshold
    }
}