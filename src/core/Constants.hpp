#pragma once

namespace nlm {

// Neural network and simulation constants
namespace NeuralConstants {
    // Neuron properties
    constexpr float LIF_THRESHOLD = 1.0f;           // Leaky Integrate-and-Fire threshold
    constexpr float LIF_RESTING_POTENTIAL = 0.0f;  // Resting membrane potential
    constexpr float LIF_DECAY_RATE = 0.1f;         // Membrane potential decay rate
    constexpr float LIF_TIME_CONSTANT = 10.0f;     // Membrane time constant (ms)
    
    // Synapse properties
    constexpr float SYNAPSE_INITIAL_WEIGHT = 0.01f;  // Initial synaptic weight
    constexpr float SYNAPSE_MAX_WEIGHT = 1.0f;        // Maximum synaptic weight
    constexpr float SYNAPSE_MIN_WEIGHT = -1.0f;       // Minimum synaptic weight
    constexpr float SYNAPSE_DECAY_RATE = 0.95f;       // Weight decay rate
    
    // Connection properties
    constexpr float CONNECTION_PROBABILITY_DEFAULT = 0.15f;  // Default connection probability
    constexpr float CONNECTION_DELAY_DEFAULT = 1.0f;         // Default synaptic delay
}

// Neuromodulation constants
namespace NeuromodulationConstants {
    // Dopamine properties
    constexpr float DOPAMINE_RANGE = 2.0f;           // Dopamine signal range (-1 to +1)
    constexpr float DOPAMINE_NOISE_LEVEL = 0.01f;    // Dopamine baseline noise
    constexpr float DOPAMINE_DECAY = 0.9f;          // Dopamine decay rate
    
    // Reward processing
    constexpr float REWARD_DISCOUNT_FACTOR = 0.99f;    // Reward discount factor
    constexpr float REWARD_SCALE_FACTOR = 10.0f;       // Reward scaling factor
    constexpr float PREDICTION_ERROR_THRESHOLD = 0.5f; // Prediction error significance threshold
}

// Plasticity constants
namespace PlasticityConstants {
    // STDP parameters
    constexpr float STDP_LTP_WEIGHT = 0.02f;         // Long-Term Potentiation weight
    constexpr float STDP_LTD_WEIGHT = 0.015f;        // Long-Term Depression weight
    constexpr float STDP_TIME_CONSTANT = 20.0f;      // STDP time constant
    constexpr float STDP_TRACE_DECAY = 0.95f;        // STDP trace decay rate
    
    // Hebbian plasticity
    constexpr float HEBBIAN_LEARNING_RATE = 0.01f;   // Hebbian learning rate
    constexpr float HEBBIAN_THRESHOLD = 0.5f;        // Hebbian activation threshold
    
    // Structural plasticity
    constexpr float SYNAPTOGENESIS_RATE_DEFAULT = 0.0001f;  // Default synaptogenesis rate
    constexpr float PRUNING_RATE_DEFAULT = 0.00001f;        // Default pruning rate
    constexpr float PLASTICITY_MODIFIER_MIN = 0.1f;          // Minimum plasticity modifier
    constexpr float PLASTICITY_MODIFIER_MAX = 2.0f;         // Maximum plasticity modifier
}

// Development constants
namespace DevelopmentConstants {
    // Developmental stages
    constexpr double DEVELOPMENT_INITIAL_THRESHOLD = 60.0;     // Initial development stage
    constexpr double DEVELOPMENT_CRITICAL_PERIOD_THRESHOLD = 300.0;  // Critical period
    constexpr double DEVELOPMENT_MATURATION_THRESHOLD = 900.0;       // Maturation stage
    constexpr double DEVELOPMENT_ADULT_THRESHOLD = 3600.0;           // Adult stage (~1 hour)
    
    // Developmental effects
    constexpr float DEVELOPMENT_PLASTICITY_INITIAL = 1.0f;    // High plasticity in initial stage
    constexpr float DEVELOPMENT_PLASTICITY_CRITICAL = 0.8f;   // Moderate plasticity in critical period
    constexpr float DEVELOPMENT_PLASTICITY_MATURATION = 0.5f;  // Lower plasticity in maturation
    constexpr float DEVELOPMENT_PLASTICITY_ADULT = 0.2f;       // Stable plasticity in adult
    
