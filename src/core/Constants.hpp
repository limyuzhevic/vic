#pragma once

namespace nlm {

// Core simulation constants
namespace SimulationConstants {
    // Timestep constants
    constexpr double DEFAULT_TIMESTEP = 0.001;  // seconds
    constexpr TimestepDuration DEFAULT_STEP_COUNT = 1;
    
    // Stability constants
    constexpr float EPSILON = 1e-6f;
    constexpr float MACHINE_EPSILON = 1e-7f;
    
    // Neuron constants
    constexpr float RESTING_POTENTIAL = -70.0f;      // mV
    constexpr float THRESHOLD_POTENTIAL = -55.0f;    // mV
    constexpr float RESET_POTENTIAL = -65.0f;        // mV
    constexpr float LEAK_CONDUCTANCE = 0.01f;          // mS/cm²
    constexpr float MEMBRANE_CAPACITANCE = 1.0f;       // μF/cm²
}

// Learning constants
namespace LearningConstants {
    // STDP constants
    constexpr float DEFAULT_LTP_WEIGHT = 0.01f;
    constexpr float DEFAULT_LTD_WEIGHT = 0.012f;
    constexpr float DEFAULT_STDP_TIME_CONST = 20.0f;   // ms
    
    // Hebbian constants
    constexpr float DEFAULT_HEBBIAN_RATE = 0.01f;
    constexpr float DEFAULT_MAX_WEIGHT = 1.0f;
    constexpr float DEFAULT_MIN_WEIGHT = -1.0f;
    
    // Plasticity bounds
    constexpr float WEIGHT_MIN = -1.0f;
    constexpr float WEIGHT_MAX = 1.0f;
}

// Memory constants
namespace MemoryConstants {
    // Working memory
    constexpr size_t DEFAULT_WM_CAPACITY = 100;
    constexpr float DEFAULT_WM_DECAY_RATE = 0.01f;
    constexpr float WM_TRACE_THRESHOLD = 0.01f;
    constexpr float WM_COMPETITION_THRESHOLD = 0.3f;
    
    // Episodic memory
    constexpr size_t DEFAULT_MAX_EPISODES = 1000;
    constexpr size_t DEFAULT_REPLAY_INTERVAL = 100;
    constexpr size_t DEFAULT_CONSOLIDATION_INTERVAL = 1000;
    
    // Associative memory
    constexpr float DEFAULT_ASSOCIATION_THRESHOLD = 0.7f;
    constexpr float DEFAULT_FORGETTING_RATE = 0.05f;
}

// Neuromodulation constants
namespace NeuromodulationConstants {
    // Dopamine
    constexpr float DEFAULT_DOPAMINE_BASELINE = 0.0f;
    constexpr float DEFAULT_DOPAMINE_PEAK = 1.0f;
    constexpr float DEFAULT_DOPAMINE_DECAY_RATE = 0.1f;
    constexpr float DEFAULT_DOPAMINE_RELEASE_RATE = 1.0f;
    constexpr float DOPAMINE_CLAMP_MIN = 0.0f;
    constexpr float DOPAMINE_CLAMP_MAX = 1.0f;
    
    // Curiosity
    constexpr float DEFAULT_NOVELTY_WEIGHT = 0.5f;
    constexpr float DEFAULT_PREDICTION_ERROR_WEIGHT = 0.5f;
    constexpr float DEFAULT_CURIOUSITY_DECAY_RATE = 0.05f;
    constexpr float CURIOUSITY_EXPLORATION_THRESHOLD = 0.3f;
    constexpr float MAX_EXPLORATION_CHANCE = 0.3f;
    
    // Novelty
    constexpr float DEFAULT_NOVELTY_DECAY_RATE = 0.1f;
    constexpr float DEFAULT_NOVELTY_THRESHOLD = 0.3f;
    
    // Prediction error
    constexpr float PREDICTION_ERROR_MARGIN = 0.001f;
}

// Development constants
namespace DevelopmentConstants {
    // Developmental stages
    constexpr double INITIAL_STAGE_DURATION = 60.0;      // seconds
    constexpr double CRITICAL_PERIOD_DURATION = 240.0;  // seconds
    constexpr double MATURATION_DURATION = 900.0;       // seconds
    
    // Plasticity modulation during development
    constexpr float INITIAL_STAGE_MODULATION = 1.0f;
    constexpr float CRITICAL_PERIOD_MODULATION = 0.8f;
    constexpr float MATURATION_MODULATION = 0.5f;
    constexpr float ADULT_MODULATION = 0.2f;
}

// Structural plasticity constants
namespace StructuralPlasticityConstants {
    constexpr float DEFAULT_SYNAPTOGENESIS_RATE = 0.0001f;
    constexpr float DEFAULT_PRUNING_RATE = 0.00001f;
    constexpr float DEFAULT_FORMATION_RADIUS = 0.5f;
    constexpr float DEFAULT_LINK_STRENGTH = 0.5f;
}

// Action constants
namespace ActionConstants {
    constexpr size_t MAX_MOTOR_NEURONS = 6;
    constexpr size_t VISION_WIDTH = 16;
    constexpr size_t VISION_HEIGHT = 16;
    constexpr size_t TOUCH_SENSORS = 8;
    constexpr size_t INTERNAL_SENSORS = 4;
    constexpr size_t PROPRIOCEPTION_SENSORS = 6;
    
    // Sensory input sizes
    constexpr size_t SENSE_VISION_SIZE = VISION_WIDTH * VISION_HEIGHT;
    constexpr size_t SENSE_TOTAL_SIZE = SENSE_VISION_SIZE + TOUCH_SENSORS + INTERNAL_SENSORS + PROPRIOCEPTION_SENSORS;
}

// Simulation control constants
namespace SimulationControlConstants {
    constexpr size_t DEFAULT_REPLAY_INTERVAL = 100;
    constexpr size_t DEFAULT_CONSOLIDATION_INTERVAL = 1000;
    constexpr size_t DEFAULT_DEVELOPMENT_UPDATE_INTERVAL = 1000;
    constexpr float CHECKPOINT_DECAY_FACTOR = 0.3f;
}

} // namespace nlm