#pragma once

namespace nlm {
namespace constants {

// Neural simulation constants
constexpr float DEFAULT_TIMESTEP = 0.001f;           // 1ms simulation timestep
constexpr size_t DEFAULT_REPLAY_INTERVAL = 100;      // Replay memories every 100 steps
constexpr size_t DEFAULT_CONSOLIDATION_INTERVAL = 1000; // Consolidate memories every 1000 steps
constexpr float DEFAULT_SYNAPTOGENESIS_RATE = 0.0001f;  // Synaptogenesis rate
constexpr float DEFAULT_PRUNING_RATE = 0.00001f;     // Pruning rate
constexpr float DEFAULT_REFACTORY_PERIOD_MS = 2.0f;    // Default refractory period in ms
constexpr float DEFAULT_MEMBRANE_CAPACITANCE = 1.0f;   // Membrane capacitance (nF)
constexpr float DEFAULT_TIME_CONSTANT = 20.0f;       // Membrane time constant (ms)
constexpr size_t DEFAULT_SPIKE_HISTORY_SIZE = 100;    // Maximum spike history entries
constexpr float DEFAULT_THRESHOLD = -55.0f;          // Default firing threshold (mV)
constexpr float DEFAULT_RESTORING_POTENTIAL = -70.0f; // Default resting potential (mV)
constexpr float MIN_MEMBRANE_POTENTIAL = -100.0f;    // Minimum membrane potential (mV)
constexpr float MAX_MEMBRANE_POTENTIAL = 50.0f;      // Maximum membrane potential (mV)
constexpr float ADAPTATION_DECAY_RATE = 0.95f;       // Spike-frequency adaptation decay rate
constexpr float DEFAULT_LEAK_CONDUCTANCE = 0.1f;      // Default leak conductance
constexpr float DEFAULT_EXCITATION_INHIBITION_RATIO = 1.0f; // Default E/I balance

// Developmental stage transition steps
constexpr size_t INITIAL_TO_CRITICAL_STEPS = 10;
constexpr size_t CRITICAL_TO_MATURATION_STEPS = 100;
constexpr size_t MATURATION_TO_ADULT_STEPS = 500;

// Memory system constants
constexpr float WORKING_MEMORY_DECAY_RATE = 0.99f;   // Working memory decay rate
constexpr float EPISODIC_MEMORY_MAX_ENTRIES = 1000;   // Maximum episodic memory entries
constexpr float CONSOLIDATION_THRESHOLD = 0.3f;      // Memory consolidation threshold
constexpr float EPISODE_REPLAY_COUNT = 3.0f;          // Number of episodes to replay

// Neuromodulation constants
constexpr float DOPAMINE_MAX_LEVEL = 1.0f;             // Maximum dopamine level
constexpr float CURIOUSITY_THRESHOLD = 0.5f;         // Novelty threshold for curiosity
constexpr float NOVELTY_DECAY_RATE = 0.95f;            // Novelty decay rate

// Simulation constants
constexpr size_t DEFAULT_MAX_SIMULATION_STEPS = 10000;
constexpr float DEFAULT_SIMULATION_TIME_LIMIT = 0.0f;
constexpr size_t DEFAULT_AGENT_WORLD_SIZE = 20;     // Default world dimensions

// Performance constants
constexpr size_t OPTIMIZATION_BATCH_SIZE = 100;     // Batch size for optimizations
constexpr size_t CACHE_LINE_SIZE = 64;               // Cache line size for optimization

// Development constants
constexpr float SYNAPTOGENESIS_RATE_INITIAL = 0.001f;
constexpr float PRUNING_RATE_INITIAL = 0.0001f;
constexpr float SYNAPTOGENESIS_RATE_ADULT = 0.0001f;
constexpr float PRUNING_RATE_ADULT = 0.00001f;

} // namespace constants
} // namespace nlm