#ifndef NLM_CONSTANTS_H
#define NLM_CONSTANTS_H

#include <cstdint>

namespace nlm {

// Core simulation parameters
namespace Simulation {
    constexpr double TIMESTEP_MS = 0.001;              // 1ms timestep for biological realism
    constexpr double TIMESTEP_SECONDS = 0.001;          // 1ms timestep in seconds
    constexpr double SIMULATION_TIME_SECONDS = 10000.0; // Maximum simulation duration
    constexpr size_t MAX_SIMULATION_STEPS = 10000;     // Maximum number of simulation steps
}

// Development parameters (synaptogenesis/pruning rates)
namespace Development {
    constexpr double SYNAPTOGENESIS_RATE = 0.0001f;     // Rate of new synapse formation
    constexpr double PRUNING_RATE = 0.00001f;           // Rate of synaptic elimination
    constexpr double PLASTICITY_DECAY_FACTOR = 0.99f;    // Synaptic weight decay
}

// Plasticity parameters (STDP weights, Hebbian learning rates)
namespace Plasticity {
    // STDP parameters
    constexpr float STDP_LTP_WEIGHT = 0.02f;            // Long-term potentiation weight
    constexpr float STDP_LTD_WEIGHT = 0.015f;           // Long-term depression weight  
    constexpr float STDP_TIME_CONSTANT = 20.0f;         // STDP time window (ms)
    
    // Hebbian learning
    constexpr float HEBBIAN_LEARNING_RATE = 0.01f;      // Hebbian learning rate
    constexpr float SYNAPTIC_LEARNING_RATE = 0.012f;    // Synaptic LTD weight
    
    // Structural plasticity
    constexpr float STRUCTURAL_PLASTICITY_THRESHOLD = 0.0001; // Threshold for structural changes
    constexpr float STRUCTURAL_DECAY_RATE = 0.00001f;   // Rate of structural changes
}

// Memory system parameters (capacity, intervals)
namespace Memory {
    constexpr size_t EPISODIC_MEMORY_CAPACITY = 1000;   // Number of episodic memories
    constexpr size_t WORKING_MEMORY_SLOTS = 50;         // Working memory slots
    constexpr double MEMORY_DECAY_TIME_MS = 60000.0f;    // 60 seconds memory decay
    constexpr double RECALL_INTERVAL_MS = 1000.0f;      // 1 second recall interval
}

// Action/decision thresholds
namespace Decision {
    constexpr float ACTION_THRESHOLD = 0.5f;            // Minimum activity for action selection
    constexpr float CURIOSITY_THRESHOLD = 0.3f;          // Novelty threshold for exploration
    constexpr float EXPLORATION_CHANCE = 0.5f;          // Base exploration probability
    constexpr float CONSERVATION_FACTOR = 0.9f;         // Preference for familiar actions
}

// Sensory processing constants
namespace Sensory {
    constexpr float VISUAL_INPUT_SCALING = 5.0f;        // Vision input scaling factor
    constexpr float TACTILE_INPUT_SCALING = 8.0f;       // Touch input scaling factor
    constexpr float INTERIOR_INPUT_SCALING = 2.0f;      // Internal signal scaling
    constexpr float PROPRIOCEPTIVE_INPUT_SCALING = 3.0f; // Proprioception scaling
    
    // Sensory input dimensions
    constexpr size_t VISION_INPUT_SIZE = 256;           // 16x16 vision grid
    constexpr size_t TACTILE_INPUT_SIZE = 8;             // Touch sensors
    constexpr size_t INTERIOR_INPUT_SIZE = 4;           // Internal state sensors
    constexpr size_t PROPRIOCEPTIVE_INPUT_SIZE = 6;     // Proprioceptive sensors
    
    // Novelty processing
    constexpr float NOVELTY_DECAY_FACTOR = 0.99f;       // Novelty decay rate
    constexpr float CURIOSITY_SENSITIVITY = 2.0f;        // Curiosity response sensitivity
}

// Neurologically inspired values
namespace Neuro {
    // Structural parameters
    constexpr size_t DEFAULT_NEURON_COUNT = 500;        // Default neuron population size
    constexpr size_t MOTOR_OUTPUT_COUNT = 6;             // Number of motor actions
    constexpr float CONNECTION_PROBABILITY = 0.15f;     // Neural connectivity probability
    constexpr size_t MAX_SPIKE_HISTORY = 100;            // Spike recording window
    
