#pragma once

namespace nlm {

// Brain configuration constants
struct BrainConstants {
    // Simulation parameters
    static constexpr double TIMESTEP = 0.001;  // ms
    
    // Synaptic current scaling factor for delayed spike delivery
    static constexpr float SYNAPTIC_CURRENT_SCALE = 10.0f;
    
    // Time multiplier for spike detection window
    static constexpr float TIMESTEP_MULTIPLIER = 20.0f;
    
    // Normalizer for neuron activation level calculation
    static constexpr float NORMALIZATION_DIVISOR = 10.0f;
    
    // Dopamine modulation factor for neuronal excitability
    static constexpr float DOPAMINE_EXCITABILITY_MOD = 0.5f;
    
    // Plasticity adjustment factor for weight updates
    static constexpr float PLASTICITY_ADJUSTMENT = 0.001f;
    
    // Interval between keep-alive checkpoints
    static constexpr size_t KEEP_ALIVE_INTERVAL = 100;
    
    // STDP development factor for plasticity modulation
    static constexpr float DEVELOPMENT_STDP_FACTOR1 = 0.01f;
    static constexpr float DEVELOPMENT_STDP_FACTOR2 = 0.012f;
    
    // Simulation timestep multiplier for development updates
    static constexpr double DEVELOPMENT_TIMESTEP_MULTIPLIER = 1000.0;
    
    // Maximum number of entries in checkpoint files
    static constexpr size_t CHECKPOINT_MAX_ENTRIES = 10000;
    
    // Maximum capacity for episodic memory
    static constexpr size_t EPISODE_CAPACITY = 1000;
    
    // Number of episodes to replay during memory consolidation
    static constexpr size_t REPLAY_EPISODES = 3;
    
    // Threshold for memory consolidation (0.0 to 1.0)
    static constexpr float CONSOLIDATION_THRESHOLD = 0.3f;
    
    // Synaptic scaling factor for weight normalization
    static constexpr float SYNAPTIC_SCALING = 10.0f;
    
    // Activation threshold for neural competition
    static constexpr float ACTIVATION_THRESHOLD = 5.0f;
    
    // Normalization factor for activation levels
    static constexpr float ACTIVATION_NORMALIZATION = 20.0f;
};

} // namespace nlm
