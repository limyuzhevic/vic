#pragma once

namespace nlm {

/**
 * Experiment constants - Centralized magic numbers and thresholds
 */
class ExperimentConstants {
public:
    // Simulation parameters
    static constexpr uint64_t DEFAULT_MAX_STEPS = 10000;
    static constexpr size_t DEFAULT_NEURON_COUNT = 1000;
    static constexpr size_t DEFAULT_REGION_COUNT = 1;
    static constexpr float DEFAULT_CONNECTION_PROBABILITY = 0.1f;
    
    // Development and plasticity
    static constexpr float DEFAULT_DEVELOPMENT_TIME_STEP = 0.001f;
    static constexpr float DEFAULT_STDP_LTP_WEIGHT = 0.01f;
    static constexpr float DEFAULT_STDP_LTD_WEIGHT = 0.012f;
    static constexpr float DEFAULT_SYNAPTOGENESIS_RATE = 0.0001f;
    static constexpr float DEFAULT_PRUNING_RATE = 0.00001f;
    
    // Memory systems
    static constexpr size_t MIN_NEURONS_FOR_MEMORY = 100;
    static constexpr size_t MAX_NEURONS_FOR_TESTING = 1000;
    static constexpr size_t MIN_EPISODES_FOR_REPLAY = 1;
    static constexpr size_t MAX_EPISODES_FOR_REPLAY = 10;
    
    // Neuromodulation
    static constexpr float DEFAULT_DOPAMINE_LEVEL = 1.0f;
    static constexpr float DEFAULT_NOVELTY_THRESHOLD = 0.5f;
    static constexpr float DEFAULT_CURIOSTY_LEVEL = 0.5f;
    
    // Checkpointing
    static constexpr const char* DEFAULT_CHECKPOINT_PATH = "./checkpoint_test.bin";
    static constexpr const char* TEMP_CHECKPOINT_PATH = "/tmp/nlm_checkpoint_test.bin";
    
    // Performance and monitoring
    static constexpr size_t STATUS_REPORT_INTERVAL = 1000;
    static constexpr float MAX_REASONABLE_FIRING_RATE = 100.0f;
    static constexpr float MIN_REASONABLE_REWARD = -10.0f;
    static constexpr float MAX_REASONABLE_REWARD = 10.0f;
    
    // Integration testing
    static constexpr size_t INTEGRATION_TEST_STEPS = 200;
    static constexpr size_t MEMORY_TEST_STEPS = 100;
    static constexpr size_t NEUROMODULATION_TEST_STEPS = 50;
    static constexpr size_t CHECKPOINT_TEST_STEPS = 100;
    
    // Validation thresholds
    static constexpr float MIN_VALID_REWARD = -100.0f;
    static constexpr float MAX_VALID_REWARD = 100.0f;
    static constexpr float MIN_VALID_FIRING_RATE = 0.0f;
    static constexpr float MAX_VALID_FIRING_RATE = 100.0f;
    static constexpr float MIN_VALID_NOVELTY = 0.0f;
    static constexpr float MAX_VALID_NOVELTY = 1.0f;
    static constexpr float MIN_VALID_CURIOSTY = 0.0f;
    static constexpr float MAX_VALID_CURIOSTY = 1.0f;
    static constexpr float MIN_VALID_DOPAMINE = 0.0f;
    static constexpr float MAX_VALID_DOPAMINE = 2.0f;
};

} // namespace nlm