    // Learning and adaptation
    constexpr float NOISE_VARIANCE = 0.001f;             // Synaptic noise level
    constexpr float EXPLORATION_BONUS = 0.1f;           // Exploration reward bonus
    
    // Random seed constants
    constexpr uint64_t DEFAULT_RANDOM_SEED = 42;        // Seed for reproducible experiments
    constexpr uint64_t SYSTEM_RANDOM_SEED = 1;          // System initialization seed
    
    // Time constants
    constexpr float NEURON_TIME_CONSTANT = 20.0f;       // Membrane time constant (ms)
    constexpr float ADAPTATION_TIME_CONSTANT = 5000.0f; // Adaptation decay time (ms)
}

// Experimental parameters
namespace Experiment {
    constexpr size_t CONNECTION_TEST_NEURONS = 10;      // Neurons to test in connectivity test
    constexpr size_t STDP_TEST_STEPS = 1000;             // STDP learning duration
    constexpr size_t PLASTICITY_TEST_STEPS = 1000;       // Plasticity experiment duration
    constexpr size_t LEARNING_EXPERIMENT_STEPS = 1000;  // Learning experiment duration
}

// Neuromodulation parameters
namespace Neuromodulation {
    constexpr float DOPAMINE_GAIN = 0.5f;                // Dopamine modulation strength
    constexpr float NOVELTY_WEIGHT = 0.3f;                // Novelty importance in curiosity
    constexpr float PREDICTION_ERROR_THRESHOLD = 0.01f;  // Prediction error threshold
    constexpr float REWARD_PREDICTION_DECAY = 0.95f;    // Reward prediction decay
}

// Developmental stage parameters
namespace Development {
    constexpr double INITIAL_STAGE_DURATION = 60.0;       // Initial plasticity period (~1 minute)
    constexpr double CRITICAL_PERIOD_DURATION = 300.0;   // Critical period (~5 minutes)
    constexpr double MATURATION_DURATION = 900.0;        // Maturation period (~15 minutes)
    constexpr double ADULT_STAGE_START = 900.0;           // Adult stage begins (~15 minutes)
}

// Reward calculation parameters
namespace Reward {
    constexpr float IMMEDIATE_REWARD_WEIGHT = 0.5f;      // Weight of immediate rewards
    constexpr float PREDICTION_ERROR_WEIGHT = 0.3f;      // Weight of prediction error
    constexpr float NOVELTY_REWARD_WEIGHT = 0.2f;         // Weight of novelty rewards
    constexpr float ACTION_COST = 0.01f;                // Cost of taking action
}

// Action selection parameters
namespace Action {
    constexpr float MAX_ACTION_CONFIDENCE = 0.99f;      // Maximum action confidence
    constexpr float MIN_ACTION_CONFIDENCE = 0.01f;      // Minimum action confidence
    constexpr float CONSERVATIVE_BIAS = 0.8f;            // Bias towards default actions
    constexpr float EXPLORATION_EXPLOSION_THRESHOLD = 0.8f; // High curiosity leads to more exploration
}

// Memory processing parameters
namespace Memory {
    constexpr float EPISODIC_RECALL_PROBABILITY = 0.5f;  // Probability of recalling episodic memory
    constexpr float WORKING_MEMORY_DECAY_RATE = 0.95f;   // Working memory decay rate
    constexpr float CONCEPT_FORMATION_THRESHOLD = 0.5f;  // Threshold for concept formation
    constexpr float ASSOCIATION_STRENGTH_DECAY = 0.8f;  // Association strength decay
}

// Cognitive processing parameters
namespace Cognition {
    constexpr float ATTENTION_FOCUS_FACTOR = 0.5f;       // Attention focus factor
    constexpr float ATTENTION_SALIENCE_FACTOR = 0.3f;   // Attention salience factor
    constexpr float PERCEPTION_NOISE_LEVEL = 0.01f;      // Perception noise level
    constexpr float DECISION_CONFIDENCE_THRESHOLD = 0.8f; // Decision confidence threshold
}

} // namespace nlm

#endif // NLM_CONSTANTS_H
