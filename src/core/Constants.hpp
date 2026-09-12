#pragma once

// Global constants for NLM simulation
// Organized by functional domain for easier maintenance

namespace nlm {

// =======================
// NEURON DYNAMICS CONSTANTS
// =======================

namespace NeuronConstants {
    // Membrane properties
    constexpr float MEMBRANE_CAPACITANCE = 1.0f;  // nF
    constexpr float TIME_CONSTANT = 20.0f;       // ms
    constexpr float LEAK_CONDUCTANCE_DEFAULT = 10.0f; // nS
    
    // Resting and threshold potentials (mV)
    constexpr float RESTING_POTENTIAL = -70.0f;
    constexpr float THRESHOLD_DEFAULT = -55.0f;
    constexpr float RESET_POTENTIAL = -70.0f;
    
    // Firing adaptation
    constexpr float ADAPTATION_INCREMENT = 1.0f;
    constexpr float ADAPTATION_DECAY = 0.95f;
    constexpr float ADAPTATION_DECAY_PER_STEP = 0.01f;
    
    // Refractory period (simulation steps)
    constexpr uint32_t REFRACTORY_PERIOD_MIN = 2;
    constexpr uint32_t REFRACTORY_PERIOD_MAX = 10;
    
    // Membrane potential bounds (mV)
    constexpr float MEMBRANE_POTENTIAL_MIN = -100.0f;
    constexpr float MEMBRANE_POTENTIAL_MAX = 50.0f;
    
    // Simulation timestep (ms)
    constexpr double DEFAULT_TIMESTEP = 0.001;
}

// ========================
// SYNAPTIC CONSTANTS
// ========================

namespace SynapseConstants {
    // Weight bounds
    constexpr float MIN_WEIGHT = -1.0f;
    constexpr float MAX_WEIGHT = 1.0f;
    
    // Synaptic delay (simulation steps)
    constexpr Delay DELAY_MIN = 1;
    constexpr Delay DELAY_MAX = 5;
    
    // Short-term plasticity parameters
    constexpr float STP_FACILITATION_TAU = 100.0f;  // ms
    constexpr float STP_DEPRESSION_TAU = 200.0f;    // ms
    constexpr float STP_U_MAX = 1.0f;                // Max utilization
    
    // Efficacy bounds
    constexpr float EFFICACY_MIN = 0.0f;
    constexpr float EFFICACY_MAX = 2.0f;
    
    // Eligibility trace parameters
    constexpr float ELIGIBILITY_TRACE_DECAY_FAST = 0.001f;
    constexpr float ELIGIBILITY_TRACE_DECAY_MEDIUM = 0.01f;
    constexpr float ELIGIBILITY_TRACE_DECAY_SLOW = 0.1f;
    constexpr float ELIGIBILITY_TRACE_THRESHOLD = 0.001f;
}

// ==========================
// NEUROMODULATION CONSTANTS
// ==========================

namespace NeuromodulationConstants {
    // Dopamine system
    constexpr float DOPAMINE_CLAMP_MIN = -1.0f;
    constexpr float DOPAMINE_CLAMP_MAX = 1.0f;
    constexpr float PLASTICITY_FACTOR_MIN = 0.1f;
    constexpr float PLASTICITY_FACTOR_MAX = 2.0f;
    
    // Reward prediction error scaling
    constexpr float REWARD_SCALE = 1.0f;
    constexpr float PREDICTED_REWARD_DECAY = 0.95f;
    constexpr float PREDICTED_REWARD_UPDATE = 0.05f;
    
    // Curiosity system
    constexpr float CURIOSITY_NOVELTY_WEIGHT = 2.0f;
    constexpr float CURIOSITY_PREDICTION_ERROR_WEIGHT = 0.5f;
    constexpr float CURIOSITY_CLAMP_MIN = 0.0f;
    constexpr float CURIOSITY_CLAMP_MAX = 1.0f;
    
    // Novelty detection
    constexpr float NOVELTY_DECAY = 0.99f;
    constexpr float NOVELTY_THRESHOLD = 0.5f;
    
    // Prediction error system
    constexpr float PREDICTION_ERROR_CLAMP = 2.0f;
}

// ==========================
// SENSOR CONSTANTS
// ==========================

namespace SensorConstants {
    // Vision
    constexpr size_t VISION_WIDTH_DEFAULT = 16;
    constexpr size_t VISION_HEIGHT_DEFAULT = 16;
    constexpr size_t VISION_CHANNELS_DEFAULT = 3;
    constexpr float VISION_SCALE_FACTOR = 5.0f;
    
    // Touch
    constexpr size_t TOUCH_SENSORS_DEFAULT = 8;
    constexpr float TOUCH_SCALE_FACTOR = 8.0f;
    
    // Internal signals
    constexpr size_t INTERNAL_SIGNALS_DEFAULT = 4;
    constexpr float INTERNAL_SCALE_FACTOR = 5.0f;
    constexpr float INTERNAL_CENTER_OFFSET = 1.0f;
    
    // Proprioception
    constexpr size_t PROPRIOCEPTION_DEFAULT = 6;
    constexpr float PROPRIOCEPTION_SCALE_FACTOR = 3.0f;
    
    // Audio
    constexpr size_t AUDIO_SAMPLES_DEFAULT = 0;
    
