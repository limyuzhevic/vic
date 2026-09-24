// NLM Configuration Constants
// This file defines all configurable parameters with their default values
// to replace hardcoded magic numbers throughout the codebase

#pragma once

#include <string>
#include "../core/Config/Config.hpp"

namespace nlm {

namespace ConfigConstants {
    // === NEURAL DYNAMICS ===
    constexpr const char* NEURON_COUNT = "neuron_count";
    constexpr const char* REGION_COUNT = "region_count";
    constexpr const char* CONNECTION_PROBABILITY = "connection_probability";
    constexpr const char* SIMULATION_TIMESTEP = "simulation_timestep";
    constexpr const char* RANDOM_SEED = "random_seed";

    // === SPANNING ===
    constexpr const char* DELAY_SCALE_FACTOR = "delay_scale_factor";
    constexpr const char* SPIKE_DELIVERY_SCALE = "spike_delivery_scale";

    // === PLASTICITY - STDP ===
    constexpr const char* STDP_LTP_WEIGHT = "stdp_ltp_weight";
    constexpr const char* STDP_LTD_WEIGHT = "stdp_ltd_weight";
    constexpr const char* STDP_TIME_CONSTANT = "stdp_time_constant";
    constexpr const char* STDP_MIN_WEIGHT = "stdp_min_weight";
    constexpr const char* STDP_MAX_WEIGHT = "stdp_max_weight";

    // === PLASTICITY - HEBBIAN ===
    constexpr const char* HEBBIAN_LEARNING_RATE = "hebbian_learning_rate";
    constexpr const char* HEBBIAN_MAX_WEIGHT = "hebbian_max_weight";
    constexpr const char* HEBBIAN_MIN_WEIGHT = "hebbian_min_weight";
    constexpr const char* HEBBIAN_CORRELATION_THRESHOLD = "hebbian_correlation_threshold";

    // === PLASTICITY - STRUCTURAL ===
    constexpr const char* SYNAPTAGENESIS_RATE = "synaptogenesis_rate";
    constexpr const char* PRUNING_RATE = "pruning_rate";
    constexpr const char* STRUCTURE_PLAP_LITICITY_SCALE = "structural_plasticity_scale";

    // === MOTOR SYSTEM ===
    constexpr const char* MOTOR_GROUP_COUNT = "motor_group_count";
    constexpr const char* MOTOR_ACTIVITY_THRESHOLD = "motor_activity_threshold";
    constexpr const char* MOTOR_EXPLORATION_FACTOR = "motor_exploration_factor";

    // === SENSORY SYSTEM ===
    constexpr const char* VISION_INPUT_SIZE = "vision_input_size";
    constexpr const char* VISION_SCALE_FACTOR = "vision_scale_factor";
    constexpr const char* TOUCH_INPUT_SIZE = "touch_input_size";
    constexpr const char* TOUCH_SCALE_FACTOR = "touch_scale_factor";
    constexpr const char* INTERNAL_INPUT_SIZE = "internal_input_size";
    constexpr const char* INTERNAL_SCALE_FACTOR = "internal_scale_factor";
    constexpr const char* PROPRIOCEPTION_INPUT_SIZE = "proprioception_input_size";
    constexpr const char* PROPRIOCEPTION_SCALE_FACTOR = "proprioception_scale_factor";

    // === NEUROMODULATION ===
    constexpr const char* DOPAMINE_PEAK_LEVEL = "dopamine_peak_level";
    constexpr const char* DOPAMINE_DECAY_RATE = "dopamine_decay_rate";
    constexpr const char* DOPAMINE_RELEASE_RATE = "dopamine_release_rate";
    constexpr const char* NEUROMODULATION_LTP_MODIFIER = "neuromodulation_ltp_modifier";
    constexpr const char* NEUROMODULATION_LTD_MODIFIER = "neuromodulation_ltd_modifier";