    // Structural development
    constexpr float DEVELOPMENT_SYNAPTOGENESIS_RATE = 0.0001f;  // Synaptogenesis rate
    constexpr float DEVELOPMENT_PRUNING_RATE = 0.00001f;        // Pruning rate
    constexpr float DEVELOPMENT_MATURATION_RATE = 0.000005f;    // Neural maturation rate
}

// Memory constants
namespace MemoryConstants {
    // Working memory
    constexpr size_t WORKING_MEMORY_MAX_TRACES = 1000;  // Maximum working memory traces
    constexpr float WORKING_MEMORY_DECAY = 0.9f;         // Working memory decay rate
    constexpr float WORKING_MEMORY_THRESHOLD = 0.5f;     // Working memory activation threshold
    
    // Episodic memory
    constexpr size_t EPISODIC_MEMORY_MAX_EPISODES = 10000;  // Maximum stored episodes
    constexpr float EPISODIC_MEMORY_COMPRESSION = 0.8f;       // Memory compression factor
    constexpr float EPISODIC_MEMORY_RETRIEVAL_THRESHOLD = 0.3f; // Retrieval threshold
    
    // Semantic memory
    constexpr size_t SEMANTIC_MEMORY_MAX_CONCEPTS = 500;   // Maximum concepts
    constexpr float SEMANTIC_MEMORY_LEARNING_RATE = 0.01f; // Concept learning rate
}

// Sensory processing constants
namespace SensoryConstants {
    // Vision processing
    constexpr size_t VISION_WIDTH = 16;                   // Vision width (pixels)
    constexpr size_t VISION_HEIGHT = 16;                  // Vision height (pixels)
    constexpr size_t VISION_CHANNELS = 1;                 // Vision channels
    constexpr float VISION_SENSITIVITY = 5.0f;            // Vision sensitivity factor
    constexpr float VISION_NOISE_LEVEL = 0.01f;          // Vision noise level
    
    // Touch processing
    constexpr size_t TOUCH_SENSORS = 8;                   // Number of touch sensors
    constexpr float TOUCH_SENSITIVITY = 8.0f;             // Touch sensitivity factor
    constexpr float TOUCH_NOISE_LEVEL = 0.02f;            // Touch noise level
    
    // Internal signals
    constexpr size_t INTERNAL_SIGNALS = 4;               // Internal signal count
    constexpr float INTERNAL_SENSITIVITY = 5.0f;          // Internal signal sensitivity
    
    // Proprioception
    constexpr size_t PROPRIOCEPTION_SENSORS = 6;         // Proprioception sensors
    constexpr float PROPRIOCEPTION_SENSITIVITY = 3.0f;   // Proprioception sensitivity
    
    // Novelty detection
    constexpr float NOVELTY_DECAY = 0.99f;                // Novelty decay factor
    constexpr float NOVELTY_THRESHOLD = 0.1f;             // Novelty significance threshold
}

// Motor control constants
namespace MotorConstants {
    // Motor commands
    constexpr size_t MOTOR_COMMANDS_COUNT = 6;           // Number of motor commands
    constexpr float MOTOR_ACTIVITY_THRESHOLD = 0.5f;     // Motor activity threshold
    constexpr float MOTOR_EXPLORATION_CHANCE = 0.3f;    // Base exploration chance
    constexpr float MOTOR_NOISE_LEVEL = 0.01f;          // Motor noise level
    
    // Movement properties
    constexpr float MOVEMENT_SPEED = 1.0f;               // Movement speed factor
    constexpr float TURN_SPEED = 0.5f;                  // Turning speed factor
    constexpr float INTERACTION_FORCE = 2.0f;           // Interaction force
}

// Environment constants
namespace EnvironmentConstants {
    // World properties
    constexpr size_t WORLD_WIDTH = 16;                   // World width
    constexpr size_t WORLD_HEIGHT = 16;                  // World height
    constexpr float TIME_STEP = 0.001f;                 // Simulation timestep (seconds)
    constexpr size_t MAX_STEPS = 5000;                  // Maximum simulation steps
    
    // Rewards
    constexpr float REWARD_STEP = 0.0f;                  // Step reward
    constexpr float REWARD_GOAL = 10.0f;                 // Goal achievement reward
    constexpr float REWARD_COLLISION = -1.0f;            // Collision penalty
    constexpr float REWARD_FINISH = 100.0f;              // Finish reward
    
