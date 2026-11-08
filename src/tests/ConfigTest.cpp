// ConfigTest.cpp
// Configuration validation tests

#include "ConfigTest.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <string>

namespace test_config {

void testDuplicateDopamineBaselineFix() {
    std::cout << "=== Testing Duplicate Dopamine Baseline Fix ===" << std::endl;
    
    // Test 1: Verify no duplicate dopamine_baseline configuration
    nlm::Config config;
    
    // Set up configuration with dopamine_baseline (should not have duplicates)
    config.set("dopamine_baseline", 0.5f, nlm::ConfigSource::Runtime);
    
    // Verify configuration is properly set
    assert(config.has("dopamine_baseline"));
    
    // Get and verify value
    auto baseline = config.get<double>("dopamine_baseline");
    assert(baseline.has_value());
    assert(std::abs(*baseline - 0.5f) < 0.001f);
    
    std::cout << "    Dopamine baseline configuration: " << *baseline << std::endl;
    
    // Test 2: Verify configuration system handles duplicates properly
    config.set("dopamine_baseline", 0.7f, nlm::ConfigSource::Runtime);
    
    // Should overwrite existing value, not create duplicate
    assert(config.has("dopamine_baseline"));
    
    auto newBaseline = config.get<double>("dopamine_baseline");
    assert(newBaseline.has_value());
    assert(std::abs(*newBaseline - 0.7f) < 0.001f);
    
    std::cout << "    Configuration overwrite (no duplicates): PASSED" << std::endl;
    
    // Test 3: Verify configuration source tracking
    auto entry = config.getEntry("dopamine_baseline");
    assert(entry.has_value());
    assert(entry->source == nlm::ConfigSource::Runtime);
    
    std::cout << "    Configuration source tracking: PASSED" << std::endl;
    
    std::cout << "    Duplicate dopamine baseline fix test PASSED" << std::endl;
}

void testConfigurationValidation() {
    std::cout << "=== Testing Configuration Validation ===" << std::endl;
    
    // Test 1: Complete configuration validation
    nlm::Config config;
    
    // Set up a complete configuration like Phase6Config
    config.set("max_steps", 10000ULL, nlm::ConfigSource::Runtime);
    config.set("neuron_count", 1000ULL, nlm::ConfigSource::Runtime);
    config.set("region_count", 1ULL, nlm::ConfigSource::Runtime);
    config.set("connection_probability", 0.1f, nlm::ConfigSource::Runtime);
    config.set("enable_checkpointing", true, nlm::ConfigSource::Runtime);
    config.set("enable_replay", true, nlm::ConfigSource::Runtime);
    config.set("enable_development", true, nlm::ConfigSource::Runtime);
    config.set("checkpoint_path", "./checkpoint.bin", nlm::ConfigSource::Runtime);
    
    // Validate all configuration parameters
    assert(config.has("max_steps"));
    assert(config.has("neuron_count"));
    assert(config.has("region_count"));
    assert(config.has("connection_probability"));
    assert(config.has("enable_checkpointing"));
    assert(config.has("enable_replay"));
    assert(config.has("enable_development"));
    assert(config.has("checkpoint_path"));
    
    // Get and verify values
    auto maxSteps = config.get<uint64_t>("max_steps");
    auto neuronCount = config.get<size_t>("neuron_count");
    auto regionCount = config.get<size_t>("region_count");
    auto connectionProb = config.get<double>("connection_probability");
    auto enableCheckpoint = config.get<bool>("enable_checkpointing");
    auto enableReplay = config.get<bool>("enable_replay");
    auto enableDev = config.get<bool>("enable_development");
    auto checkpointPath = config.get<std::string>("checkpoint_path");
    
    assert(maxSteps.has_value() && *maxSteps == 10000ULL);
    assert(neuronCount.has_value() && *neuronCount == 1000ULL);
    assert(regionCount.has_value() && *regionCount == 1ULL);
    assert(connectionProb.has_value() && std::abs(*connectionProb - 0.1f) < 0.001f);
    assert(enableCheckpoint.has_value() && *enableCheckpoint == true);
    assert(enableReplay.has_value() && *enableReplay == true);
    assert(enableDev.has_value() && *enableDev == true);
    assert(checkpointPath.has_value() && *checkpointPath == "./checkpoint.bin");
    
    std::cout << "    Configuration validation: PASSED" << std::endl;
    
    // Test 2: Configuration loading from file
    // Create a temporary config file
    std::string configFile = "./test_config.ini";
    std::ofstream outFile(configFile);
    outFile << "# Test configuration\n";
    outFile << "max_steps = 5000\n";
    outFile << "neuron_count = 500\n";
    outFile << "dopamine_baseline = 0.3\n";
    outFile << "enable_development = true\n";
    outFile.close();
    
    bool loadSuccess = config.loadFromFile(configFile);
    assert(loadSuccess);
    
    // Verify loaded values
    auto loadedSteps = config.get<uint64_t>("max_steps");
    auto loadedDopamine = config.get<double>("dopamine_baseline");
    auto loadedDev = config.get<bool>("enable_development");
    
    assert(loadedSteps.has_value() && *loadedSteps == 5000ULL);
    assert(loadedDopamine.has_value() && std::abs(*loadedDopamine - 0.3f) < 0.001f);
    assert(loadedDev.has_value() && *loadedDev == true);
    
    std::cout << "    Configuration file loading: PASSED" << std::endl;
    
    // Cleanup
    std::remove(configFile.c_str());
    
    std::cout << "    Configuration validation test PASSED" << std::endl;
}

void testConfigurationBackwardCompatibility() {
    std::cout << "=== Testing Configuration Backward Compatibility ===" << std::endl;
    
    // Test 1: Existing configuration patterns still work
    nlm::Config config;
    
    // Old-style configuration access patterns
    config.set("old_param1", 42, nlm::ConfigSource::Default);
    config.set("old_param2", "test_string", nlm::ConfigSource::File);
    config.set("old_param3", 3.14, nlm::ConfigSource::CommandLine);
    
    // Verify old patterns still work
    assert(config.has("old_param1"));
    assert(config.has("old_param2"));
    assert(config.has("old_param3"));
    
    auto param1 = config.get<int64_t>("old_param1");
    auto param2 = config.get<std::string>("old_param2");
    auto param3 = config.get<double>("old_param3");
    
    assert(param1.has_value() && *param1 == 42);
    assert(param2.has_value() && *param2 == "test_string");
    assert(param3.has_value() && std::abs(*param3 - 3.14) < 0.001f);
    
    std::cout << "    Old configuration patterns: PASSED" << std::endl;
    
    // Test 2: Configuration removal works as expected
    config.remove("old_param1");
    assert(!config.has("old_param1"));
    
    std::cout << "    Configuration removal: PASSED" << std::endl;
    
    // Test 3: Configuration keys retrieval
    config.set("key1", 1, nlm::ConfigSource::Runtime);
    config.set("key2", 2, nlm::ConfigSource::Runtime);
    
    auto keys = config.getKeys();
    assert(keys.size() >= 2);  // At least these two
    
    std::cout << "    Configuration keys retrieval: PASSED" << std::endl;
    
    std::cout << "    Configuration backward compatibility test PASSED" << std::endl;
}

void runAllConfigTests() {
    std::cout << "=== Configuration Tests ===" << std::endl << std::endl;
    
    testDuplicateDopamineBaselineFix();
    std::cout << std::endl;
    
    testConfigurationValidation();
    std::cout << std::endl;
    
    testConfigurationBackwardCompatibility();
    std::cout << std::endl;
    
    std::cout << "=== ALL CONFIGURATION TESTS PASSED ===" << std::endl;
}

} // namespace test_config