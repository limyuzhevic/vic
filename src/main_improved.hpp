// NLM - Improved Main Entry Point
// Phase 2: Real Neural Computation
// Refactored for better modularity, error handling, and maintainability

#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <chrono>
#include <memory>

#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"
#include "environment/Environment.hpp"
#include "experiments/ExperimentRunner.hpp"

namespace nlm {
namespace main {

// Constants for better maintainability
namespace Constants {
    constexpr int64_t DEFAULT_RANDOM_SEED = 42;
    constexpr double DEFAULT_TIMESTEP = 0.001;
    constexpr int64_t DEFAULT_NEURON_COUNT = 500;
    constexpr int64_t DEFAULT_REGION_COUNT = 1;
    constexpr float DEFAULT_CONNECTION_PROBABILITY = 0.15f;
    constexpr float DEFAULT_STDP_LTP_WEIGHT = 0.02f;
    constexpr float DEFAULT_STDP_LTD_WEIGHT = 0.015f;
    constexpr float DEFAULT_STDP_TAU = 20.0f;
    constexpr float DEFAULT_SYNAPTOGENESIS_RATE = 0.0001f;
    constexpr float DEFAULT_PRUNING_RATE = 0.00001f;
    constexpr SimulationStep BASIC_CONNECTIVITY_TEST_STEPS = 50;
    constexpr SimulationStep PLASTICITY_EXPERIMENT_STEPS = 1000;
    constexpr SimulationStep STDP_VERIFICATION_TRIALS = 50;
}

// Enhanced configuration management
class NLMConfig {
public:
    struct Parameters {
        int64_t random_seed = Constants::DEFAULT_RANDOM_SEED;
        double simulation_timestep = Constants::DEFAULT_TIMESTEP;
        int64_t neuron_count = Constants::DEFAULT_NEURON_COUNT;
        int64_t region_count = Constants::DEFAULT_REGION_COUNT;
        float connection_probability = Constants::DEFAULT_CONNECTION_PROBABILITY;
        float stdp_ltp_weight = Constants::DEFAULT_STDP_LTP_WEIGHT;
        float stdp_ltd_weight = Constants::DEFAULT_STDP_LTD_WEIGHT;
        float stdp_tau = Constants::DEFAULT_STDP_TAU;
        float synaptogenesis_rate = Constants::DEFAULT_SYNAPTOGENESIS_RATE;
        float pruning_rate = Constants::DEFAULT_PRUNING_RATE;
        bool enable_profiling = false;
        std::string config_file = "configs/default.cfg";
        std::string checkpoint_file = "";
        std::string log_file = "";
        int log_level = static_cast<int>(LogLevel::Info);
        bool enable_detailed_logging = true;
    };

    static Parameters loadFromFile(const std::string& filename);
    static Parameters loadFromArgs(int argc, char** argv);
    static Parameters getDefaults();
    static bool validateParameters(const Parameters& params);
    static void printConfigurationSummary(const Parameters& params);
};

// Enhanced Learning Experiment with better statistics
class LearningExperiment {
public:
    struct Results {
        size_t synapse_count = 0;
        float initial_mean_weight = 0.0f;
        float final_mean_weight = 0.0f;
        float weight_change = 0.0f;
        size_t strengthened_count = 0;
        size_t weakened_count = 0;
        size_t unchanged_count = 0;
        size_t total_spikes = 0;
        std::vector<NeuronId> most_active_neurons;
        bool learning_detected = false;
    };

    explicit LearningExperiment(std::shared_ptr<Brain> brain, uint64_t seed);
    void recordInitialState();
    void recordFinalState();
    Results computeResults() const;
    void printResults() const;
    bool learningOccurred() const;

private:
    std::shared_ptr<Brain> brain;
    uint64_t seed;
    std::vector<float> initial_weights;
    std::vector<float> final_weights;
    size_t initial_synapse_count = 0;
};

// Enhanced test runners with better error handling
class TestRunner {
public:
    static bool runBasicConnectivityTest(std::shared_ptr<Brain> brain);
    static bool runPlasticityExperiment(std::shared_ptr<Brain> brain);
    static bool runStdpVerification(std::shared_ptr<Brain> brain);
    static bool runPerformanceBenchmark(std::shared_ptr<Brain> brain, const NLMConfig::Parameters& params);
    static bool runCheckpointTest(std::shared_ptr<Brain> brain);
    static void printTestHeader(const std::string& testName);
    static void printTestResult(const std::string& testName, bool success, const std::string& message = "");
};

// System initialization and management
class NLMSystem {
public:
    static bool initializeSystem(std::shared_ptr<Config> config);
    static bool setupBrain(std::shared_ptr<Brain> brain);
    static bool setupLogger(const std::string& logFile, int logLevel);
    static void cleanupSystem();
    static void printSystemInfo();
};

// Main execution modes
enum class ExecutionMode {
    BASIC_TEST,
    PLASTICITY_TEST,
    STDP_TEST,
    FULL_TEST,
    BENCHMARK,
    CHECKPOINT_TEST,
    CONFIG_TEST
};

class NLMApplication {
public:
    NLMApplication() = default;
    ~NLMApplication() = default;
    
    bool initialize(int argc, char** argv);
    bool run();
    void cleanup();
    bool isRunning() const { return is_running_; }
    const NLMConfig::Parameters& getConfig() const { return config_; }
    
private:
    bool is_running_ = false;
    NLMConfig::Parameters config_;
    std::shared_ptr<Config> core_config_;
    std::shared_ptr<Brain> brain_;
    std::chrono::steady_clock::time_point start_time_;
    
    ExecutionMode parseExecutionMode(const std::string& modeStr);
    std::string getExecutionModeString(ExecutionMode mode) const;
    void runInMode(ExecutionMode mode);
    void printUsage() const;
    void printVersion() const;
    double getElapsedSeconds() const;
};

}}} // namespace nlm::main