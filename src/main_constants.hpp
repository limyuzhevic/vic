#pragma once

namespace nlm {

// Main configuration constants
struct MainConfig {
    // Current injection values for testing experiments
    static constexpr float TEST_CURRENT_INJECTION = 50.0f;
    static constexpr float PLASTICITY_EXPERIMENT_CURRENT_INJECTION = 30.0f;
    static constexpr float STDP_VERIFICATION_CURRENT_INJECTION = 60.0f;
    
    // Weight change thresholds for learning experiment
    static constexpr float WEIGHT_CHANGE_THRESHOLD_POSITIVE = 0.01f;
    static constexpr float WEIGHT_CHANGE_THRESHOLD_NEGATIVE = 0.001f;
    
    // STDP and plasticity defaults
    static constexpr float STDP_LTP_WEIGHT = 0.01f;
    static constexpr float STDP_LTD_WEIGHT = 0.012f;
    static constexpr float PLASTICITY_ADJUSTMENT = 0.01f;
    
    // Simulation duration constants
    static constexpr size_t SIMULATION_DURATION = 1000;
    static constexpr size_t CHECKPOINT_FILE_SIZE_LIMIT = 10000;
    static constexpr size_t EPISODE_CAPACITY = 1000;
    static constexpr size_t EPISODES_FOR_REPLAY = 3;
    
    // Consolidation threshold (0.0 to 1.0)
    static constexpr float CONSOLIDATION_THRESHOLD = 0.3f;
    
    // Synaptic scaling and activation thresholds
    static constexpr float SYNAPTIC_SCALING = 10.0f;
    static constexpr float ACTIVATION_THRESHOLD = 5.0f;
    static constexpr float ACTIVATION_NORMALIZATION = 20.0f;
};

} // namespace nlm
