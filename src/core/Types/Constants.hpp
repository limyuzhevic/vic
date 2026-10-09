#pragma once

namespace nlm {

// Neural network constants
namespace NeuralConstants {
    // Neuron parameters
    constexpr float V_THRESH = -50.0f;      // Threshold potential (mV)
    constexpr float V_REST = -70.0f;       // Resting potential (mV)
    constexpr float V_RESET = -75.0f;      // Reset potential after spike (mV)
    constexpr float TAU_MEM = 20.0f;       // Membrane time constant (ms)
    constexpr float TAU_REF = 2.0f;        // Refractory period (ms)
    
    // Connection parameters
    constexpr float CONNECTION_PROBABILITY = 0.15f;
    constexpr float INITIAL_WEIGHT_MEAN = 0.5f;
    constexpr float INITIAL_WEIGHT_STD = 0.1f;
    
    // Plasticity parameters
    constexpr float PLASTICITY_LEARNING_RATE = 0.01f;
    constexpr float STDP_LTP_WEIGHT = 0.02f;
    constexpr float STDP_LTD_WEIGHT = 0.015f;
    constexpr float STDP_TIME_CONSTANT = 20.0f;
    constexpr float SYNAPTOGENESIS_RATE = 0.0001f;
    constexpr float PRUNING_RATE = 0.00001f;
    
    // Neuromodulation parameters
    constexpr float DOPAMINE_BASELINE = 0.1f;
    constexpr float REWARD_DISCOUNT_FACTOR = 0.99f;
    
    // Development parameters
    constexpr float DEVELOPMENT_SYNAPTOGENESIS_RATE = 0.001f;
    constexpr float DEVELOPMENT_PRUNING_RATE = 0.0001f;
    constexpr float DEVELOPMENT_DURATION = 600.0;  // seconds
    
    // Curiosity parameters
    constexpr float CURIOSITY_THRESHOLD = 0.3f;
    constexpr float NOVELTY_DECAY = 0.99f;
    
    // Memory parameters
    constexpr float MAX_MEMORY_EPISODES = 100;
    constexpr size_t WORKING_MEMORY_SIZE = 50;
    constexpr size_t REPLAY_EPISODES = 10;
    constexpr size_t CHECKPOINT_FREQUENCY = 1000;
    
    // Sensory processing parameters
    constexpr size_t VISION_WIDTH = 16;
    constexpr size_t VISION_HEIGHT = 16;
    constexpr size_t TOUCH_SENSORS = 8;
    constexpr size_t INTERNAL_SIGNALS = 4;
    constexpr size_t PROPRIOCEPTION_SENSORS = 6;
    constexpr size_t AUDIO_CHANNELS = 0;
    
    // Motor output parameters
    constexpr size_t MOTOR_ACTIONS = 6;
    constexpr size_t MOTOR_FORWARD = 0;
    constexpr size_t MOTOR_BACKWARD = 1;
    constexpr size_t MOTOR_TURN_LEFT = 2;
    constexpr size_t MOTOR_TURN_RIGHT = 3;
    constexpr size_t MOTOR_INTERACT = 4;
    constexpr size_t MOTOR_WAIT = 5;
    
    // Agent behavior parameters
    constexpr float CURRENT_INJECTION_STRENGTH = 50.0f;
    constexpr float CURRENT_INJECTION_WEAK = 5.0f;
    constexpr float NEUROMODULATION_SCALE = 1.0f;
    constexpr float PLASTICITY_MODIFIER = 1.0f;
    
    // Simulation parameters
    constexpr double SIMULATION_TIMESTEP = 0.001;
    constexpr uint64_t DEFAULT_STEPS = 1000;
    constexpr uint64_t BENCHMARK_STEPS = 5000;
    constexpr uint64_t BATCH_STEPS = 100;
    constexpr uint64_t STEPS_PER_EPISODE = 100;
    
    // Debug and logging parameters
    constexpr size_t LOG_PROGRESS_INTERVAL = 1000;
    constexpr float LEARNING_OCCURRED_THRESHOLD = 0.001f;
    constexpr size_t MOST_ACTIVE_NEURONS = 10;
    
    // Experimental parameters
    constexpr float EXPERIMENT_REWARD_VALUE = 1.0f;
    constexpr float EXPERIMENT_PUNISHMENT_VALUE = -1.0f;
    constexpr float NOVELTY_THRESHOLD = 0.1f;
}

// Memory constants
namespace MemoryConstants {
    constexpr float WORKING_MEMORY_DECAY = 0.95f;
    constexpr float EPISODIC_MEMORY_COMPRESSION = 0.8f;
    constexpr float ASSOCIATIVE_STRENGTH_THRESHOLD = 0.7f;
    constexpr float CONSOLIDATION_THRESHOLD = 0.5f;
}

// Development constants
namespace DevelopmentConstants {
    constexpr float INITIAL_STAGE_PLACILITY = 1.0f;
    constexpr float CRITICAL_PERIOD_PLACILITY = 0.8f;
    constexpr float MATURATION_PLACILITY = 0.5f;
    constexpr float ADULT_PLACILITY = 0.2f;
    constexpr double INITIAL_STAGE_DURATION = 60.0;   // seconds
    constexpr double CRITICAL_PERIOD_DURATION = 240.0; // seconds
    constexpr double MATURATION_DURATION = 720.0;      // seconds
}

// Performance constants
namespace PerformanceConstants {
    constexpr double TARGET_WALL_CLOCK_TIME = 60.0;   // seconds
    constexpr size_t MAX_THREADS = 8;
    constexpr size_t MEMORY_POOL_SIZE = 1024;
    constexpr size_t EVENT_QUEUE_SIZE = 1024;
}

} // namespace nlm