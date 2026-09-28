// NLM Phase 6 Configuration Constants
// This file contains all configurable constants for the NLM brain simulation
// These values can be modified in the configuration file to tune behavior

#ifndef NLM_CONFIG_CONSTANTS_H
#define NLM_CONFIG_CONSTANTS_H

namespace nlm {

// Neural Dynamics Constants
const float DEFAULT_SIMULATION_TIMESTEP = 0.001f;      // Simulation time step in seconds
const float DEFAULT_MEMBRANE_POTENTIAL_DECAY = 0.04f;    // LIF neuron decay constant
const float DEFAULT_MEMBRANE_CAPACITANCE = 250.0f;       // Membrane capacitance
const float DEFAULT_SPIKE_THRESHOLD = -55.0f;            // Neuron spike threshold (mV)
const float DEFAULT_RESTING_POTENTIAL = -70.0f;          // Resting membrane potential

// Synaptic Connection Constants
const float DEFAULT_SYNAPSE_WEIGHT_STRENGTHEN = 0.2f;    // Maximum synaptic weight
const float DEFAULT_SYNAPSE_WEIGHT_WEAKEN = 0.1f;        // Minimum synaptic weight
const float DEFAULT_SYNAPSE_DELAY = 1.0f;                // Default synaptic delay (ms)
const float SYNAPTIC_CURRENT_SCALE = 10.0f;              // Scale factor for synaptic current

// Plasticity Rule Constants
const float DEFAULT_STDP_LTP_WEIGHT = 0.01f;             // STDP Long-Term Potentiation weight
const float DEFAULT_STDP_LTD_WEIGHT = 0.012f;            // STDP Long-Term Depression weight
const float DEFAULT_STDP_TAU = 20.0f;                    // STDP time constant (ms)
const float DEFAULT_HEBBIAN_LEARNING_RATE = 0.01f;       // Hebbian learning rate

// Structural Plasticity Constants
const float DEFAULT_SYNAPTOGENESIS_RATE = 0.0001f;      // New synapse formation rate
const float DEFAULT_PRUNING_RATE = 0.00001f;             // Synapse removal rate

// Memory System Constants
const size_t DEFAULT_WORKING_MEMORY_CAPACITY = 100;      // Working memory capacity
const size_t DEFAULT_MAX_EPISODIC_EPISODES = 1000;       // Max episodic memory episodes
const float DEFAULT_EPISODIC_REPLAY_INTERVAL = 100;      // Replay every 100 steps
const float DEFAULT_CONSOLIDATION_INTERVAL = 1000;       // Consolidate every 1000 steps

// Cognitive System Constants
const size_t DEFAULT_PLANNING_DEPTH = 5;                 // Maximum planning horizon
const float DEFAULT_ATTENTION_INHIBITION_STRENGTH = 0.5f;  // Attention inhibition
const float DEFAULT_ATTENTION_EXCITATION_STRENGTH = 1.5f;   // Attention excitation
const float DEFAULT_ATTENTION_TEMPORAL_WINDOW = 10.0f;    // Attention temporal window

// Neuromodulation Constants
const float DEFAULT_NOVELTY_THRESHOLD = 0.5f;            // Novelty detection threshold
const float DEFAULT_CURIOUSITY_STRENGTH = 0.1f;          // Curiosity motivation strength
const float DEFAULT_DOPAMINE_RELEASE_RATE = 0.05f;       // Dopamine release rate

// Development System Constants
const float DEFAULT_DEVELOPMENT_RATE = 0.01f;            // Neural development rate
const size_t DEFAULT_DEVELOPMENT_STAGES = 10;            // Number of developmental stages

// Population Distribution Constants (percentages)
const float SENSORY_NEURON_RATIO = 0.25f;                // 25% of neurons are sensory
const float INTERNAL_NEURON_RATIO = 0.5f;               // 50% of neurons are internal
const float MOTOR_NEURON_RATIO = 0.25f;                 // 25% of neurons are motor

// Network Topology Constants
const float DEFAULT_CONNECTION_PROBABILITY = 0.1f;       // Random connection probability
const size_t DEFAULT_REPLAY_FREQUENCY = 10;              // Replay every 10 steps
const size_t DEFAULT_CONSOLIDATION_FREQUENCY = 100;      // Consolidate every 100 steps

// Performance Monitoring Constants
const float DEFAULT_PERFORMANCE_SAMPLING_INTERVAL = 1.0f; // Sample performance every second
const size_t DEFAULT_MAX_PERFORMANCE_HISTORY = 10000;    // Max history size

// Validation Constants
const float MIN_SYNAPSE_WEIGHT = -1.0f;                 // Minimum allowable synapse weight
const float MAX_SYNAPSE_WEIGHT = 2.0f;                  // Maximum allowable synapse weight
const float MIN_LEARNING_RATE = 0.0001f;                // Minimum learning rate
const float MAX_LEARNING_RATE = 1.0f;                   // Maximum learning rate

// Error Handling Constants
const size_t MAX_CONFIG_RETRY_ATTEMPTS = 3;              // Max retry attempts for config operations
const float DEFAULT_VALIDATION_TOLERANCE = 1e-6f;       // Numerical validation tolerance

// Logging and Output Constants
const size_t LOG_FILE_MAX_SIZE = 10 * 1024 * 1024;       // Max log file size (10MB)
const bool ENABLE_PERFORMANCE_LOGGING = true;            // Enable performance logging
const bool ENABLE_DETAILED_LOGGING = false;             // Enable detailed operation logging

} // namespace nlm

#endif // NLM_CONFIG_CONSTANTS_H