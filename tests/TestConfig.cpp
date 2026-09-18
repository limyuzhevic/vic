// Test configuration implementation
// Provides configurable test parameters for improved test maintainability

#include "TestConfig.hpp"

namespace nlm {

// Global default test configuration
static TestConfig globalConfig("Global Default");

// Initialize global configuration with realistic test parameters
void initializeTestConfig() {
    TestConfig defaultConfig("Default Test Configuration");
    
    // Basic test parameters
    defaultConfig.params.tolerance = 1e-6;
    defaultConfig.params.iterations = 100;
    defaultConfig.params.steps = 10;
    defaultConfig.params.neurons = 100;
    defaultConfig.params.regions = 1;
    
    // STDP parameters
    defaultConfig.params.weightLTP = 0.01f;
    defaultConfig.params.weightLTD = 0.012f;
    defaultConfig.params.timeConstant = 20.0f;
    
    // Neuron parameters
    defaultConfig.params.learningRate = 0.01;
    defaultConfig.params.minWeight = -1.0f;
    defaultConfig.params.maxWeight = 1.0f;
    defaultConfig.params.maxFiringRate = 100.0;
    
    // Brain parameters
    defaultConfig.params.connectionProbability = 0.1;
    defaultConfig.params.synapseMinWeight = 0.05f;
    defaultConfig.params.activityThreshold = 0.001f;
    defaultConfig.params.maxSynapsesPerNeuron = 100;
    defaultConfig.params.maxTotalSynapses = 1000000;
    defaultConfig.params.useActivityDependency = true;
    
    // Timing parameters
    defaultConfig.params.timestep = 0.001;
    defaultConfig.params.duration = 1.0;
    
    // Spike pattern parameters
    defaultConfig.params.preSpikeCount = 5;
    defaultConfig.params.postSpikeCount = 5;
    defaultConfig.params.preSpikeInterval = 10.0;
    defaultConfig.params.postSpikeInterval = 10.0;
    
    // Test-specific parameters
    defaultConfig.params.plasticityTestIterations = 5;
    defaultConfig.params.brainTestSteps = 10;
    defaultConfig.params.neuronCurrent = 5.0;
    defaultConfig.params.neuronPotential = -65.0;
    
    // Validation thresholds
    defaultConfig.params.firingRateThreshold = 0.5;
    defaultConfig.params.weightChangeThreshold = 1e-3;
    defaultConfig.params.activityThreshold = 0.001;
    defaultConfig.params.minWeightThreshold = 0.05;
    
    defaultConfig.isolation.resetBefore = true;
    defaultConfig.isolation.resetAfter = true;
    
    defaultConfig.metadata.description = "Default test configuration for NLM Phase 2 tests";
    defaultConfig.metadata.tags = {"unit", "integration", "performance"};
    defaultConfig.metadata.category = "neural_computation";
    defaultConfig.metadata.version = "2.0";
    
    globalConfig = defaultConfig;
}

// Get global test configuration
TestConfig& getTestConfig() {
    return globalConfig;
}

// Set test configuration from file
bool loadTestConfigFromFile(const std::string& filepath) {
    // TODO: Implement JSON file loading for test configuration
    // This would allow users to override test parameters via config files
    return false;
}

// Print test configuration summary
void printTestConfigSummary(const TestConfig& config) {
    std::cout << "=== Test Configuration Summary ===" << std::endl;
    std::cout << "Test Name: " << config.name << std::endl;
    std::cout << "Enabled: " << (config.enabled ? "Yes" : "No") << std::endl;
    std::cout << "Scenario: " << static_cast<int>(config.scenario) << std::endl;
    std::cout << "Parameters:" << std::endl;
    std::cout << "  Tolerance: " << config.params.tolerance << std::endl;
    std::cout << "  Iterations: " << config.params.iterations << std::endl;
    std::cout << "  Neurons: " << config.params.neurons << std::endl;
    std::cout << "  Regions: " << config.params.regions << std::endl;
    std::cout << "  Learning Rate: " << config.params.learningRate << std::endl;
    std::cout << "  STDP LTP Weight: " << config.params.weightLTP << std::endl;
    std::cout << "  STDP LTD Weight: " << config.params.weightLTD << std::endl;
    std::cout << "  STDP Time Constant: " << config.params.timeConstant << std::endl;
    std::cout << "  Max Weight: " << config.params.maxWeight << std::endl;
    std::cout << "  Connection Probability: " << config.params.connectionProbability << std::endl;
    std::cout << "======================================" << std::endl;
}

// Update statistics after test execution
void updateTestStatistics(TestConfig& config, bool testPassed) {
    ++config.stats.totalTests;
    if (testPassed) {
        ++config.stats.passedTests;
    } else {
        ++config.stats.failedTests;
    }
    config.stats.totalExecutionTime += config.getExecutionTimeMs();
    ++config.execution.runCount;
}

} // namespace nlm