    // Actions
    constexpr size_t ACTION_FORWARD = 0;                // Move forward action
    constexpr size_t ACTION_BACKWARD = 1;               // Move backward action
    constexpr size_t ACTION_LEFT = 2;                   // Turn left action
    constexpr size_t ACTION_RIGHT = 3;                  // Turn right action
    constexpr size_t ACTION_INTERACT = 4;               // Interact action
    constexpr size_t ACTION_WAIT = 5;                   // Wait action
}

// Performance modes
namespace PerformanceConstants {
    constexpr const char* PERFORMANCE_FAST = "fast";     // Fast performance mode
    constexpr const char* PERFORMANCE_BALANCED = "balanced"; // Balanced performance mode
    constexpr const char* PERFORMANCE_ACCURATE = "accurate";   // Accurate performance mode
    
    // Performance settings for each mode
    struct PerformanceMode {
        const char* name;
        float neuronMultiplier;
        float stepMultiplier;
        float accuracyBoost;
    };
    
    constexpr PerformanceMode PERFORMANCE_MODES[] = {
        {PERFORMANCE_FAST, 0.5f, 0.5f, 0.7f},
        {PERFORMANCE_BALANCED, 1.0f, 1.0f, 1.0f},
        {PERFORMANCE_ACCURATE, 2.0f, 2.0f, 1.5f}
    };
    
    constexpr size_t PERFORMANCE_MODE_COUNT = sizeof(PERFORMANCE_MODES) / sizeof(PerformanceMode);
}

// Configuration keys (for documentation)
namespace ConfigKeys {
    // Neural network
    constexpr const char* NEURON_COUNT = "neuron_count";
    constexpr const char* REGION_COUNT = "region_count";
    constexpr const char* CONNECTION_PROBABILITY = "connection_probability";
    
    // Plasticity
    constexpr const char* STDP_LTP_WEIGHT = "stdp_ltp_weight";
    constexpr const char* STDP_LTD_WEIGHT = "stdp_ltd_weight";
    constexpr const char* STDP_TIME_CONSTANT = "stdp_time_constant";
    constexpr const char* HEBBIAN_LEARNING_RATE = "plasticity_learning_rate";
    constexpr const char* SYNAPTOGENESIS_RATE = "synaptogenesis_rate";
    constexpr const char* PRUNING_RATE = "pruning_rate";
    
    // Neuromodulation
    constexpr const char* DOPAMINE_BASELINE = "dopamine_baseline";
    constexpr const char* REWARD_DISCOUNT_FACTOR = "reward_discount_factor";
    
    // Development
    constexpr const char* DEVELOPMENT_ENABLED = "development_enabled";
    constexpr const char* STRUCTUREAL_PLASTICITY = "structural_plasticity_enabled";
    
    // Memory
    constexpr const char* MEMORY_EPISODES = "memory_episodes";
    
    // Environment
    constexpr const char* ENVIRONMENT_NAME = "environment_name";
    constexpr const char* ENVIRONMENT_WIDTH = "environment_width";
    constexpr const char* ENVIRONMENT_HEIGHT = "environment_height";
    
    // Logging
    constexpr const char* LOG_LEVEL = "log_level";
    constexpr const char* LOG_TO_FILE = "log_to_file";
    constexpr const char* LOG_FILENAME = "log_filename";
    
    // Performance
    constexpr const char* PERFORMANCE_MODE = "performance_mode";
    constexpr const char* CHECKPOINT_PATH = "checkpoint_path";
}

// Utility functions
namespace Constants {
    // Clamp value between min and max
    template<typename T>
    T clamp(const T& value, const T& min, const T& max) {
        return (value < min) ? min : ((value > max) ? max : value);
    }
    
    // Linear interpolation
    template<typename T>
    T lerp(const T& start, const T& end, float t) {
        return start + (end - start) * t;
    }
    
    // Map value from one range to another
    template<typename T>
    T mapRange(const T& value, const T& inMin, const T& inMax, const T& outMin, const T& outMax) {
        if (inMin == inMax) return outMin;
        float normalized = (value - inMin) / (inMax - inMin);
        normalized = (normalized < 0.0f) ? 0.0f : ((normalized > 1.0f) ? 1.0f : normalized);
        return outMin + (outMax - outMin) * normalized;
    }
    
    // Round to nearest multiple
    float roundToMultiple(float value, float multiple) {
        return std::round(value / multiple) * multiple;
    }
}

} // namespace nlm