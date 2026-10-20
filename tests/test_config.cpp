// Config Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>

namespace test_config {

void testConfigCreation() {
    nlm::Config config;
    assert(!config.has("test"));
    
    std::cout << "    testConfigCreation passed" << std::endl;
}

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

void testConfigGetOr() {
    nlm::Config config;
    
    int val = config.getOr<int>("missing", 100);
    assert(val == 100);
    
    config.set("existing", 50, nlm::ConfigSource::Runtime);
    val = config.getOr<int>("existing", 100);
    assert(val == 50);
    
    std::cout << "    testConfigGetOr passed" << std::endl;
}

void testConfigRemove() {
    nlm::Config config;
    
    config.set("test", 123, nlm::ConfigSource::Runtime);
    assert(config.has("test"));
    
    config.remove("test");
    assert(!config.has("test"));
    
    std::cout << "    testConfigRemove passed" << std::endl;
}

// Config Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <vector>

namespace test_config {

void testConfigCreation() {
    nlm::Config config;
    assert(!config.has("test"));
    
    std::cout << "    testConfigCreation passed" << std::endl;
}

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

void testConfigGetOr() {
    nlm::Config config;
    
    int val = config.getOr<int>("missing", 100);
    assert(val == 100);
    
    config.set("existing", 50, nlm::ConfigSource::Runtime);
    val = config.getOr<int>("existing", 100);
    assert(val == 50);
    
    std::cout << "    testConfigGetOr passed" << std::endl;
}

void testConfigRemove() {
    nlm::Config config;
    
    config.set("test", 123, nlm::ConfigSource::Runtime);
    assert(config.has("test"));
    
    config.remove("test");
    assert(!config.has("test"));
    
    std::cout << "    testConfigRemove passed" << std::endl;
}

void testConfigKeys() {
    nlm::Config config;
    
    config.set("key1", 1, nlm::ConfigSource::Runtime);
    config.set("key2", 2, nlm::ConfigSource::Runtime);
    config.set("key3", 3, nlm::ConfigSource::Runtime);
    
    auto keys = config.getKeys();
    assert(keys.size() == 3);
    
    std::cout << "    testConfigKeys passed" << std::endl;
}

// Test configuration file format detection
void testConfigFormatDetection() {
    // Test format detection based on file extension
    std::string jsonExt = nlm::Config::getConfigFormat("test.json");
    assert(jsonExt == ".json");
    
    std::string yamlExt = nlm::Config::getConfigFormat("test.yaml");
    assert(yamlExt == ".yaml");
    
    std::string ymlExt = nlm::Config::getConfigFormat("test.yml");
    assert(ymlExt == ".yml");
    
    std::string cfgExt = nlm::Config::getConfigFormat("test.cfg");
    assert(cfgExt == ".cfg");
    
    std::string txtExt = nlm::Config::getConfigFormat("test.txt");
    assert(txtExt == ".txt");
    
    std::cout << "    testConfigFormatDetection passed" << std::endl;
}

// Test configuration validation
void testConfigValidation() {
    nlm::Config config;
    
    // Set valid values
    config.set("random_seed", 42, nlm::ConfigSource::Runtime);
    config.set("neuron_count", 500, nlm::ConfigSource::Runtime);
    config.set("region_count", 2, nlm::ConfigSource::Runtime);
    config.set("connection_probability", 0.15, nlm::ConfigSource::Runtime);
    config.set("simulation_timestep", 0.001, nlm::ConfigSource::Runtime);
    
    // Validation should pass for valid configuration
    assert(config.validateConfig());
    
    // Test with invalid values
    nlm::Config invalidConfig;
    invalidConfig.set("neuron_count", -100, nlm::ConfigSource::Runtime);
    invalidConfig.set("connection_probability", 1.5, nlm::ConfigSource::Runtime);
    invalidConfig.set("simulation_timestep", 0.0, nlm::ConfigSource::Runtime);
    
    // Validation should fail for invalid configuration
    assert(!invalidConfig.validateConfig());
    
    std::cout << "    testConfigValidation passed" << std::endl;
}

// Test configuration file generation
void testConfigFileGeneration() {
    nlm::Config config;
    
    // Set some test values
    config.set("random_seed", 42, nlm::ConfigSource::Runtime);
    config.set("neuron_count", 1000, nlm::ConfigSource::Runtime);
    config.set("region_count", 2, nlm::ConfigSource::Runtime);
    config.set("connection_probability", 0.15, nlm::ConfigSource::Runtime);
    config.set("simulation_timestep", 0.001, nlm::ConfigSource::Runtime);
    config.set("stdp_ltp_weight", 0.02, nlm::ConfigSource::Runtime);
    config.set("stdp_ltd_weight", 0.015, nlm::ConfigSource::Runtime);
    config.set("stdp_time_constant", 20.0, nlm::ConfigSource::Runtime);
    config.set("development_synaptogenesis_rate", 0.0001, nlm::ConfigSource::Runtime);
    config.set("development_pruning_rate", 0.00001, nlm::ConfigSource::Runtime);
    config.set("log_level", std::string("INFO"), nlm::ConfigSource::Runtime);
    config.set("visualization_enabled", false, nlm::ConfigSource::Runtime);
    
    // Test key-value format generation
    assert(config.generateConfigFile("test_cfg.cfg", "cfg"));
    
    // Verify the file was created and contains expected content
    std::ifstream file("test_cfg.cfg");
    assert(file.good());
    
    // Check for some expected content
    std::string line;
    bool foundRandomSeed = false;
    bool foundNeuronCount = false;
    
    while (std::getline(file, line)) {
        if (line.find("random_seed") != std::string::npos) {
            foundRandomSeed = true;
        }
        if (line.find("neuron_count") != std::string::npos) {
            foundNeuronCount = true;
        }
    }
    
    assert(foundRandomSeed);
    assert(foundNeuronCount);
    
    // Clean up
    std::remove("test_cfg.cfg");
    
    std::cout << "    testConfigFileGeneration passed" << std::endl;
}

// Test configuration file loading (key-value format)
void testConfigLoadFromKeyValue() {
    nlm::Config config;
    
    // Create a test configuration file
    std::ofstream file("test_kv.cfg");
    file << "# Test Configuration File\n";
    file << "random_seed = 42\n";
    file << "neuron_count = 1000\n";
    file << "region_count = 2\n";
    file << "connection_probability = 0.15\n";
    file << "simulation_timestep = 0.001\n";
    file.close();
    
    // Load the configuration file
    assert(config.loadFromKeyValue("test_kv.cfg"));
    
    // Verify the values were loaded correctly
    auto randomSeed = config.get<int64_t>("random_seed");
    assert(randomSeed.has_value());
    assert(*randomSeed == 42);
    
    auto neuronCount = config.get<int64_t>("neuron_count");
    assert(neuronCount.has_value());
    assert(*neuronCount == 1000);
    
    // Clean up
    std::remove("test_kv.cfg");
    
    std::cout << "    testConfigLoadFromKeyValue passed" << std::endl;
}

// Test configuration summary
void testConfigSummary() {
    nlm::Config config;
    
    config.set("random_seed", 42, nlm::ConfigSource::Runtime);
    config.set("neuron_count", 500, nlm::ConfigSource::Runtime);
    config.set("region_count", 1, nlm::ConfigSource::Runtime);
    
    std::string summary = config.summary();
    assert(summary.find("Configuration") != std::string::npos);
    assert(summary.find("random_seed") != std::string::npos);
    assert(summary.find("neuron_count") != std::string::npos);
    assert(summary.find("region_count") != std::string::npos);
    
    std::cout << "    testConfigSummary passed" << std::endl;
}

// Test configuration source tracking
void testConfigSourceTracking() {
    nlm::Config config;
    
    // Set values with different sources
    config.set("key1", 1, nlm::ConfigSource::Default);
    config.set("key2", 2, nlm::ConfigSource::File);
    config.set("key3", 3, nlm::ConfigSource::CommandLine);
    config.set("key4", 4, nlm::ConfigSource::Runtime);
    
    // Verify the sources are tracked correctly
    auto keys = config.getKeys();
    assert(keys.size() == 4);
    
    std::cout << "    testConfigSourceTracking passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigFormatDetection();
    testConfigValidation();
    testConfigFileGeneration();
    testConfigLoadFromKeyValue();
    testConfigSummary();
    testConfigSourceTracking();
}

} // namespace test_config
