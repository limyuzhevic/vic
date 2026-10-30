#pragma once

#include <cstdint>

namespace nlm {
namespace Phase2Constants {

// Core experiment parameters
static constexpr int64_t EXPERIMENT_NEURON_COUNT = 500;
static constexpr int64_t EXPERIMENT_REGION_COUNT = 1;
static constexpr float EXPERIMENT_CONNECTION_PROBABILITY = 0.15f;
static constexpr double EXPERIMENT_TIMESTEP = 0.001;
static constexpr int64_t EXPERIMENT_RANDOM_SEED = 42;

// Plasticity parameters
static constexpr float EXPERIMENT_STDP_LTP_WEIGHT = 0.02f;
static constexpr float EXPERIMENT_STDP_LTD_WEIGHT = 0.015f;
static constexpr float EXPERIMENT_STDP_TAU = 20.0f;
static constexpr float EXPERIMENT_SYNAPTOGENESIS_RATE = 0.0001f;
static constexpr float EXPERIMENT_PRUNING_RATE = 0.00001f;

// Test 1: Basic connectivity
static constexpr size_t TEST1_NEUTRAL_INJECTION_COUNT = 10;
static constexpr float TEST1_INJECTION_CURRENT = 50.0f;
static constexpr SimulationStep TEST1_STEP_COUNT = 50;

// Test 2: Plasticity learning
static constexpr size_t TEST2_INPUT_PATTERN_SIZE = 20;
static constexpr float TEST2_INPUT_CURRENT = 30.0f;
static constexpr SimulationStep TEST2_STEP_COUNT = 1000;
static constexpr size_t TEST2_PROGRESS_LOG_INTERVAL = 100;

// Test 3: STDP verification
static constexpr size_t TEST3_SYNAPSE_TEST_COUNT = 5;
static constexpr size_t TEST3_STDP_TRIAL_COUNT = 50;
static constexpr float TEST3_STDP_PRE_CURRENT = 60.0f;
static constexpr float TEST3_STDP_POST_CURRENT = 60.0f;

// Experiment result thresholds
static constexpr float LEARNING_WEIGHT_CHANGE_THRESHOLD = 0.01f;
static constexpr float LEARNING_MEAN_WEIGHT_CHANGE_THRESHOLD = 0.001f;

// Configuration file
static constexpr const char* DEFAULT_CONFIG_FILE = "configs/default.cfg";

// Test descriptions
static constexpr const char* TEST1_DESCRIPTION = "Basic Neural Connectivity";
static constexpr const char* TEST2_DESCRIPTION = "Plasticity Learning Experiment";
static constexpr const char* TEST3_DESCRIPTION = "STDP Verification";

} // namespace Phase2Constants

namespace Phase3Constants {

// Phase 3 demo parameters
static constexpr int PHASE3_DEFAULT_EPISODES = 5;
static constexpr int PHASE3_DEFAULT_STEPS_PER_EPISODE = 200;
static constexpr int PHASE3_MAX_EPISODES = 100;
static constexpr int PHASE3_MAX_STEPS_PER_EPISODE = 1000;

// Environment configuration
static constexpr int PHASE3_ENVIRONMENT_WIDTH = 20;
static constexpr int PHASE3_ENVIRONMENT_HEIGHT = 20;
static constexpr int PHASE3_VISION_WIDTH = 16;
static constexpr int PHASE3_VISION_HEIGHT = 16;

// World configuration
static constexpr float PHASE3_WORLD_WIDTH = 20.0f;
static constexpr float PHASE3_WORLD_HEIGHT = 20.0f;
static constexpr float PHASE3_WORLD_SIMULATION_TIME = 0.001f;

// Agent configuration
static constexpr int PHASE3_AGENT_BRAIN_NEURONS = 500;
static constexpr int PHASE3_AGENT_BRAIN_REGIONS = 2;
static constexpr float PHASE3_AGENT_CONNECTION_PROBABILITY = 0.1f;

// Neural parameters
static constexpr uint64_t PHASE3_RANDOM_SEED = 42u;
static constexpr double PHASE3_SIMULATION_TIMESTEP = 0.001;
static constexpr float PHASE3_STDP_LTP_WEIGHT = 0.01f;
static constexpr float PHASE3_STDP_LTD_WEIGHT = 0.012f;
static constexpr float PHASE3_STDP_TAU = 20.0f;
static constexpr float PHASE3_SYNAPTOGENESIS_RATE = 0.0001f;
static constexpr float PHASE3_PRUNING_RATE = 0.00001f;

// Motor commands for phase 3
static constexpr const char* PHASE3_MOTOR_COMMAND_NAMES[] = {
    "MoveForward", "MoveBackward", "TurnLeft", "TurnRight",
    "LookLeft", "LookRight", "Interact", "Wait"
};

} // namespace Phase3Constants

namespace Phase4Constants {

// Phase 4 experiment parameters
static constexpr size_t PHASE4_TRIAL_COUNT = 50;
static constexpr size_t PHASE4_PLANNING_TRIAL_COUNT = 25;
static constexpr size_t PHASE4_SOCIAL_LEARNING_TRIAL_COUNT = 25;
static constexpr size_t PHASE4_CONTINUAL_LEARNING_TRIAL_COUNT = 50;

// Performance thresholds
static constexpr float PHASE4_PREDICTION_ACCURACY_THRESHOLD = 0.5f;
static constexpr float PHASE4_PREDICTION_ERROR_THRESHOLD = 0.3f;
static constexpr float PHASE4_PLANNING_ACCURACY_THRESHOLD = 0.4f;
static constexpr float PHASE4_PLANNING_CONFIDENCE_THRESHOLD = 0.5f;
static constexpr float PHASE4_SELF_PREDICTION_ACCURACY_THRESHOLD = 0.3f;
static constexpr float PHASE4_BODY_AWARENESS_THRESHOLD = 0.5f;
static constexpr float PHASE4_IMITATION_ACCURACY_THRESHOLD = 0.2f;
static constexpr float PHASE4_CONCEPT_STABILITY_THRESHOLD = 0.7f;
static constexpr float PHASE4_GENERALIZATION_ABILITY_THRESHOLD = 0.6f;
static constexpr float PHASE4_ATTENTION_SELECTIVITY_THRESHOLD = 0.5f;
static constexpr float PHASE4_DISTRACTION_RESISTANCE_THRESHOLD = 0.6f;
static constexpr float PHASE4_WORKING_MEMORY_RETENTION_THRESHOLD = 0.7f;
static constexpr float PHASE4_WORKING_MEMORY_CAPACITY = 100.0f;
static constexpr float PHASE4_EPISODIC_RECALL_ACCURACY_THRESHOLD = 0.8f;
static constexpr float PHASE4_CONCEPTS_FORMED_THRESHOLD = 1;
static constexpr float PHASE4_BEHAVIOR_IMPROVEMENT_THRESHOLD = 0.5f;
static constexpr float PHASE4_TRANSFER_PERFORMANCE_THRESHOLD = 0.6f;

// Timing constants
static constexpr float PHASE4_EXPERIMENT_DURATION = 60.0f;
static constexpr float PHASE4_MEASUREMENT_INTERVAL = 10.0f;

} // namespace Phase4Constants

namespace Phase6Constants {

// Phase 6 integration experiment parameters
static constexpr int64_t PHASE6_NEURON_COUNT = 500;
static constexpr int64_t PHASE6_REGION_COUNT = 1;
static constexpr float PHASE6_CONNECTION_PROBABILITY = 0.15f;
static constexpr uint64_t PHASE6_MAX_STEPS = 2000;
static constexpr uint64_t PHASE6_CHECKPOINT_INTERVAL = 500;
static constexpr float PHASE6_CHECKPOINT_TIMESTEP = 1.0f;

// Integration test thresholds
static constexpr float PHASE6_INTEGRATION_SUCCESS_RATE = 0.95f;
static constexpr float PHASE6_PERFORMANCE_THRESHOLD = 0.8f;

// Checkpoint and replay
static constexpr const char* PHASE6_DEFAULT_CHECKPOINT_PATH = "/tmp/nlm_checkpoint.bin";
static constexpr const char* PHASE6_DEFAULT_REPLAY_PATH = "/tmp/nlm_replay.bin";
static constexpr float PHASE6_REPLAY_FRACTION = 0.3f;

// Development constants
static constexpr float PHASE6_DEVELOPMENT_ENABLE_AT_STEP = 1000;
static constexpr float PHASE6_DEVELOPMENT_RATE = 0.001f;

} // namespace Phase6Constants

namespace ExperimentLimits {

// Global limits across all experiments
static constexpr size_t MAX_BRAIN_REGIONS = 16;
static constexpr size_t MIN_BRAIN_REGIONS = 1;
static constexpr float MIN_CONNECTION_PROBABILITY = 0.001f;
static constexpr float MAX_CONNECTION_PROBABILITY = 1.0f;
static constexpr double MIN_TIMESTEP = 0.0001;
static constexpr double MAX_TIMESTEP = 1.0;
static constexpr int64_t MIN_NEURONS = 10;
static constexpr int64_t MAX_NEURONS = 100000;
static constexpr float MIN_STDP_WEIGHT = 0.001;
static constexpr float MAX_STDP_WEIGHT = 0.5;
static constexpr float MIN_SYNAPTOGENESIS_RATE = 0.000001;
static constexpr float MAX_SYNAPTOGENESIS_RATE = 0.1;
static constexpr float MIN_PRUNING_RATE = 0.0000001;
static constexpr float MAX_PRUNING_RATE = 0.01;

} // namespace ExperimentLimits

namespace LoggingConstants {

// Logging parameters
static constexpr const char* LOG_FILENAME = "nlm.log";
static constexpr size_t LOG_MAX_FILE_SIZE = 10 * 1024 * 1024; // 10MB
static constexpr size_t LOG_MAX_MESSAGES = 10000;
static constexpr const char* LOG_DATE_FORMAT = "%Y-%m-%d %H:%M:%S";
static constexpr const char* LOG_CONSOLE_FORMAT = "[%level] [%time] %message";
static constexpr const char* LOG_FILE_FORMAT = "%level %time %message";

// Console output formatting
static constexpr int CONSOLE_WIDTH = 80;
static constexpr int CONSOLE_SECTION_WIDTH = 60;
static constexpr int CONSOLE_TIMESTAMP_WIDTH = 12;
static constexpr const char* CONSOLE_SEPARATOR = "============================================================";

} // namespace LoggingConstants

namespace BenchmarkConstants {

// Performance benchmarking
static constexpr double BENCHMARK_MIN_RUNTIME = 0.1;
static constexpr double BENCHMARK_MAX_RUNTIME = 300.0;
static constexpr size_t BENCHMARK_SAMPLE_COUNT = 10;
static constexpr double BENCHMARK_CPU_USAGE_THRESHOLD = 0.8;
static constexpr size_t BENCHMARK_MEMORY_THRESHOLD_MB = 2048;

// Benchmark modes
static constexpr const char* BENCHMARK_MODE_SIMPLE = "simple";
static constexpr const char* BENCHMARK_MODE_COMPREHENSIVE = "comprehensive";
static constexpr const char* BENCHMARK_MODE_STRESS = "stress";

} // namespace BenchmarkConstants

namespace PluginConstants {

// Plugin system
static constexpr const char* PLUGIN_EXTENSION = ".nlmplug";
static constexpr const char* PLUGIN_INTERFACE_VERSION = "1.0.0";
static constexpr size_t MAX_PLUGINS = 50;
static constexpr size_t MAX_PLUGIN_NAME_LENGTH = 64;

// Experiment plugin types
static constexpr const char* PLUGIN_TYPE_DEMO = "demo";
static constexpr const char* PLUGIN_TYPE_EXPERIMENT = "experiment";
static constexpr const char* PLUGIN_TYPE_VISUALIZATION = "visualization";
static_cast<SimulationStep>(1000)

} // namespace PluginConstants

// Helper functions for constants
namespace Constants {
    inline bool isValidNeuronCount(int64_t count) {
        return count >= ExperimentLimits::MIN_NEURONS && count <= ExperimentLimits::MAX_NEURONS;
    }
    
    inline bool isValidRegionCount(size_t regions) {
        return regions >= ExperimentLimits::MIN_BRAIN_REGIONS && regions <= ExperimentLimits::MAX_BRAIN_REGIONS;
    }
    
    inline bool isValidConnectionProbability(float prob) {
        return prob >= ExperimentLimits::MIN_CONNECTION_PROBABILITY && prob <= ExperimentLimits::MAX_CONNECTION_PROBABILITY;
    }
    
    inline bool isValidTimestep(double timestep) {
        return timestep >= ExperimentLimits::MIN_TIMESTEP && timestep <= ExperimentLimits::MAX_TIMESTEP;
    }
    
    inline bool isValidSTDPWeight(float weight) {
        return weight >= ExperimentLimits::MIN_STDP_WEIGHT && weight <= ExperimentLimits::MAX_STDP_WEIGHT;
    }
    
    inline bool isValidPlasticityRate(float rate) {
        return rate >= ExperimentLimits::MIN_SYNAPTOGENESIS_RATE && rate <= ExperimentLimits::MAX_SYNAPTOGENESIS_RATE;
    }

} // namespace Constants

} // namespace nlm