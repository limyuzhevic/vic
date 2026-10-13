// Universal Constants
// Core constants used throughout the NLM codebase

#ifndef SRC_CORE_CONSTANTS_HPP
#define SRC_CORE_CONSTANTS_HPP

#include <cstdint>
#include <limits>

namespace nlm {

// Time constants
static constexpr double SIMULATION_TIMESTEP = 0.001;      // 1ms timestep
static constexpr double DEVELOPMENT_TIMESTEP = 0.001;     // 1ms development timestep
static constexpr double AGENT_BRAIN_DECAY_STEP = 0.001;    // Brain decay step

// Connection probabilities
static constexpr double DEFAULT_CONNECTION_PROBABILITY = 0.1f;    // 10% connection probability

// Replay and consolidation intervals
static constexpr size_t DEFAULT_REPLAY_INTERVAL = 100;        // Replay every 100 steps
static constexpr size_t DEFAULT_CONSOLIDATION_INTERVAL = 1000; // Consolidate every 1000 steps

// Memory and learning
static constexpr size_t DEFAULT_NEURON_COUNT = 1000;          // Default neuron count
static constexpr size_t MAX_SPIKE_HISTORY = 100;              // Maximum spike history per neuron

// Performance and optimization
static constexpr size_t CHECKPOINT_MAX_EPISODES = 10000;      // Maximum episodes for checkpointing
static constexpr size_t MAX_REPLAY_EPISODES = 10000;          // Maximum replay episodes

// Environment dimensions
static constexpr size_t DEFAULT_WORLD_WIDTH = 20;             // Default world width
static constexpr size_t DEFAULT_WORLD_HEIGHT = 20;           // Default world height

// Debugging and logging
static constexpr size_t MAX_DEBUG_STEPS = 10000;             // Maximum debug steps

// Brain and agent constants
static constexpr float BRAIN_EXCITATORY_WEIGHT = 5.0f;        // Excitatory input scaling
static constexpr float BRAIN_INHIBITORY_WEIGHT = 3.0f;       // Inhibitory input scaling
static constexpr float BRAIN_REFRACTORY_PERIOD = 5;          // Refractory period (steps)
static constexpr float BRAIN_LEAK_CONDUCTANCE = 10.0f;        // Leak conductance (nS)

// Voltage clamping
static constexpr float MIN_VOLTAGE_CLAMP = -100.0f;          // Minimum voltage clamp (mV)
static constexpr float MAX_VOLTAGE_CLAMP = 50.0f;           // Maximum voltage clamp (mV)

// Adaptation and decay
static constexpr float ADAPTATION_RATE = 0.01f;              // Adaptation rate
static constexpr float ADAPTATION_DECAY = 0.95f;             // Adaptation decay factor

} // namespace nlm

#endif // SRC_CORE_CONSTANTS_HPP