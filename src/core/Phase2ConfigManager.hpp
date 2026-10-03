// Phase 2 Test Configuration Management
#pragma once

#include "core/Config/Config.hpp"
#include "core/TestConstants.hpp"
#include <memory>
#include <string>
#include <vector>

namespace nlm {
namespace phase2 {

/**
 * Phase 2 test configuration structure
 */
struct Phase2TestConfig {
    // Simulation parameters
    uint64_t randomSeed;
    double simulationTimestep;
    uint64_t neuronCount;
    uint64_t regionCount;
    double connectionProbability;
    
    // Plasticity parameters
    float stdpLtpWeight;
    float stdpLtdWeight;
    float stdpTau;
    float synaptogenesisRate;
    float pruningRate;
    
    // Test parameters
    uint32_t plasticitySteps;
    uint32_t connectivityTestSteps;
    uint32_t stdpTestTrials;
    
    // Verbosity and output
    bool verbose;
    bool enableBenchmark;
    bool enableProfiling;
    bool saveResults;
    std::string outputFile;
    std::string checkpointFile;
    
    // Test selection
    bool runConnectivityTest;
    bool runPlasticityTest;
    bool runStdpTest;
    bool runLearningExperiment;
    
    Phase2TestConfig() : 
        randomSeed(42),
        simulationTimestep(PHASE2_SIMULATION_TIMESTEP),
        neuronCount(PHASE2_NEURON_COUNT),
        regionCount(PHASE2_REGION_COUNT),
        connectionProbability(PHASE2_CONNECTION_PROBABILITY),
        stdpLtpWeight(PHASE2_STDP_LTP_WEIGHT),
        stdpLtdWeight(PHASE2_STDP_LTD_WEIGHT),
        stdpTau(PHASE2_STDP_TAU),
        synaptogenesisRate(PHASE2_SYNAPTOGENESIS_RATE),
        pruningRate(PHASE2_PRUNING_RATE),
        plasticitySteps(PHASE2_TEST_STEPS),
        connectivityTestSteps(PHASE2_CONNECTIVITY_TEST_STEPS),
        stdpTestTrials(PHASE2_STDP_TEST_TRIALS),
        verbose(false),
        enableBenchmark(false),
        enableProfiling(false),
        saveResults(false),
        outputFile(""),
        checkpointFile(PHASE2_CHECKPOINT_FILE),
        runConnectivityTest(true),
        runPlasticityTest(true),
        runStdpTest(true),
        runLearningExperiment(true) {}
};

/**
 * Phase 2 test configuration manager
 */
class Phase2TestConfigManager {
public:
    Phase2TestConfigManager();
    ~Phase2TestConfigManager();
    
    // Disable copying
    Phase2TestConfigManager(const Phase2TestConfigManager&) = delete;
    Phase2TestConfigManager& operator=(const Phase2TestConfigManager&) = delete;
    
    // Load configuration from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Load configuration from file
    bool loadFromFile(const std::string& filepath);
    
    // Get the current configuration
    const Phase2TestConfig& getConfig() const { return config_; }
    
    // Update configuration values
    void updateConfig(const Phase2TestConfig& newConfig) { config_ = newConfig; }
    
    // Print configuration summary
    void printConfigSummary() const;
    
    // Get the global configuration manager instance
    static Phase2TestConfigManager& getInstance();
    
    // Set global configuration manager
    static void setInstance(std::shared_ptr<Phase2TestConfigManager> manager);
    
    // Convert to NLM core configuration
    std::shared_ptr<Config> toCoreConfig() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl_;
    Phase2TestConfig config_;
    
    // Internal helper functions
    void parseCommandLineArgs(int argc, char** argv);
    void applyDefaults();
    void validateConfig();
};

} // namespace phase2
} // namespace nlm