    // === DEVELOPMENT ===
    constexpr const char* DEVELOPMENT_PLATENCY_MODIFIER = "development_plasticity_modifier";
    constexpr const char* DEVELOPMENT_STAGE_DURATION_INITIAL = "development_stage_duration_initial";
    constexpr const char* DEVELOPMENT_STAGE_DURATION_CRITICAL = "development_stage_duration_critical";
    constexpr const char* DEVELOPMENT_STAGE_DURATION_MATURATION = "development_stage_duration_maturation";
    constexpr const char* DEVELOPMENT_STAGE_DURATION_ADULT = "development_stage_duration_adult";

    // === MEMORY SYSTEMS ===
    constexpr const char* WORKING_MEMORY_CAPACITY = "working_memory_capacity";
    constexpr const char* EPISODIC_MEMORY_MAX_EPISODES = "episodic_memory_max_episodes";
    constexpr const char* EPISODIC_MEMORY_REPLAY_INTERVAL = "episodic_memory_replay_interval";
    constexpr const char* EPISODIC_MEMORY_CONSOLIDATION_INTERVAL = "episodic_memory_consolidation_interval";
    constexpr const char* EPISODIC_MEMORY_CONSOLIDATION_THRESHOLD = "episodic_memory_consolidation_threshold";

    // === COGNITION SYSTEMS ===
    constexpr const char* PLANNER_PLANNING_DEPTH = "planner_planning_depth";
    constexpr const char* PLANNER_EXPLORATION_WEIGHT = "planner_exploration_weight";
    constexpr const char* ATTENTION_INHIBITION_STRENGTH = "attention_inhibition_strength";
    constexpr const char* ATTENTION_EXCITATION_STRENGTH = "attention_excitation_strength";

    // === EXPERIMENTS AND TESTING ===
    constexpr const char* TEST_INPUT_STRENGTH = "test_input_strength";
    constexpr const char* TEST_PLASTICITY_THRESHOLD = "test_plasticity_threshold";
    constexpr const char* TEST_STDP_CORRELATION_WINDOW = "test_stdp_correlation_window";
    constexpr const char* TEST_STEP_COUNT = "test_step_count";
    constexpr const char* TEST_BENCHMARK_ITERATIONS = "test_benchmark_iterations";

    // === CHECKPOINTING ===
    constexpr const char* CHECKPOINT_DIR = "checkpoint_dir";
    constexpr const char* CHECKPOINT_MAX_FILES = "checkpoint_max_files";
    constexpr const char* CHECKPOINT_COMPRESSION_LEVEL = "checkpoint_compression_level";
    constexpr const char* CHECKPOINT_AUTO_SAVE_INTERVAL = "checkpoint_auto_save_interval";

