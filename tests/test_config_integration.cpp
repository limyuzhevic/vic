// Configuration Integration Test
// Tests the new configuration system with centralized constants and key consistency

#include "core/Config/Config.hpp"
#include "core/Config/ConfigInitializer.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>

namespace test_config_integration {

void testConfigInitializerExists() {
    std::cout << "Testing configuration initializer exists..." << std::endl;
    
    // Test that ConfigInitializer.hpp is properly included
    auto config = std::make_shared<nlm::Config>();
    
    // Test that we can initialize with defaults
    nlm::initializeConfigWithDefaults(config);
    
    // Verify that key constants are accessible
    assert(config->has(nlm::ConfigKeys::RANDOM_SEED));
    assert(config->has(nlm::ConfigKeys::NEURON_COUNT));
    assert(config->has(nlm::ConfigKeys::SYNAPTOGENESIS_RATE));
    
    std::cout << "    ✓ Configuration initializer works" << std::endl;
}

void testKeyConsistency() {
    std::cout << "Testing key consistency between code and defaults..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    nlm::initializeConfigWithDefaults(config);
    
    // Verify that all expected keys are present with correct types
    auto randomSeed = config->getOr<int64_t>(nlm::ConfigKeys::RANDOM_SEED, 0);
    assert(randomSeed == nlm::DefaultConfig::RANDOM_SEED);
    
    auto neuronCount = config->getOr<int64_t>(nlm::ConfigKeys::NEURON_COUNT, 0);
    assert(neuronCount == nlm::DefaultConfig::NEURON_COUNT);
    
    auto connectionProbability = config->getOr<float>(nlm::ConfigKeys::CONNECTION_PROBABILITY, 0.0f);
    assert(connectionProbability == nlm::DefaultConfig::CONNECTION_PROBABILITY);
    
    auto synaptogenesisRate = config->getOr<float>(nlm::ConfigKeys::SYNAPTOGENESIS_RATE, 0.0f);
    assert(synaptogenesisRate == nlm::DefaultConfig::SYNAPTOGENESIS_RATE);
    
    // Verify that problematic old keys are not present (or are mapped)
    // The old "development_synaptogenesis_rate" is now handled by the new key
    
    std::cout << "    ✓ Key consistency verified" << std::endl;
}

void testConfigParsingImproved() {
    std::cout << "Testing improved configuration parsing..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    
    // Create a test config file with various data types
    std::string testConfig = R"(
# Test configuration
random_seed = 12345
neuron_count = 500
region_count = 2
connection_probability = 0.25
simulation_timestep = 0.002
synaptogenesis_rate = 0.0005
pruning_rate = 0.00005
log_level = INFO
log_to_file = true
max_simulation_steps = 5000
""";
    
    // Write test config file
    std::ofstream testFile("test_config.cfg");
    testFile << testConfig;
    testFile.close();
    
    // Load configuration
    bool loaded = config->loadFromFile("test_config.cfg");
    assert(loaded);
    
    // Verify parsed values
    auto randomSeed = config->getOr<int64_t>("random_seed", 0);
    assert(randomSeed == 12345);
    
    auto neuronCount = config->getOr<int64_t>("neuron_count", 0);
    assert(neuronCount == 500);
    
    auto connectionProbability = config->getOr<float>("connection_probability", 0.0f);
    assert(std::abs(connectionProbability - 0.25f) < 0.0001f);
    
    auto simulationTimestep = config->getOr<double>("simulation_timestep", 0.0);
    assert(std::abs(simulationTimestep - 0.002) < 0.000001);
    
    auto logToFile = config->getOr<bool>("log_to_file", false);
    assert(logToFile == true);
    
    // Clean up test file
    std::remove("test_config.cfg");
    
    std::cout << "    ✓ Improved configuration parsing works" << std::endl;
}

void testPhase2SpecificValues() {
    std::cout << "Testing Phase 2 specific configuration values..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    nlm::initializeConfigWithDefaults(config);
    
    // Set Phase 2 specific values using new config keys
    config->set(nlm::ConfigKeys::STDP_LTP_WEIGHT, 0.02f);
    config->set(nlm::ConfigKeys::STDP_LTD_WEIGHT, 0.015f);
    config->set(nlm::ConfigKeys::STDP_TAU, 20.0f);
    config->set(nlm::ConfigKeys::SYNAPTOGENESIS_RATE, 0.0001f);
    config->set(nlm::ConfigKeys::PRUNING_RATE, 0.00001f);
    
    // Verify the values were set correctly
    auto stdpLtpWeight = config->getOr<float>(nlm::ConfigKeys::STDP_LTP_WEIGHT, 0.0f);
    assert(std::abs(stdpLtpWeight - 0.02f) < 0.0001f);
    
    auto stdpLtdWeight = config->getOr<float>(nlm::ConfigKeys::STDP_LTD_WEIGHT, 0.0f);
    assert(std::abs(stdpLtdWeight - 0.015f) < 0.0001f);
    
    auto stdpTau = config->getOr<float>(nlm::ConfigKeys::STDP_TAU, 0.0f);
    assert(std::abs(stdpTau - 20.0f) < 0.0001f);
    
    std::cout << "    ✓ Phase 2 specific values set correctly" << std::endl;
}

void testConfigSchemaExists() {
    std::cout << "Testing configuration schema exists..." << std::endl;
    
    // The schema is defined in ConfigInitializer.hpp
    // We can verify it's accessible by including it
    std::vector<nlm::ConfigSchema::Entry> schema = nlm::ConfigSchema::getSchema();
    
    // Verify that schema contains expected entries
    bool foundRandomSeed = false;
    bool foundNeuronCount = false;
    bool foundConnectionProbability = false;
    
    for (const auto& entry : schema) {
        if (entry.key == nlm::ConfigKeys::RANDOM_SEED) {
            foundRandomSeed = true;
            assert(entry.type == "int64");
            assert(entry.required == false);
        } else if (entry.key == nlm::ConfigKeys::NEURON_COUNT) {
            foundNeuronCount = true;
            assert(entry.type == "int64");
        } else if (entry.key == nlm::ConfigKeys::CONNECTION_PROBABILITY) {
            foundConnectionProbability = true;
            assert(entry.type == "float");
        }
    }
    
    assert(foundRandomSeed);
    assert(foundNeuronCount);
    assert(foundConnectionProbability);
    
    std::cout << "    ✓ Configuration schema exists and is valid" << std::endl;
}

void runAllTests() {
    std::cout << "=== Configuration Integration Tests ===" << std::endl << std::endl;
    
    testConfigInitializerExists();
    testKeyConsistency();
    testConfigParsingImproved();
    testPhase2SpecificValues();
    testConfigSchemaExists();
    
    std::cout << std::endl;
    std::cout << "=== All Configuration Integration Tests Passed ===" << std::endl;
}

} // namespace test_config_integration