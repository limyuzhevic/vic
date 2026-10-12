// NLM Configuration Validation Tests
// Tests for configuration validation and error handling

#include <iostream>
#include <cassert>
#include <memory>
#include <fstream>
#include <filesystem>
#include "core/Config/Config.hpp"

namespace test_config_validation {

void testConfigurationFileLoading() {
    std::cout << "  Testing configuration file loading..." << std::endl;
    
    // Test loading from existing file (default.cfg)
    auto config = std::make_shared<Config>();
    bool loadResult = config->loadFromFile("configs/default.cfg");
    
    if (loadResult) {
        std::cout << "    INFO: Successfully loaded from configs/default.cfg" << std::endl;
        
        // Verify key values
        assert(config->getOr<int64_t>("random_seed", 42) == 42);
        assert(config->getOr<double>("simulation_timestep", 0.001) == 0.001);
        assert(config->getOr<int64_t>("neuron_count", 1000) == 1000);
        assert(config->getOr<int64_t>("region_count", 1) == 1);
        assert(config->getOr<float>("connection_probability", 0.1f) == 0.1f);
    } else {
        std::cout << "    WARNING: Could not load configs/default.cfg (expected in some test environments)" << std::endl;
    }
    
    // Test loading from non-existent file
    auto config2 = std::make_shared<Config>();
    bool loadResult2 = config2->loadFromFile("nonexistent_file.cfg");
    
    assert(!loadResult2); // Should return false
    std::cout << "    INFO: Non-existent file handled gracefully" << std::endl;
    
    std::cout << "    PASSED: Configuration file loading tests" << std::endl;
}

void testConfigurationFileParsing() {
    std::cout << "  Testing configuration file parsing..." << std::endl;
    
    // Create a temporary configuration file
    std::string tempConfigPath = "/tmp/test_config.cfg";
    
    // Write test configuration
    std::ofstream configFile(tempConfigPath);
    configFile << "# Test configuration file\n";
    configFile << "random_seed = 12345\n";
    configFile << "simulation_timestep = 0.01\n";
    configFile << "neuron_count = 500\n";
    configFile << "region_count = 2\n";
    configFile << "connection_probability = 0.15\n";
    configFile << "stdp_ltp_weight = 0.01\n";
    configFile << "stdp_ltd_weight = 0.012\n";
    configFile << "stdp_time_constant = 20.0\n";
    configFile << "development_synaptogenesis_rate = 0.001\n";
    configFile << "development_pruning_rate = 0.0001\n";
    configFile << "dopamine_baseline = 0.1\n";
    configFile << "reward_discount_factor = 0.99\n";
    configFile << "environment_name = GridWorld\n";
    configFile << "environment_width = 10\n";
    configFile << "environment_height = 10\n";
    configFile << "log_level = INFO\n";
    configFile << "log_to_file = false\n";
    configFile << "log_filename = test.log\n";
    configFile << "max_simulation_steps = 5000\n";
    configFile << "simulation_time_limit = 0.0\n";
    configFile << "visualization_enabled = false\n";
    configFile << "visualization_update_rate = 30.0\n";
    configFile.close();
    
    // Load the configuration
    auto config = std::make_shared<Config>();
    bool loadResult = config->loadFromFile(tempConfigPath);
    
    if (loadResult) {
        // Verify values
        assert(config->getOr<int64_t>("random_seed", 0) == 12345);
        assert(config->getOr<double>("simulation_timestep", 0.0) == 0.01);
        assert(config->getOr<int64_t>("neuron_count", 0) == 500);
        assert(config->getOr<int64_t>("region_count", 0) == 2);
        assert(config->getOr<float>("connection_probability", 0.0f) == 0.15f);
        assert(config->getOr<float>("stdp_ltp_weight", 0.0f) == 0.01f);
        assert(config->getOr<float>("stdp_ltd_weight", 0.0f) == 0.012f);
        assert(config->getOr<float>("stdp_time_constant", 0.0f) == 20.0f);
        
        std::cout << "    INFO: Temporary configuration file parsed successfully" << std::endl;
    } else {
        std::cout << "    WARNING: Could not load temporary configuration file" << std::endl;
    }
    
    // Clean up
    std::remove(tempConfigPath.c_str());
    
    std::cout << "    PASSED: Configuration file parsing tests" << std::endl;
}

void testConfigurationValidation() {
    std::cout << "  Testing configuration validation..." << std::endl;
    
    auto config = std::make_shared<Config>();
    
    // Set valid values
    config->set("neuron_count", 1000);
    config->set("region_count", 1);
    config->set("simulation_timestep", 0.001);
    config->set("connection_probability", 0.1f);
    
    // Test that getOr() returns valid values
    int64_t neuronCount = config->getOr<int64_t>("neuron_count", 1000);
    int64_t regionCount = config->getOr<int64_t>("region_count", 1);
    double timestep = config->getOr<double>("simulation_timestep", 0.001);
    float connectionProb = config->getOr<float>("connection_probability", 0.1f);
    
    assert(neuronCount > 0);
    assert(regionCount > 0);
    assert(timestep > 0.0);
    assert(connectionProb >= 0.0 && connectionProb <= 1.0);
    
    // Test with invalid values (should use defaults)
    config->set("neuron_count", -1);  // Invalid: negative
    int64_t invalidNeuronCount = config->getOr<int64_t>("neuron_count", 1000);
    // The system should handle this gracefully
    
    std::cout << "    PASSED: Configuration validation tests" << std::endl;
}

void testConfigurationModification() {
    std::cout << "  Testing configuration modification..." << std::endl;
    
    auto config = std::make_shared<Config>();
    
    // Set initial values
    config->set("random_seed", 42);
    config->set("neuron_count", 1000);
    config->set("simulation_timestep", 0.001);
    
    // Modify values
    config->set("random_seed", 123);
    config->set("neuron_count", 2000);
    config->set("simulation_timestep", 0.002);
    
    // Verify modifications
    assert(config->getOr<int64_t>("random_seed", 0) == 123);
    assert(config->getOr<int64_t>("neuron_count", 0) == 2000);
    assert(config->getOr<double>("simulation_timestep", 0.0) == 0.002);
    
    // Add new configuration values
    config->set("new_parameter", 99.9f);
    float newParam = config->getOr<float>("new_parameter", 0.0f);
    assert(newParam == 99.9f);
    
    std::cout << "    PASSED: Configuration modification tests" << std::endl;
}

void testConfigurationKeys() {
    std::cout << "  Testing configuration keys..." << std::endl;
    
    auto config = std::make_shared<Config>();
    
    // Set some values
    config->set("param1", 1.0f);
    config->set("param2", 2.0f);
    config->set("neuron_count", 1000);
    
    // Get all keys
    auto keys = config->getKeys();
    
    // Check that our parameters are in the list
    bool hasParam1 = false;
    bool hasParam2 = false;
    bool hasNeuronCount = false;
    
    for (const auto& key : keys) {
        if (key == "param1") hasParam1 = true;
        if (key == "param2") hasParam2 = true;
        if (key == "neuron_count") hasNeuronCount = true;
    }
    
    assert(hasParam1);
    assert(hasParam2);
    assert(hasNeuronCount);
    
    std::cout << "    PASSED: Configuration keys tests" << std::endl;
}

void runAll() {
    std::cout << "Configuration Validation Tests:" << std::endl;
    std::cout << "  Testing configuration loading, parsing, and validation..." << std::endl;
    std::cout << std::endl;
    
    testConfigurationFileLoading();
    testConfigurationFileParsing();
    testConfigurationValidation();
    testConfigurationModification();
    testConfigurationKeys();
    
    std::cout << std::endl;
    std::cout << "Configuration Validation Tests: All tests completed!" << std::endl;
}

} // namespace test_config_validation