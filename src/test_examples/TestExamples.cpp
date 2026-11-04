// Example NLM Test Suite
// Demonstrates how to use the NLM Test Framework
// This file contains example tests for the NLM configuration system

#include "test_framework/TestFramework.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <cmath>
#include <string>
#include <vector>

namespace test_examples {

// Example 1: Basic Configuration Test
void testConfigCreation() {
    nlm::Config config;
    assert(!config.has("test"));
    std::cout << "    testConfigCreation passed" << std::endl;
}

// Example 2: Configuration Set and Get Test
void testConfigSetAndGet() {
    nlm::Config config;
    
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    assert(config.has("int_value"));
    
    auto intVal = config.get<int>("int_value");
    assert(intVal.has_value());
    assert(*intVal == 42);
    
    config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
    auto doubleVal = config.get<double>("double_value");
    assert(doubleVal.has_value());
    assert(*doubleVal > 3.13 && *doubleVal < 3.15);
    
    config.set("string_value", std::string("hello"), nlm::ConfigSource::Runtime);
    auto stringVal = config.get<std::string>("string_value");
    assert(stringVal.has_value());
    assert(*stringVal == "hello");
    
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    auto boolVal = config.get<bool>("bool_value");
    assert(boolVal.has_value());
    assert(*boolVal == true);
    
    std::cout << "    testConfigSetAndGet passed" << std::endl;
}

// Example 3: Configuration GetOr Test
void testConfigGetOr() {
    nlm::Config config;
    
    int val = config.getOr<int>("missing", 100);
    assert(val == 100);
    
    config.set("existing", 50, nlm::ConfigSource::Runtime);
    val = config.getOr<int>("existing", 100);
    assert(val == 50);
    
    std::cout << "    testConfigGetOr passed" << std::endl;
}

// Example 4: Configuration Remove Test
void testConfigRemove() {
    nlm::Config config;
    
    config.set("test", 123, nlm::ConfigSource::Runtime);
    assert(config.has("test"));
    
    config.remove("test");
    assert(!config.has("test"));
    
    std::cout << "    testConfigRemove passed" << std::endl;
}

// Example 5: Configuration Keys Test
void testConfigKeys() {
    nlm::Config config;
    
    config.set("key1", 1, nlm::ConfigSource::Runtime);
    config.set("key2", 2, nlm::ConfigSource::Runtime);
    config.set("key3", 3, nlm::ConfigSource::Runtime);
    
    auto keys = config.getKeys();
    assert(keys.size() == 3);
    
    std::cout << "    testConfigKeys passed" << std::endl;
}

// Example 6: Configuration Validation Test
void testConfigValidation() {
    auto config = nlm::TestFramework::createTestConfig();
    
    // Test that required keys exist
    assert(config->has("random_seed"));
    assert(config->has("simulation_timestep"));
    assert(config->has("neuron_count"));
    assert(config->has("region_count"));
    
    // Test that values are valid
    auto seed = config->getOr<int64_t>("random_seed", 0);
    assert(seed >= 0);
    
    auto timestep = config->getOr<double>("simulation_timestep", 0.0);
    assert(timestep > 0.0 && timestep <= 1.0);
    
    auto neuronCount = config->getOr<int64_t>("neuron_count", 0);
    assert(neuronCount > 0);
    
    auto regionCount = config->getOr<int64_t>("region_count", 0);
    assert(regionCount > 0);
    
    auto connProb = config->getOr<double>("connection_probability", 0.0);
    assert(connProb >= 0.0 && connProb <= 1.0);
    
    std::cout << "    testConfigValidation passed" << std::endl;
}

// Example 7: Configuration Type Conversion Test
void testConfigTypeConversion() {
    nlm::Config config;
    
    // Set as string
    config.set("value", std::string("42"), nlm::ConfigSource::Runtime);
    
    // Get as different types
    auto intVal = config.get<int>("value");
    assert(intVal.has_value());
    assert(*intVal == 42);
    
    auto doubleVal = config.get<double>("value");
    assert(doubleVal.has_value());
    assert(std::abs(*doubleVal - 42.0) < 0.001);
    
    // Set as number
    config.set("count", 123, nlm::ConfigSource::Runtime);
    
    // Get as string
    auto countStr = config.get<std::string>("count");
    assert(countStr.has_value());
    assert(*countStr == "123");
    
    std::cout << "    testConfigTypeConversion passed" << std::endl;
}

// Example 8: Configuration Source Tracking Test
void testConfigSourceTracking() {
    nlm::Config config;
    
    // Set with default source
    config.set("default_value", 1, nlm::ConfigSource::Default);
    
    // Set with file source
    config.set("file_value", 2, nlm::ConfigSource::File);
    
    // Set with command line source
    config.set("cli_value", 3, nlm::ConfigSource::CommandLine);
    
    // Set with runtime source
    config.set("runtime_value", 4, nlm::ConfigSource::Runtime);
    
    // Check that values are set correctly
    assert(config.getOr<int>("default_value", 0) == 1);
    assert(config.getOr<int>("file_value", 0) == 2);
    assert(config.getOr<int>("cli_value", 0) == 3);
    assert(config.getOr<int>("runtime_value", 0) == 4);
    
    // Check that sources are tracked (implementation dependent)
    auto keys = config.getKeys();
    assert(keys.size() == 4);
    
    std::cout << "    testConfigSourceTracking passed" << std::endl;
}

// Example 9: Configuration Summary Test
void testConfigSummary() {
    nlm::Config config;
    
    // Add some test entries
    config.set("test1", 1, nlm::ConfigSource::Default);
    config.set("test2", "hello", nlm::ConfigSource::File);
    config.set("test3", 3.14, nlm::ConfigSource::CommandLine);
    config.set("test4", true, nlm::ConfigSource::Runtime);
    
    // Get summary
    std::string summary = config.summary();
    
    // Check that summary contains expected information
    assert(summary.find("Configuration") != std::string::npos);
    assert(summary.find("test1") != std::string::npos);
    assert(summary.find("test2") != std::string::npos);
    assert(summary.find("test3") != std::string::npos);
    assert(summary.find("test4") != std::string::npos);
    
    std::cout << "    testConfigSummary passed" << std::endl;
}

// Example 10: Configuration Load from Environment Test
void testConfigLoadFromEnv() {
    nlm::Config config;
    
    // Set an environment variable
    setenv("NLM_TEST_VAR", "test_value", 1);
    
    // Load from environment
    bool loaded = config.loadFromEnv("NLM_TEST_VAR");
    assert(loaded);
    
    // Check that value was loaded
    assert(config.getOr<std::string>("test_var", "") == "test_value");
    
    // Clean up
    unsetenv("NLM_TEST_VAR");
    
    std::cout << "    testConfigLoadFromEnv passed" << std::endl;
}

// Run all example tests
void runAll() {
    std::cout << "Running example tests..." << std::endl;
    
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigValidation();
    testConfigTypeConversion();
    testConfigSourceTracking();
    testConfigSummary();
    testConfigLoadFromEnv();
    
    std::cout << "All example tests passed!" << std::endl;
}

} // namespace test_examples
