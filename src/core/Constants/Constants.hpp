// Platform-independent constants for NLM
#ifndef NLM_CONSTANTS_HPP
#define NLM_CONSTANTS_HPP

#include <cstdint>

namespace nlm {

namespace Constants {
    // Simulation constants
    constexpr double DEFAULT_TIMESTEP = 0.001;      // 1ms simulation timestep
    constexpr double MAX_TIMESTEP = 0.01;           // 10ms maximum timestep
    constexpr SimulationStep DEFAULT_MAX_STEPS = 10000;
    
    // Neural constants
    constexpr int DEFAULT_NEURON_COUNT = 1000;      // Default number of neurons
    constexpr int DEFAULT_REGION_COUNT = 1;         // Default number of regions
    constexpr NeuronId DEFAULT_REGION_ID = 1;       // First region ID
    
    // Connectivity constants
    constexpr float DEFAULT_CONNECTION_PROBABILITY = 0.1f;  // 10% connection probability
    constexpr float DEFAULT_EXCITATORY_WEIGHT = 0.2f;       // Default excitatory synaptic weight
    constexpr float DEFAULT_INHIBITORY_WEIGHT = -0.1f;      // Default inhibitory synaptic weight
    
    // Plasticity constants
    constexpr float DEFAULT_STDP_LTP_WEIGHT = 0.02f;        // STDP LTP weight
    constexpr float DEFAULT_STDP_LTD_WEIGHT = 0.015f;       // STDP LTD weight
    constexpr float DEFAULT_STDP_TAU = 20.0f;              // STDP time constant (ms)
    
    constexpr float DEFAULT_HEBBIAN_LEARNING_RATE = 0.01f;  // Hebbian learning rate
    constexpr float DEFAULT_HEBBIAN_THRESHOLD = 0.5f;       // Hebbian activation threshold
    
    // Structural plasticity
    constexpr float DEFAULT_SYNAPTAGENESIS_RATE = 0.0001f;  // Rate of new synapse formation
    constexpr float DEFAULT_PRUNING_RATE = 0.00001f;         // Rate of synapse removal
    
    // Neuromodulation ranges
    constexpr float DEFAULT_DOPAMINE_RANGE = 2.0f;           // Dopamine signal range
    constexpr float DEFAULT_NOVITY_THRESHOLD = 0.3f;        // Novelty detection threshold
    constexpr float DEFAULT_CURIOUSITY_THRESHOLD = 0.5f;    // Curiosity threshold for exploration
    
    // Memory constants
    constexpr size_t DEFAULT_WORKING_MEMORY_CAPACITY = 1000;    // Working memory capacity
    constexpr size_t DEFAULT_MAX_EPISODIC_EPISODES = 1000;      // Max episodic episodes
    constexpr size_t DEFAULT_ASSOCIATIVE_MEMORY_SIZE = 500;     // Associative memory size
    
    // Development constants
    constexpr size_t DEVELOPMENT_STAGE_INITIAL = 0;         // Initial developmental stage
    constexpr size_t DEVELOPMENT_STAGE_CRITICAL_PERIOD = 1; // Critical period stage
    constexpr size_t DEVELOPMENT_STAGE_MATURATION = 2;       // Maturation stage
    constexpr size_t DEVELOPMENT_STAGE_ADULT = 3;            // Adult stage
    
    // Action constants
    constexpr size_t DEFAULT_ACTION_SPACE_SIZE = 6;           // Number of motor actions
    constexpr float DEFAULT_ACTION_THRESHOLD = 0.5f;         // Action threshold
    
    // Timer and timing
    constexpr Timestamp DEFAULT_REPLAY_INTERVAL = 100;       // Replay interval in steps
    constexpr Timestamp DEFAULT_CONSOLIDATION_INTERVAL = 1000; // Consolidation interval
    
    // Quality of life constants
    constexpr float CLAMP_MIN = 0.0f;                         // Minimum value for clamping
    constexpr float CLAMP_MAX = 1.0f;                         // Maximum value for clamping
    constexpr float EPSILON = 1e-6f;                          // Small value for comparisons
}

} // namespace nlm

#endif // NLM_CONSTANTS_HPP
