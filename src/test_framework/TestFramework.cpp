// Test Framework Implementation
// Provides comprehensive test framework for NLM Phase 2

#include "test_framework/TestFramework.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <chrono>
#include <algorithm>

// Include specific test implementations
#include "BasicConnectivityTest.hpp"
#include "PlasticityExperimentTest.hpp"
#include "StdpVerificationTest.hpp"

// Test framework implementation
TestConfig globalTestConfig;

void addTestsToRunner(TestRunner& runner) {
    // Add all available tests to the runner
    runner.addTest(std::make_shared<BasicConnectivityTest>());
    runner.addTest(std::make_shared<PlasticityExperimentTest>());
    runner.addTest(std::make_shared<StdpVerificationTest>());
}

void applyConfigToTestConfig(const nlm::Config& config, TestConfig& testConfig) {
    // Apply configuration from nlm::Config to TestConfig
    if (config.has("brain.connectivity_test")) {
        testConfig.runConnectivityTest = config.getOr<bool>("brain.connectivity_test", true);
    }
    if (config.has("brain.plasticity_test")) {
        testConfig.runPlasticityTest = config.getOr<bool>("brain.plasticity_test", true);
    }
    if (config.has("brain.stdp_test")) {
        testConfig.runStdpTest = config.getOr<bool>("brain.stdp_test", true);
    }
    
    if (config.has("brain.connectivity_neuron_count")) {
        testConfig.connectivityNeuronCount = config.getOr<size_t>("brain.connectivity_neuron_count", 10);
    }
    if (config.has("brain.connectivity_current")) {
        testConfig.connectivityCurrent = config.getOr<float>("brain.connectivity_current", 50.0f);
    }
    if (config.has("brain.connectivity_steps")) {
        testConfig.connectivitySteps = config.getOr<size_t>("brain.connectivity_steps", 50);
    }
    
    if (config.has("brain.plasticity_steps")) {
        testConfig.plasticitySteps = config.getOr<size_t>("brain.plasticity_steps", 1000);
    }
    if (config.has("brain.plasticity_neuron_count")) {
        testConfig.plasticityNeuronCount = config.getOr<size_t>("brain.plasticity_neuron_count", 20);
    }
    if (config.has("brain.plasticity_current")) {
        testConfig.plasticityCurrent = config.getOr<float>("brain.plasticity_current", 30.0f);
    }
    if (config.has("brain.plasticity_weight_threshold")) {
        testConfig.plasticityWeightThreshold = config.getOr<float>("brain.plasticity_weight_threshold", 0.01f);
    }
    if (config.has("brain.plasticity_learning_threshold")) {
        testConfig.plasticityLearningThreshold = config.getOr<float>("brain.plasticity_learning_threshold", 0.001f);
    }
    
    if (config.has("brain.stdp_trials")) {
        testConfig.stdpTrials = config.getOr<size_t>("brain.stdp_trials", 50);
    }
    if (config.has("brain.stdp_synapse_count")) {
        testConfig.stdpSynapseCount = config.getOr<size_t>("brain.stdp_synapse_count", 5);
    }
    if (config.has("brain.stdp_current")) {
        testConfig.stdpCurrent = config.getOr<float>("brain.stdp_current", 60.0f);
    }
    if (config.has("brain.stdp_delta_threshold")) {
        testConfig.stdpDeltaThreshold = config.getOr<float>("brain.stdp_delta_threshold", 0.001f);
    }
    
    if (config.has("brain.seed")) {
        testConfig.seed = config.getOr<uint64_t>("brain.seed", 42);
    }
    if (config.has("brain.timestep")) {
        testConfig.timestep = config.getOr<double>("brain.timestep", 0.001);
    }
    if (config.has("brain.reset_between_tests")) {
        testConfig.resetBetweenTests = config.getOr<bool>("brain.reset_between_tests", true);
    }
    if (config.has("brain.log_detailed_results")) {
        testConfig.logDetailedResults = config.getOr<bool>("brain.log_detailed_results", false);
    }
    if (config.has("brain.enable_progress_reporting")) {
        testConfig.enableProgressReporting = config.getOr<bool>("brain.enable_progress_reporting", true);
    }
}

// Helper function to join strings with delimiter
std::string joinStrings(const std::vector<std::string>& strings, const std::string& delimiter) {
    std::ostringstream oss;
    for (size_t i = 0; i < strings.size(); ++i) {
        if (i > 0) oss << delimiter;
        oss << strings[i];
    }
    return oss.str();
}
