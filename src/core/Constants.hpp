// Core simulation constants for the Neural Learning Machine (NLM)

#pragma once

namespace nlm {

// Membrane potential constants (mV)
namespace MembranePotentials {
    constexpr float RESTING = -70.0f;      // Typical neuronal resting potential
    constexpr float THRESHOLD = -55.0f;    // Firing threshold
    constexpr float RESET = -70.0f;       // Post-spike reset value
    constexpr float INHIBITORY_REVERSAL = -70.0f;  // For GABA synapses
    constexpr float EXCITATORY_REVERSAL = 0.0f;    // For AMPA synapses
}

// Neuron parameters
namespace NeuronParameters {
    constexpr float MEMBRANE_CAPACITANCE = 1.0f;        // nF
    constexpr float TIME_CONSTANT = 20.0f;             // ms
    constexpr float MAX_MEMBRANE_POTENTIAL = 50.0f;    // Upper clamp
    constexpr float MIN_MEMBRANE_POTENTIAL = -100.0f;  // Lower clamp
    constexpr float SPIKE_HISTORY_MAX = 100;            // Maximum recorded spikes
}

// Plasticity parameters
namespace PlasticityParameters {
    constexpr float STDP_LTP_WEIGHT = 0.01f;            // Long-term potentiation weight
    constexpr float STDP_LTD_WEIGHT = 0.012f;           // Long-term depression weight
    constexpr float HEBBIAN_LEARNING_RATE = 0.001f;     // Hebbian learning rate
    constexpr float REWARD_MODULATION_BASE = 0.5f;      // Base reward modulation
    constexpr float PLASTICITY_CLAMP_MIN = 0.1f;        // Minimum plasticity factor
    constexpr float PLASTICITY_CLAMP_MAX = 2.0f;        // Maximum plasticity factor
}

// Development parameters
namespace DevelopmentParameters {
    constexpr double INITIAL_AGE = 0.0;                 // Starting developmental age
    constexpr double CRITICAL_PERIOD_END = 60.0;        // ~1 minute in ms
    constexpr double MATURATION_END = 300.0;            // ~5 minutes in ms
    constexpr double ADULT_END = 900.0;                 // ~15 minutes in ms
    constexpr float INITIAL_PLASTICITY = 1.0f;          // High initial plasticity
    constexpr float CRITICAL_PLASTICITY = 0.8f;         // Critical period plasticity
    constexpr float MATURATION_PLASTICITY = 0.5f;       // Maturation plasticity
    constexpr float ADULT_PLASTICITY = 0.2f;            // Adult stability
}

// Neuromodulation parameters
namespace NeuromodulationParameters {
    constexpr float DOPAMINE_CLAMP_MIN = -1.0f;         // Minimum dopamine level
    constexpr float DOPAMINE_CLAMP_MAX = 1.0f;          // Maximum dopamine level
    constexpr float NOVELTY_DECAY = 0.99f;              // Novelty decay factor
    constexpr float CURIOUSITY_THRESHOLD = 0.3f;        // Threshold for curiosity-driven exploration
    constexpr float HIGH_CURIOUSITY_THRESHOLD = 0.5f;   // High curiosity threshold
    constexpr float EXPLORATION_CHANCE_MAX = 0.3f;      // Maximum exploration probability
    constexpr float ADAPTATION_DECAY = 0.95f;           // Adaptation decay factor
    constexpr float ADAPTATION_INCREMENT = 1.0f;        // Adaptation increment per spike
}

// Sensory processing parameters
namespace SensoryParameters {
    constexpr float VISION_SCALE = 5.0f;                // Vision input scaling
    constexpr float TOUCH_SCALE = 8.0f;                 // Touch input scaling
    constexpr float INTERNAL_SCALE = 5.0f;              // Internal signal scaling
    constexpr float PROPRIOCEPTION_SCALE = 3.0f;        // Proprioception scaling
    constexpr float VISION_SIZE = 16.0f;                // Vision grid dimension
    constexpr float PROP_SIZE = 6.0f;                   // Proprioception channels
}

// Motor control parameters
namespace MotorParameters {
    constexpr size_t MOTOR_ACTIONS = 6;                 // Number of motor actions
    constexpr size_t SENSE_ACTIONS = 4;                  // Number of sensory actions
    constexpr float MOTOR_ACTIVITY_THRESHOLD = 0.5f;     // Minimum activity for action selection
    constexpr float MIN_ACTIVITY = 0.0f;                // Minimum possible activity
}

// Simulation parameters
namespace SimulationParameters {
    constexpr double DEFAULT_TIMESTEP = 0.001;          // 1 ms timestep
    constexpr double DEFAULT_CLOCK_TICKS_PER_STEP = 1.0; // One tick per step
    constexpr size_t MAX_SIMULATION_STEPS = 1000000;    // Maximum simulation steps
    constexpr double MAX_SIMULATION_TIME = 1000.0;      // Maximum simulation time (seconds)
}

// Memory system parameters
namespace MemoryParameters {
    constexpr size_t WORKING_MEMORY_SIZE = 100;         // Working memory capacity
    constexpr size_t EPISODIC_MEMORY_SIZE = 1000;       // Episodic memory capacity
    constexpr float EPISODIC_DECAY = 0.95f;              // Episodic memory decay
    constexpr float SEMANTIC_DECAY = 0.9f;               // Semantic memory decay
}

// Experimental parameters
namespace ExperimentParameters {
    constexpr int DEFAULT_EPISODES = 100;               // Default number of training episodes
    constexpr int MAX_EPISODES = 10000;                 // Maximum episodes
    constexpr float REWARD_RANGE_MIN = -10.0f;          // Minimum possible reward
    constexpr float REWARD_RANGE_MAX = 10.0f;           // Maximum possible reward
    constexpr int STEPS_PER_EPISODE = 1000;            // Steps per episode
}

} // namespace nlm