    // === ADVANCED OPTIONS ===
    constexpr const char* ENABLE_DEBUG_LOGGING = "enable_debug_logging";
    constexpr const char* ENABLE_PERFORMANCE_LOGGING = "enable_performance_logging";
    constexpr const char* ENABLE_VALIDATION_CHECKS = "enable_validation_checks";
    constexpr const char* RANDOM_SEED_OVERRIDE = "random_seed_override";
}

// Function to apply default configuration to a Config instance
inline void applyDefaultConfiguration(std::shared_ptr<Config> config) {
    if (!config) return;

    // === NEURAL DYNAMICS ===
    config->set(ConfigConstants::NEURON_COUNT, 500, ConfigSource::Default);
    config->set(ConfigConstants::REGION_COUNT, 1, ConfigSource::Default);
    config->set(ConfigConstants::CONNECTION_PROBABILITY, 0.15f, ConfigSource::Default);
    config->set(ConfigConstants::SIMULATION_TIMESTEP, 0.001, ConfigSource::Default);
    config->set(ConfigConstants::RANDOM_SEED, static_cast<int64_t>(42), ConfigSource::Default);

    // === SPANNING ===
    config->set(ConfigConstants::DELAY_SCALE_FACTOR, 10.0f, ConfigSource::Default);
    config->set(ConfigConstants::SPIKE_DELIVERY_SCALE, 10.0f, ConfigSource::Default);

    // === PLASTICITY - STDP ===
    config->set(ConfigConstants::STDP_LTP_WEIGHT, 0.01f, ConfigSource::Default);
    config->set(ConfigConstants::STDP_LTD_WEIGHT, 0.012f, ConfigSource::Default);
    config->set(ConfigConstants::STDP_TIME_CONSTANT, 20.0f, ConfigSource::Default);
    config->set(ConfigConstants::STDP_MIN_WEIGHT, -1.0f, ConfigSource::Default);
    config->set(ConfigConstants::STDP_MAX_WEIGHT, 1.0f, ConfigSource::Default);

    // === PLASTICITY - HEBBIAN ===
    config->set(ConfigConstants::HEBBIAN_LEARNING_RATE, 0.01f, ConfigSource::Default);
    config->set(ConfigConstants::HEBBIAN_MAX_WEIGHT, 10.0f, ConfigSource::Default);
    config->set(ConfigConstants::HEBBIAN_MIN_WEIGHT, -10.0f, ConfigSource::Default);
    config->set(ConfigConstants::HEBBIAN_CORRELATION_THRESHOLD, 100.0f, ConfigSource::Default);

    // === PLASTICITY - STRUCTURAL ===
    config->set(ConfigConstants::SYNAPTAGENESIS_RATE, 0.0001f, ConfigSource::Default);
    config->set(ConfigConstants::PRUNING_RATE, 0.00001f, ConfigSource::Default);
    config->set(ConfigConstants::STRUCTURE_PLAP_LITICITY_SCALE, 1.0f, ConfigSource::Default);

    // === MOTOR SYSTEM ===
    config->set(ConfigConstants::MOTOR_GROUP_COUNT, 6, ConfigSource::Default);
    config->set(ConfigConstants::MOTOR_ACTIVITY_THRESHOLD, 0.5f, ConfigSource::Default);
    config->set(ConfigConstants::MOTOR_EXPLORATION_FACTOR, 0.3f, ConfigSource::Default);

    // === SENSORY SYSTEM ===
    config->set(ConfigConstants::VISION_INPUT_SIZE, 256, ConfigSource::Default);
    config->set(ConfigConstants::VISION_SCALE_FACTOR, 5.0f, ConfigSource::Default);
    config->set(ConfigConstants::TOUCH_INPUT_SIZE, 8, ConfigSource::Default);
    config->set(ConfigConstants::TOUCH_SCALE_FACTOR, 8.0f, ConfigSource::Default);
    config->set(ConfigConstants::INTERNAL_INPUT_SIZE, 4, ConfigSource::Default);
    config->set(ConfigConstants::INTERNAL_SCALE_FACTOR, 5.0f, ConfigSource::Default);
    config->set(ConfigConstants::PROPRIOCEPTION_INPUT_SIZE, 6, ConfigSource::Default);
    config->set(ConfigConstants::PROPRIOCEPTION_SCALE_FACTOR, 3.0f, ConfigSource::Default);

    // === NEUROMODULATION ===
    config->set(ConfigConstants::DOPAMINE_PEAK_LEVEL, 1.0f, ConfigSource::Default);
    config->set(ConfigConstants::DOPAMINE_DECAY_RATE, 0.1f, ConfigSource::Default);
    config->set(ConfigConstants::DOPAMINE_RELEASE_RATE, 1.0f, ConfigSource::Default);
    config->set(ConfigConstants::NEUROMODULATION_LTP_MODIFIER, 0.01f, ConfigSource::Default);
    config->set(ConfigConstants::NEUROMODULATION_LTD_MODIFIER, 0.012f, ConfigSource::Default);

    // === DEVELOPMENT ===
    config->set(ConfigConstants::DEVELOPMENT_PLATENCY_MODIFIER, 1.0f, ConfigSource::Default);
    config->set(ConfigConstants::DEVELOPMENT_STAGE_DURATION_INITIAL, 60.0, ConfigSource::Default);
    config->set(ConfigConstants::DEVELOPMENT_STAGE_DURATION_CRITICAL, 300.0, ConfigSource::Default);
    config->set(ConfigConstants::DEVELOPMENT_STAGE_DURATION_MATURATION, 900.0, ConfigSource::Default);
    config->set(ConfigConstants::DEVELOPMENT_STAGE_DURATION_ADULT, 3600.0, ConfigSource::Default);

    // === MEMORY SYSTEMS ===
    config->set(ConfigConstants::WORKING_MEMORY_CAPACITY, 500, ConfigSource::Default);
    config->set(ConfigConstants::EPISODIC_MEMORY_MAX_EPISODES, 1000, ConfigSource::Default);
    config->set(ConfigConstants::EPISODIC_MEMORY_REPLAY_INTERVAL, 100, ConfigSource::Default);
    config->set(ConfigConstants::EPISODIC_MEMORY_CONSOLIDATION_INTERVAL, 1000, ConfigSource::Default);
    config->set(ConfigConstants::EPISODIC_MEMORY_CONSOLIDATION_THRESHOLD, 0.3f, ConfigSource::Default);

    // === COGNITION SYSTEMS ===
    config->set(ConfigConstants::PLANNER_PLANNING_DEPTH, 5, ConfigSource::Default);
    config->set(ConfigConstants::PLANNER_EXPLORATION_WEIGHT, 0.5f, ConfigSource::Default);
    config->set(ConfigConstants::ATTENTION_INHIBITION_STRENGTH, 0.5f, ConfigSource::Default);
    config->set(ConfigConstants::ATTENTION_EXCITATION_STRENGTH, 1.5f, ConfigSource::Default);

    // === EXPERIMENTS AND TESTING ===
    config->set(ConfigConstants::TEST_INPUT_STRENGTH, 30.0f, ConfigSource::Default);
    config->set(ConfigConstants::TEST_PLASTICITY_THRESHOLD, 0.01f, ConfigSource::Default);
    config->set(ConfigConstants::TEST_STDP_CORRELATION_WINDOW, 100.0f, ConfigSource::Default);
    config->set(ConfigConstants::TEST_STEP_COUNT, 1000, ConfigSource::Default);
    config->set(ConfigConstants::TEST_BENCHMARK_ITERATIONS, 100, ConfigSource::Default);

    // === CHECKPOINTING ===
    config->set(ConfigConstants::CHECKPOINT_DIR, "./checkpoints", ConfigSource::Default);
    config->set(ConfigConstants::CHECKPOINT_MAX_FILES, 10000, ConfigSource::Default);
    config->set(ConfigConstants::CHECKPOINT_COMPRESSION_LEVEL, 1, ConfigSource::Default); // 1=Balanced
    config->set(ConfigConstants::CHECKPOINT_AUTO_SAVE_INTERVAL, 1000, ConfigSource::Default);

    // === ADVANCED OPTIONS ===
    config->set(ConfigConstants::ENABLE_DEBUG_LOGGING, false, ConfigSource::Default);
    config->set(ConfigConstants::ENABLE_PERFORMANCE_LOGGING, false, ConfigSource::Default);
    config->set(ConfigConstants::ENABLE_VALIDATION_CHECKS, true, ConfigSource::Default);
    config->set(ConfigConstants::RANDOM_SEED_OVERRIDE, std::nullopt, ConfigSource::Default);
}

// Helper function to get float parameter with default
inline float getFloatParam(const std::shared_ptr<Config>& config, const char* key, float defaultValue) {
    if (!config) return defaultValue;
    auto val = config->getOr<float>(key, defaultValue);
    return val;
}

// Helper function to get int parameter with default
inline int getIntParam(const std::shared_ptr<Config>& config, const char* key, int defaultValue) {
    if (!config) return defaultValue;
    auto val = config->getOr<int>(key, defaultValue);
    return val;
}

// Helper function to get bool parameter with default
inline bool getBoolParam(const std::shared_ptr<Config>& config, const char* key, bool defaultValue) {
    if (!config) return defaultValue;
    auto val = config->getOr<bool>(key, defaultValue);
    return val;
}

// Helper function to get string parameter with default
inline std::string getStringParam(const std::shared_ptr<Config>& config, const char* key, const std::string& defaultValue) {
    if (!config) return defaultValue;
    auto val = config->getOr<std::string>(key, defaultValue);
    return val;
}

} // namespace ConfigConstants