    // Neural inputs
    constexpr float SENSORY_INPUTS_CLAMP_MAX = 10.0f;
}

// =========================
// MOTOR CONSTANTS
// =========================

namespace MotorConstants {
    // Action count and types
    constexpr size_t MOTOR_ACTIONS_COUNT = 6;
    constexpr size_t MOTOR_COMMANDS_COUNT = 7;
    
    // Activity threshold for action selection
    constexpr float ACTIVITY_THRESHOLD = 0.5f;
    constexpr float MIN_ACTIVITY_FOR_ACTION = 0.5f;
    
    // Exploration parameters
    constexpr float EXPLORATION_FACTOR_MAX = 0.3f;
    constexpr float CURIOSITY_THRESHOLD_FOR_EXPLORATION = 0.3f;
    constexpr float HIGH_CURIOSITY_EXPLORATION_CHANCE = 0.3f;
    
    // Motor neuron distribution
    constexpr size_t MOTOR_GROUP_COUNT = 6;
    constexpr size_t SENSORY_GROUP_COUNT = 4;
}

// ==========================
// DEVELOPMENT CONSTANTS
// ==========================

namespace DevelopmentConstants {
    // Developmental stages (simulation time in steps)
    constexpr double DEVELOPMENTAL_STAGE_INITIAL = 60.0;     // ~1 minute
    constexpr double DEVELOPMENTAL_STAGE_CRITICAL = 300.0;   // ~5 minutes
    constexpr double DEVELOPMENTAL_STAGE_MATURATION = 900.0; // ~15 minutes
    constexpr double DEVELOPMENTAL_STAGE_ADULT = 1e6;        // Long-term stability
    
    // Plasticity modulation with age
    constexpr float PLASTICITY_INITIAL = 1.0f;
    constexpr float PLASTICITY_CRITICAL = 0.8f;
    constexpr float PLASTICITY_MATURATION = 0.5f;
    constexpr float PLASTICITY_ADULT = 0.2f;
    
    // Structural plasticity
    constexpr float SYNAPTONGENESIS_RATE_INITIAL = 0.0001f;
    constexpr float PRUNING_RATE_INITIAL = 0.00001f;
    constexpr float SYNAPTONGENESIS_SCALE_FACTOR = 1000.0f;
    constexpr float PRUNING_SCALE_FACTOR = 10000.0f;
}

// ==========================
// PREDICTION CONSTANTS
// ==========================

namespace PredictionConstants {
    // Prediction system
    constexpr float PREDICTION_ERROR_CLAMP_MIN = -2.0f;
    constexpr float PREDICTION_ERROR_CLAMP_MAX = 2.0f;
    constexpr float PREDICTION_WEIGHT_DEFAULT = 0.5f;
    constexpr float PREDICTION_ERROR_DECAY = 0.9f;
    
    // Confidence and uncertainty
    constexpr float CONFIDENCE_THRESHOLD = 0.5f;
    constexpr float UNCERTAINTY_MAX = 1.0f;
}

// =========================
// MEMORY CONSTANTS
// =========================

namespace MemoryConstants {
    // Working memory
    constexpr size_t WORKING_MEMORY_CAPACITY = 100;
    constexpr float WORKING_MEMORY_DECAY_RATE = 0.95f;
    
    // Episodic memory
    constexpr size_t EPISODIC_MEMORY_CAPACITY = 1000;
    constexpr float EPISODE_REPLAY_INTERVAL = 100.0f;
    constexpr float EPISODE_CONSOLIDATION_INTERVAL = 1000.0f;
    constexpr float EPISODE_IMPORTANCE_THRESHOLD = 0.8f;
    
    // Associative memory
    constexpr size_t ASSOCIATIVE_MEMORY_CAPACITY = 500;
    constexpr float ASSOCIATION_STRENGTH_MAX = 1.0f;
    constexpr float ASSOCIATION_STRENGTH_MIN = 0.01f;
}

// ============================
// EXPERIMENT CONSTANTS
// ============================

namespace ExperimentConstants {
    // Phase experiments
    constexpr double PHASE3_EXPERIMENT_DURATION = 1000.0;
    constexpr double PHASE4_EXPERIMENT_DURATION = 2000.0;
    constexpr double PHASE5_EXPERIMENT_DURATION = 3000.0;
    constexpr double PHASE6_EXPERIMENT_DURATION = 5000.0;
    
    // Metrics and measurements
    constexpr float LEARNING_RATE_THRESHOLD = 0.01f;
    constexpr float PERFORMANCE_CONVERGENCE = 0.95f;
    constexpr float STABILITY_THRESHOLD = 0.9f;
    constexpr float EXPLORATION_EXPLOITATION_BALANCE = 0.5f;
}

// ============================
// VISUALIZATION CONSTANTS
// ============================

namespace VisualizationConstants {
    // Display parameters
    constexpr int DEFAULT_WINDOW_WIDTH = 800;
    constexpr int DEFAULT_WINDOW_HEIGHT = 600;
    constexpr double DEFAULT_FPS = 60.0;
    
    // Neural visualization
    constexpr float NEURON_RADIUS = 3.0f;
    constexpr float SYNAPSE_WIDTH = 1.0f;
    constexpr float CONNECTION_ALPHA = 0.3f;
    
    // Color mapping
    constexpr float EXCITATION_GAIN = 1.0f;
    constexpr float INHIBITION_GAIN = 0.5f;
    constexpr float MODULATION_GAIN = 0.8f;
}

} // namespace nlm